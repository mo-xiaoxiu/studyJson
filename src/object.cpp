#include "object.hpp"




Object::Object() //默认初始化未空值
{
    m_type = J_NULL;
    m_value = "null";
}

Object::Object(int_t value) // int类型赋值
{
    Int(value);
}

Object::Object(double_t value) // double类型赋值
{
    Double(value);
}

Object::Object(bool_t value) // bool类型赋值
{
    Bool(value);
}

Object::Object(str_t const &value) //字符串类型赋值
{
    Str(value);
}

Object::Object(list_t value) // list类型赋值
{
    List(std::move(value));
}

Object::Object(dict_t value) // dict类型赋值
{
    Dict(sdt::move(value));
}

void Object::Null()
{
    m_type = J_NULL;
    m_value = "null";
}

void Object::Int(int_t value)
{
    m_type = J_INT;
    m_value = value;
}

void Object::Double(double_t value)
{
    m_type = J_DOUBLE;
    m_value = value;
}

void Object::Bool(bool_t value)
{
    m_type = J_BOOL;
    m_value = value;
}

void Object::Str(std::string_view value)
{
    m_type = J_STR;
    m_value = string(value);
}

void Object::List(list_t value)
{
    m_type = J_LIST;
    m_value = std::move(value);
}

void Object::Dict(dict_t value)
{
    m_type = J_DICT;
    m_value = std::move(value);
}
