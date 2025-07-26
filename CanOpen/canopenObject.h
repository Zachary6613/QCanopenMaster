#pragma once

#include <QObject>
#include <QVariantMap>
#include <QReadWriteLock>

class CanopenObject : public QObject {
    Q_OBJECT
public:
    explicit CanopenObject(QObject *parent = nullptr);

    void initDefaultData();
    QString keyString(uint16_t index, uint8_t subIndex) const;

    void insert(uint16_t index, uint8_t subIndex, const QVariantMap &data);
    void update(uint16_t index, uint8_t subIndex, const QVariantMap &data);

    QVariantMap get(uint16_t index, uint8_t subIndex) const;
    QVariantMap getAll() const;
    void clear();

public slots:
    void syncFromRemote(const QString &key, const QVariantMap &item); // 同步不发信号

signals:
    void ItemChanged(const QString &key, const QVariantMap &item);
    void ListChanged(const QVariantMap &allData);

private:


    QVariantMap m_map;  // key: "1000_00" -> value: QVariantMap
    mutable QReadWriteLock m_lock;
};
