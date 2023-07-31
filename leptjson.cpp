#include "leptjson.h"

void leptjson::json::parse_whitespace() {
    const char *p = c.json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        ++p;
    c.json = p;
}

leptjson::json::Status leptjson::json::parse_null() {
    EXPECT('n');
    if (c.json[0] != 'u' || c.json[1] != 'l' || c.json[2] != 'l')
        return PARSE_INVALID_VALUE;
    c.json += 3;
    v.type = _NULL;
    return PARSE_OK;
}

leptjson::json::Status leptjson::json::parse_true() {
    EXPECT('t');
    if (c.json[0] != 'r' || c.json[1] != 'u' || c.json[2] != 'e')
        return PARSE_INVALID_VALUE;
    c.json += 3;
    v.type = _NULL;
    return PARSE_OK;
}

leptjson::json::Status leptjson::json::parse_false() {
    EXPECT('f');
    if (c.json[0] != 'a' || c.json[1] != 'l' || c.json[2] != 's' || c.json[3] != 'e')
        return PARSE_INVALID_VALUE;
    c.json += 4;
    v.type = _NULL;
    return PARSE_OK;
}

leptjson::json::Status leptjson::json::parse_value() {
    switch (*c.json) {
        case 'n':   return parse_null();
        case 't':   return parse_true();
        case 'f':   return parse_false();
        case '\0':  return PARSE_EXPECT_VALUE;
        default:    return PARSE_INVALID_VALUE;
    }
}

leptjson::json::Status leptjson::json::parse(const char *json) {
    Status ret;
    c.json = json;
    v.type = _NULL;
    parse_whitespace();
    if ((ret = parse_value()) == PARSE_OK) {
        parse_whitespace();
        if (*c.json != '\0')
            ret = PARSE_ROOT_NOT_SINGULAR;
    }
    return ret;
}
