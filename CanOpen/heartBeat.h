#pragma once

#include <QObject>
#include <QDateTime>
#include <QDebug>

#include "driver.h"
#include "canopenObject.h"

class Heartbeat : public QObject
{
    Q_OBJECT

public:
    static Heartbeat* instance(CANopenStr* canOpenStr = nullptr);


    // 处理心跳帧
    void process();
    void heartBeatHandle();

    CanopenObject canopenObject;

public slots:
    void receiveCanFrame(CANFrameStr data);

private:
    explicit Heartbeat(CANopenStr* canOpenStr, QObject *parent = nullptr);
    ~Heartbeat();

    CANopenStr* canOpenStr = nullptr;

    QVector<CANFrameStr> frameBuffer;   // 缓冲队列
    uint32_t cycleTime = 10;
    bool hasNewCanFrame = false;        // 有新CAN帧标志
    QMutex bufferLock;                  // 互斥锁
};
