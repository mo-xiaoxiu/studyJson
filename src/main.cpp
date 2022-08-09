#include <iostream>
#include "object.hpp"
#include "parse.hpp"
#include <fstream>
#include "Timer.hpp"

using namespace zjpJson;

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

void test_class_serialization()
{
    Mytest test{.id = 32, .name = "fda"};
    auto item = Parser::FromJson<Mytest>(R"({"base":{"pp":0,"qq":""},"id":32,"name":"fda"} )"); // serialization
    std::cout << Parser::ToJSON(item);                                                          // deserialization
}

void test_string_parser()
{
    {
        Timer t;
        std::ifstream fin("../../test_source/test.json");
        std::string text((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
        zjpJson::Parser p;
        p.init(text);
        auto q = p.parse();
        std::ofstream fout("../../test_source/test_out.json");
        fout << q.to_string();
    }
}

int main()
{
    std::variant<string, int> var;
    var = "falsjslf";
    auto &p = std::get<string>(var);
    p = "测试匹配";
    auto &pp = std::get<string>(var);
    std::cout << pp;

    return 0;
}
