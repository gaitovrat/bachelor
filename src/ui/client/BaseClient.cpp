#include "BaseClient.h"

BaseClient::BaseClient(QObject *parent)
    : QObject(parent) {}

void BaseClient::connect(const QIODevice *sender, IOSignal signal) {
    QObject::connect(sender, signal, this, &BaseClient::read);
}

void BaseClient::read() { this->emitData(); }
