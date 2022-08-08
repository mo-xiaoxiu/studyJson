#include "object.hpp"

#define GET_VALUE(type) *((type *)value)

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
    Dict(std::move(value));
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

//获取variant
void *Object::value()
{
    switch (m_type)
    {
    case J_NULL:
        return get_if<str_t>(&m_value);
    case J_BOOL:
        return get_if<bool_t>(&m_value);
    case J_INT:
        return get_if<int_t>(&m_value);
    case J_DOUBLE:
        return get_if<double_t>(&m_value);
    case J_LIST:
        return get_if<list_t>(&m_value);
    case J_DICT:
        return get_if<dict_t>(&m_value);
    case J_STR:
        return get_if<str_t>(&m_value);
    default:
        return nullptr;
    }
}

//将Json转换为字符串
string Object::to_string()
{
    void *value = this->value();
    std::ostringstream os;

    switch (m_type)
    {
    case J_NULL:
        os << "null";
        break;
    case J_BOOL:
        if (GET_VALUE(bool))
            os << "true";
        else
            os << "false";
        break;
    case J_INT:
        os << GET_VALUE(int);
        break;
    case J_DOUBLE:
        os << GET_VALUE(double);
        break;
    case J_STR:
        os << '\"' << GET_VALUE(string) << '\"';
        break;
    case J_LIST:
    {
        list_t &list = GET_VALUE(list_t);
        os << '[';
        for (auto i = 0; i < list.size(); i++)
        {
            if (i != list.size() - 1)
            {
                os << ((list[i]).to_string());
                os << ',';
            }
            else
                os << ((list[i]).to_string());
        }
        os << ']';
        break;
    }
    case J_DICT:
    {
        dict_t &dict = GET_VALUE(dict_t);
        os << '{';
        for (auto it = dict.begin(); it != dict.end(); ++it)
        {
            if (it != dict.begin()) //为了保证最后的json格式正确
                os << ',';
            os << '\"' << it->first << "\":" << it->second.to_string();
        }
        os << '}';
        break;
    }
    default:
        return "";
    }

    return os.str();
}