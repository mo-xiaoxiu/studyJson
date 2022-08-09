#ifndef MYUTIL_OBJECT_H
#define MYUTIL_OBJECT_H

#include <stdexcept>
#include <utility>
#include <variant>
#include <map>
#include <vector>
#include <string>
#include <string_view>
#include <sstream>

namespace zjpJson
{
    using std::get_if;
    using std::map;
    using std::string;
    using std::string_view;
    using std::stringstream;
    using std::variant;
    using std::vector;

    enum JTYPE
    {
        J_NULL,
        J_BOOL,
        J_INT,
        J_DOUBLE,
        J_STR,
        J_LIST,
        J_DICT
    };

    class Object;

    using null_t = string;
    using int_t = int32_t;
    using bool_t = bool;
    using double_t = double;
    using str_t = string;
    using list_t = vector<Object>;
    using dict_t = map<string, Object>;

#define IS_TYPE(typea, typeb) std::is_same<typea, typeb>::value

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

    class Object
    {
    public:
        using value_t = variant<bool_t, int_t, double_t, str_t, list_t, dict_t>;

        Object() //默认为null类型
        {
            m_type = J_NULL;
            m_value = "null";
        }

        Object(int_t value)
        {
            Int(value);
        }

        Object(bool_t value)
        {
            Bool(value);
        }

        Object(double_t value)
        {
            Double(value);
        }

        Object(str_t const &value)
        {
            Str(value);
        }

        Object(list_t value)
        {
            List(std::move(value));
        }

        Object(dict_t value)
        {
            Dict(std::move(value));
        }

        void Null()
        {
            m_type = J_NULL;
            m_value = "null";
        }

        void Int(int_t value)
        {
            m_value = value;
            m_type = J_INT;
        }

        void Bool(bool_t value)
        {
            m_value = value;
            m_type = J_BOOL;
        }

        void Double(double_t value)
        {
            m_type = J_DOUBLE;
            m_value = value;
        }

        void Str(string_view value)
        {
            m_value = string(value);
            m_type = J_STR;
        }

        void List(list_t value)
        {
            m_value = std::move(value);
            m_type = J_LIST;
        }

        void Dict(dict_t value)
        {
            m_value = std::move(value);
            m_type = J_DICT;
        }

#define THROW_GET_ERROR(erron) throw std::logic_error("type error in get " #erron " value!")

        template <class V>
        V &Value()
        {
            //添加安全检查
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
                if (m_type != J_LIST)
                    THROW_GET_ERROR(LIST);
            }
            else if constexpr (IS_TYPE(V, dict_t))
            {
                if (m_type != J_DICT)
                    THROW_GET_ERROR(DICT);
            }

            void *v = value();
            if (v == nullptr)
                throw std::logic_error("unknown type in JObject::Value()");
            return *((V *)v);
        }

        JTYPE Type()
        {
            return m_type;
        }

        string to_string();

        void push_back(Object item)
        {
            if (m_type == J_LIST)
            {
                auto &list = Value<list_t>();
                list.push_back(std::move(item));
                return;
            }
            throw std::logic_error("not a list type! JObjcct::push_back()");
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

        Object &operator[](string const &key)
        {
            if (m_type == J_DICT)
            {
                auto &dict = Value<dict_t>();
                return dict[key];
            }
            throw std::logic_error("not dict type! JObject::opertor[]()");
        }

    private:
        //根据类型获取值的地址，直接硬转为void*类型，然后外界调用Value函数进行类型的强转
        void *value();

    private:
        JTYPE m_type;
        value_t m_value;
    };
}

#endif // MYUTIL_OBJECT_H
