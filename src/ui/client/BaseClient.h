#ifndef BASECLIENT_H
#define BASECLIENT_H

#include <QIODevice>
#include <QObject>

class BaseClient : public QObject {
    Q_OBJECT

    using IOSignal = void (QIODevice::*)();

   public:
    BaseClient(QObject *parent);

    void connect(const QIODevice *sender, IOSignal signal);

    virtual ~BaseClient() = default;

    virtual void emitData() = 0;

   public slots:
    void read();
};

#endif
