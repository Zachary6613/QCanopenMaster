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
    // CAN 收发线程
    driver = Driver::instance(&canopenStr);
    QThread* driverThread = new QThread;
    driver->moveToThread(driverThread);

    QObject::connect(this, &QCanopenMaster::initDeviceSig, driver, &Driver::initDevice);
    QObject::connect(this, &QCanopenMaster::openDeviceSig, driver, &Driver::openDevice);
    QObject::connect(this, &QCanopenMaster::closeDeviceSig, driver, &Driver::closeDevice);
    QObject::connect(this, &QCanopenMaster::resetCanDeviceSig, driver, &Driver::resetCanDevice);
    QObject::connect(this, &QCanopenMaster::sendRequestSig, driver, &Driver::sendRequest);
    QObject::connect(driver, &Driver::sendError, this, &QCanopenMaster::sendError);
    QObject::connect(driver, &Driver::sendCanFrameToUi, this, &QCanopenMaster::sendCanFrameToUi);

    QObject::connect(driverThread, &QThread::started, driver, &Driver::process);
    driverThread->start();

    // NMT 线程
    nmt = NMT::instance(&canopenStr);
    QThread* nmtThread = new QThread;
    nmt->moveToThread(nmtThread);

    QObject::connect(this, &QCanopenMaster::sendNMTSig, nmt, &NMT::sendNMT);

    QObject::connect(nmtThread, &QThread::started, nmt, &NMT::process);
    nmtThread->start();

    // 心跳线程
    heartBeat = Heartbeat::instance(&canopenStr);
    QThread* heartBeatThread = new QThread;
    heartBeat->moveToThread(heartBeatThread);

    QObject::connect(driver, &Driver::HeartBeatMsg, heartBeat, &Heartbeat::receiveCanFrame);

    QObject::connect(heartBeatThread, &QThread::started, heartBeat, &Heartbeat::process);
    nmtThread->start();


    //  Object同步：heartBeat、

}

void QCanopenMaster::process() {
    // TODO: implement
}

void QCanopenMaster::setCurrentNodeId(uint8_t nodeId) {
    canopenStr.canOpenSlave.currentNodeId = nodeId;
}

void QCanopenMaster::initDevice(QVariantMap params) {
    emit initDeviceSig(params);
}

void QCanopenMaster::openDevice() {
    emit openDeviceSig();
}

void QCanopenMaster::closeDevice() {
    emit closeDeviceSig();
}

void QCanopenMaster::resetCanDevice() {
    emit resetCanDeviceSig();
}


void QCanopenMaster::sendRequest(QVariantMap params) {
    CANFrameStr data;
    if (params.contains("canId")) {
        QString canIdStr = params["canId"].toString().trimmed();
        bool ok = false;
        data.canId = canIdStr.toUInt(&ok, 16);
        if (!ok) {
            qWarning() << "Invalid CAN ID:" << canIdStr;
            return;
        }
    }

    data.time = QDateTime::currentMSecsSinceEpoch();

    // 解析 data（十六进制字符串，无空格）
    if (params.contains("data")) {
        QString hexStr = params["data"].toString().trimmed();
        int length = qMin(hexStr.length() / 2, 8);  // 最多8字节
        data.dlc = static_cast<uint8_t>(length);

        for (int i = 0; i < length; ++i) {
            QString byteStr = hexStr.mid(i * 2, 2);
            bool ok = false;
            data.data[i] = static_cast<uint8_t>(byteStr.toUInt(&ok, 16));
            if (!ok)
                data.data[i] = 0; // 解析失败则填0
        }
    }

    emit sendRequestSig(data);
}

void QCanopenMaster::sendNMT(uint8_t command) {
    emit sendNMTSig(command);
}

void QCanopenMaster::sendError(QString error) {
    emit sendErrorSig(error);
}

void QCanopenMaster::sendCanFrameToUi(CANFrameStr data) {
    QVariantMap item;
    item["canId"] = QString::number(data.canId, 16).toUpper().rightJustified(3, '0');
    item["dlc"] = data.dlc;
    QString dataStr;
    for (int i = 0; i < data.dlc; ++i) {
        dataStr += QString::number(data.data[i], 16).toUpper().rightJustified(2, '0') + " ";
    }
    item["data"] = dataStr.trimmed();
    item["time"] = data.time;
    emit sendCanFrameToUiSig(item);
}
