#pragma once

#include "QCanopenMaster_global.h"
#include <QObject>
#include <QTimer>
#include <QThread>

#include "CanOpen/canopenObject.h"
#include "CanOpen/NMT.h"


class QCANOPENMASTER_EXPORT QCanopenMaster : public QObject{
    Q_OBJECT
public:
    static QCanopenMaster* instance();
    void init();
    void process();  // 主站周期任务

public: // API For external use
    Q_INVOKABLE void setCurrentNodeId(uint8_t nodeId);

    Q_INVOKABLE void initDevice(QVariantMap params); // canType: "SocketCan", channel: "0", nominalBaudrate: "500000", dataBaudrate: "2000000"
    Q_INVOKABLE void openDevice();
    Q_INVOKABLE void closeDevice();
    Q_INVOKABLE void resetCanDevice();
    Q_INVOKABLE void sendRequest(QVariantMap params); //canId: "601", data: "01020304"

    Q_INVOKABLE void sendNMT(uint8_t command); // 0x01: Start, 0x02: Stop, 0x80: Pre-Operational, 0x81: Reset Node, 0x82: Reset Communication
signals: // Sig For external use
    void sendErrorSig(QString error);
    void sendCanFrameToUiSig(QVariantMap item); //

public slots:
    void sendError(QString error);
    void sendCanFrameToUi(CANFrameStr data);

signals:
    void initDeviceSig(QVariantMap params);
    void openDeviceSig();
    void closeDeviceSig();
    void resetCanDeviceSig();
    void sendRequestSig(CANFrameStr data);

    void sendNMTSig(uint8_t command);

private:
    explicit QCanopenMaster(QObject* parent = nullptr);
    ~QCanopenMaster();

    CANopenStr canopenStr;
    QMap<QString, CanopenObject> canopenObject;
    Driver* driver;
    NMT* nmt;
};

