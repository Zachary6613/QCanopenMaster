#include <cstdint>
#include <QString>
#include <QMap>

// CAN帧结构体定义
struct CANFrameStr {
    uint32_t canId = 0;                                     // CAN标识符
    uint8_t dlc = 0;                                        // 数据长度（0-8）
    uint8_t data[8] = {0};                                  // 数据内容
    uint64_t time;                                          // 时间戳（可换成uint64_t毫秒等）
};

// CAN设备类型
enum class CanDeviceType {
    SocketCan,
    ZLG_USBCANFD_200U
};

// CAN 通道结构体
struct CanChannelStr {
    CanDeviceType canType = CanDeviceType::SocketCan;
    void* channelPtr = nullptr;                              // 通道指针（可指向驱动或句柄）
    uint32_t nominalBaudrate = 1000000;                      // 仲裁域波特率（如 500000 bps）
    uint32_t dataBaudrate = 1000000;                         // 数据域波特率（如 2Mbps，CAN FD）
    uint32_t busUsage = 0;                                   // 总线占用率 *100
    uint32_t frameRate = 0;                                  // 帧率  帧/s *100
};

// 从站状态
enum class NMTState : uint8_t {
    Initializing     = 0x00,  // 设备刚启动
    Stopped          = 0x04,  // Stopped 模式
    Operational      = 0x05,  // 正常工作模式
    PreOperational   = 0x7F,  // 预操作模式
    Unknown          = 0xFF   // 未知状态或错误状态
};

// 单个从站的描述信息
struct SlaveInfo {
    uint8_t nodeId = 0;                                     // 从站 Node ID（1~127）
    QString model;                                          // 型号，如 "Drive A100"
    QString name;                                           // 可选：从站名称或别名
    NMTState slaveNMTState = NMTState::Unknown;
    uint64_t lastFrameTimestamp = 0;
    uint8_t ctrlMode = 0;                                   // 控制模式
    // 可以继续添加如固件版本、状态等信息
};

// CANopen 从站描述
struct CanOpenSlaveStr {
    uint8_t currentNodeId = 0;                              // 节点 ID（1~127）
    uint8_t slaveCount = 0;
    QMap<QString, SlaveInfo> slaveList;                     // 所有从站信息列表
};

// CANopen 描述
struct CANopenStr
{
    CanOpenSlaveStr canOpenSlave;
    CanChannelStr CanChanne;
};
