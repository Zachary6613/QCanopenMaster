#include "NMT.h"

NMT* NMT::instance(CANopenStr* canOpenStr)
{
    static NMT* s_instance = nullptr;
    if (!s_instance && canOpenStr) {
        s_instance = new NMT(canOpenStr);
    }
    return s_instance;
}

NMT::NMT(CANopenStr* canOpenStr, QObject *parent)
{
    // 初始化操作
    this->canOpenStr = canOpenStr;
}

NMT::~NMT()
{
    // 资源清理
}

void NMT::process()
{
    // 主循环或定时处理逻辑
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        // 事件循环
        // ......
    });
    timer->start(cycleTime);
}

void NMT::sendNMT(uint8_t command)
{
    // 发送NMT命令逻辑
    // 构造NMT帧
    CANFrameStr frame;
    frame.canId = 0x000;                                            // NMT命令帧CAN-ID固定为0x000
    frame.dlc = 2;                                                  // 数据长度为2字节
    frame.data[0] = command;                                        // 命令字节
    frame.data[1] = canOpenStr->canOpenSlave.currentNodeId;         // 节点ID
    frame.time = QDateTime::currentMSecsSinceEpoch();               // 当前时间戳（毫秒）

    // 发送信号
    emit sendCanFrame(frame);

}
