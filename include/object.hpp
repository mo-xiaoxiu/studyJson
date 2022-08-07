#ifndef OBJECT_H_
#define OBJECT_H_
#include "jsontype.hpp"
#include <string>
#include <stdint.h>
#include <vector>
#include <map>
#include <variant>
using namespace std;


//定义值类型
using null_t = string;
using int_t = int32_t;
using bool_t = bool;
using double_t = double;
using str_t = string;
using list_t = vector<Object>;
using dict_t = map<string, Object>;

class Object{
public:
    using value_t = variant<bool_t, int_t, double_t, str_t, list_t, dict_t>;

    Object(); //默认初始化未空值
    Object(int_t value); //int类型赋值
    Object(double_t value); //double类型赋值
    Object(bool_t value); //bool类型赋值
    Object(str_t const &value); //字符串类型赋值
    Object(list_t value); //list类型赋值
    Object(dict_t value); //dict类型赋值

    void Null();
    void Int(int_t value);
    void Double(double_t value);
    void Bool(bool_t value);
    void Str(std::string_view value);
    void List(list_t value);
    void Dict(dict_t value);
    
private:
    JSONTYPE m_type; //key 
    value_t m_value; //value
};


#endif