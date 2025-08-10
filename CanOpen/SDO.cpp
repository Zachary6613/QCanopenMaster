#include "SDO.h"

SDO* SDO::instance(CANopenStr* canOpenStr) {
    static SDO* s_instance = nullptr;
    if (!s_instance && canOpenStr) {
        s_instance = new SDO(canOpenStr);
    }
    return s_instance;
}

SDO::SDO(CANopenStr* canOpenStr, QObject* parent)
{
    this->canOpenStr = canOpenStr;
}

SDO::~SDO() {

}

void SDO::uploadSDO(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariant data) {
    QMutexLocker locker(&bufferLock);

    UploadSdoItem item = {nodeId, index, subIndex, data};
    this->uploadSdoIndexList.append(item);
    this->hasNewUpload = true;
}

void SDO::downloadSDO(uint16_t nodeId, uint16_t index, uint8_t subIndex, QVariant data) {
    QMutexLocker locker(&bufferLock);

    DownloadSdoItem item = {nodeId, index, subIndex, data};
    this->downloadSdoIndexList.append(item);
    this->hasNewDownload = true;
}

void SDO::receiveCanFrame(CANFrameStr data){
    QMutexLocker locker(&bufferLock);

    if (frameBuffer.size() >= 1000) {
        frameBuffer.remove(0);  // 删除最早的一帧，保证缓冲区不超过1000帧
    }

    frameBuffer.append(data);
    hasNewCanFrame = true;
}

void SDO::process() {
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        sdoHandle();
        errorHandle();
    });
}

