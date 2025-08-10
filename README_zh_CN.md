<div align=center>

# Qt Canopen Master
[English](README.md) | 简体中文
</div>

# QCanopenMaster
QCanopenMaster 是基于 Qt 的轻量级 CANopen 主站组件，封装了设备驱动、NMT 管理、心跳监测与 SDO 访问等核心能力，提供简洁一致的 QObject/QML 友好接口，便于快速集成到 Qt 应用中。

- 语言/框架：C++17/Qt 5/6（基于 QtCore）
- 构建系统：CMake
- 适用场景：CANopen 主站控制、设备调试、上位机工具、自动化测试
## 功能特性
- 单例管理：QCanopenMaster::instance() 全局唯一实例
- 设备生命周期：initDevice/openDevice/closeDevice/resetCanDevice
- 主站周期任务：process() 可由外部定时器驱动
- NMT 控制：Start/Stop/Pre-Operational/Reset Node/Reset Communication
- SDO 访问：读（uploadSDO）与写（downloadSDO）
- 原始 CAN 帧发送：sendRequest
- 事件信号：错误上报、CAN 帧上报、SDO 结果上报
- Q_INVOKABLE：方便在 QML/Qt 元对象系统中直接调用

## 目录结构
- QCanopenMaster_global.h：导出宏定义
- qcanopenmaster.h / .cpp：主类与对外 API
- CanOpen/
  - driver.*：底层 CAN 驱动抽象
  - NMT.*：NMT 管理
  - heartBeat.*：心跳监测
  - SDO.*：SDO 访问实现
  - canopenObject.*：对象字典/设备相关抽象
- utils/
  - ByteParser.*：字节序与解析工具

## 依赖与环境
- Qt 5.15+ 或 Qt 6.x（Core 必需）
- CMake 3.16+
- C++17 编译器
- 平台支持：与驱动实现相关（示例参数中包含 SocketCAN，可在 Linux 上使用；Windows 需相应 CAN 驱动支持）

## 接口
### 设备管理
- **`setCurrentNodeId(uint8_t nodeId)`**  
  设置当前工作节点 ID。

- **`initDevice(QVariantMap params)`**  
  初始化设备，示例参数：  
  ```json
  { "canType": "SocketCan", "channel": "0", "nominalBaudrate": "500000", "dataBaudrate": "2000000" }
  ```
- **`openDevice()`**  
打开设备连接。

- **`closeDevice()`**  
关闭设备连接。

- **`resetCanDevice()`**  
重置设备。

### 通信与控制
- **`sendRequest(QVariantMap params)`**  
  发送原始 CAN 帧，示例：  
  ```json
  {
      "canId": 1537,
      "dlc": 4,
      "data": [11, 22, 33, 44, 55, 66, 77, 88],
      "time": 1723267200000
    }
  ```
- **`sendNMT(uint8_t command)`**  
发送 NMT 指令：  
  - `0x01` Start
  - `0x02` Stop
  - `0x80` Pre-Operational
  - `0x81` Reset Node
  - `0x82` Reset Communication

### SDO 访问
- **`uploadSDO(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariant data)`**  
  读取对象字典数据。

- **`downloadSDO(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariant data)`**  
  写入对象字典数据。

- **`resetSDOState()`**  
  重置 SDO 状态机。

### 信号
- **`sendErrorSig(QString error)`**  
  错误信息上报。

- **`sendCanFrameToUiSig(QVariantMap item)`**  
  收到和发送 CAN 帧时上报给 UI。

- **`sendSdoItemSig(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariantMap sdoItem)`**  
  收到 SDO 数据时上报。

# 许可协议

本项目采用 [MIT 许可证](./LICENSE) 进行开源。  
详细内容请查看仓库根目录的 LICENSE 文件。  
使用本项目代码时，请遵守该许可协议。  