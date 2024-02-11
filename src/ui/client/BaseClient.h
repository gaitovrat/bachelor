#ifndef BASECLIENT_H
#define BASECLIENT_H

#include <optional>

#include <QIODevice>
#include <QObject>

#include "Data.h"

class BaseClient : public QObject {
    Q_OBJECT

    using IOSignal = void (QIODevice::*)();

   public:
    BaseClient(QObject *parent);

    void bind(const QIODevice *sender, IOSignal signal);

    virtual ~BaseClient() = default;

    virtual std::optional<Data> getData() = 0;

    virtual bool connected() const = 0;

    virtual void connect() = 0;

   public slots:
    void read();

   signals:
    void dataReady(const Data &data);
};

#endif
