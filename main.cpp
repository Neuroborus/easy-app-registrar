#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "config.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Easy_App_Registrar_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    Config::createDefault();
    Config::validate();
    //
    MainWindow w;
    // w.setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
    w.show();
    return a.exec();
}
