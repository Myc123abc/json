#ifndef LEPTJSON_H__
#define LEPTJSON_H__

#include <cassert>
#include <cstddef>
#include <memory>
#include <cstring>

namespace leptjson {

class json {
public:
    enum Type {
        _NULL, _FALSE, _TRUE, _NUMBER, _STRING, _ARRAY, _OBJECT
    };

    enum Status {
        PARSE_OK,
        PARSE_EXPECT_VALUE,
        PARSE_INVALID_VALUE,
        PARSE_ROOT_NOT_SINGULAR,
        PARSE_NUMBER_TOO_BIG,
        PARSE_MISS_QUOTATION_MARK,
        PARSE_INVALID_STRING_ESCAPE,
        PARSE_INVALID_STRING_CHAR,
        PARSE_INVALID_UNICODE_SURROGATE,
        PARSE_INVALID_UNICODE_HEX
    };

    json(): vp(std::make_unique<Value>()), c() {}

    struct Value {
        Value(): type(_NULL) {}
        ~Value() { free(); }

        void free() { if (this->type == _STRING) delete []this->s; this->type = _NULL; }

        union {
            struct { char* s; size_t len; };    // string
            double n;   // number
        };
        Type type;
    };

    struct Context {
        Context(): json(nullptr), stack(nullptr), size(0), top(0) {}
        void init() { json = nullptr; stack = nullptr; size = top = 0; }
        const char* json;
        char* stack;
        size_t size, top;
    };

    Status parse(const char* json);

    void        init()              { vp->free(); vp->type = _NULL; c.init(); }

    Type        get_type() const    { return vp->type; }

    void        set_null()          { vp->free(); }

    int         get_boolean() const { assert(vp->type == _TRUE || vp->type == _FALSE); return vp->type == _TRUE; }
    void        set_boolean(int b)  { vp->free(); vp->type = b ? _TRUE : _FALSE; }

    double      get_number() const  { assert(vp->type == _NUMBER); return vp->n; }
    void        set_number(double n){ vp->free(); vp->n = n; vp->type = _NUMBER; } 

    const char* get_string() const  { assert(vp->type == _STRING); return vp->s; }
    size_t      get_string_length() const { assert(vp->type == _STRING); return vp->len; }
    void        set_string(const char* s, size_t len);

    void* context_push(size_t size);
    void* context_pop(size_t size) { assert(c.top >= size); return c.stack + (c.top -= size); }

private:
    void EXPECT(char ch) {
        assert(*c.json == (ch));
        ++c.json;
    }
    
    void   parse_whitespace(); 
    Status parse_literal(const char *target, Type type);
    Status parse_value();
    Status parse_number();
    Status parse_string();
    const char* parse_hex4(const char* p, unsigned* u);
    void encode_utf8(unsigned u);

    std::unique_ptr<Value> vp;
    Context c;
};

}

#endif  /*  LEPTJSON_H__  */
