#pragma once

#include <QObject>
#include <QVariantMap>
#include <QReadWriteLock>
#include <QString>

class CanopenObject : public QObject {
    Q_OBJECT
public:
    explicit CanopenObject(QObject *parent = nullptr);

    QString keyString(uint16_t index, uint8_t subIndex) const;

    void insert(uint16_t nodeId, uint16_t index, uint8_t subIndex, const QVariantMap &data);
    void update(uint16_t nodeId, uint16_t index, uint8_t subIndex, const QVariantMap &data);

    QVariantMap get(uint16_t nodeId, uint16_t index, uint8_t subIndex) const;
    QVariantMap getAll(uint16_t nodeId) const;
    void clear();

public slots:
    void syncItem(const QString &nodeId, const QString &key, const QVariantMap &item); // 同步不发信号
    void syncList(const QString &nodeId, const QVariantMap &item); // 同步nodeId列表

signals:
    void ItemChanged(const QString &nodeId, const QString &key, const QVariantMap &item);
    void ListChanged(const QString &nodeId,const QVariantMap &item);

private:
    void addNode(QString nodeId);

    QMap<QString, QVariantMap> m_map;          // <node, data(key: "1000_00" -> value: QVariantMap)>

    mutable QReadWriteLock m_lock;
};
