#include "BaseClient.h"

BaseClient::BaseClient(const QIODevice *sender, IOSignal signal,
                       QObject *parent)
    : QObject(parent) {
    connect(sender, signal, this, &BaseClient::read);
}

void BaseClient::read() { this->emitData(); }
