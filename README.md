<div align=center>

# Qt Canopen Master
English | [简体中文](README_zh_CN.md)
</div>
# QCanopenMaster

QCanopenMaster is a lightweight CANopen master component based on Qt. It encapsulates core functionalities such as device drivers, NMT management, heartbeat monitoring, and SDO access. It provides a simple and consistent QObject/QML-friendly interface for easy and fast integration into Qt applications.

- Language/Framework: C++17 / Qt 5/6 (based on QtCore)  
- Build System: CMake  
- Suitable Scenarios: CANopen master control, device debugging, upper computer tools, automated testing  

## Features

- Singleton management: QCanopenMaster::instance() global unique instance  
- Device lifecycle: initDevice / openDevice / closeDevice / resetCanDevice  
- Master periodic task: process() can be driven by external timer  
- NMT control: Start / Stop / Pre-Operational / Reset Node / Reset Communication  
- SDO access: read (uploadSDO) and write (downloadSDO)  
- Raw CAN frame sending: sendRequest  
- Event signals: error reporting, CAN frame reporting, SDO result reporting  
- Q_INVOKABLE: easy to call directly from QML/Qt meta-object system  

## Directory Structure

- QCanopenMaster_global.h: export macro definitions  
- qcanopenmaster.h / .cpp: main class and external API  
- CanOpen/  
  - driver.*: underlying CAN driver abstraction  
  - NMT.*: NMT management  
  - heartBeat.*: heartbeat monitoring  
  - SDO.*: SDO access implementation  
  - canopenObject.*: object dictionary/device abstraction  
- utils/  
  - ByteParser.*: byte order and parsing utilities  

## Dependencies and Environment

- Qt 5.15+ or Qt 6.x (QtCore required)  
- CMake 3.16+  
- C++17 compiler  
- Platform support depends on driver implementation (example parameters include SocketCAN, usable on Linux; Windows requires corresponding CAN driver support)  

## API

### Device Management

- **`setCurrentNodeId(uint8_t nodeId)`**  
  Set the current working node ID.

- **`initDevice(QVariantMap params)`**  
  Initialize the device. Example parameters:  
  ```json
  { "canType": "SocketCan", "channel": "0", "nominalBaudrate": "500000", "dataBaudrate": "2000000" }
  ```
- **`openDevice()`**  
Open the device connection.

- **`closeDevice()`**  
Close the device connection.

- **`resetCanDevice()`**  
Reset the device.

### Communication and Control
- **`sendRequest(QVariantMap params)`**  
  Send original CAN frame, example:  
  ```json
  {
      "canId": 1537,
      "dlc": 4,
      "data": [11, 22, 33, 44, 55, 66, 77, 88],
      "time": 1723267200000
    }
  ```
- **`sendNMT(uint8_t command)`**  
Send NMT instruction:  
  - `0x01` Start
  - `0x02` Stop
  - `0x80` Pre-Operational
  - `0x81` Reset Node
  - `0x82` Reset Communication

### SDO Visit
- **`uploadSDO(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariant data)`**  
  Read object dictionary data.

- **`downloadSDO(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariant data)`**  
  Write object dictionary data.

- **`resetSDOState()`**  
  Reset the SDO state machine.

### Signal
- **`sendErrorSig(QString error)`**  
  Error message reporting.

- **`sendCanFrameToUiSig(QVariantMap item)`**  
  Report to the UI when CAN frames are received and sent.

- **`sendSdoItemSig(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariantMap sdoItem)`**  
  Report when SDO data is received.

# 许可协议

This project is open-sourced under the [MIT License](./LICENSE).  
Please refer to the LICENSE file in the repository root for details.  
When using this project’s code, please comply with the license terms.  