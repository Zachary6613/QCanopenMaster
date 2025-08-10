#pragma once

#include <QObject>
#include <QDateTime>
#include <QDebug>

#include "driver.h"
#include "canopenObject.h"

class SDO : public QObject
{
    Q_OBJECT

public:
    static SDO* instance(CANopenStr* canOpenStr = nullptr);
    void process();
    void sdoHandle();
    void errorHandle();

    CanopenObject canopenObject;

public slots:
    void receiveCanFrame(CANFrameStr data);
    void uploadSDO(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariant data);
    void downloadSDO(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariant data);
    void resetSDOState();

signals:
    void sendCanFrame(CANFrameStr frame);
    void sendSdoList(QVariantMap sdoList);
    void sendSdoItem(QVariantMap sdoItem);
    void sendError(QString error);

private:
    explicit SDO(CANopenStr* canOpenStr, QObject *parent = nullptr);
    ~SDO();

    enum class SDOState {
        Idle,               // 空闲状态
        UploadInit,         // 发起读请求中
        UploadSegment,      // 分段读取中
        DownloadInit,       // 发起写请求中
        DownloadSegment,    // 分段写入中
        Finished,           // 成功完成
        Aborted             // 异常中止
    };
    SDOState state = SDOState::Idle;

    // 故障、未回复等待时间
    qint64 stateEnterTime = 0;           // 状态进入的时间戳
    qint64 currentEnterTime = 0;
    SDOState lastState = SDOState::Idle; // 上一轮的状态
    const qint64 TIMEOUT = 500;          // 停留超时阈值，单位：毫秒

    // 上传标志位
    bool hasNewUpload = false;
    struct UploadSdoItem {
        uint16_t nodeId;
        uint16_t index;
        uint8_t subIndex;
        QVariant data;
    };
    QVector<UploadSdoItem> uploadSdoIndexList;

    // 下载标志位
    bool hasNewDownload = false;
    struct DownloadSdoItem {
        uint16_t nodeId;
        uint16_t index;
        uint8_t subIndex;
        QVariant data;
    };
    QVector<DownloadSdoItem> downloadSdoIndexList;

    CANopenStr* canOpenStr = nullptr;

    QVector<CANFrameStr> frameBuffer;   // 缓冲队列
    uint32_t cycleTime = 10;
    bool hasNewCanFrame = false;        // 有新CAN帧标志
    QMutex bufferLock;                  // 互斥锁
};
