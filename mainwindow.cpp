#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filesystem.h"
#include "config.h"

#include <QProcess>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardItemModel>
#include <unistd.h>
#include <QTreeView>

#include "exception.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    if (geteuid() != 0) { // is not root access
        QMessageBox::warning(
                    this,
                    "No root privileges!",
                    "Run this application as root!",
                    QMessageBox::Ok
                    );
        QCoreApplication::quit();
        this->~MainWindow(); // todo: replace with something healthy
        return;
    }
    this->listModel = new QStandardItemModel(this);
    ui->setupUi(this);
    ui->listView->setModel(listModel);
    this->loadDefaults();
    ui->progressBar->setVisible(false);
    this->updateStatic();
}

MainWindow::~MainWindow()
{
    delete this->listModel;
    delete ui;
}

void MainWindow::loadDefaults()
{
    ConfigStruct conf = Config::get();
    ui->lineEdit->setText(conf.appTemplate.name);
    ui->lineEdit_2->setText(conf.appTemplate.exec);
    ui->lineEdit_3->setText(conf.appTemplate.icon);
    //
    ui->checkBox->setChecked(conf.isCopyResourcesToAppsDir);
    // Global / local
    ui->radioButton->setChecked(conf.isPreferGlobal);
    ui->radioButton_2->setChecked(!conf.isPreferGlobal);

    this->updateStatic();
}

StateStruct MainWindow::getState()
{
    StateStruct state;
    state.appName = ui->lineEdit->text();
    state.appPath = ui->lineEdit_2->text();
    state.iconPath = ui->lineEdit_3->text();

    QString rawName = state.appName;
    QString validName = rawName.replace(QRegularExpression("[^\\w\\.]+"), "-");
    state.launcherName = validName + ".desktop";
    state.folderName = validName;

    state.isCopyResources = ui->checkBox->isChecked();
    state.isGlobal = ui->radioButton->isChecked();

    ConfigStruct conf = Config::get();

    if (ui->checkBox_2->isChecked()) { // if edit mode
        state.launcherContent = ui->textBrowser->toPlainText();
    } else {
    if (ui->checkBox->isChecked()) { // if copying turn on
        QFileInfo execInfo(state.appPath);
        QFileInfo iconInfo(state.iconPath);
        const QString folderPath = Config::get().appsDir + state.folderName; // todo rm code dups
        state.launcherContent =
                conf.appTemplate.header +
                "\n" +
                "Name=" + state.appName +
                "\n" +
                "Exec=" + folderPath + "/" + QFileInfo(state.appPath).fileName() +
                "\n" +
                "Type=" + conf.appTemplate.type +
                "\n" +
                "Icon=" + folderPath + "/" + QFileInfo(state.iconPath).fileName() +
                "\n" +
                "StartupNotify=" + conf.appTemplate.startupNotify +
                "\n";

    } else {
    state.launcherContent =
            conf.appTemplate.header +
            "\n" +
            "Name=" + state.appName +
            "\n" +
            "Exec=" + state.appPath +
            "\n" +
            "Type=" + conf.appTemplate.type +
            "\n" +
            "Icon=" + state.iconPath +
            "\n" +
            "StartupNotify=" + conf.appTemplate.startupNotify +
            "\n";
        }
    }

    int n = this->listModel->rowCount();
    if (ui->checkBox->isChecked() && n > 0) {
        state.copyingPaths.push_back(state.appPath);
        state.copyingPaths.push_back(state.iconPath);
        for (int row = 2; row < n; ++row) {
            QModelIndex index = this->listModel->index(row, 0); // Assuming single column
            QString element = this->listModel->data(index, Qt::DisplayRole).toString();
            state.copyingPaths.push_back(element);
        }
    } else {
        state.copyingPaths.push_back(state.appPath);
        state.copyingPaths.push_back(state.iconPath);
    }

    return state;
}

void MainWindow::updateStatic()
{
    StateStruct state = this->getState();
    ui->label_4->setText("Launcher name: " + state.launcherName);
    ui->label_5->setText("Folder name: " + state.folderName);

    ui->textBrowser->setText(state.launcherContent);

    this->listModel->clear();
    if (ui->checkBox->isChecked()) { // Copy resources
        ui->pushButton_4->setVisible(true);
        ui->listView->setVisible(true);

        foreach (QString cpyI, state.copyingPaths) {
            this->listModel->appendRow(new QStandardItem(cpyI));
        }
    } else {
        ui->pushButton_4->setVisible(false);
        ui->listView->setVisible(false);
    }
}


void MainWindow::on_pushButton_clicked() // Defaults
{
    QMessageBox::StandardButton confirmation =
            QMessageBox::question(
                this,
                "Are you sure?",
                "All inputs will be lost!",
                QMessageBox::YesToAll | QMessageBox::Cancel
                );
    if (confirmation == QMessageBox::YesToAll) {
        this->loadDefaults();
    }
}


