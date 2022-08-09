#include "object.hpp"

using namespace zjpJson;

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
            return std::get_if<str_t>(&m_value);
        default:
            return nullptr;
    }
}
//用于简化指针强转过程的宏
#define GET_VALUE(type) *((type*) value)

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
            else os << "false";
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
                } else os << ((list[i]).to_string());
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