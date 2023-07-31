#include "leptjson.h"
#include <iostream>
#include <array>
#include <typeinfo>

using namespace leptjson;

json j;

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;
 
std::array<const char*, 7> arrType = { 
    "_NULL", "_FALSE", "_TRUE", "_NUMBER", "_STRING", "_ARRAY", "_OBJECT"
}; 
std::array<const char*, 4> arrStatus = {
    "PARSE_OK", "PARSE_EXPECT_VALUE", "PARSE_INVALID_VALUE", "PARSE_ROOT_NOT_SINGULAR"
}; 

using Type = leptjson::json::Type;
using Status = leptjson::json::Status;

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
    EXPECT_EQ(Status::PARSE_OK, j.parse("null"));
    EXPECT_EQ(Type::_NULL, j.get_type());
}

static void test_parse_true() {
    EXPECT_EQ(Status::PARSE_OK, j.parse("true"));
    EXPECT_EQ(Type::_NULL, j.get_type());
}

static void test_parse_false() {
    EXPECT_EQ(Status::PARSE_OK, j.parse("false"));
    EXPECT_EQ(Type::_NULL, j.get_type());
}

static void test_parse_expect_value() {
    EXPECT_EQ(Status::PARSE_EXPECT_VALUE, j.parse(""));
    EXPECT_EQ(Type::_NULL, j.get_type());
    j.clear();
    EXPECT_EQ(Status::PARSE_EXPECT_VALUE, j.parse(" "));
    EXPECT_EQ(Type::_NULL, j.get_type());
}

static void test_parse_invalid_value() {
    EXPECT_EQ(Status::PARSE_INVALID_VALUE, j.parse("nul"));
    EXPECT_EQ(Type::_NULL, j.get_type());
    j.clear();
    EXPECT_EQ(Status::PARSE_INVALID_VALUE, j.parse("?"));
    EXPECT_EQ(Type::_NULL, j.get_type());
}

static void test_parse_root_not_singular() {
    EXPECT_EQ(Status::PARSE_ROOT_NOT_SINGULAR, j.parse("null x"));
    EXPECT_EQ(Type::_NULL, j.get_type());
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