void SDO::sdoHandle() {
    QMutexLocker locker(&bufferLock);
    switch (state){
        case SDOState::Idle:{
            if(this->hasNewUpload){
                UploadSdoItem item = uploadSdoIndexList.first();
                this->uploadMap = item.data.toMap();

                // 构造CAN帧
                CANFrameStr frame;
                memset(frame.data, 0, sizeof(frame.data));
                frame.data[0] = 0x40;
                frame.data[1] = item.index & 0xFF;
                frame.data[2] = (item.index >> 8) & 0xFF;
                frame.data[3] = item.subIndex;

                frame.dlc = 8; // 数据长度为8字节
                frame.canId = 0x600 + item.nodeId;
                this->state = SDOState::UploadInit;

                emit sendCanFrame(frame);
            }else if(this->hasNewDownload){
                DownloadSdoItem item = downloadSdoIndexList.first();
                this->downloadMap = item.data.toMap();
                CANFrameStr frame;
                memset(frame.data, 0, sizeof(frame.data));
                QString type = this->downloadMap["type"].toString();
                int32_t size = this->downloadMap["dlc"].toInt();

                if(size > 4){
                    frame.data[0] = 0x21;
                    size = this->downloadMap["pendingValue"].toString().length();
                    memcpy(&frame.data[4], &size, sizeof(uint32_t));
                    this->downloadBytes = size;
                    this->downloadSdoBuffer = this->downloadMap["pendingValue"].toString().toUtf8();
                    this->downloadIndex = item.index;
                    this->downloadSubIndex = item.subIndex;
                    this->state = SDOState::DownloadSegment;
                }else{
                    frame.data[0] = (size == 1 ? 0x2F : size == 2 ? 0x2B : size == 4 ? 0x23 : 0x23);
                    ByteParser::write(type, this->downloadMap["pendingValue"], &frame.data[4]);
                    this->state = SDOState::DownloadInit;
                }

                frame.data[1] = item.index & 0xFF;
                frame.data[2] = (item.index >> 8) & 0xFF;
                frame.data[3] = item.subIndex;

                frame.dlc = 8; // 数据长度为8字节
                frame.canId = 0x600 + item.nodeId;

                if (this->downloadSdoIndexList.isEmpty()) {
                    this->hasNewDownload = false;
                }
                emit sendCanFrame(frame);
            }
            break;
        }
        case SDOState::UploadInit:{
            // 响应从站回复处理
            if(!hasNewCanFrame) return;

            CANFrameStr frame = frameBuffer.takeFirst();

            uint8_t nodeId = frame.canId & 0x7F;
            const uint8_t* data = frame.data;
            uint64_t time = frame.time;
            switch(data[0]){
                case 0x4F:
                case 0x4B:
                case 0x47:
                case 0x43:{
                    uint16_t targetIndex = 0;
                    memcpy(&targetIndex, &data[1], sizeof(uint16_t));
                    uint8_t targetSubIndex = data[3];
                    QVariantMap item = canopenObject.get(nodeId, targetIndex, targetSubIndex);
                    QString typeStr = item["type"].toString();
                    item["oldValue"] = item["currentValue"];
                    item["currentValue"] = ByteParser::parse(typeStr, &data[4]);
                    canopenObject.update(nodeId, targetIndex, targetSubIndex, item);
                    emit sendSdoItem(nodeId, targetIndex, targetSubIndex, item);

                    this->uploadMap.clear();
                    uploadSdoIndexList.removeFirst();
                    if (this->uploadSdoIndexList.isEmpty()) {
                        this->hasNewUpload = false;
                    }
                    this->state = SDOState::Idle;
                    break;
                }
                case 0x41:{
                    memcpy(&this->uploadBytes, &data[4], sizeof(uint32_t));
                    memcpy(&this->uploadIndex, &data[1], sizeof(uint16_t));
                    memcpy(&this->uploadSubIndex, &data[3], sizeof(uint8_t));
                    // 构造CAN帧
                    CANFrameStr frame;
                    memset(frame.data, 0, 8);
                    frame.data[0] = 0x60;
                    this->uploadSendCSFlag = 0x60;
                    frame.dlc = 8; // 数据长度为8字节

                    frame.canId = 0x600 + this->uploadNodeId;
                    emit sendCanFrame(frame);
                    this->state = SDOState::UploadSegment;

                    break;
                }
                case 0x80:{
                    this->state = SDOState::Aborted;
                    emit sendError(tr("SDO upload false"));
                    break;
                }
            }
            break;
        }
        case SDOState::UploadSegment:{
            if(!hasNewCanFrame) return;

            CANFrameStr frame = frameBuffer.takeFirst();

            uint8_t nodeId = frame.canId & 0x7F;
            const uint8_t* data = frame.data;
            uint64_t time = frame.time;
            switch(data[0]){
            // 分段上传命令
                case 0x00:
                case 0x10:{
                    this->uploadSdoBuffer.append(reinterpret_cast<const char*>(&data[1]), 7);
                    CANFrameStr frame;
                    memset(frame.data, 0, 8);
                    if(this->uploadSendCSFlag == 0x60){
                        frame.data[0] = 0x70;
                    }else{
                        frame.data[0] = 0x60;
                    }
                    this->uploadSendCSFlag = frame.data[0];
                    frame.canId = 0x600 + this->uploadNodeId;
                    frame.dlc = 8; // 数据长度为8字节
                    emit sendCanFrame(frame);
                    break;
                }
                case 0x0F:
                case 0x0D:
                case 0x0B:
                case 0x09:
                case 0x07:
                case 0x05:
                case 0x03:
                case 0x1F:
                case 0x1D:
                case 0x1B:
                case 0x19:
                case 0x17:
                case 0x15:
                case 0x13:{
                    this->uploadSdoBuffer.append(reinterpret_cast<const char*>(&data[1]), 7);

                    // uploadSdoBuffer转字符串
                    QByteArray validData = this->uploadSdoBuffer.left(uploadBytes);
                    int nullIndex = validData.indexOf('\0'); // 查找 0x00 的位置

                    if (nullIndex != -1) {
                        validData = validData.left(nullIndex); // 截断 0x00 之后的部分
                    }

                    QString asciiStr = QString::fromLatin1(validData);
                    QVariantMap item = canopenObject.get(uploadNodeId, uploadIndex, uploadSubIndex);
                    item["currentValue"] = asciiStr;
                    canopenObject.update(uploadNodeId, uploadIndex, uploadSubIndex, item);
                    emit sendSdoItem(uploadNodeId, uploadIndex, uploadSubIndex, item);

                    // 恢复状态
                    this->uploadMap.clear();
                    this->uploadBytes = 0;
                    this->uploadSdoBuffer.clear();
                    this->uploadSendCSFlag = 0;
                    this->uploadIndex = 0;
                    this->uploadSubIndex = 0;

                    // 移除已完成读命令
                    uploadSdoIndexList.removeFirst();
                    if (this->uploadSdoIndexList.isEmpty()) {
                        this->hasNewUpload = false;
                    }

                    // 重置SDO状态
                    this->state = SDOState::Idle;

                    break;
                }
                case 0x80:{
                    this->state = SDOState::Aborted;
                    emit sendError(tr("SDO upload false"));
                    break;
                }
            }
            break;
        }
        case SDOState::DownloadInit:{
            if(!hasNewCanFrame) return;

            CANFrameStr frame = frameBuffer.takeFirst();

            uint8_t nodeId = frame.canId & 0x7F;
            const uint8_t* data = frame.data;
            uint64_t time = frame.time;
            if(data[0] != 0x80){
                uint16_t targetIndex = 0;
                memcpy(&targetIndex, &data[1], sizeof(uint16_t));
                uint8_t targetSubIndex = data[3];
                QVariantMap item = canopenObject.get(nodeId, targetIndex, targetSubIndex);
                item["oldValue"] = this->downloadMap["currentValue"];
                item["currentValue"] = this->downloadMap["pendingValue"];
                item["pendingValue"] = "";
                canopenObject.update(nodeId, targetIndex, targetSubIndex, item);
                emit sendSdoItem(nodeId, targetIndex, targetSubIndex, item);

                downloadSdoIndexList.removeFirst();
                this->downloadSdoBuffer.clear();
                if (this->downloadSdoIndexList.isEmpty()) {
                    this->hasNewDownload = false;
                }
                this->state = SDOState::Idle;

            }else{
                this->state = SDOState::Aborted;
                emit sendError(tr("SDO Download False"));
            }
            break;
        }
        case SDOState::DownloadSegment:{
            if(!hasNewCanFrame) return;

            CANFrameStr frame = frameBuffer.takeFirst();

            uint8_t nodeId = frame.canId & 0x7F;
            const uint8_t* data = frame.data;
            uint64_t time = frame.time;
            switch(data[0]){
            case 0x60:
            case 0x20:
            case 0x30:{
                CANFrameStr frame;
                memset(frame.data, 0, sizeof(frame.data));
                frame.dlc = 8; // 数据长度为8字节
                frame.canId = 0x600 + canOpenStr->canOpenSlave.currentNodeId;
                if(this->downloadSendCSFlag == 0x10)
                    this->downloadSendCSFlag = 0x00;
                else
                    this->downloadSendCSFlag = 0x10;

                if(this->downloadSdoBuffer.isEmpty()){
                    QVariantMap item = canopenObject.get(downloadNodeId, downloadIndex, downloadSubIndex);
                    item["oldValue"] = this->downloadMap["currentValue"];
                    item["currentValue"] = this->downloadMap["pendingValue"];
                    item["pendingValue"] = "";
                    canopenObject.update(downloadNodeId, downloadIndex, downloadSubIndex, item);
                    emit sendSdoItem(downloadNodeId, downloadIndex, downloadSubIndex, item);

                    downloadSdoIndexList.removeFirst();
                    this->state = SDOState::Idle;

                    // 写SDO完毕，恢复状态
                    this->downloadBytes = 0;
                    this->downloadSdoBuffer.clear();
                    this->downloadSendCSFlag = 0x10;
                    this->downloadIndex = 0x00;
                    this->downloadSubIndex = 0x00;
                    this->downloadMap.clear();

                }else if(this->downloadSdoBuffer.size() <= 7){
                    frame.data[0] = this->downloadSendCSFlag & (0x0F - 2 * (this->downloadSdoBuffer.size() - 1));
                    memcpy(&frame.data[1], downloadSdoBuffer.constData(), this->downloadSdoBuffer.size());
                    downloadSdoBuffer.remove(0, this->downloadSdoBuffer.size());
                }else{
                    frame.data[0] =this->downloadSendCSFlag;
                    memcpy(&frame.data[1], downloadSdoBuffer.constData(), 7);
                    downloadSdoBuffer.remove(0, 7);
                }
                emit sendCanFrame(frame);
                break;
            }
            case 0x80:
                this->state = SDOState::Aborted;
                emit sendError(tr("SDO Download False"));
                break;

            }
            break;
        }
        case SDOState::Finished:{
            break;
        }
        case SDOState::Aborted:{
            break;
        }
    }

}

