#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct StateStruct {
    QString appName;
    QString appPath;
    QString iconPath;
    bool isCopyResources;
    bool isGlobal;
    //
    QString launcherName;
    QString folderName;
    //
    QString launcherContent;
    //
    QStringList copyingPaths;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    // services
    void loadDefaults();
    StateStruct getState();
    void updateStatic();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_actionOpen_triggered();

    void on_actionReset_2_triggered();

    void on_lineEdit_textEdited(const QString &arg1);

    void on_pushButton_3_clicked();

    void on_lineEdit_2_textEdited(const QString &arg1);

    void on_lineEdit_3_textEdited(const QString &arg1);

    void on_checkBox_2_stateChanged(int arg1);

    void on_checkBox_stateChanged(int arg1);

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel* listModel;
};

#endif // MAINWINDOW_H
