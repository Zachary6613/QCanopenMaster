#include "ByteParser.h"
#include <cstring>

static void swapBytes(void* value, size_t size) {
    uint8_t* p = static_cast<uint8_t*>(value);
    for (size_t i = 0; i < size / 2; i++)
        std::swap(p[i], p[size - 1 - i]);
}

const QHash<QString, ByteParser::ParseFunc>& ByteParser::registry() {
    static QHash<QString, ParseFunc> map = {
        { "int8_t",    [](const uint8_t* d, bool){ int8_t v; memcpy(&v,d,sizeof(v)); return QVariant(v); }},
        { "uint8_t",   [](const uint8_t* d, bool){ uint8_t v; memcpy(&v,d,sizeof(v)); return QVariant(v); }},
        { "int16_t",   [](const uint8_t* d, bool be){ int16_t v; memcpy(&v,d,sizeof(v)); if(be) swapBytes(&v,sizeof(v)); return QVariant(v); }},
        { "uint16_t",  [](const uint8_t* d, bool be){ uint16_t v; memcpy(&v,d,sizeof(v)); if(be) swapBytes(&v,sizeof(v)); return QVariant(v); }},
        { "int32_t",   [](const uint8_t* d, bool be){ int32_t v; memcpy(&v,d,sizeof(v)); if(be) swapBytes(&v,sizeof(v)); return QVariant(v); }},
        { "uint32_t",  [](const uint8_t* d, bool be){ uint32_t v; memcpy(&v,d,sizeof(v)); if(be) swapBytes(&v,sizeof(v)); return QVariant(v); }},
        { "float",     [](const uint8_t* d, bool be){ float v; memcpy(&v,d,sizeof(v)); if(be) swapBytes(&v,sizeof(v)); return QVariant(v); }},
        { "float32",   [](const uint8_t* d, bool be){ float v; memcpy(&v,d,sizeof(v)); if(be) swapBytes(&v,sizeof(v)); return QVariant(v); }},
        { "double",    [](const uint8_t* d, bool be){ double v; memcpy(&v,d,sizeof(v)); if(be) swapBytes(&v,sizeof(v)); return QVariant(v); }},
        { "float64",   [](const uint8_t* d, bool be){ double v; memcpy(&v,d,sizeof(v)); if(be) swapBytes(&v,sizeof(v)); return QVariant(v); }},
        { "string",    [](const uint8_t* d, bool){ return QVariant(QString::fromUtf8(reinterpret_cast<const char*>(d))); }}
    };
    return map;
}

const QHash<QString, ByteParser::WriteFunc>& ByteParser::registryWrite() {
    static QHash<QString, WriteFunc> map = {
        { "int8_t", [](const QVariant& v, uint8_t* buf, bool){ int8_t val = static_cast<int8_t>(v.toInt()); memcpy(buf, &val, sizeof(val)); }},
        { "uint8_t", [](const QVariant& v, uint8_t* buf, bool){ uint8_t val = static_cast<uint8_t>(v.toUInt()); memcpy(buf, &val, sizeof(val)); }},
        { "int16_t", [](const QVariant& v, uint8_t* buf, bool be){ int16_t val = static_cast<int16_t>(v.toInt()); memcpy(buf, &val, sizeof(val)); if(be) swapBytes(buf, sizeof(val)); }},
        { "uint16_t", [](const QVariant& v, uint8_t* buf, bool be){ uint16_t val = static_cast<uint16_t>(v.toUInt()); memcpy(buf, &val, sizeof(val)); if(be) swapBytes(buf, sizeof(val)); }},
        { "int32_t", [](const QVariant& v, uint8_t* buf, bool be){ int32_t val = static_cast<int32_t>(v.toInt()); memcpy(buf, &val, sizeof(val)); if(be) swapBytes(buf, sizeof(val)); }},
        { "uint32_t", [](const QVariant& v, uint8_t* buf, bool be){ uint32_t val = static_cast<uint32_t>(v.toUInt()); memcpy(buf, &val, sizeof(val)); if(be) swapBytes(buf, sizeof(val)); }},
        { "float", [](const QVariant& v, uint8_t* buf, bool be){ float val = static_cast<float>(v.toFloat()); memcpy(buf, &val, sizeof(val)); if(be) swapBytes(buf, sizeof(val)); }},
        { "float32", [](const QVariant& v, uint8_t* buf, bool be){ float val = static_cast<float>(v.toFloat()); memcpy(buf, &val, sizeof(val)); if(be) swapBytes(buf, sizeof(val)); }},
        { "double", [](const QVariant& v, uint8_t* buf, bool be){ double val = static_cast<double>(v.toDouble()); memcpy(buf, &val, sizeof(val)); if(be) swapBytes(buf, sizeof(val)); }},
        { "float64", [](const QVariant& v, uint8_t* buf, bool be){ double val = static_cast<double>(v.toDouble()); memcpy(buf, &val, sizeof(val)); if(be) swapBytes(buf, sizeof(val)); }},
        { "string", [](const QVariant& v, uint8_t* buf, bool) {QByteArray ba = v.toString().toUtf8();size_t len = ba.size();memcpy(buf, ba.constData(), len);buf[len] = '\0'; }}
    };
    return map;
}

QVariant ByteParser::parse(const QString& typeName, const uint8_t* data, bool bigEndian) {
    auto it = registry().find(typeName.toLower());
    if (it != registry().end())
        return it.value()(data, bigEndian);
    return QVariant();
}

bool ByteParser::write(const QString& typeName, const QVariant& value, uint8_t* buffer, bool bigEndian) {
    auto it = registryWrite().find(typeName.toLower());
    if (it != registryWrite().end()) {
        it.value()(value, buffer, bigEndian);
        return true;
    }
    return false;
}