void SDO::errorHandle() {
    QMutexLocker locker(&bufferLock);
    if (this->state != this->lastState){
        this->stateEnterTime = QDateTime::currentMSecsSinceEpoch();
        this->lastState = this->state;
    }else{
        switch(this->state){
            case SDOState::DownloadSegment:{
                this->currentEnterTime = QDateTime::currentMSecsSinceEpoch();
                if(this->currentEnterTime - this->stateEnterTime < 100){
                    break;
                }
            }
            case SDOState::DownloadInit:{
                this->downloadBytes = 0;
                this->downloadSdoBuffer.clear();
                this->downloadSendCSFlag = 0x10;
                this->downloadNodeId = 0x00;
                this->downloadIndex = 0x00;
                this->downloadSubIndex = 0x00;
                this->downloadMap.clear();
                this->downloadSdoIndexList.clear();
                this->downloadSdoIndexList.removeFirst();
                if (this->downloadSdoIndexList.isEmpty()) {
                    this->hasNewDownload = false;
                }
                this->frameBuffer.clear();
                this->hasNewCanFrame = false;
                this->state = SDOState::Idle;
                this->lastState = SDOState::Idle;
                this->stateEnterTime = 0;
                emit sendError(tr("SDO CAN Frame Lost!"));
                break;
            }
            case SDOState::UploadSegment:{
                this->currentEnterTime = QDateTime::currentMSecsSinceEpoch();
                if(this->currentEnterTime - this->stateEnterTime < 200){
                    break;
                }
            }
            case SDOState::UploadInit:{
                this->uploadBytes = 0;
                this->uploadSdoBuffer.clear();
                this->uploadSendCSFlag = 0;
                this->uploadNodeId = 0x00;
                this->uploadIndex = 0;
                this->uploadSubIndex = 0;
                this->uploadMap.clear();
                this->uploadSdoIndexList.clear();
                this->uploadSdoIndexList.removeFirst();
                if (this->uploadSdoIndexList.isEmpty()) {
                    this->hasNewUpload = false;
                }
                this->frameBuffer.clear();
                this->hasNewCanFrame = false;
                this->state = SDOState::Idle;
                this->lastState = SDOState::Idle;
                this->stateEnterTime = 0;
                emit sendError(tr("SDO CAN Frame Lost!"));
                break;
            }
        }
    }
}

void SDO::resetSDOState() {
    QMutexLocker locker(&bufferLock);

}
