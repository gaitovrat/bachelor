#include <QApplication>
#include <QFile>

#include "Window/MainWindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    MainWindow window(app.applicationName());
    QFile style(":/resources/style.qss");
    style.open(QFile::ReadOnly);
    app.setStyleSheet(style.readAll());

    window.show();
    return app.exec();
}
