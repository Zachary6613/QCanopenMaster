#include "heartBeat.h"

Heartbeat* Heartbeat::instance(CANopenStr* canOpenStr) {
    static Heartbeat* s_instance = nullptr;
    if (!s_instance && canOpenStr) {
        s_instance = new Heartbeat(canOpenStr);
    }
    return s_instance;
}

Heartbeat::Heartbeat(CANopenStr* canOpenStr, QObject *parent){

    this->canOpenStr = canOpenStr;
}

Heartbeat::~Heartbeat() {
    // 析构处理，例如停止线程、释放资源
}


void Heartbeat::process()
{
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        heartBeatHandle();
    });
    timer->start(cycleTime); // 每 10ms 扫一次
}

void Heartbeat::receiveCanFrame(CANFrameStr data){
    QMutexLocker locker(&bufferLock);

    if (frameBuffer.size() >= 1000) {
        frameBuffer.remove(0);  // 删除最早的一帧，保证缓冲区不超过1000帧
    }

    frameBuffer.append(data);
    hasNewCanFrame = true;
}

void Heartbeat::heartBeatHandle()
{
    QMutexLocker locker(&bufferLock);

    // 遍历离线设备

    // 获取当前时间（毫秒）
    uint64_t currentTime = QDateTime::currentMSecsSinceEpoch();

    // 超时时间（单位：毫秒），例如：如果超过3000ms未收到心跳就认为离线
    const uint64_t offlineTimeoutMs  = 3000;
    const uint64_t deleteTimeoutMs = 60000;      // 超过1分钟删除

    // 遍历所有设备，判断是否离线
    auto it = canOpenStr->canOpenSlave.slaveList.begin();
    while (it != canOpenStr->canOpenSlave.slaveList.end())
    {
        SlaveInfo& slave = it.value();
        uint64_t diff = currentTime - slave.lastFrameTimestamp;

        if (diff > offlineTimeoutMs){
            slave.slaveNMTState = NMTState::Unknown;  // 标记为离线
        }

        if (diff > deleteTimeoutMs){
            it = canOpenStr->canOpenSlave.slaveList.erase(it); // 删除超时从站并移动迭代器
            canOpenStr->canOpenSlave.slaveCount--;
        }
        else{
            ++it;  // 正常遍历
        }
    }

    if(!hasNewCanFrame) return;

    // 处理总线CAN帧，扫描设备
    for (const CANFrameStr& frame : frameBuffer)
    {
        // 检查是否是心跳帧（CAN ID 范围为 0x701 ~ 0x77F）
        if ((frame.canId & 0x780) == 0x700)
        {
            uint8_t nodeId = frame.canId & 0x7F;
            uint8_t state = frame.data[0];
            uint64_t time = frame.time;

            // 查找是否已存在该 nodeId 的从站
            auto it = this->canOpenStr->canOpenSlave.slaveList.find(QString::number(nodeId));
            if (it != this->canOpenStr->canOpenSlave.slaveList.end()) {
                it->slaveNMTState = static_cast<NMTState>(state);
                it->lastFrameTimestamp = time;
            }else{
                SlaveInfo newSlave;
                newSlave.nodeId = nodeId;
                newSlave.slaveNMTState = static_cast<NMTState>(state);
                newSlave.model = QString("Unknown");  // 可选：占位
                newSlave.name = QString("Node %1").arg(nodeId);  // 可选：默认名称
                newSlave.lastFrameTimestamp = time;
                this->canOpenStr->canOpenSlave.slaveList.insert(QString::number(nodeId), newSlave);
                this->canOpenStr->canOpenSlave.slaveCount++;
            }

        }
    }
    // 清空缓冲区
    frameBuffer.clear();
    hasNewCanFrame = false;
}
