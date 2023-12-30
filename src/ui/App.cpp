#include "App.h"

App::App(int argc, char **argv) : QGuiApplication(argc, argv) {
    const QUrl url(u"qrc:/qt/qml/carui/assets/main.qml"_qs);

    QObject::connect(
        &this->engine, &QQmlApplicationEngine::objectCreationFailed, this,
        []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    this->engine.load(url);
}
