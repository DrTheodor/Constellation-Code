#include <QApplication>

#include "protocol/singleapplication.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(application);

    SingleApplication app(argc, argv, true);
    MainWindow window;
    if(app.isSecondary()) {
        app.sendMessage(app.arguments().at(1).toUtf8());
        return 0;
    } else {
        QObject::connect(
            &app,
            &SingleApplication::receivedMessage,
            &window,
            &MainWindow::openByProtocol
        );
    }
    window.show();

    return app.exec();
}
