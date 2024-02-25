#include "BaseClient.h"

#include <optional>

using namespace CarQt;

BaseClient::BaseClient(QObject *parent) : QObject(parent) {}

void BaseClient::Bind(const QIODevice *sender, IOSignal signal) {
    QObject::connect(sender, signal, this, &BaseClient::Read);
}

void BaseClient::Read() {
    std::optional<Shared::Data> data = Data();

    if (data.has_value()) {
        emit DataReady(*data);
    }
}
