﻿
#include "XVideoWidget.h"
#include <QDebug>
#include <QTimer>
extern "C" {
#include <libavutil/frame.h>
}

//自动加双引号
#define GET_STR(x) #x
#define A_VER 3
#define T_VER 4

FILE *fp = NULL;

//顶点shader
const char *vString = GET_STR(
	attribute vec4 vertexIn;
    attribute vec2 textureIn;
    varying vec2 textureOut;
    void main(void)
    {
        gl_Position = vertexIn;
        textureOut = textureIn;
    }
);


//片元shader
const char *tString = GET_STR(
	varying vec2 textureOut;
    uniform sampler2D tex_y;
    uniform sampler2D tex_u;
    uniform sampler2D tex_v;
    void main(void)
    {
        vec3 yuv;
        vec3 rgb;
        yuv.x = texture2D(tex_y, textureOut).r;
        yuv.y = texture2D(tex_u, textureOut).r - 0.5;
        yuv.z = texture2D(tex_v, textureOut).r - 0.5;
        rgb = mat3(1.0, 1.0, 1.0,
            0.0, -0.39465, 2.03211,
            1.13983, -0.58060, 0.0) * yuv;
        gl_FragColor = vec4(rgb, 1.0);
    }

);



//准备yuv数据
// ffmpeg -i v1080.mp4 -t 10 -s 240x128 -pix_fmt yuv420p  out240x128.yuv
XVideoWidget::XVideoWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
}

XVideoWidget::~XVideoWidget()
{
}

void XVideoWidget::repaint(AVFrame *frame)
{
	if (!frame)return;
	mux.lock();
	//容错，保证尺寸正确
    if (!datas[0] || width*height == 0 || frame->width != this->width || frame->height != this->height)
	{
		av_frame_free(&frame);
		mux.unlock();
		return;
	}
	memcpy(datas[0], frame->data[0], width*height);
	memcpy(datas[1], frame->data[1], width*height/4);
	memcpy(datas[2], frame->data[2], width*height/4);
	//行对齐问题
	mux.unlock();

	//刷新显示
	update();
}

// 绘图事件
void XVideoWidget::paintEvent(QPaintEvent *e)
{

    QOpenGLWidget::paintEvent(e);
}

void XVideoWidget::init(int width, int height)
{
	mux.lock();
	this->width = width;
	this->height = height;
	delete datas[0];
	delete datas[1];
	delete datas[2];
	///分配材质内存空间
	datas[0] = new unsigned char[width*height];		//Y
	datas[1] = new unsigned char[width*height / 4];	//U
	datas[2] = new unsigned char[width*height / 4];	//V


	if (texs[0])
	{
		glDeleteTextures(3, texs);
	}
	//创建材质
	glGenTextures(3, texs);

	//Y
	glBindTexture(GL_TEXTURE_2D, texs[0]);
	//放大过滤，线性插值   GL_NEAREST(效率高，但马赛克严重)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//创建材质显卡空间
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	//U
	glBindTexture(GL_TEXTURE_2D, texs[1]);
	//放大过滤，线性插值
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//创建材质显卡空间
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	//V
	glBindTexture(GL_TEXTURE_2D, texs[2]);
	//放大过滤，线性插值
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//创建材质显卡空间
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);


	mux.unlock();


}

//初始化opengl
void XVideoWidget::initializeGL()
{
	qDebug() << "initializeGL";
	mux.lock();
	//初始化opengl （QOpenGLFunctions继承）函数 
	initializeOpenGLFunctions();

	//program加载shader（顶点和片元）脚本
	//片元（像素）
	qDebug() << program.addShaderFromSourceCode(QGLShader::Fragment, tString);
	//顶点shader
	qDebug() << program.addShaderFromSourceCode(QGLShader::Vertex, vString);

	//设置顶点坐标的变量
	program.bindAttributeLocation("vertexIn", A_VER);

	//设置材质坐标
	program.bindAttributeLocation("textureIn", T_VER);

	//编译shader
	qDebug() << "program.link() = " << program.link();

	qDebug() << "program.bind() = " << program.bind();

	//传递顶点和材质坐标
	//顶点
	static const GLfloat ver[] = {
		-1.0f,-1.0f,
		1.0f,-1.0f,
		-1.0f, 1.0f,
		1.0f,1.0f
	};

	//材质
	static const GLfloat tex[] = {
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};

	//顶点
	glVertexAttribPointer(A_VER, 2, GL_FLOAT, 0, 0, ver);
	glEnableVertexAttribArray(A_VER);

	//材质
	glVertexAttribPointer(T_VER, 2, GL_FLOAT, 0, 0, tex);
	glEnableVertexAttribArray(T_VER);


	//从shader获取材质
	unis[0] = program.uniformLocation("tex_y");
	unis[1] = program.uniformLocation("tex_u");
	unis[2] = program.uniformLocation("tex_v");

	mux.unlock();

	//fp = fopen("out240x128.yuv", "rb");
	//if (!fp)
	//{
	//	qDebug() << "out240x128.yuv file open failed!";
	//}


	////启动定时器
	//QTimer *ti = new QTimer(this);
	//connect(ti, SIGNAL(timeout()), this, SLOT(update()));
	//ti->start(40);
}

//刷新显示
void XVideoWidget::paintGL()
{
	//if (feof(fp))
	//{
	//	fseek(fp, 0, SEEK_SET);
	//}
	//fread(datas[0], 1, width*height, fp);
	//fread(datas[1], 1, width*height / 4, fp);
	//fread(datas[2], 1, width*height / 4, fp);
	mux.lock();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texs[0]); //0层绑定到Y材质
										   //修改材质内容(复制内存内容)
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, datas[0]);
	//与shader uni遍历关联
	glUniform1i(unis[0], 0);


	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texs[1]); //1层绑定到U材质
										   //修改材质内容(复制内存内容)
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_RED, GL_UNSIGNED_BYTE, datas[1]);
	//与shader uni遍历关联
	glUniform1i(unis[1], 1);


	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, texs[2]); //2层绑定到V材质
										   //修改材质内容(复制内存内容)
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_RED, GL_UNSIGNED_BYTE, datas[2]);
	//与shader uni遍历关联
	glUniform1i(unis[2], 2);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	qDebug() << "paintGL";
	mux.unlock();
}


// 窗口尺寸变化
void XVideoWidget::resizeGL(int width, int height)
{
	mux.lock();
	qDebug() << "resizeGL " << width << ":" << height;
	mux.unlock();
}
