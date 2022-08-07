#ifndef JSONTYPE_H_
#define JSONTYPE_H_


enum JSONTYPE {
    J_NULL, // null
    J_INT,
    J_DOUBLE,
    J_BOOL,
    J_STR,
    J_LIST, // vector<Object>
    J_DICT // map<string, Object>
};


#endif