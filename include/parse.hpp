#ifndef PARSE_H_
#define PARSE_H_
#include "object.hpp"
#include <iostream>

class Parse{
public:
    Parse() = default;
    Object parse();
    char get_next_token(); //获取下一个字符串

    Object pares_null();
    Object parse_bool();
    Object pares_number();
    Object parse_list();
    Object parse_dict();
    Object parse_string();

    void skip_comment();
private:
    string m_str;
    ssize_t m_idx{}; //遍历索引
};

#endif