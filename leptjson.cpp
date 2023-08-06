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
    vp->type = type;
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
    vp->n = strtod(c.json, NULL);
    if (errno == ERANGE && (vp->n == HUGE_VAL || vp->n == -HUGE_VAL))  return PARSE_NUMBER_TOO_BIG;
    c.json = p;
    vp->type = _NUMBER;
    return PARSE_OK;
}

#define PUTC(ch) do { *(char*)c.context_push(sizeof(char)) = (ch); } while(0)
#define STRING_ERROR(ret) do { c.top = head; return ret; } while(0)
leptjson::json::Status leptjson::json::parse_string() {
    size_t head = c.top, len = 0;
    unsigned u = 0, u2 = 0;
    const char* p = nullptr;
    EXPECT('\"');
    p = c.json;
    while (true) {
        char ch = *p++;
        switch(ch) {
            case '\\':
                switch (*p++) {
                    case '\"':  PUTC('\"');    break;
                    case '\\':  PUTC('\\');    break;
                    case '/':   PUTC('/');    break;
                    case 'b':   PUTC('\b');    break;
                    case 'f':   PUTC('\f');    break;
                    case 'n':   PUTC('\n');    break;
                    case 'r':   PUTC('\r');    break;
                    case 't':   PUTC('\t');    break;
                    case 'u':
                        if (!(p = parse_hex4(p, &u)))
                            STRING_ERROR(PARSE_INVALID_UNICODE_HEX);
                        if (u >= 0xD800 && u <= 0xDBFF) {
                            if (*p++ != '\\')
                                STRING_ERROR(PARSE_INVALID_UNICODE_SURROGATE);
                            if (*p++ != 'u')
                                STRING_ERROR(PARSE_INVALID_UNICODE_SURROGATE);
                            if (!(p = parse_hex4(p, &u2)))
                                STRING_ERROR(PARSE_INVALID_UNICODE_HEX);
                            if (u2 < 0xDC00 || u2 > 0xDFFF)
                                STRING_ERROR(PARSE_INVALID_UNICODE_SURROGATE);
                            u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
                        }
                        encode_utf8(u);
                        break;
                    default:
                        STRING_ERROR(PARSE_INVALID_STRING_ESCAPE);
                }   break;
            case '\"':
                len = c.top - head;
                vp->set_string(static_cast<const char*>(c.context_pop(len)), len);
                c.json = p;
                return PARSE_OK;
            case '\0':
                STRING_ERROR(PARSE_MISS_QUOTATION_MARK);
            default:
                if (static_cast<unsigned char>(ch) < 0x20)
                    STRING_ERROR(PARSE_INVALID_STRING_CHAR);
                PUTC(ch);
        }
    }
}

leptjson::json::Status leptjson::json::parse_value() {
    switch (*c.json) {
        case 'n':   return parse_literal("null", _NULL);
        case 't':   return parse_literal("true", _TRUE);
        case 'f':   return parse_literal("false", _FALSE); 
        default:    return parse_number();
        case '"':   return parse_string();
        case '[':   return parse_array();
        case '\0':  return PARSE_EXPECT_VALUE;
    }
}

leptjson::json::Status leptjson::json::parse(const char* json) {
    Status ret;
    init();
    c.json = json;
    parse_whitespace();
    if ((ret = parse_value()) == PARSE_OK) {
        parse_whitespace();
        if (*c.json != '\0') {
            vp->type = _NULL;
            ret = PARSE_ROOT_NOT_SINGULAR;
        }
    }
    assert(c.top == 0);
    delete []c.stack;
    return ret;
}

void leptjson::json::Value::set_string(const char* s2, size_t len2) {
    assert(s2 != nullptr || len2 == 0);
    free();
    s = new char[len2 + 1];
    memcpy(s, s2, len2);
    s[len2] = '\0';
    len = len2;
    type = _STRING;
}


void* leptjson::json::Context::context_push(size_t size2) {
#ifndef PARSE_STACK_INIT_SIZE
#define PARSE_STACK_INIT_SIZE   256
    void* ret = nullptr;
    assert(size2 > 0);
    if (top + size2 >= size) {
        if (size == 0)    size = PARSE_STACK_INIT_SIZE;
        while (top + size2 >= size)
            size += size >> 1;
        stack = new char[size];
    }
    ret = stack + top;
    top += size2;
    return ret;
#undef  PARSE_STACK_INIT_SIZE
#endif
}

const char* leptjson::json::parse_hex4(const char* p, unsigned* u) {
    int i = 0;
    *u = 0;
    for (; i < 4; ++i) {
        char ch = *p++;
        *u <<= 4;
        if      (ch >= '0' && ch <= '9') *u |= ch - '0';
        else if (ch >= 'A' && ch <= 'F') *u |= ch - ('A' - 10);
        else if (ch >= 'a' && ch <= 'f') *u |= ch - ('a' - 10);
        else return NULL;
    }
    return p;
}

void leptjson::json::encode_utf8(unsigned u) {
    if (u <= 0x7F)
        PUTC(u & 0xFF);
    else if (u <= 0x7FF) {
        PUTC(0xC0 | ((u >> 6) & 0xFF));
        PUTC(0x80 | ( u       & 0x3F));
    }
    else if (u <= 0xFFFF) {
        PUTC(0xE0 | ((u >> 12) & 0xFF)); 
        PUTC(0x80 | ((u >>  6) & 0x3F)); 
        PUTC(0x80 | ( u        & 0x3F)); 
    }
    else {
        assert(u <= 0x10FFFF);
        PUTC(0xF0 | ((u >> 18) & 0xFF)); 
        PUTC(0x80 | ((u >> 12) & 0x3F)); 
        PUTC(0x80 | ((u >>  6) & 0x3F)); 
        PUTC(0x80 | ( u        & 0x3F)); 
    }
}

leptjson::json::Status leptjson::json::parse_array() {
    size_t i = 0, size = 0;
    Status ret;
    EXPECT('[');
    parse_whitespace();
    if (*c.json == ']') {
        ++c.json;
        vp->type = _ARRAY;
        vp->size = 0;
        vp->e = nullptr;
        return PARSE_OK;
    }
    while (true) {
        if ((ret = parse_value()) != PARSE_OK)
            break;
        memcpy(c.context_push(sizeof(Value)), vp.get(), sizeof(Value));
        vp->type = _NULL;
        ++size;
        parse_whitespace();
        if (*c.json == ',') {
            ++c.json;
            parse_whitespace();
        }
        else if (*c.json == ']') {
            ++c.json;
            vp->type = _ARRAY;
            vp->size = size;
            memcpy(vp->e = new Value[size], c.context_pop(size * sizeof(Value)), size * sizeof(Value));
            return PARSE_OK;
        } else {
            ret = PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
            break;
        }
    }
    for (i = 0; i < size; ++i)
        static_cast<Value*>(c.context_pop(sizeof(Value)))->free();
    return ret;
}

void leptjson::json::Value::free() {
    size_t i = 0;
    switch (type) {
        case _STRING:   
            delete []s; break;
        case _ARRAY:
            for (; i < size; ++i)
                e[i].free();
            delete []e;
            break;
        default: break;
    }
    type = _NULL;
}
