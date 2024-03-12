#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QScreen>

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

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();

    double WidthCoef = (double)(800) / (double)(1920);
    double heightCoef = (double)(475) / (double)(1080);

    int windowWidth = screenGeometry.width() * WidthCoef;
    int windowHeight = screenGeometry.height() * heightCoef;
    int windowX = (screenGeometry.width() - windowWidth) / 2;
    int windowY = (screenGeometry.height() - windowHeight) / 2;
    w.setGeometry(windowX, windowY, windowWidth, windowHeight);
    w.setFixedSize(windowWidth, windowHeight);

    w.show();
    return a.exec();
}
