#ifndef BASECLIENT_H
#define BASECLIENT_H

#include <optional>

#include <QIODevice>
#include <QObject>

#include "Shared/Data.h"

namespace CarQt {
class BaseClient : public QObject {
    Q_OBJECT

    using IOSignal = void (QIODevice::*)();

   public:
    BaseClient(QObject *parent);

    void Bind(const QIODevice *sender, IOSignal signal);

    virtual ~BaseClient() = default;

    virtual std::optional<Shared::Data> Data() = 0;

    virtual bool IsConnected() const = 0;

    virtual void Connect() = 0;

   public slots:
    void Read();

   signals:
    void DataReady(const Shared::Data &data);

    void DataReceived(qint64 size);
};
}

#endif
