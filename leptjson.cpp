#include "leptjson.h"
#include <cassert>

using namespace leptjson;

struct Context {
    const char *json;
};

inline static void EXPECT(Context *c, char ch) {
    assert(*c->json == (ch));
    ++(c->json);
}

static void parse_whitespace(Context *c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        ++p;
    c->json = p;
}

static Status parse_null(Context *c, Value *v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = _NULL;
    return PARSE_OK;
}

static Status parse_true(Context *c, Value *v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = _NULL;
    return PARSE_OK;
}

static Status parse_false(Context *c, Value *v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = _NULL;
    return PARSE_OK;
}

static Status parse_value(Context *c, Value *v) {
    switch (*c->json) {
        case 'n':   return parse_null(c, v);
        case 't':   return parse_true(c, v);
        case 'f':   return parse_false(c, v);
        case '\0':  return PARSE_EXPECT_VALUE;
        default:    return PARSE_INVALID_VALUE;
    }
}

Status leptjson::parse(Value *v, const char *json) {
    Context c;
    Status ret;
    assert(v != nullptr);
    c.json = json;
    v->type = _NULL;
    parse_whitespace(&c);
    if ((ret = parse_value(&c, v)) == PARSE_OK) {
        parse_whitespace(&c);
        if (*c.json != '\0')
            ret = PARSE_ROOT_NOT_SINGULAR;
    }
    return ret;
}

Type leptjson::get_type(const Value *v) {
    assert(v != nullptr);
    return v->type; 
}
