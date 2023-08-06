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
 
static constexpr std::array<const char*, 7> arrType = { 
    "_NULL", "_FALSE", "_TRUE", "_NUMBER", "_STRING", "_ARRAY", "_OBJECT"
}; 
static constexpr std::array<const char*, 11> arrStatus = {
    "PARSE_OK", "PARSE_EXPECT_VALUE", "PARSE_INVALID_VALUE", "PARSE_ROOT_NOT_SINGULAR", "PARSE_NUMBER_TOO_BIG",
    "PARSE_MISS_QUOTATION_MARK", "PARSE_INVALID_STRING_ESCAPE", "PARSE_INVALID_STRING_CHAR",
    "PARSE_INVALID_UNICODE_SURROGATE", "PARSE_INVALID_UNICODE_HEX", "PARSE_MISS_COMMA_OR_SQUARE_BRACKET"
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
    j.init();  \
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



/*----------     Test for number     ----------*/
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
    j.init();  \
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



/*----------     String     ----------*/
#define EXPECT_EQ_STRING(expect, actual, alength) { \
    ++test_count;   \
    if (sizeof(expect) - 1 == (alength) && memcmp(expect, actual, alength) == 0)    \
        ++test_pass;    \
    else {  \
        std::cerr<< __FILE__ << ":" << __LINE__ << ": expect: " \
        << expect << " actual: " << actual << "\n"; \
        main_ret = 1;   \
    }   \
}

#define TEST_STRING(expect, json)   {   \
    j.init();   \
    EXPECT_EQ(Status::PARSE_OK, j.parse(json)); \
    EXPECT_EQ(Type::_STRING, j.get_type());   \
    EXPECT_EQ_STRING(expect, j.get_string(), j.get_string_length());    \
}

static void test_parse_string() {
    TEST_STRING("", "\"\"");
    TEST_STRING("Hello", "\"Hello\"");
    TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    TEST_STRING("Hello\0World", "\"Hello\\u0000World\"");
    TEST_STRING("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
    TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
    TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */

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

static void test_parse_missing_quotation_mark() {
    TEST_ERROR(PARSE_MISS_QUOTATION_MARK, "\"");
    TEST_ERROR(PARSE_MISS_QUOTATION_MARK, "\"abc");
}

static void test_parse_invalid_string_escape() {
    TEST_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
    TEST_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\'\"");
    TEST_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\0\"");
    TEST_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\x12\"");
}

static void test_parse_invalid_string_char() {
    TEST_ERROR(PARSE_INVALID_STRING_CHAR, "\"\x01\"");
    TEST_ERROR(PARSE_INVALID_STRING_CHAR, "\"\x1F\"");
}



static void test_access_null() {
    j.init();
    j.set_string("a", 1);
    j.set_null();
    EXPECT_EQ(Type::_NULL, j.get_type());
}

static void test_access_string() {
    j.init();
    j.set_string("", 0);
    EXPECT_EQ_STRING("", j.get_string(),  j.get_string_length());
    j.set_string("Hello", 5);
    EXPECT_EQ_STRING("Hello", j.get_string(),  j.get_string_length());
}

static void test_access_boolean() {
    j.init();
    j.set_string("a", 1);
    j.set_boolean(1);
    EXPECT_EQ(1, j.get_boolean());
    j.set_boolean(0);
    EXPECT_EQ(0, j.get_boolean());
}

static void test_access_number() {
    j.init();
    j.set_string("a", 1);
    j.set_number(1234.5);
    EXPECT_EQ_NUMBER(1234.5, j.get_number());
}



/*----------     UNICODE     ----------*/
static void test_parse_invalid_unicode_hex() {
    TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u0\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u01\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u012\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u/000\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\uG000\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u0/00\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u0G00\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u00/0\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u00G0\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u000/\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u000G\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u 123\"");
}

static void test_parse_invalid_unicode_surrogate() {
    TEST_ERROR(PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_SURROGATE, "\"\\uDBFF\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\\\\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uDBFF\"");
    TEST_ERROR(PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uE000\"");
}



/*----------     ARRAY     ----------*/
static void test_parse_array() {
    j.init();
    EXPECT_EQ(Status::PARSE_OK, j.parse("[ ]"));
    EXPECT_EQ(Type::_ARRAY, j.get_type());
    EXPECT_EQ(0, j.get_array_size());
    
    TEST_ERROR(PARSE_INVALID_VALUE, "[1,]");
    TEST_ERROR(PARSE_INVALID_VALUE, "[\"a\", nul]");

    size_t i = 0, i2 = 0;
    j.init();
    EXPECT_EQ(Status::PARSE_OK, j.parse("[ null , false , true , 123 , \"abc\" ]"));
    EXPECT_EQ(Type::_ARRAY, j.get_type());
    EXPECT_EQ(5, j.get_array_size());
    EXPECT_EQ(Type::_NULL,   j.get_array_element(0)->get_type());
    EXPECT_EQ(Type::_FALSE,  j.get_array_element(1)->get_type());
    EXPECT_EQ(Type::_TRUE,   j.get_array_element(2)->get_type());
    EXPECT_EQ(Type::_NUMBER, j.get_array_element(3)->get_type());
    EXPECT_EQ(Type::_STRING, j.get_array_element(4)->get_type());
    EXPECT_EQ(123.0, j.get_array_element(3)->get_number());
    EXPECT_EQ_STRING("abc", j.get_array_element(4)->get_string(), j.get_array_element(4)->get_string_length());

    j.init();
    EXPECT_EQ(Status::PARSE_OK, j.parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]"));
    EXPECT_EQ(Type::_ARRAY, j.get_type());
    EXPECT_EQ(4, j.get_array_size());
    for (i = 0; i < 4; ++i) {
        json::Value* a = j.get_array_element(i);
        EXPECT_EQ(Type::_ARRAY, a->type);
        EXPECT_EQ(i, a->size);
        for (i2 = 0; i2 < i; ++i2) {
            json::Value* e = a->get_array_element(i2);
            EXPECT_EQ(Type::_NUMBER, e->get_type());
            EXPECT_EQ(i2, e->get_number());
        }
    }
}

static void test_parse_miss_comma_or_square_bracket() {
    TEST_ERROR(PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1");
    TEST_ERROR(PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1}");
    TEST_ERROR(PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1 2");
    TEST_ERROR(PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[[]");
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

    test_parse_string();
    test_parse_missing_quotation_mark();
    test_parse_invalid_string_escape();
    test_parse_invalid_string_char();

    test_access_null();
    test_access_string();
    test_access_number();
    test_access_boolean();

    test_parse_invalid_unicode_hex();
    test_parse_invalid_unicode_surrogate();

    test_parse_array();
    test_parse_miss_comma_or_square_bracket();
}

int main() {
    test_parse();
    std::cout << test_pass << "/" << test_count << " ";
    std::cout << std::setprecision(2) << std::fixed << test_pass * 100.0 / test_count << "% passed\n"; 
    return main_ret;
}
