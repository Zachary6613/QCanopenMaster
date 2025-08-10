#pragma once

#include "QCanopenMaster_global.h"
#include <QObject>
#include <QTimer>
#include <QThread>

#include "CanOpen/canopenObject.h"
#include "CanOpen/NMT.h"
#include "CanOpen/heartBeat.h"
#include "CanOpen/SDO.h"


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

    Q_INVOKABLE void uploadSDO(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariant data);
    Q_INVOKABLE void downloadSDO(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariant data);
    Q_INVOKABLE void resetSDOState();
signals: // Sig For external use
    void sendErrorSig(QString error);
    void sendCanFrameToUiSig(QVariantMap item);

    void sendSdoItemSig(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariantMap sdoItem);

public slots:
    void sendError(QString error);
    void sendCanFrameToUi(CANFrameStr data);
    void sendSdoItem(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariantMap sdoItem);

signals:
    void initDeviceSig(QVariantMap params);
    void openDeviceSig();
    void closeDeviceSig();
    void resetCanDeviceSig();
    void sendRequestSig(CANFrameStr data);

    void sendNMTSig(uint8_t command);

    void uploadSDOSig(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariant data);
    void downloadSDOSig(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariant data);
    void resetSDOStateSig();

private:
    explicit QCanopenMaster(QObject* parent = nullptr);
    ~QCanopenMaster();

    CANopenStr canopenStr;
    QMap<QString, CanopenObject> canopenObject;
    Driver* driver;
    NMT* nmt;
    Heartbeat* heartBeat;
    SDO* sdo;
};

