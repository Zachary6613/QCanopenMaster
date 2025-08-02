#pragma once
#include <QObject>
#include <QDateTime>
#include <QDebug>
#include "driver.h"

class NMT : public QObject {
    Q_OBJECT
public:
    static NMT* instance(CANopenStr* canOpenStr = nullptr);
    void process();


public slots:
    void sendNMT(uint8_t command);

signals:
    void sendCanFrame(CANFrameStr frame);

private:

    explicit NMT(CANopenStr* canOpenStr, QObject *parent = nullptr);
    ~NMT();

    CANopenStr* canOpenStr = nullptr;
    uint32_t cycleTime = 10;            // NMT处理周期，单位毫秒
    QVector<CANFrameStr> frameBuffer;   // 缓冲队列
    bool hasNewCanFrame = false;        // 有新CAN帧标志
    QMutex bufferLock;                  // 互斥锁
};
