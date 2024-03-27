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
    explicit BaseClient(QObject *parent);

    void Bind(const QIODevice *sender, IOSignal signal) const;

    ~BaseClient() override = default;

    virtual std::optional<Shared::Data> Data() = 0;

    [[nodiscard]] virtual bool IsConnected() const = 0;

    virtual void Connect() = 0;

  public slots:
    void Read();

  signals:
    void DataReady(const Shared::Data &data);

    void DataReceived(qint64 size);
};
} // namespace CarQt

#endif