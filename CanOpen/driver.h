#pragma once

#include <QThread>
#include <QMutex>
#include <QString>
#include <QByteArray>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include "parameters.h"

class Driver : public QObject {
    Q_OBJECT

public:

    static Driver* instance(CANopenStr* canOpenStr = nullptr);
    void process();

public slots:

    void initDevice(QVariantMap params);
    void openDevice();
    void closeDevice();
    void resetCanDevice();
    void sendRequest(CANFrameStr data);


signals:
    void NMTMsg(CANFrameStr data);
    void SDOMsg(CANFrameStr data);
    void HeartBeatMsg(CANFrameStr data);
    void PDOMsg(CANFrameStr data);
    void sendError(QString error);
    void sendCanFrameToUi(CANFrameStr data);

private:
    explicit Driver(CANopenStr* canOpenStr, QObject* parent = nullptr);
    ~Driver();


    void receiveCanFrame(uint32_t len);
    bool sendCanFrame(CANFrameStr data);

private:

    CANopenStr* canOpenStr = nullptr;

};
