#include "canopenObject.h"

CanopenObject::CanopenObject(QObject *parent) : QObject(parent) {

}

QString CanopenObject::keyString(uint16_t index, uint8_t subIndex) const {
    return QString("%1_%2")
    .arg(index, 4, 16, QChar('0'))
    .arg(subIndex, 2, 16, QChar('0'))
    .toUpper();
}

void CanopenObject::insert(uint16_t nodeId, uint16_t index, uint8_t subIndex, const QVariantMap &data) {
    QString key = keyString(index, subIndex);
    QString nodeKey = QString::number(nodeId);
    {
        QWriteLocker locker(&m_lock);
        if (m_map.contains(nodeKey)) {
            m_map[nodeKey].insert(key, data);
        }
    }
    emit ItemChanged(nodeKey, key, data);
}

void CanopenObject::update(uint16_t nodeId, uint16_t index, uint8_t subIndex, const QVariantMap &data) {
    QString key = keyString(index, subIndex);
    QString nodeKey = QString::number(nodeId);
    {
        QWriteLocker locker(&m_lock);
        if (m_map.contains(nodeKey)) {
            if (m_map[nodeKey].contains(key)) {
                m_map[nodeKey][key] = data;
            }
        }
    }
    emit ItemChanged(nodeKey, key, data);
}

// 这个函数是外部同步用，不发信号，避免死循环
void CanopenObject::syncItem(const QString &nodeId, const QString &key, const QVariantMap &item) {
    QWriteLocker locker(&m_lock);
    if (m_map.contains(nodeId)) {
        if (m_map[nodeId].contains(key)) {
            m_map[nodeId][key] = item;
        }
    }
    // 不发信号，避免循环
}

void CanopenObject::syncList(const QString &nodeId, const QVariantMap &item) {
    QWriteLocker locker(&m_lock);
    if(nodeId == "0") {
        m_map.clear();  // 清空所有节点
    } else {
        m_map.insert(nodeId, item);
    }
    // 不发信号，避免循环
}

QVariantMap CanopenObject::get(uint16_t nodeId, uint16_t index, uint8_t subIndex) const {
    QReadLocker locker(&m_lock);
    QVariantMap list = m_map.value(QString::number(nodeId));
    QVariant item = list.value(keyString(index, subIndex));
    return item.toMap();  // 转换成 QVariantMap 返回
}

QVariantMap CanopenObject::getAll(uint16_t nodeId) const {
    QReadLocker locker(&m_lock);
    return m_map.value(QString::number(nodeId));
}

void CanopenObject::clear() {
    {
        QWriteLocker locker(&m_lock);
        m_map.clear();
    }
    emit ListChanged("0", QVariantMap{});
}

