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

private:
    JSONTYPE m_type; //key 
    value_t m_value; //value
};


#endif