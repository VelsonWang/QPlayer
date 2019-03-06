#include <QFile>
#include <QDebug>
#include <QSettings>
#include <QCoreApplication>
#include <QDir>

#include "GlobalHelper.h"

const QString g_PlayerConfigBaseDir = QDir::tempPath();
const QString g_PlayerConfig = "player_config.ini";
const QString g_AppVersion = "0.0.1.0";

GlobalHelper::GlobalHelper()
{
}

QString GlobalHelper::getQssStr(QString strQssPath)
{
    QString strQss = QString();
    QFile fileQss(strQssPath);

    if (fileQss.open(QIODevice::ReadOnly)) {
        strQss = fileQss.readAll();
        fileQss.close();
    } else {
        qDebug() << "读取样式表失败" << strQssPath;
    }

    return strQss;
}

void GlobalHelper::setIcon(QPushButton* btn, int iconSize, QChar icon)
{
    QFont font;
    font.setFamily("FontAwesome");
    font.setPointSize(iconSize);

    btn->setFont(font);
    btn->setText(icon);
}

void GlobalHelper::savePlaylist(QStringList& playList)
{
    QString strCfgFileName = g_PlayerConfigBaseDir + QDir::separator() + g_PlayerConfig;
    QSettings settings(strCfgFileName, QSettings::IniFormat);
    settings.beginWriteArray("playlist");

    for (int i = 0; i < playList.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("movie", playList.at(i));
    }

    settings.endArray();
}

void GlobalHelper::getPlaylist(QStringList& playList)
{
    QString strCfgFileName = g_PlayerConfigBaseDir + QDir::separator() + g_PlayerConfig;
    QSettings settings(strCfgFileName, QSettings::IniFormat);

    int size = settings.beginReadArray("playlist");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        playList.append(settings.value("movie").toString());
    }
    settings.endArray();
}

void GlobalHelper::savePlayVolume(double& nVolume)
{
    QString strCfgFileName = g_PlayerConfigBaseDir + QDir::separator() + g_PlayerConfig;
    QSettings settings(strCfgFileName, QSettings::IniFormat);
    settings.setValue("volume/size", nVolume);
}

void GlobalHelper::getPlayVolume(double& nVolume)
{
    QString strCfgFileName = g_PlayerConfigBaseDir + QDir::separator() + g_PlayerConfig;
    QSettings settings(strCfgFileName, QSettings::IniFormat);
    QString str = settings.value("volume/size").toString();
    nVolume = settings.value("volume/size", nVolume).toDouble();
}

QString GlobalHelper::getAppVersion()
{
    return g_AppVersion;
}

