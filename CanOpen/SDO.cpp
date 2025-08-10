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
            break;
        }
        case SDOState::UploadInit:{
            break;
        }
        case SDOState::UploadSegment:{
            break;
        }
        case SDOState::DownloadInit:{
            break;
        }
        case SDOState::DownloadSegment:{
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
                // this->downloadBytes = 0;
                // this->downloadSdoBuffer.clear();
                // this->downloadSendCSFlag = 0x10;
                // this->downloadIndex = 0x00;
                // this->downloadSubIndex = 0x00;
                // this->downloadMap.clear();
                // this->downloadSdoIndexList.clear();
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
                // this->uploadBytes = 0;
                // this->uploadSdoBuffer.clear();
                // this->uploadSendCSFlag = 0;
                // this->uploadIndex = 0;
                // this->uploadSubIndex = 0;
                // this->uploadMap.clear();
                // this->uploadSdoIndexList.clear();
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
