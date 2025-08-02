#pragma once

#include "QCanopenMaster_global.h"
#include <QObject>
#include <QTimer>
#include <QThread>

#include "CanOpen/NMT.h"
#include "CanOpen/canopenObject.h"

class QCANOPENMASTER_EXPORT QCanopenMaster : public QObject{
    Q_OBJECT
public:
    static QCanopenMaster* instance();
    void init();
    void process();  // 主站周期任务

    Q_INVOKABLE void setCurrentNodeId(uint8_t nodeId);

    Q_INVOKABLE void initDevice(QVariantMap params);
    Q_INVOKABLE void openDevice();
    Q_INVOKABLE void closeDevice();
    Q_INVOKABLE void resetCanDevice();
    Q_INVOKABLE void queryDeviceState();
    Q_INVOKABLE void sendRequest(QVariantMap params);

    Q_INVOKABLE void sendNMT(uint8_t command);

private slots:
    void initDeviceSig();
    void openDeviceSig();
    void closeDeviceSig();
    void resetCanDeviceSig();
    void queryDeviceSig();
    void sendRequestSig(CANFrameStr data);

private:
    explicit QCanopenMaster(QObject* parent = nullptr);
    ~QCanopenMaster();

    CANopenStr canopenStr;
    QMap<QString, CanopenObject> canopenObject;
    Driver* driver;
    NMT* nmt;
};

