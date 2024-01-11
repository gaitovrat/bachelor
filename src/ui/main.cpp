#include <QtWidgets/qapplication.h>

#include <QApplication>

#include "window/MainWindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    MainWindow window;

    window.show();
    return app.exec();
}
