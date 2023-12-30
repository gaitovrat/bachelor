#ifndef APP_H
#define APP_H

#include <QGuiApplication>
#include <QQmlApplicationEngine>

class App : public QGuiApplication {
    QQmlApplicationEngine engine;

public:
    App(int argc, char **argv);
};

#endif
