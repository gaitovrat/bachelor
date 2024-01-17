#ifndef BASECLIENT_H
#define BASECLIENT_H

#include <QIODevice>
#include <QObject>
#include <optional>

#include "Data.h"

class BaseClient : public QObject {
    Q_OBJECT

    using IOSignal = void (QIODevice::*)();

   public:
    BaseClient(QObject *parent);

    void connect(const QIODevice *sender, IOSignal signal);

    virtual ~BaseClient() = default;

    virtual std::optional<Data> getData() = 0;

   public slots:
    void read();

   signals:
    void dataReady(const Data &data);
};

#endif
