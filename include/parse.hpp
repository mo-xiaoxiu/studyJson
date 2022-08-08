#ifndef PARSE_H_
#define PARSE_H_
#include "object.hpp"
#include <iostream>

#define FUNC_TO_NAME _to_json
#define FUNC_FROM_NAME _from_json

#define START_TO_JSON                    \
    void FUNC_TO_NAME(Object &obj) const \
    {
#define to(key) obj[key]
// push一个自定义类型的成员
#define to_struct(key, struct_member) \
    Object tmp((dict_t()));           \
    struct_member.FUNC_TO_NAME(tmp);  \
    obj[key] = tmp
#define END_TO_JSON }

#define START_FROM_JSON              \
    void FUNC_FROM_NAME(Object &obj) \
    {
#define from(key, type) obj[key].Value<type>()
#define from_struct(key, struct_member) struct_member.FUNC_FROM_NAME(obj[key])
#define END_FROM_JSON }

class Parse
{
public:
    Parse() = default;
    Object parse();
    char get_next_token(); //获取下一个字符串

    void init(string_view content);
    void trim_right();

    //利用一个对象完成解析过程，不需要每次解析都创建
    static Object fromString(string_view content);

    Object pares_null();
    Object parse_bool();
    Object pares_number();
    Object parse_list();
    Object parse_dict();
    Object parse_string();

    void skip_comment();
    bool is_esc_consum(size_t pos);

    template <class T>
    static string ToJSON(T const &src)
    {
        //如果是基本类型
        if constexpr (IS_TYPE(T, int_t))
        {
            Object object(src);
            return object.to_string();
        }
        else if constexpr (IS_TYPE(T, bool_t))
        {
            Object object(src);
            return object.to_string();
        }
        else if constexpr (IS_TYPE(T, double_t))
        {
            Object object(src);
            return object.to_string();
        }
        else if constexpr (IS_TYPE(T, str_t))
        {
            Object object(src);
            return object.to_string();
        }
        //如果是自定义类型调用方法完成dict的赋值，然后to_string即可
        Object obj((dict_t()));
        src.FUNC_TO_NAME(obj);
        return obj.to_string();
    }

    template <class T>
    static T fromJson(string_view src)
    {
        Object object = fromString(src);
        //如果是基本类型
        if constexpr (is_basic_type<T>())
        {
            return object.template Value<T>();
        }

        //调用T类型对应的成岩函数
        if (object.Type() != J_DICT)
            throw std::logic_error("not dict type fromjson");
        T ret;
        ret.FUNC_FROM_NAME(object);
        return ret;
    }

private:
    string m_str;
    ssize_t m_idx{}; //遍历索引
};

#endif