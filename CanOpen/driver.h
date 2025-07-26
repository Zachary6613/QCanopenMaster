#pragma once

#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QByteArray>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include "canopenObject.h"
#include "parameters.h"

class Driver : public QObject {
    Q_OBJECT

public:

    static Driver* instance(CANopenStr* canOpenStr = nullptr);
    explicit Driver(CANopenStr* canOpenStr, QObject* parent = nullptr);
    ~Driver();



public slots:

    void openDevice();
    void closeDevice();
    void resetCanDevice();
    void process();
    void sendRequest(CANFrameStr data);


signals:
    void NMTMsg(CANFrameStr data);
    void SDOMsg(CANFrameStr data);
    void HeartBeat(CANFrameStr data);
    void PDOMsg(CANFrameStr data);
    void sendError(QString error);
    void sendCanFrameToQml(QVariantMap item);

private:
    void receiveCanFrame(uint32_t len);
    bool sendCanFrame(CANFrameStr data);

private:

    CANopenStr* canOpenStr = nullptr;

};
