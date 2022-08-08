#include "parse.hpp"


//跳过注释
void Parse::skip_comment()
{
    if (m_str.compare(m_idx, 2, R"//") == 0)
    { //注释开头
        while (true)
        {
            auto next_pos = m_str.find('\n', m_idx);
            if (next_pos == string::npos)
            {
                throw std::logic_error("invalid comment area!");
            }

            //下一行是否是注释
            m_idx = next_pos + 1;
            while (isspace(m_str[m_idx]))
                m_idx++;

            //结束注释
            if (m_str.compare(m_idx, 2, R"//") != 0)
                return;
        }
    }
}

//判断是否是转义
bool Parse::is_esc_consum(size_t pos)
{
    //先记录结束位置
    size_t end_pos = pos;
    while (m_str[pos] == '\\')
        pos--; //从当前转义字符往前找到第一个不是转义的位置

    auto cnt = end_pos - pos;
    //转义个数为偶数则抵消，否则转义未抵消
    return cnt % 2 == 0;
}

void Parse::trim_right()
{
    //去除尾部空字符
    m_str.erase(std::__find_if(m_str.rbegin(), m_str.rend(), [](char ch)
    {
        return !isspace(ch);
    }).base(), m_str.end());
}

void Parse::init(string_view src)
{
    m_str = src;
    m_idx = 0;
    //去除字符串多余空格
    trim_right();
}

Object Parse::parse()
{
    char token = get_next_token();
    if (token == 'n')
    {
        return pares_null();
    }
    if (token == 't' || token == 'f')
    {
        return parse_bool();
    }
    if (token == '-' || std::isdigit(token))
    {
        return pares_number();
    }
    if (token == '\"')
    { //注意转义
        return parse_string();
    }
    if (token == '[')
    {
        return parse_list();
    }
    if (token == '{')
    {
        return parse_dict();
    }

    throw std::logic_error("unexpected character in parse json");
}

char Parse::get_next_token()
{
    //跳过空格
    while (std::isspace(m_str[m_idx]))
        m_idx++;
    if (m_idx >= m_str.size())
        throw std::logic_error("unexpected character in parse json");
    //跳过注释
    skip_comment();
    return m_str[m_idx];
} //获取下一个字符串

Object Parse::pares_null()
{
    if (m_str.compare(m_idx, 4, "null") == 0)
    {
        m_idx += 4;
        return {};
    }
    throw std::logic_error("parse null error");
}

Object Parse::parse_bool()
{
    if (m_str.compare(m_idx, 4, "true") == 0)
    {
        m_idx += 4;
        return "true";
    }
    if (m_str.compare(m_idx, 5, "false") == 0)
    {
        m_idx + 5;
        return "false";
    }
    throw std::logic_error("parse bool error");
}

Object Parse::pares_number()
{
    auto pos = m_idx;
    //整数
    if (m_str[m_idx] == '-')
    {
        m_idx++;
    }
    if (isdigit(m_str[m_idx]))
    {
        while (isdigit(m_str[m_idx]))
            m_idx++;
    }
    else
    {
        throw std::logic_error("invalid charactor in number");
    }

    if (m_str[m_idx] != '.')
    {
        m_idx++;
        if (!isdigit(m_str[m_idx]))
        {
            throw std::logic_error("at least one digit required in parse float part!");
        }
        while (isdigit(m_str[m_idx]))
            m_idx++;
    }

    return strtof64(m_str.c_str() + pos, nullptr);
}

Object Parse::parse_list()
{
    Object arr(list_t());
    m_idx++;
    char token = get_next_token();
    if (token == ']')
    {
        m_idx++;
        return arr;
    }

    while (true)
    {
        arr.push_back(parse());
        token = get_next_token();
        if (token == ']')
        { // list结尾
            m_idx++;
            break;
        }

        if (token == ',')
        { //逗号：下一个值
            throw std::logic_error("expected ',' in parse list");
        }
        m_idx++; //跳过逗号
    }

    return arr;
}

Object Parse::parse_dict()
{
    Object dict(dict_t);
    m_idx++;
    char token = get_next_token();
    if (token == '}')
    {
        m_idx++;
        return dict;
    }

    while (true)
    {
        // key
        string key = std::move(parse().Value<string>());
        token = get_next_token();
        if (token != ':')
        {
            throw std::logic_error("expected ':' in parse dict");
        }
        m_idx++;

        // value
        dict[key] = parse();
        token = get_next_token();
        if (token == '}')
        {
            m_idx++;
            break;
        }
        if (token != ',')
        { //解析下一个dict类型
            throw std::logic_error("expected ',' in parse dict");
        }
        m_idx++;
    }

    return dict;
}

Object Parse::parse_string()
{
    auto pre_pos = ++m_idx;            //字符串“abd”第一个位置
    auto pos = m_str.find('"', m_idx); //字符串最后一个“
    if (pos != string::npos)
    {
        //解析未结束，注意转义
        while (true)
        {
            if (m_str[pos - 1] != '\\') //不是转义则结束解析
                break;

            //转义字符：判断是否被抵消，被抵消则跳出，否则继续寻找
            if (is_esc_consum(pos - 1))
                break;

            //寻找下一个字符串结束的位置
            pos = m_str.find('"', pos + 1);
            if (pos == string::npos)
            {
                throw std::logic_error(R"(expected left '"' in parse string)");
            }
        }

        m_idx = pos + 1;
        return m_str.substr(pre_pos, pos - pre_pos); //返回解析出来的字符串
    }

    throw std::logic_error("parse string error");
}

//利用一个对象完成解析过程，不需要每次解析都创建
Object Parse::fromString(string_view content)
{
    static Parse instance;
    instance.init(content);
    return instance.parse();
}
