#include "qcanopenmaster.h"

QCanopenMaster* QCanopenMaster::instance() {
    static QCanopenMaster* s_instance = nullptr;
    if (!s_instance) {
        s_instance = new QCanopenMaster();
    }
    return s_instance;
}

QCanopenMaster::QCanopenMaster(QObject* parent){

}
QCanopenMaster::~QCanopenMaster(){

}

void QCanopenMaster::init() {
    // TODO: implement
}

void QCanopenMaster::process() {
    // TODO: implement
}

void QCanopenMaster::setCurrentNodeId(uint8_t nodeId) {
    Q_UNUSED(nodeId)
    // TODO: implement
}

void QCanopenMaster::initDevice(QVariantMap params) {
    Q_UNUSED(params)
    // TODO: implement
}

void QCanopenMaster::openDevice() {
    // TODO: implement
}

void QCanopenMaster::closeDevice() {
    // TODO: implement
}

void QCanopenMaster::resetCanDevice() {
    // TODO: implement
}

void QCanopenMaster::queryDeviceState() {
    // TODO: implement
}

void QCanopenMaster::sendRequest(QVariantMap params) {
    Q_UNUSED(params)
    // TODO: implement
}

void QCanopenMaster::sendNMT(uint8_t command) {
    Q_UNUSED(command)
    // TODO: implement
}
