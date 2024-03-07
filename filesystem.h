#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QString>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
// For username under root getting
#include <unistd.h>
//

const QFileDevice::Permissions DEFAULT_PERMS =
        QFile::WriteOwner | QFile::ReadOwner | QFile::ExeOwner |
        QFile::ReadGroup | QFile::ExeGroup |
        QFile::ReadOther | QFile::ExeOther;
const int DEFAULT_OWNER_ID = 0;
const int DEFAULT_GROUP_ID = 0;

struct PermsPlusStruct {
    QFileDevice::Permissions perms = DEFAULT_PERMS;
    int ownerId = DEFAULT_OWNER_ID;
    int groupId = DEFAULT_GROUP_ID;
};

class Filesystem
{
private:
    PermsPlusStruct static copyItem(const QString &sourcePath, const QString &destinationPath);

public:
    Filesystem();
    ~Filesystem();

    bool static isFileExists(const QString &filepath);
    bool static isDirExists(const QString &directoryPath);
    void static deleteFile(const QString &filepath);
    void static createFile(const QString &filepath, QString contains = "");
    PermsPlusStruct static copyToDirectory(const QString &sourcePath, const QString &destinationDirectory);
    void static setPermissions(const QString &sourcePath,
                               const PermsPlusStruct &permsPlus);
    void static setPermissions(const QFile &file,
                               const PermsPlusStruct &permsPlus);
    void static setPermissions(const QString &sourcePath,
                               QFileDevice::Permissions perms = DEFAULT_PERMS,
                               int ownerId = DEFAULT_OWNER_ID,
                               int groupId = DEFAULT_GROUP_ID);
    void static setPermissions(const QFile &file,
                               QFileDevice::Permissions perms  = DEFAULT_PERMS,
                               int ownerId = DEFAULT_OWNER_ID,
                               int groupId = DEFAULT_GROUP_ID);

    QString static getFile(const QString &filepath);
    PermsPlusStruct static getPermsPlus(const QString &sourcePath);
};

#endif // FILESYSTEM_H
