//
// Created by XMD on 2021/2/14.
//
#include "serializer.h"

#include <utility>

namespace serializer {
#define DEF_PRIMITIVE_TYPE_TO_STRING(TYPE)                                         \
    std::string ToString(TYPE &obj) {                                              \
        std::string ret(reinterpret_cast<const char*>(&obj), sizeof(TYPE));        \
        return ret;                                                                \
    }

#define DEF_PRIMITIVE_TYPE_TO_OBJ(TYPE)                    \
    ToObjSize ToObj(const std::string &str, TYPE &obj) {   \
        memcpy(&obj, str.data(), sizeof(TYPE));            \
        return {sizeof(TYPE), sizeof(TYPE)};               \
    }

#define DEF_PRIMITIVE_TYPE_WRITE(TYPE)             \
    void Write(FileOutputStream &os, TYPE &obj) {  \
        std::string content = ToString(obj);       \
        os.Write(content.data(), content.size());  \
    }

#define DEF_PRIMITIVE_TYPE_READ(TYPE)              \
    void Read(FileInputStream &is, TYPE &obj) {    \
        char buffer[sizeof(TYPE)] = {0};           \
        is.Read(buffer, sizeof(TYPE));             \
        std::string str(buffer, sizeof(TYPE));     \
        (void)ToObj(str, obj);                     \
    }

#define DEF_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(TYPE)          \
    DEF_PRIMITIVE_TYPE_TO_STRING(TYPE)                     \
    DEF_PRIMITIVE_TYPE_TO_OBJ(TYPE)                        \
    DEF_PRIMITIVE_TYPE_WRITE(TYPE)                         \
    DEF_PRIMITIVE_TYPE_READ(TYPE)

    DEF_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(char)

    DEF_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(signed char)

    DEF_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(unsigned char)

    DEF_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(short)

    DEF_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(unsigned short)

    DEF_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(int)

    DEF_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(unsigned int)

    DEF_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(long)

    DEF_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(unsigned long)

    DEF_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(long long)

    DEF_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(unsigned long long)

    DEF_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(bool)

    DEF_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(float)

    DEF_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(double)

    std::string ToString(std::string &in) {
        size_t size = in.size();
        std::string ret = ToString(size);
        ret.append(in);
        return ret;
    }

    ToObjSize ToObj(const std::string &str, std::string &out) {
        size_t size;
        ToObjSize offset = ToObj(str, size);
        out.append(str.substr(offset.offset, size));
        return {offset.offset + size, size};
    }

    void Write(FileOutputStream &os, std::string &str) {
        std::string content = ToString(str);
        os.Write(content.data(), content.size());
    }

    void Read(FileInputStream &is, std::string &out) {
        char buffer[sizeof(size_t)] = {0};
        is.Read(buffer, sizeof(size_t));
        std::string lengthStr(buffer, sizeof(size_t));
        size_t length;
        ToObj(lengthStr, length);
        char *strBuffer = reinterpret_cast<char*>(malloc(length));
        is.Read(strBuffer, length);
        std::string content(strBuffer, length);
        free(strBuffer);
        out.append(content);
    }

    FileOutputStream::FileOutputStream(std::string filePath) : filePath(std::move(filePath)) {}

    FileOutputStream::~FileOutputStream() {
        Close();
    }

    bool FileOutputStream::Open() {
        fileStream.open(filePath, std::ios::out | std::ios::binary);
        return fileStream.is_open();
    }

    bool FileOutputStream::Close() {
        if (fileStream.is_open()) {
            fileStream.close();
        }
        return true;
    }

    FileInputStream::FileInputStream(std::string filePath) : filePath(std::move(filePath)) {}

    FileInputStream::~FileInputStream() {
        Close();
    }

    bool FileInputStream::Open() {
        fileStream.open(filePath, std::ios::in | std::ios::binary);
        return fileStream.is_open();
    }

    bool FileInputStream::Close() {
        if (fileStream.is_open()) {
            fileStream.close();
        }
        return true;
    }
} // namespace serializer