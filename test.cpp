#include "leptjson.h"
#include <iostream>
#include <array>
#include <typeinfo>

using namespace leptjson;

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;
 
std::array<const char*, 7> arrType = { 
    "_NULL", "_FALSE", "_TRUE", "_NUMBER", "_STRING", "_ARRAY", "_OBJECT"
}; 
std::array<const char*, 4> arrStatus = {
    "PARSE_OK", "PARSE_EXPECT_VALUE", "PARSE_INVALID_VALUE", "PARSE_ROOT_NOT_SINGULAR"
}; 


#define EXPECT_EQ(expect, actual) { \
    ++test_count;                   \
    if (expect == actual)           \
        ++test_pass;                \
    else {                          \
        std::cerr << __FILE__ << ":" << __LINE__ << ": expect: ";   \
        if (typeid(expect) == typeid(Type))     \
            std::cerr << arrType[expect];       \
        if (typeid(expect) == typeid(Status))   \
            std::cerr << arrStatus[expect];     \
        std::cerr  << " actual: ";              \
        if (typeid(actual) == typeid(Type))     \
            std::cerr << arrType[actual];       \
        if (typeid(actual) == typeid(Status))   \
            std::cerr << arrStatus[actual];     \
        std::cerr << "\n";          \
        main_ret = 1;               \
    }                               \
}

static void test_parse_null() {
    Value v;
    v.type = _FALSE; 
    EXPECT_EQ(PARSE_OK, parse(&v, "null"));
    EXPECT_EQ(_NULL, get_type(&v));
}

static void test_parse_true() {
    Value v;
    v.type = _FALSE;
    EXPECT_EQ(PARSE_OK, parse(&v, "true"));
    EXPECT_EQ(_NULL, get_type(&v));
}

static void test_parse_false() {
    Value v;
    v.type = _FALSE;
    EXPECT_EQ(PARSE_OK, parse(&v, "false"));
    EXPECT_EQ(_NULL, get_type(&v));
}

static void test_parse_expect_value() {
    Value v;
    v.type = _FALSE;
    EXPECT_EQ(PARSE_EXPECT_VALUE, parse(&v, ""));
    EXPECT_EQ(_NULL, get_type(&v));

    v.type = _FALSE;
    EXPECT_EQ(PARSE_EXPECT_VALUE, parse(&v, " "));
    EXPECT_EQ(_NULL, get_type(&v));
}

static void test_parse_invalid_value() {
    Value v;
    v.type = _FALSE;
    EXPECT_EQ(PARSE_INVALID_VALUE, parse(&v, "nul"));
    EXPECT_EQ(_NULL, get_type(&v));

    v.type = _FALSE;
    EXPECT_EQ(PARSE_INVALID_VALUE, parse(&v, "?"));
    EXPECT_EQ(_NULL, get_type(&v));
}

static void test_parse_root_not_singular() {
    Value v;
    v.type = _FALSE;
    EXPECT_EQ(PARSE_ROOT_NOT_SINGULAR, parse(&v, "null x"));
    EXPECT_EQ(_NULL, get_type(&v));
}

static void test_parse() {
    test_parse_null();
    test_parse_false();
    test_parse_true();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
}

int main() {
    test_parse();
    std::cout << test_pass << "/" << test_count << " ";
    std::cout.precision(4);
    std::cout << test_pass * 100.0 / test_count << "% passed\n"; 
    return main_ret;
}
