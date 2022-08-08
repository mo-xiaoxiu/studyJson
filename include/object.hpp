#ifndef OBJECT_H_
#define OBJECT_H_
#include "jsontype.hpp"
#include <string>
#include <stdint.h>
#include <vector>
#include <map>
#include <variant>
#include <stdexcept>
#include <utility>
#include <sstream>
using namespace std;

#define IS_TYPE(typeA, typeB) std::is_same<typeA, typeB>::value

template <class T>
constexpr bool is_basic_type()
{
    if constexpr (IS_TYPE(T, str_t) ||
                  IS_TYPE(T, bool_t) ||
                  IS_TYPE(T, double_t) ||
                  IS_TYPE(T, int_t))
        return true;

    return false;
}

//定义值类型
using null_t = string;
using int_t = int32_t;
using bool_t = bool;
using double_t = double;
using str_t = string;
using list_t = vector<Object>;
using dict_t = map<string, Object>;

class Object
{
public:
    using value_t = variant<bool_t, int_t, double_t, str_t, list_t, dict_t>;

    Object();                   //默认初始化未空值
    Object(int_t value);        // int类型赋值
    Object(double_t value);     // double类型赋值
    Object(bool_t value);       // bool类型赋值
    Object(str_t const &value); //字符串类型赋值
    Object(list_t value);       // list类型赋值
    Object(dict_t value);       // dict类型赋值

    void Null();
    void Int(int_t value);
    void Double(double_t value);
    void Bool(bool_t value);
    void Str(std::string_view value);
    void List(list_t value);
    void Dict(dict_t value);

    //将Json转换为字符串
    string to_string();

    JSONTYPE Type()
    {
        return m_type;
    }

#define THROW_GET_ERROR(errno) throw std::logic_error("type error in get " #erron " value!")

    template <class V>
    V &Value()
    {
        //安全检查
        if constexpr (IS_TYPE(V, str_t))
        {
            if (m_type != J_STR)
                THROW_GET_ERROR(string);
        }
        else if constexpr (IS_TYPE(V, bool_t))
        {
            if (m_type != J_BOOL)
                THROW_GET_ERROR(BOOL);
        }
        else if constexpr (IS_TYPE(V, int_t))
        {
            if (m_type != J_INT)
                THROW_GET_ERROR(INT);
        }
        else if constexpr (IS_TYPE(V, double_t))
        {
            if (m_type != J_DOUBLE)
                THROW_GET_ERROR(DOUBLE);
        }
        else if constexpr (IS_TYPE(V, list_t))
        {
            if (m_type != T_LIST)
                THROW_GET_ERROR(LIST);
        }
        else if constexpr (IS_TYPE(V, dict_t))
        {
            if (m_type != T_DICT)
                THROW_GET_ERROR(DICT);
        }

        void *v = value();
        if (v == nullptr)
        {
            throw std::logic_error("unknown type in JObject::Value()");
        }
        //强转
        return *((V *)v);
    }

    //重载：dict更好用
    Object &operator[](string const &key)
    {
        if (m_type == J_DICT)
        {
            auto &dict = Value<dict_t>();
            return dict[key];
        }
        throw std::logic_error("not dict type! JObject::opertor[]()");
    }

    // list的push_back()和pop_back()方法
    void push_back(Object item)
    {
        if (m_type == J_LIST)
        {
            auto &list = Value<list_t>();
            list.push_back(std::move(item));
            return;
        }
        throw std::logic_error("not list type! JObjcct::push_back()");
    }
    void pop_back()
    {
        if (m_type == J_LIST)
        {
            auto &list = Value<list_t>();
            list.pop_back();
            return;
        }
        throw std::logic_error("not list type! JObjcct::pop_back()");
    }

private:
    //获取variant
    void *value();

    JSONTYPE m_type; // key
    value_t m_value; // value
};


#endif
