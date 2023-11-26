#include "baseclient.h"

BaseClient::BaseClient(Sender *sender, Signal signal, QObject *parent) : QObject(parent) {
    connect(sender, signal, this, &BaseClient::read);
}

void BaseClient::read() {
    this->emitData();
}
