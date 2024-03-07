#include "filesystem.h"

bool Filesystem::isFileExists(const QString &filepath)
{
    QFileInfo check_file(filepath);
    return check_file.exists() && check_file.isFile();
}

bool Filesystem::isDirExists(const QString &directoryPath)
{
    QDir directory(directoryPath);
    return directory.exists();
}

void Filesystem::deleteFile(const QString &filepath)
{
    QFile file (filepath);
    file.remove();
}

void Filesystem::createFile(const QString &filepath, QString contains)
{
    // Path
    QFileInfo fileInfo(filepath);
    QDir dir(fileInfo.absoluteDir());

    // Folders
    if (!dir.exists()) {
        if (!dir.mkpath(dir.absolutePath())) {
            qDebug() << "Failed to create directories";
            return;
        }
    }

    // File
    QFile file(filepath);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << contains << Qt::endl;
        file.close();
    } else {
        qDebug() << "Failed to open file for writing";
    }
}

PermsPlusStruct Filesystem::copyItem(const QString &sourcePath, const QString &destinationPath)
{    
    QFileInfo sourceInfo(sourcePath);
    PermsPlusStruct permsPlus = Filesystem::getPermsPlus(sourcePath);

    QString destination;

    // If sourcePath is a folder, append its name to destinationPath
    if (sourceInfo.isDir()) {
        QString sourceDirName = QDir(sourcePath).dirName();
        destination = QDir(destinationPath).filePath(sourceDirName);
    } else {
        // If sourcePath is a file, simply copy the file name to destinationPath
        destination = QDir(destinationPath).filePath(sourceInfo.fileName());
    }

    if (sourceInfo.isDir()) {
        QDir destinationDir(destination);
        if (!destinationDir.exists()) {
            if (!destinationDir.mkpath(destination)) {
                qDebug() << "Failed to create destination directory" << destination;
                return permsPlus;
            }
            // folders permissions
            Filesystem::setPermissions(destination, permsPlus);
        }

        QDir sourceDir(sourcePath);
        QStringList files = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (const QString &file, files) {
            QString sourceFilePath = sourceDir.filePath(file);
            QString destinationFilePath = QDir(destination).filePath(file);
            copyItem(sourceFilePath, destination);
        }
    } else if (sourceInfo.isFile()) {
        if (!QFile::copy(sourcePath, destination)) {
            qDebug() << "Failed to copy file" << sourcePath << "to" << destination;
        } else { // Permissions
            Filesystem::setPermissions(destination, permsPlus);
        }
    }


    return permsPlus;
}

PermsPlusStruct Filesystem::copyToDirectory(const QString &sourcePath, const QString &destinationDirectory)
{
    PermsPlusStruct permsStruct;
    QDir destinationDir(destinationDirectory);
    if (!destinationDir.exists()) {
        if (!destinationDir.mkpath(destinationDirectory)) {
            qDebug() << "Failed to create destination directory" << destinationDirectory;
            return permsStruct;
        }
    }
    return copyItem(sourcePath, destinationDirectory);
}

void Filesystem::setPermissions(
        const QString &sourcePath,
        const PermsPlusStruct &permsPlus
        )
{
    Filesystem::setPermissions(sourcePath, permsPlus.perms, permsPlus.ownerId, permsPlus.groupId);
}
void Filesystem::setPermissions(
        const QFile &file,
        const PermsPlusStruct &permsPlus
        )
{
    Filesystem::setPermissions(file, permsPlus.perms, permsPlus.ownerId, permsPlus.groupId);
}

void Filesystem::setPermissions(
        const QString &sourcePath,
        const QFileDevice::Permissions perms,
        int ownerId,
        int groupId
        )
{
    QFile::setPermissions(sourcePath, perms);
    if (ownerId && groupId) {
        QByteArray ba = sourcePath.toLocal8Bit();
        const char* c_str = ba.data();
        chown(c_str, ownerId, groupId);
    }
}
void Filesystem::setPermissions(
        const QFile &file,
        const QFileDevice::Permissions perms,
        int ownerId,
        int groupId
        )
{
    Filesystem::setPermissions(file.fileName(), perms, ownerId, groupId);
}

QString Filesystem::getFile(const QString &filepath)
{
    QFile file(filepath);

    QString line;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){
            line.append(stream.readLine()+"\n");
        }
    }
    file.close();
    return line;
}

PermsPlusStruct Filesystem::getPermsPlus(const QString &sourcePath) {
    QFileInfo sourceInfo(sourcePath);
    PermsPlusStruct permsPlus;
    permsPlus.perms = sourceInfo.permissions();
    permsPlus.ownerId = sourceInfo.ownerId();
    permsPlus.groupId = sourceInfo.groupId();
    return permsPlus;
}