void MainWindow::on_pushButton_2_clicked() // Create
{
    ui->pushButton_2->setVisible(false);
    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(true);
    try {
        StateStruct state = this->getState();
        //
        ui->progressBar->setValue(10);

        QFileInfo execInfo(state.appPath);
        QFileInfo iconInfo(state.iconPath);
        //
        ui->progressBar->setValue(20);

        if (!execInfo.exists()){
            const QString ex = "Exec does not exists!";
            QMessageBox::warning(this, "Aborted!", ex, QMessageBox::Abort);
            throw Exception();
        }
        if (!iconInfo.exists()){
            const QString ex = "Icon does not exists!";
            QMessageBox::warning(this, "Aborted!", ex, QMessageBox::Abort);
            throw Exception();
        }
        //
        ui->progressBar->setValue(30);

        const PermsPlusStruct permsPlusExec = Filesystem::getPermsPlus(state.appPath);
        QStringList missedfilesOrDirs;
        if (state.isCopyResources) {
            const QString folderPath = Config::get().appsDir + state.folderName;

            foreach (QString fileOrDir, state.copyingPaths) {
                if(Filesystem::isFileExists(fileOrDir) || Filesystem::isDirExists(fileOrDir)) {
                    Filesystem::copyToDirectory(fileOrDir, folderPath);

                } else {
                    missedfilesOrDirs.append(fileOrDir);
                }
            }
        }
        //
        ui->progressBar->setValue(50);

        if (missedfilesOrDirs.size() > 0){
            QString message = "Some files or dirs are missing: ";
            foreach (QString file, missedfilesOrDirs) {
                message += "\n" + file;
            }
            QMessageBox::warning(this, "Aborted!", message, QMessageBox::Abort);
            qDebug() << "Missed files: " << missedfilesOrDirs << Qt::endl;
            throw Exception();
        }

        QString registerPath;
        if (state.isGlobal) {
            registerPath = Config::get().globalApplicationsPath + state.launcherName;
        } else {
            registerPath = Config::get().localApplicationsPath + state.launcherName;
        }
        //
        ui->progressBar->setValue(80);

        Filesystem::createFile(registerPath, state.launcherContent);
        if (state.isGlobal) {
            Filesystem::setPermissions(registerPath);
        } else {
            Filesystem::setPermissions(registerPath, permsPlusExec);
        }
        //
        ui->progressBar->setValue(100);

        // todo: checksum comparing

        QMessageBox::information(this, "Success!", "Job completed successfully!", QMessageBox::Ok);
        } catch (const QException &e) {
            qDebug() << e.what() << Qt::endl;
        }
    ui->pushButton_2->setVisible(true);
    ui->progressBar->setVisible(false);
}

void MainWindow::on_pushButton_3_clicked() // Create defaults
{
    QMessageBox::StandardButton confirmation =
            QMessageBox::question(
                this,
                "Are you sure?",
                "All settings will be overwritten!",
                QMessageBox::YesToAll | QMessageBox::Cancel
                );
   if (confirmation == QMessageBox::YesToAll) {
       StateStruct state = MainWindow::getState();
       ConfigStruct templ;
       templ.appTemplate.name = state.appName;
       templ.appTemplate.exec = state.appPath;
       templ.appTemplate.icon = state.iconPath;
       templ.isCopyResourcesToAppsDir = state.isCopyResources;
       templ.isPreferGlobal = state.isGlobal;


       Config::updateTemplate(templ);
   }
}

void MainWindow::on_pushButton_4_clicked() // Add resources button (files and folders)
{
    const StateStruct state = this->getState();
    QFileDialog* f_dlg = new QFileDialog(this);
    f_dlg->setDirectory(QFileInfo(state.appPath).absoluteDir());
    f_dlg->setFileMode(QFileDialog::Directory);
    f_dlg->setOption(QFileDialog::DontUseNativeDialog, true);

    // Try to select multiple files and directories at the same time in QFileDialog
    QListView *l = f_dlg->findChild<QListView*>("listView");
    if (l) {
      l->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    QTreeView *t = f_dlg->findChild<QTreeView*>();
    if (t) {
       t->setSelectionMode(QAbstractItemView::MultiSelection);
    }

    int _nMode = f_dlg->exec();
    QStringList _fnames = f_dlg->selectedFiles();

    foreach (QString cpyI, _fnames) {
       this->listModel->appendRow(new QStandardItem(cpyI));
    }
}

void MainWindow::on_pushButton_5_clicked() // Search app
{
    const StateStruct state = this->getState();
    const QString dir = QFileInfo(state.appPath).absoluteDir().absolutePath();
    QString fileApp = QFileDialog::getOpenFileName(this, "Select File", dir, "All Files (*)");
    ui->lineEdit_2->setText(fileApp);
    this->updateStatic();
}


void MainWindow::on_pushButton_6_clicked() // Search icon
{
    const StateStruct state = this->getState();
    const QString dir = QFileInfo(state.appPath).absoluteDir().absolutePath();
    QString fileIcon = QFileDialog::getOpenFileName(this, "Select File", dir, "All Files (*)");
    ui->lineEdit_3->setText(fileIcon);
    this->updateStatic();
}


void MainWindow::on_actionOpen_triggered() // Config
{
    QProcess process;
    process.startDetached("gedit", QStringList() << CONFIG_PATH);
}


void MainWindow::on_actionReset_2_triggered() // Config
{
    QMessageBox::StandardButton confirmation =
            QMessageBox::question(
                this,
                "Are you sure?",
                "All configs will be lost!",
                QMessageBox::YesToAll | QMessageBox::Cancel
                );
   if (confirmation == QMessageBox::YesToAll) {
       Config::reset();
   }
}

void MainWindow::on_lineEdit_textEdited(const QString &_arg1) // App name
{
    this->updateStatic();
}


void MainWindow::on_lineEdit_2_textEdited(const QString &_arg1) // Exec
{
    this->updateStatic();
}


void MainWindow::on_lineEdit_3_textEdited(const QString &_arg1) // Icon
{
    this->updateStatic();
}

void MainWindow::on_checkBox_stateChanged(int _arg1) // Copy resources
{
    this->updateStatic();
}

void MainWindow::on_checkBox_2_stateChanged(int _arg1) // Edit mode | 2 - on; 0 - off;
{
    if (ui->checkBox_2->isChecked()) {
        ui->textBrowser->setReadOnly(false);
    } else {
        ui->textBrowser->setReadOnly(true);
    }
}