void CanopenObject::addNode(QString nodeId){
    QVariantMap item;
    item.clear();

    // 1000H-CIA301字典组
    item.insert("1000_00", QVariantMap{{"index", 0x1000}, {"subIndex", 0x00}, {"name", tr("Device type")}, {"access", "RO"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1005_00", QVariantMap{{"index", 0x1005}, {"subIndex", 0x00}, {"name", tr("COB-ID SYNC message")}, {"access", "RO"}, {"defaultValue", "128"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1006_00", QVariantMap{{"index", 0x1006}, {"subIndex", 0x00}, {"name", tr("communication cycle period")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1008_00", QVariantMap{{"index", 0x1008}, {"subIndex", 0x00}, {"name", tr("manufacturer device name")}, {"access", "RO"}, {"defaultValue", "Sanhua-RJM"}, {"type", "string"}, {"dlc", 0}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1010_00", QVariantMap{{"index", 0x1010}, {"subIndex", 0x00}, {"name", tr("store parameters number of subindex")}, {"access", "RO"}, {"defaultValue", "1"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1010_01", QVariantMap{{"index", 0x1010}, {"subIndex", 0x01}, {"name", tr("store all parameters")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1011_00", QVariantMap{{"index", 0x1011}, {"subIndex", 0x00}, {"name", tr("restore default parameters number of subindex")}, {"access", "RO"}, {"defaultValue", "1"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1011_01", QVariantMap{{"index", 0x1011}, {"subIndex", 0x01}, {"name", tr("Restore all parameters")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("100A_00", QVariantMap{{"index", 0x100A}, {"subIndex", 0x00}, {"name", tr("Manufacturer software version")}, {"access", "RO"}, {"defaultValue", ""}, {"type", "string"}, {"dlc", 0}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1014_00", QVariantMap{{"index", 0x1014}, {"subIndex", 0x00}, {"name", tr("COB ID emergency")}, {"access", "RO"}, {"defaultValue", "129"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1015_00", QVariantMap{{"index", 0x1015}, {"subIndex", 0x00}, {"name", tr("inhibit time EMCY")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", "0.1ms"}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1016_00", QVariantMap{{"index", 0x1016}, {"subIndex", 0x00}, {"name", tr("consumer heartbeat time number of subindex")}, {"access", "RO"}, {"defaultValue", "5"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1016_01", QVariantMap{{"index", 0x1016}, {"subIndex", 0x01}, {"name", tr("consumer heartbeat time 1")}, {"access", "RW"}, {"defaultValue", "1"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", "ms"}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1017_00", QVariantMap{{"index", 0x1017}, {"subIndex", 0x00}, {"name", tr("producer heartbeat time")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", "ms"}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1018_00", QVariantMap{{"index", 0x1018}, {"subIndex", 0x00}, {"name", tr("identity object number of subindex")}, {"access", "RO"}, {"defaultValue", "4"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1018_01", QVariantMap{{"index", 0x1018}, {"subIndex", 0x01}, {"name", tr("vendor ID")}, {"access", "RO"}, {"defaultValue", "86"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1018_02", QVariantMap{{"index", 0x1018}, {"subIndex", 0x02}, {"name", tr("product code")}, {"access", "RO"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1018_03", QVariantMap{{"index", 0x1018}, {"subIndex", 0x03}, {"name", tr("revision number")}, {"access", "RO"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1018_04", QVariantMap{{"index", 0x1018}, {"subIndex", 0x04}, {"name", tr("serial number")}, {"access", "RO"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1200_00", QVariantMap{{"index", 0x1200}, {"subIndex", 0x00}, {"name", tr("SDO server parameter number of subindex")}, {"access", "RO"}, {"defaultValue", "2"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1200_01", QVariantMap{{"index", 0x1200}, {"subIndex", 0x01}, {"name", tr("client to server COBID")}, {"access", "RO"}, {"defaultValue", "1537"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1200_02", QVariantMap{{"index", 0x1200}, {"subIndex", 0x02}, {"name", tr("server to client COBID")}, {"access", "RO"}, {"defaultValue", "1409"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1400_00", QVariantMap{{"index", 0x1400}, {"subIndex", 0x00}, {"name", tr("RPDO1 communication parameter number of subindex")}, {"access", "RO"}, {"defaultValue", "5"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1400_01", QVariantMap{{"index", 0x1400}, {"subIndex", 0x01}, {"name", tr("COBID")}, {"access", "RW"}, {"defaultValue", "513"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1400_02", QVariantMap{{"index", 0x1400}, {"subIndex", 0x02}, {"name", tr("transmission type")}, {"access", "RO"}, {"defaultValue", "255"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1400_03", QVariantMap{{"index", 0x1400}, {"subIndex", 0x03}, {"name", tr("inhibit timer")}, {"access", "RO"}, {"defaultValue", "255"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1400_05", QVariantMap{{"index", 0x1400}, {"subIndex", 0x05}, {"name", tr("event timer")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1401_00", QVariantMap{{"index", 0x1401}, {"subIndex", 0x00}, {"name", tr("RPDO2 communication parameter number of subindex")}, {"access", "RO"}, {"defaultValue", "5"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1401_01", QVariantMap{{"index", 0x1401}, {"subIndex", 0x01}, {"name", tr("COBID")}, {"access", "RW"}, {"defaultValue", "769"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1401_02", QVariantMap{{"index", 0x1401}, {"subIndex", 0x02}, {"name", tr("transmission type")}, {"access", "RO"}, {"defaultValue", "255"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1401_03", QVariantMap{{"index", 0x1401}, {"subIndex", 0x03}, {"name", tr("inhibit timer")}, {"access", "RO"}, {"defaultValue", "255"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1401_05", QVariantMap{{"index", 0x1401}, {"subIndex", 0x05}, {"name", tr("event timer")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1402_00", QVariantMap{{"index", 0x1402}, {"subIndex", 0x00}, {"name", tr("RPDO3 communication parameter number of subindex")}, {"access", "RO"}, {"defaultValue", "5"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1402_01", QVariantMap{{"index", 0x1402}, {"subIndex", 0x01}, {"name", tr("COBID")}, {"access", "RW"}, {"defaultValue", "1025"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1402_02", QVariantMap{{"index", 0x1402}, {"subIndex", 0x02}, {"name", tr("transmission type")}, {"access", "RO"}, {"defaultValue", "255"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1402_03", QVariantMap{{"index", 0x1402}, {"subIndex", 0x03}, {"name", tr("inhibit timer")}, {"access", "RO"}, {"defaultValue", "255"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1402_05", QVariantMap{{"index", 0x1402}, {"subIndex", 0x05}, {"name", tr("event timer")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1403_00", QVariantMap{{"index", 0x1403}, {"subIndex", 0x00}, {"name", tr("RPDO4 communication parameter number of subindex")}, {"access", "RO"}, {"defaultValue", "5"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1403_01", QVariantMap{{"index", 0x1403}, {"subIndex", 0x01}, {"name", tr("COBID")}, {"access", "RW"}, {"defaultValue", "1281"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1403_02", QVariantMap{{"index", 0x1403}, {"subIndex", 0x02}, {"name", tr("transmission type")}, {"access", "RO"}, {"defaultValue", "255"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1403_03", QVariantMap{{"index", 0x1403}, {"subIndex", 0x03}, {"name", tr("inhibit timer")}, {"access", "RO"}, {"defaultValue", "255"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1403_05", QVariantMap{{"index", 0x1403}, {"subIndex", 0x05}, {"name", tr("event timer")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1600_00", QVariantMap{{"index", 0x1600}, {"subIndex", 0x00}, {"name", tr("RPDO1 mapping parameter number of subindex")}, {"access", "RW"}, {"defaultValue", "7"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1600_01", QVariantMap{{"index", 0x1600}, {"subIndex", 0x01}, {"name", tr("mapped objects 1")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1600_02", QVariantMap{{"index", 0x1600}, {"subIndex", 0x02}, {"name", tr("mapped objects 2")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1600_03", QVariantMap{{"index", 0x1600}, {"subIndex", 0x03}, {"name", tr("mapped objects 3")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1600_04", QVariantMap{{"index", 0x1600}, {"subIndex", 0x04}, {"name", tr("mapped objects 4")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1600_05", QVariantMap{{"index", 0x1600}, {"subIndex", 0x05}, {"name", tr("mapped objects 5")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1600_06", QVariantMap{{"index", 0x1600}, {"subIndex", 0x06}, {"name", tr("mapped objects 6")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1600_07", QVariantMap{{"index", 0x1600}, {"subIndex", 0x07}, {"name", tr("mapped objects 7")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1600_08", QVariantMap{{"index", 0x1600}, {"subIndex", 0x08}, {"name", tr("mapped objects 8")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});

    item.insert("1601_00", QVariantMap{{"index", 0x1601}, {"subIndex", 0x00}, {"name", tr("RPDO2 mapping parameter number of subindex")}, {"access", "RW"}, {"defaultValue", "7"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1601_01", QVariantMap{{"index", 0x1601}, {"subIndex", 0x01}, {"name", tr("mapped objects 1")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1601_02", QVariantMap{{"index", 0x1601}, {"subIndex", 0x02}, {"name", tr("mapped objects 2")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1601_03", QVariantMap{{"index", 0x1601}, {"subIndex", 0x03}, {"name", tr("mapped objects 3")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1601_04", QVariantMap{{"index", 0x1601}, {"subIndex", 0x04}, {"name", tr("mapped objects 4")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1601_05", QVariantMap{{"index", 0x1601}, {"subIndex", 0x05}, {"name", tr("mapped objects 5")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1601_06", QVariantMap{{"index", 0x1601}, {"subIndex", 0x06}, {"name", tr("mapped objects 6")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1601_07", QVariantMap{{"index", 0x1601}, {"subIndex", 0x07}, {"name", tr("mapped objects 7")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1601_08", QVariantMap{{"index", 0x1601}, {"subIndex", 0x08}, {"name", tr("mapped objects 8")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});

    item.insert("1602_00", QVariantMap{{"index", 0x1602}, {"subIndex", 0x00}, {"name", tr("RPDO3 mapping parameter number of subindex")}, {"access", "RW"}, {"defaultValue", "7"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1602_01", QVariantMap{{"index", 0x1602}, {"subIndex", 0x01}, {"name", tr("mapped objects 1")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1602_02", QVariantMap{{"index", 0x1602}, {"subIndex", 0x02}, {"name", tr("mapped objects 2")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1602_03", QVariantMap{{"index", 0x1602}, {"subIndex", 0x03}, {"name", tr("mapped objects 3")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1602_04", QVariantMap{{"index", 0x1602}, {"subIndex", 0x04}, {"name", tr("mapped objects 4")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1602_05", QVariantMap{{"index", 0x1602}, {"subIndex", 0x05}, {"name", tr("mapped objects 5")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1602_06", QVariantMap{{"index", 0x1602}, {"subIndex", 0x06}, {"name", tr("mapped objects 6")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1602_07", QVariantMap{{"index", 0x1602}, {"subIndex", 0x07}, {"name", tr("mapped objects 7")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1602_08", QVariantMap{{"index", 0x1602}, {"subIndex", 0x08}, {"name", tr("mapped objects 8")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});

    item.insert("1603_00", QVariantMap{{"index", 0x1603}, {"subIndex", 0x00}, {"name", tr("RPDO4 mapping parameter number of subindex")}, {"access", "RW"}, {"defaultValue", "7"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1603_01", QVariantMap{{"index", 0x1603}, {"subIndex", 0x01}, {"name", tr("mapped objects 1")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1603_02", QVariantMap{{"index", 0x1603}, {"subIndex", 0x02}, {"name", tr("mapped objects 2")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1603_03", QVariantMap{{"index", 0x1603}, {"subIndex", 0x03}, {"name", tr("mapped objects 3")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1603_04", QVariantMap{{"index", 0x1603}, {"subIndex", 0x04}, {"name", tr("mapped objects 4")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1603_05", QVariantMap{{"index", 0x1603}, {"subIndex", 0x05}, {"name", tr("mapped objects 5")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1603_06", QVariantMap{{"index", 0x1603}, {"subIndex", 0x06}, {"name", tr("mapped objects 6")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1603_07", QVariantMap{{"index", 0x1603}, {"subIndex", 0x07}, {"name", tr("mapped objects 7")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1603_08", QVariantMap{{"index", 0x1603}, {"subIndex", 0x08}, {"name", tr("mapped objects 8")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});

    item.insert("1800_00", QVariantMap{{"index", 0x1800}, {"subIndex", 0x00}, {"name", tr("TPDO1 communication parameter number of subindex")}, {"access", "RO"}, {"defaultValue", "6"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1800_01", QVariantMap{{"index", 0x1800}, {"subIndex", 0x01}, {"name", tr("COBID")}, {"access", "RW"}, {"defaultValue", "385"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1800_02", QVariantMap{{"index", 0x1800}, {"subIndex", 0x02}, {"name", tr("transmission type")}, {"access", "RW"}, {"defaultValue", "255"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1800_03", QVariantMap{{"index", 0x1800}, {"subIndex", 0x03}, {"name", tr("inhibit timer")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", "0.1ms"}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1800_05", QVariantMap{{"index", 0x1800}, {"subIndex", 0x05}, {"name", tr("event timer")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", "ms"}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});

    item.insert("1801_00", QVariantMap{{"index", 0x1801}, {"subIndex", 0x00}, {"name", tr("TPDO2 communication parameter number of subindex")}, {"access", "RO"}, {"defaultValue", "6"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1801_01", QVariantMap{{"index", 0x1801}, {"subIndex", 0x01}, {"name", tr("COBID")}, {"access", "RW"}, {"defaultValue", "641"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1801_02", QVariantMap{{"index", 0x1801}, {"subIndex", 0x02}, {"name", tr("transmission type")}, {"access", "RW"}, {"defaultValue", "255"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1801_03", QVariantMap{{"index", 0x1801}, {"subIndex", 0x03}, {"name", tr("inhibit timer")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", "0.1ms"}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1801_05", QVariantMap{{"index", 0x1801}, {"subIndex", 0x05}, {"name", tr("event timer")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", "ms"}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});

    item.insert("1802_00", QVariantMap{{"index", 0x1802}, {"subIndex", 0x00}, {"name", tr("TPDO3 communication parameter number of subindex")}, {"access", "RO"}, {"defaultValue", "6"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1802_01", QVariantMap{{"index", 0x1802}, {"subIndex", 0x01}, {"name", tr("COBID")}, {"access", "RW"}, {"defaultValue", "897"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1802_02", QVariantMap{{"index", 0x1802}, {"subIndex", 0x02}, {"name", tr("transmission type")}, {"access", "RW"}, {"defaultValue", "255"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1802_03", QVariantMap{{"index", 0x1802}, {"subIndex", 0x03}, {"name", tr("inhibit timer")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", "0.1ms"}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1802_05", QVariantMap{{"index", 0x1802}, {"subIndex", 0x05}, {"name", tr("event timer")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", "ms"}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});

    item.insert("1803_00", QVariantMap{{"index", 0x1803}, {"subIndex", 0x00}, {"name", tr("TPDO4 communication parameter number of subindex")}, {"access", "RO"}, {"defaultValue", "6"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1803_01", QVariantMap{{"index", 0x1803}, {"subIndex", 0x01}, {"name", tr("COBID")}, {"access", "RW"}, {"defaultValue", "1153"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1803_02", QVariantMap{{"index", 0x1803}, {"subIndex", 0x02}, {"name", tr("transmission type")}, {"access", "RW"}, {"defaultValue", "255"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1803_03", QVariantMap{{"index", 0x1803}, {"subIndex", 0x03}, {"name", tr("inhibit timer")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", "0.1ms"}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1803_05", QVariantMap{{"index", 0x1803}, {"subIndex", 0x05}, {"name", tr("event timer")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint16_t"}, {"dlc", 2}, {"unit", "ms"}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});

    item.insert("1A00_00", QVariantMap{{"index", 0x1A00}, {"subIndex", 0x00}, {"name", tr("TPDO1 mapping parameter number of subindex")}, {"access", "RW"}, {"defaultValue", "7"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A00_01", QVariantMap{{"index", 0x1A00}, {"subIndex", 0x01}, {"name", tr("mapped objects 1")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A00_02", QVariantMap{{"index", 0x1A00}, {"subIndex", 0x02}, {"name", tr("mapped objects 2")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A00_03", QVariantMap{{"index", 0x1A00}, {"subIndex", 0x03}, {"name", tr("mapped objects 3")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A00_04", QVariantMap{{"index", 0x1A00}, {"subIndex", 0x04}, {"name", tr("mapped objects 4")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A00_05", QVariantMap{{"index", 0x1A00}, {"subIndex", 0x05}, {"name", tr("mapped objects 5")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A00_06", QVariantMap{{"index", 0x1A00}, {"subIndex", 0x06}, {"name", tr("mapped objects 6")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A00_07", QVariantMap{{"index", 0x1A00}, {"subIndex", 0x07}, {"name", tr("mapped objects 7")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A00_08", QVariantMap{{"index", 0x1A00}, {"subIndex", 0x08}, {"name", tr("mapped objects 8")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});

    item.insert("1A01_00", QVariantMap{{"index", 0x1A01}, {"subIndex", 0x00}, {"name", tr("TPDO2 mapping parameter number of subindex")}, {"access", "RW"}, {"defaultValue", "7"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A01_01", QVariantMap{{"index", 0x1A01}, {"subIndex", 0x01}, {"name", tr("mapped objects 1")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A01_02", QVariantMap{{"index", 0x1A01}, {"subIndex", 0x02}, {"name", tr("mapped objects 2")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A01_03", QVariantMap{{"index", 0x1A01}, {"subIndex", 0x03}, {"name", tr("mapped objects 3")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A01_04", QVariantMap{{"index", 0x1A01}, {"subIndex", 0x04}, {"name", tr("mapped objects 4")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A01_05", QVariantMap{{"index", 0x1A01}, {"subIndex", 0x05}, {"name", tr("mapped objects 5")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A01_06", QVariantMap{{"index", 0x1A01}, {"subIndex", 0x06}, {"name", tr("mapped objects 6")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A01_07", QVariantMap{{"index", 0x1A01}, {"subIndex", 0x07}, {"name", tr("mapped objects 7")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A01_08", QVariantMap{{"index", 0x1A01}, {"subIndex", 0x08}, {"name", tr("mapped objects 8")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});

    item.insert("1A02_00", QVariantMap{{"index", 0x1A02}, {"subIndex", 0x00}, {"name", tr("TPDO3 mapping parameter number of subindex")}, {"access", "RW"}, {"defaultValue", "7"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A02_01", QVariantMap{{"index", 0x1A02}, {"subIndex", 0x01}, {"name", tr("mapped objects 1")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A02_02", QVariantMap{{"index", 0x1A02}, {"subIndex", 0x02}, {"name", tr("mapped objects 2")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A02_03", QVariantMap{{"index", 0x1A02}, {"subIndex", 0x03}, {"name", tr("mapped objects 3")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A02_04", QVariantMap{{"index", 0x1A02}, {"subIndex", 0x04}, {"name", tr("mapped objects 4")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A02_05", QVariantMap{{"index", 0x1A02}, {"subIndex", 0x05}, {"name", tr("mapped objects 5")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A02_06", QVariantMap{{"index", 0x1A02}, {"subIndex", 0x06}, {"name", tr("mapped objects 6")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A02_07", QVariantMap{{"index", 0x1A02}, {"subIndex", 0x07}, {"name", tr("mapped objects 7")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A02_08", QVariantMap{{"index", 0x1A02}, {"subIndex", 0x08}, {"name", tr("mapped objects 8")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});

    item.insert("1A03_00", QVariantMap{{"index", 0x1A03}, {"subIndex", 0x00}, {"name", tr("TPDO4 mapping parameter number of subindex")}, {"access", "RW"}, {"defaultValue", "7"}, {"type", "uint8_t"}, {"dlc", 1}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A03_01", QVariantMap{{"index", 0x1A03}, {"subIndex", 0x01}, {"name", tr("mapped objects 1")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A03_02", QVariantMap{{"index", 0x1A03}, {"subIndex", 0x02}, {"name", tr("mapped objects 2")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A03_03", QVariantMap{{"index", 0x1A03}, {"subIndex", 0x03}, {"name", tr("mapped objects 3")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A03_04", QVariantMap{{"index", 0x1A03}, {"subIndex", 0x04}, {"name", tr("mapped objects 4")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A03_05", QVariantMap{{"index", 0x1A03}, {"subIndex", 0x05}, {"name", tr("mapped objects 5")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A03_06", QVariantMap{{"index", 0x1A03}, {"subIndex", 0x06}, {"name", tr("mapped objects 6")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A03_07", QVariantMap{{"index", 0x1A03}, {"subIndex", 0x07}, {"name", tr("mapped objects 7")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});
    item.insert("1A03_08", QVariantMap{{"index", 0x1A03}, {"subIndex", 0x08}, {"name", tr("mapped objects 8")}, {"access", "RW"}, {"defaultValue", "0"}, {"type", "uint32_t"}, {"dlc", 4}, {"unit", ""}, {"oldValue", ""}, {"currentValue", ""}, {"pendingValue", ""}});

    m_map.insert(nodeId, item);

    emit ListChanged(nodeId, item);
}
