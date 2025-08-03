#include "driver.h"

Driver* Driver::instance(CANopenStr* canOpenStr) {
    static Driver* s_instance = nullptr;
    if (!s_instance && canOpenStr) {
        s_instance = new Driver(canOpenStr);
    }
    return s_instance;
}

Driver::Driver(CANopenStr* canOpenStr, QObject* parent){

    this->canOpenStr = canOpenStr;
}

Driver::~Driver() {
    // 析构处理，例如停止线程、释放资源
}

void Driver::initDevice(QVariantMap params) {
    qDebug() << "initDevice called with parameters:";

    if (params.contains("canType")) {
        qDebug() << "  canType:" << params["canType"].toString();
    } else {
        qWarning() << "  Missing parameter: canType";
    }

    if (params.contains("channel")) {
        qDebug() << "  channel:" << params["channel"].toString();
    } else {
        qWarning() << "  Missing parameter: channel";
    }

    if (params.contains("nominalBaudrate")) {
        qDebug() << "  nominalBaudrate:" << params["nominalBaudrate"].toString();
    } else {
        qWarning() << "  Missing parameter: nominalBaudrate";
    }

    if (params.contains("dataBaudrate")) {
        qDebug() << "  dataBaudrate:" << params["dataBaudrate"].toString();
    } else {
        qWarning() << "  Missing parameter: dataBaudrate";
    }
}

void Driver::openDevice() {
    qDebug() << "open device";
}

void Driver::closeDevice() {
    qDebug() << "close device";
}

void Driver::resetCanDevice() {

    qDebug() << "reset can device";

}

void Driver::sendRequest(CANFrameStr data) {
    if (data.dlc > 8) {
        emit sendError("发送数据长度不能大于8字节");
        return;
    }

    if (!sendCanFrame(data) ){
        emit sendError("CAN 发送失败");
    }
}

void Driver::process() {
QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        // receiveCanFrame
    });
}

void Driver::receiveCanFrame(uint32_t len) {


}

bool Driver::sendCanFrame(CANFrameStr data) {
    qDebug() << "sendCanFrame called:";
    qDebug() << "  canId:" << QString("0x%1").arg(data.canId, 0, 16);
    qDebug() << "  dlc:" << data.dlc;
    qDebug() << "  data:";

    QString hexData;
    for (int i = 0; i < data.dlc && i < 8; ++i) {
        hexData += QString("%1 ").arg(data.data[i], 2, 16, QLatin1Char('0')).toUpper();
    }
    qDebug() << "   " << hexData.trimmed();

    qDebug() << "  time:" << QDateTime::fromMSecsSinceEpoch(data.time).toString(Qt::ISODate);
    return true;
}
