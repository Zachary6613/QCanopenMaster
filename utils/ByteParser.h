#pragma once
#include <QVariant>
#include <QString>
#include <QHash>
#include <cstdint>
#include <functional>

namespace ByteParser {
    using ParseFunc = std::function<QVariant(const uint8_t*, bool)>;
    using WriteFunc = std::function<void(const QVariant&, uint8_t*, bool)>;

    const QHash<QString, ParseFunc>& registry();
    const QHash<QString, WriteFunc>& registryWrite();

    QVariant parse(const QString& typeName, const uint8_t* data, bool bigEndian = false);
    bool write(const QString& typeName, const QVariant& value, uint8_t* buffer, bool bigEndian = false);
}
