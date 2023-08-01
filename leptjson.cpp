#include "leptjson.h"
#include <cstdlib>
#include <cerrno>
#include <cmath>

void leptjson::json::parse_whitespace() {
    const char *p = c.json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        ++p;
    c.json = p;
}

leptjson::json::Status leptjson::json::parse_literal(const char* target, Type type) {
    EXPECT(target[0]);
    size_t i = 0; 
    for (; target[i + 1]; ++i) {
        if (c.json[i] != target[i + 1])
            return PARSE_INVALID_VALUE;
    }
    c.json += i;
    v.type = type;
    return PARSE_OK;
}

leptjson::json::Status leptjson::json::parse_number() {
    auto ISDIGIT = [](const char c) { 
        if (c >= '0' && c <= '9')   return true;
        else    return false;
    };
    auto ISDIGIT1TO9 = [](const char c) { 
        if (c >= '1' && c <= '9')   return true;
        else    return false;
    };

    const char* p = c.json;

    if (*p == '-') ++p;
    if (*p == '0') ++p;
    else {
        if (!ISDIGIT1TO9(*p))   return PARSE_INVALID_VALUE;
        while (ISDIGIT(*++p));
    }
    if (*p == '.') {
        ++p;
        if (!ISDIGIT(*p))    return PARSE_INVALID_VALUE;
        while (ISDIGIT(*++p));
    }
    if (*p == 'e' || *p == 'E') {
        ++p;
        if (*p == '+' || *p == '-') ++p;
        if (!ISDIGIT(*p))    return PARSE_INVALID_VALUE;
        while (ISDIGIT(*++p));
    }
    
    errno = 0;
    v.n = strtod(c.json, NULL);
    if (errno == ERANGE && (v.n == HUGE_VAL || v.n == -HUGE_VAL))  return PARSE_NUMBER_TOO_BIG;
    c.json = p;
    v.type = _NUMBER;
    return PARSE_OK;
}

leptjson::json::Status leptjson::json::parse_value() {
    switch (*c.json) {
        case 'n':   return parse_literal("null", _NULL);
        case 't':   return parse_literal("true", _TRUE);
        case 'f':   return parse_literal("false", _FALSE); 
        default:    return parse_number();
        case '\0':  return PARSE_EXPECT_VALUE;
    }
}

leptjson::json::Status leptjson::json::parse(const char* json) {
    Status ret;
    c.json = json;
    v.type = _NULL;
    parse_whitespace();
    if ((ret = parse_value()) == PARSE_OK) {
        parse_whitespace();
        if (*c.json != '\0') {
            v.type = _NULL;
            ret = PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}
