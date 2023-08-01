#include "leptjson.h"
#include <iostream>
#include <array>
#include <typeinfo>
#include <iomanip>
#include <ios>

using namespace leptjson;

json j;

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;
 
std::array<const char*, 7> arrType = { 
    "_NULL", "_FALSE", "_TRUE", "_NUMBER", "_STRING", "_ARRAY", "_OBJECT"
}; 
std::array<const char*, 5> arrStatus = {
    "PARSE_OK", "PARSE_EXPECT_VALUE", "PARSE_INVALID_VALUE", "PARSE_ROOT_NOT_SINGULAR", "PARSE_NUMBER_TOO_BIG"
}; 

using Type = leptjson::json::Type;
using Status = leptjson::json::Status;


#define EXPECT_EQ(expect, actual) { \
    ++test_count;                   \
    if (expect == actual)           \
        ++test_pass;                \
    else {                          \
        std::cerr << __FILE__ << ":" << std::setw(3) << std::left << __LINE__ << ": expect: " << std::setw(23) << std::left;   \
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

#define TEST_ERROR(error, json) {   \
    j.clear();  \
    EXPECT_EQ(Status::error, j.parse(json));   \
    EXPECT_EQ(Type::_NULL, j.get_type());   \
}



/*----------   Test for null, true and false   ----------*/

static void test_parse_null() {
    EXPECT_EQ(Status::PARSE_OK, j.parse("null"));
    EXPECT_EQ(Type::_NULL, j.get_type());
}

static void test_parse_true() {
    EXPECT_EQ(Status::PARSE_OK, j.parse("true"));
    EXPECT_EQ(Type::_TRUE, j.get_type());
}

static void test_parse_false() {
    EXPECT_EQ(Status::PARSE_OK, j.parse("false"));
    EXPECT_EQ(Type::_FALSE, j.get_type());
}



/*----------    Test for number     ----------*/
#define EXPECT_EQ_NUMBER(expect, actual) { \
    ++test_count;                   \
    if (expect == actual)           \
        ++test_pass;                \
    else {                          \
        std::cerr << __FILE__ << ":" << __LINE__ << ": expect: "    \
        << expect << " actual: " << actual << "\n";     \
        main_ret = 1;   \
    }                   \
}
#define TEST_NUMBER(expect, json) { \
    j.clear();  \
    EXPECT_EQ(Status::PARSE_OK, j.parse(json));   \
    EXPECT_EQ(Type::_NUMBER, j.get_type());       \
    EXPECT_EQ_NUMBER(expect, j.get_number());     \
}

static void test_parse_number() {
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000");

    TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
    TEST_NUMBER( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TEST_NUMBER( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
    TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_NUMBER( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
    TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_NUMBER( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
    TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

static void test_parse_number_too_big() {
    TEST_ERROR(PARSE_NUMBER_TOO_BIG, "1e309");
    TEST_ERROR(PARSE_NUMBER_TOO_BIG, "-1e309");
}



static void test_parse_expect_value() {
    TEST_ERROR(PARSE_EXPECT_VALUE, "");
    TEST_ERROR(PARSE_EXPECT_VALUE, " ");
}

static void test_parse_invalid_value() {
    TEST_ERROR(PARSE_INVALID_VALUE, "nul");
    TEST_ERROR(PARSE_INVALID_VALUE, "?");
    
    TEST_ERROR(PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(PARSE_INVALID_VALUE, "+1");
    TEST_ERROR(PARSE_INVALID_VALUE, ".123");
    TEST_ERROR(PARSE_INVALID_VALUE, "1.");
    TEST_ERROR(PARSE_INVALID_VALUE, "INF");
    TEST_ERROR(PARSE_INVALID_VALUE, "inf");
    TEST_ERROR(PARSE_INVALID_VALUE, "NAN");
    TEST_ERROR(PARSE_INVALID_VALUE, "nan");
}

static void test_parse_root_not_singular() {
    TEST_ERROR(PARSE_ROOT_NOT_SINGULAR, "null x");
        
    TEST_ERROR(PARSE_ROOT_NOT_SINGULAR, "0123");
    TEST_ERROR(PARSE_ROOT_NOT_SINGULAR, "0x0");
    TEST_ERROR(PARSE_ROOT_NOT_SINGULAR, "0x123");
}

static void test_parse() {
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();

    test_parse_number();
    test_parse_number_too_big();
}

int main() {
    test_parse();
    std::cout << test_pass << "/" << test_count << " ";
    std::cout << std::setprecision(2) << std::fixed << test_pass * 100.0 / test_count << "% passed\n"; 
    return main_ret;
}
