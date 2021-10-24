#include <QApplication>

#include "protocol/singleapplication.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(application);

    SingleApplication app(argc, argv);
    MainWindow mainWin(argv);

    QObject::connect(&app, &SingleApplication::instanceStarted, [ &mainWin ]() {
            mainWin.raise();
            mainWin.openByProtocol();
    });

    mainWin.show();

    return app.exec();
}
