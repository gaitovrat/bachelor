#ifndef BASECLIENT_H
#define BASECLIENT_H

#include <QObject>
#include <QIODevice>

class BaseClient : public QObject {
    Q_OBJECT

    using Signal = void (QIODevice::*)();
    using Sender = const typename QtPrivate::FunctionPointer<Signal>::Object;

public:
    BaseClient(Sender *sender, Signal signal, QObject *parent = nullptr);

    virtual ~BaseClient() = default;

    virtual void emitData() = 0;

public slots:
    void read();
};

#endif
