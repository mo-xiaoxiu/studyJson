# studyJson
学习Json解析器的解析流程


<br>
学习的源仓库地址：

[Json解析器](https://github.com/ACking-you/MyUtil/tree/master/json-parser)

学习博客：

[Json解析器实现](https://www.acking-you.xyz/posts/json%E8%A7%A3%E6%9E%90%E5%99%A8%E5%AE%9E%E7%8E%B0/#%E5%AE%8C%E5%96%84jobject%E7%B1%BB)


## Object

![](https://myblog-1308923350.cos.ap-guangzhou.myqcloud.com/img/Json__Object.drawio.png)


## Parse

![](https://myblog-1308923350.cos.ap-guangzhou.myqcloud.com/img/Json__Parser.drawio.png)

## learning...

封装的Json类型，用于类型判断等

```cpp
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
```

代码中使用的Json类型

```cpp
    using null_t = string;
    using int_t = int32_t;
    using bool_t = bool;
    using double_t = double;
    using str_t = string;
    using list_t = vector<Object>;
    using dict_t = map<string, Object>;
```

## class Object

* 使用c++的`std::variant`进行类型管理
* Object构造函数：
    * 默认构造函数：null
    * 各个类型的构造函数

```cpp
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

private:
    JTYPE m_type;
    value_t m_value;
};
```

在Object中，防止使用大量的get用于取值，封装一个**模板函数**用于取值

* 类型检查
* 调用`value()`函数

```cpp
#define THROW_GET_ERROR(erron) throw std::logic_error("type error in get " #erron " value!")

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

private:
    //根据类型获取值的地址，直接硬转为void*类型，然后外界调用Value函数进行类型的强转
    void *value();
```

使用`get_if<类型>(取值地址)`，失败返回`nullptr`，返回值类型为`void*`

```cpp
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
```

Object中的其他函数

```cpp
    //返回值类型
    JTYPE Type()
    {
        return m_type;
    }

    string to_string(); //将Json数据转换成string字符串

    //对于list类型，增加push_back()和pop_back()方法，方便操作
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

    //对于自定义类型（dict），重载[]方便取值
    Object &operator[](string const &key)
    {
        if (m_type == J_DICT)
        {
            auto &dict = Value<dict_t>();
            return dict[key];
        }
        throw std::logic_error("not dict type! JObject::opertor[]()");
    }
```

接下来看一下`to_string()`函数

* 取值
* 使用`ostringstream`对象os输出string
* 根据Json类型输出内容
    * list需要遍历数组，将数据封装成`[value,value,..]`的格式
    * dict需要遍历，将数据封装成类似`{"id":32,"name":"hhh"}`的格式

```cpp
//用于简化指针强转过程的宏，获取值
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
```

## class Parser

解析类的parse()函数

* `get_next_token()`获取下一个字符

```cpp
Object Parser::parse()
{
    char token = get_next_token();
    if (token == 'n') // null
    {
        return parse_null();
    }
    if (token == 't' || token == 'f') //true or false
    {
        return parse_bool();
    }
    if (token == '-' || std::isdigit(token)) // 负值？ 或者 数组
    {
        return parse_number();
    }
    if (token == '\"') //（注意转义）字符串开头
    {
        return parse_string();
    }
    if (token == '[') //list类型开头
    {
        return parse_list();
    }
    if (token == '{') //自定义类型开头
    {
        return parse_dict();
    }

    throw std::logic_error("unexpected character in parse json");
}
```

为了避免每次解析都需要实例化一个Parser对象，所以提供一个返回单例对象函数

```cpp
Object Parser::FromString(string_view content)
{
    static Parser instance;
    instance.init(content);
    return instance.parse();
}
```

将字符串转换成Json类型数据

```cpp
template <class T>
static T FromJson(string_view src)
{
    Object object = FromString(src);
    //如果是基本类型
    if constexpr (is_basic_type<T>())
    {
        return object.template Value<T>(); //注意前面要加template
    }

    //调用T类型对应的成员函数
    if (object.Type() != J_DICT)
        throw std::logic_error("not dict type fromjson");
    T ret;
    ret.FUNC_FROM_NAME(object); //ret._from_json(object) 自定义类型的转化
    return ret;
}
```

关于自定义类型的Json数据格式的转化（包括嵌套的Json），使用宏函数进行定义

```cpp
#define FUNC_TO_NAME _to_json
#define FUNC_FROM_NAME _from_json

#define START_TO_JSON                             \
    void FUNC_TO_NAME(zjpJson::Object &obj) const \
    {
#define to(key) obj[key]
// push一个自定义类型的成员
#define to_struct(key, struct_member)         \
    zjpJson::Object tmp((zjpJson::dict_t())); \
    struct_member.FUNC_TO_NAME(tmp);          \
    obj[key] = tmp
#define END_TO_JSON }

#define START_FROM_JSON                       \
    void FUNC_FROM_NAME(zjpJson::Object &obj) \
    {
#define from(key, type) obj[key].Value<type>()
#define from_struct(key, struct_member) struct_member.FUNC_FROM_NAME(obj[key])
#define END_FROM_JSON }
```

使用的过程：

```cpp
struct Base
{
    int pp;
    string qq;

    START_FROM_JSON
        pp = from("pp", int);
        qq = from("qq", string);
    END_FROM_JSON

    START_TO_JSON
        to("pp") = pp;
        to("qq") = qq;
    END_TO_JSON
};

struct Mytest
{
    int id;
    std::string name;
    Base q;

    START_TO_JSON
        to_struct("base", q);
        to("id") = id;
        to("name") = name;
    END_TO_JSON

    START_FROM_JSON
        id = from("id", int);
        name = from("name", string);
        from_struct("base", q);
    END_FROM_JSON
};
```

相当于

```cpp
struct Base
{
    int pp;
    string qq;

    //数据赋值给对应类型：传入一个Object对象（临时对象）
    void _from_json(zjpJson::Object &obj) 
    {
        pp = obj[pp].Value<int>(); //自定义类型：key类型int
        qq = obj[qq].Value<string>(); //key类型string
    }

    //构建键值对
    void _to_json(zjpJson::Object &obj)
    {
        obj[pp] = pp;
        obj[qq] = qq;
    }
};

//嵌套Json
struct Mytest
{
    int id;
    std::string name;
    Base q;

    void _to_json(zjpJson::Object &obj)
    {
        zjpJson::Object tmp((zjpJson::dict_t())); 
        q._to_json(tmp);          
        obj[Base] = tmp; //外层结构体构建键值对
        obj[id] = id;
        obj[name] = name;
    }

    void _from_json(zjpJson::Object & obj)
    {
        //外层
        id = obj[id].Value<int>();
        name = obj[name].Value<string>();
        //内层
        q._from_json(obj[Base]);
    }
};
```

---

