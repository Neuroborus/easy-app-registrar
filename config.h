#ifndef CONFIG_H
#define CONFIG_H

#include "filesystem.h"
#include <QJsonDocument>
#include <QJsonObject>
// For username under root getting
#include <unistd.h>
//

const QString USERNAME = getlogin();
const QString DIR = "/app-registrar/";
const QString CONFIG_PATH = DIR + "app-registrar-config.json";

struct AppTemplateStruct {
    QString header = "[Desktop Entry]";
    QString name = "My App";
    QString exec = "/home/" + USERNAME + "/Downloads/exec.sh";
    QString type = "Application";
    QString icon = "/home/" + USERNAME + "/Downloads/icon.png";
    QString startupNotify = "true";
};

struct ConfigStruct {
    QString globalApplicationsPath = "/usr/share/applications/";
    QString localApplicationsPath = "/home/" + USERNAME + "/.local/share/applications/";
    QString appsDir = DIR;
    bool isCopyResourcesToAppsDir = true;
    bool isPreferGlobal = false;
    AppTemplateStruct appTemplate;
};



class Config
{
public:
    Config();
    ~Config();

    void static createDefault();
    void static reset();
    void static create();
    ConfigStruct static get();
    bool static validate();

    void static updateTemplate(const ConfigStruct &app);
};


#endif // CONFIG_H
