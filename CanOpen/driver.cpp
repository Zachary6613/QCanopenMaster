#include "driver.h"

Driver* Driver::instance(CANopenStr* canOpenStr) {
    static Driver* s_instance = nullptr;
    if (!s_instance && canOpenStr) {
        s_instance = new Driver(canOpenStr);
    }
    return s_instance;
}

Driver::Driver(CANopenStr* canOpenStr, QObject* parent){

    this->canOpenStr = canOpenStr;
}

Driver::~Driver() {
    // 析构处理，例如停止线程、释放资源
}

void Driver::openDevice() {

}

void Driver::closeDevice() {

}

void Driver::resetCanDevice() {
    // 重置 CAN 设备逻辑
    

}

void Driver::sendRequest(CANFrameStr data) {
    if (data.dlc > 8) {
        emit sendError("发送数据长度不能大于8字节");
        return;
    }

    if (!sendCanFrame(data) ){
        emit sendError("CAN 发送失败");
    }
}

void Driver::process() {


}

void Driver::receiveCanFrame(uint32_t len) {


}

bool Driver::sendCanFrame(CANFrameStr data) {
    return 0;
}
