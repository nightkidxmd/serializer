//
// Created by XMD on 2021/2/14.
//

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <string>
#include <cstddef>
#include <list>
#include <set>
#include <vector>
#include <unordered_set>
#include <ostream>
#include <map>
#include <fstream>
#include <iostream>

namespace serializer {

    typedef struct ToObjSize {
        size_t offset; // str offset
        size_t size;   // obj size;

        ToObjSize &operator+=(ToObjSize add) {
            offset += add.offset;
            size += add.size;
            return *this;
        }

        friend std::ostream &operator<<(std::ostream &os, const ToObjSize &toObjSize) {
            os << "offset: " << toObjSize.offset << " toObjSize: " << toObjSize.size;
            return os;
        }
    } ToObjSize;

    template<typename T>
    std::string ToString(T &obj) {
        return obj.ToString();
    }

    template<typename T>
    ToObjSize ToObj(const std::string &str, T &obj) {
        return obj.ToObj(str);
    }

    class FileOutputStream;
    class FileInputStream;

    template<typename T>
    void Write(FileOutputStream &os, T &obj) {
        obj.Write(os);
    }

    template<typename T>
    void Read(FileInputStream &is, T &obj) {
        obj.Read(is);
    }

#define DECLARE_PRIMITIVE_TYPE_TO_STRING(TYPE)         \
       std::string ToString(TYPE &obj);

#define DECLARE_PRIMITIVE_TYPE_TO_OBJ(TYPE)            \
       ToObjSize ToObj(const std::string &str, TYPE &obj);

#define DECLARE_PRIMITIVE_TYPE_WRITE(TYPE)         \
       void Write(FileOutputStream &os, TYPE &obj);

#define DECLARE_PRIMITIVE_TYPE_READ(TYPE)            \
       void Read(FileInputStream &is, TYPE &obj);

#define DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(TYPE)       \
       DECLARE_PRIMITIVE_TYPE_TO_STRING(TYPE)               \
       DECLARE_PRIMITIVE_TYPE_TO_OBJ(TYPE)                  \
       DECLARE_PRIMITIVE_TYPE_WRITE(TYPE)                   \
       DECLARE_PRIMITIVE_TYPE_READ(TYPE)

    DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(char)

    DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(signed char)

    DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(unsigned char)

    DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(short)

    DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(unsigned short)

    DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(int)

    DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(unsigned int)

    DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(long)

    DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(unsigned long)

    DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(long long)

    DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(unsigned long long)

    DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(bool)

    DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(float)

    DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(double)

    DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ(std::string)

#undef DECLARE_PRIMITIVE_TYPE_TO_STRING_TO_OBJ

#define DEF_STL_TO_STRING(TYPE)                      \
    std::string ToString(TYPE &stl) {                \
        size_t size = stl.size();                    \
        std::string ret = ToString(size);            \
        for (auto &item:stl) {                       \
            ret.append(ToString(item));              \
        }                                            \
        return ret;                                  \
    }

#define DEF_STL_TO_OBJ(TYPE, ELEM)                        \
    ToObjSize ToObj(const std::string &str, TYPE &stl) {  \
        size_t size;                                      \
        ToObjSize ret = ToObj(str, size);                 \
        stl.reserve(size);                                \
        for (size_t i = 0; i < size; ++i) {               \
            ELEM item;                                    \
            ret += ToObj(str.substr(ret.offset), item);   \
            stl.emplace_back(item);                       \
        }                                                 \
        return ret;                                       \
    }

#define DEF_STL_TO_STRING_TO_OBJ(TYPE, ELEM)             \
    DEF_STL_TO_STRING(TYPE)                              \
    template<typename ELEM>                              \
    DEF_STL_TO_OBJ(TYPE, ELEM)

    template<typename Elem>
    DEF_STL_TO_STRING_TO_OBJ(std::vector<Elem>, Elem)

    template<typename Elem>
    DEF_STL_TO_STRING_TO_OBJ(std::list<Elem>, Elem)

    template<typename Elem>
    DEF_STL_TO_STRING_TO_OBJ(std::set<Elem>, Elem)

    template<typename Elem>
    DEF_STL_TO_STRING_TO_OBJ(std::unordered_set<Elem>, Elem)

#undef DEF_STL_TO_STRING_TO_OBJ

    template<typename K, typename V>
    std::string ToString(std::map<K, V> &map) {
        std::string ret;
        std::vector<K> keyVec;
        std::vector<V> valueVec;
        for (auto &it:map) {
            keyVec.emplace_back(it.first);
            valueVec.emplace_back(it.second);
        }
        return ret;
    }

    template<typename K, typename V>
    ToObjSize ToObj(const std::string &str, std::map<K, V> &map) {
        std::vector<K> keyVec;
        std::vector<V> valueVec;
        ToObjSize offset = ToObj(str, keyVec);
        offset += ToObjSize(str.substr(offset.offset), valueVec);
        for (int i = 0; i < keyVec.size(); ++i) {
            map[keyVec[i]] = valueVec[i];
        }
        return offset;
    }

    class Serializable {
    public:
        virtual std::string ToString() { return std::string(); }

        virtual ToObjSize ToObj(const std::string &in) { return {0, 0}; }

        friend std::ostream &operator<<(std::ostream &os, const Serializable &serializable) {
            return os;
        }
    };

    class FileOutputStream {
    public:
        explicit FileOutputStream(std::string filePath);

        virtual ~FileOutputStream();

        bool Open();

        template<typename T>
        FileOutputStream& operator<<(T &obj) {
            Write(obj);
            return *this;
        }

        template<typename T>
        bool Write(T &obj) {
            if (!fileStream.is_open()) {
                return false;
            }
            std::string content = ToString(obj);
            fileStream.write(content.data(), content.size());
            return true;
        }

        bool Write(const char* data, size_t size) {
            if (!fileStream.is_open()) {
                return false;
            }
            fileStream.write(data, size);
            return true;
        }

        bool Close();

    private:
        std::ofstream fileStream;
        std::string filePath;
    };

    class FileInputStream {
    public:
        explicit FileInputStream(std::string filePath);

        virtual ~FileInputStream();

        bool Open();

        bool Close();

        template<typename T>
        FileInputStream& operator>>(T &obj) {
            serializer::Read(*this, obj);
            return *this;
        }

        bool Read(char *outBuffer, size_t size) {
            if (!fileStream.is_open()) {
                return false;
            }
            fileStream.read(outBuffer, size);
            return true;
        }

        template<typename T>
        bool Read(T &obj) {
            if (!fileStream.is_open()) {
                return false;
            }
            serializer::Read(this, obj);
            return true;
        }

        template<typename Elem>
        bool Read(std::vector<Elem> stl) {
            size_t size;
            fileStream.read(buffer, sizeof(size_t));
            std::string sizeStr(buffer, sizeof(size_t));

            return true;
        }

    private:
        std::ifstream fileStream;
        std::string filePath;
        char buffer[256]{0};
    };
} // namespace serializer
#endif //SERIALIZER_H
