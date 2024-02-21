#include "BaseClient.h"

#include <optional>

BaseClient::BaseClient(QObject *parent) : QObject(parent) {}

void BaseClient::bind(const QIODevice *sender, IOSignal signal) {
    QObject::connect(sender, signal, this, &BaseClient::read);
}

void BaseClient::read() {
    std::optional<Data> data = this->getData();

    if (data.has_value()) {
        emit this->dataReady(*data);
    }
}