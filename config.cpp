#include "config.h"

void Config::create()
{
    ConfigStruct conf;
    QJsonObject general;
    general.insert("globalApplicationsPath", conf.globalApplicationsPath);
    general.insert("localApplicationsPath", conf.localApplicationsPath);
    general.insert("appsDir", conf.appsDir);
    general.insert("isCopyResourcesToAppsDir", conf.isCopyResourcesToAppsDir);
    general.insert("isPreferGlobal", conf.isPreferGlobal);

    QJsonObject app;
    app.insert("header", conf.appTemplate.header);
    app.insert("name", conf.appTemplate.name);
    app.insert("exec", conf.appTemplate.exec);
    app.insert("icon", conf.appTemplate.icon);
    app.insert("type", conf.appTemplate.type);
    app.insert("startupNotify", conf.appTemplate.startupNotify);

    general.insert("appTemplate", app);

    QJsonDocument doc(general);
    Filesystem::createFile(CONFIG_PATH, doc.toJson());
}

void Config::updateTemplate(const ConfigStruct &app)
{
    QString content =  Filesystem::getFile(CONFIG_PATH);
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
    QJsonObject obj = doc.object();

    QJsonObject prevApp = obj.value(QString("appTemplate")).toObject();

    prevApp.insert("header", app.appTemplate.header);
    prevApp.insert("name", app.appTemplate.name);
    prevApp.insert("exec", app.appTemplate.exec);
    prevApp.insert("icon", app.appTemplate.icon);
    prevApp.insert("type", app.appTemplate.type);
    prevApp.insert("startupNotify", app.appTemplate.startupNotify);

    obj.insert("appTemplate", prevApp);
    obj.insert("isCopyResourcesToAppsDir", app.isCopyResourcesToAppsDir);
    obj.insert("isPreferGlobal", app.isPreferGlobal);

    QJsonDocument updated(obj);
    Config::reset();
    Filesystem::createFile(CONFIG_PATH, updated.toJson());
}

void Config::createDefault()
{
    const bool isExists = Filesystem::isFileExists(CONFIG_PATH);
    if (!isExists) {
        Config::create();
    }
}

void Config::reset()
{
    const bool isExists = Filesystem::isFileExists(CONFIG_PATH);
    if (isExists) {
        Filesystem::deleteFile(CONFIG_PATH);
    }
    Config::create();
}

ConfigStruct Config::get()
{
    const QString content =  Filesystem::getFile(CONFIG_PATH);
    const QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
    const QJsonObject obj = doc.object();

    ConfigStruct conf;
    conf.globalApplicationsPath = obj.value("globalApplicationsPath").toString();
    conf.localApplicationsPath = obj.value("localApplicationsPath").toString();
    conf.appsDir = obj.value("appsDir").toString();
    conf.isCopyResourcesToAppsDir = obj.value("isCopyResourcesToAppsDir").toBool();
    conf.isPreferGlobal = obj.value("isPreferGlobal").toBool();

    QJsonObject app = obj.value("appTemplate").toObject();
    conf.appTemplate.header = app.value("header").toString();
    conf.appTemplate.name = app.value("name").toString();
    conf.appTemplate.exec = app.value("exec").toString();
    conf.appTemplate.icon = app.value("icon").toString();
    conf.appTemplate.type = app.value("type").toString();
    conf.appTemplate.startupNotify = app.value("startupNotify").toString();

    return conf;
}

bool Config::validate()
{
    bool flag = true;
    const bool isExists = Filesystem::isFileExists(CONFIG_PATH);
    if (!isExists) {
        flag = false;
        qDebug() << "config does not exists!";
    } else {
        QString content = Filesystem::getFile(CONFIG_PATH);
        QJsonDocument d = QJsonDocument::fromJson(content.toUtf8());
        if (!d.isObject()) {
            flag = false;
            qDebug() << "config broken: is not valid object!";
        }
    }
    return flag;
}
