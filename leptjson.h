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
        PARSE_INVALID_UNICODE_HEX,
        PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
        PARSE_MISS_KEY,
        PARSE_MISS_COLON,
        PARSE_MISS_COMMA_OR_CURLY_BRACKET
    };

    json(): vp(std::make_unique<Value>()), c() {}

    struct Member;
    struct Value {
        Value(): type(_NULL) {}
        ~Value() { free(); }

        void free();

        Type   get_type() const          { return type; }

        void   set_null()                { free(); }

        int    get_boolean() const       { assert(type == _TRUE || type == _FALSE); return type == _TRUE; }
        void   set_boolean(int b)        { free(); type = b ? _TRUE : _FALSE; }

        double get_number() const        { assert(type == _NUMBER); return n; }
        void   set_number(double d)      { free(); n = d; type = _NUMBER; } 

        const char* get_string() const   { assert(type == _STRING); return s; }
        size_t get_string_length() const { assert(type == _STRING); return len; }
        void   set_string(const char* s, size_t len);
        
        size_t get_array_size() const    { assert(type == _ARRAY); return size; }
        Value* get_array_element(size_t index) const
                                         { assert(type == _ARRAY); assert(index < size); return &e[index]; }

        size_t get_object_size() const { assert(type == _OBJECT); return msize; } 
        const char* get_object_key(size_t index) const { assert(type == _OBJECT); assert(index < msize); return m[index].k;}
        size_t get_object_key_length(size_t index) const {assert(type == _OBJECT); assert(index < msize); return m[index].klen; }
        Value* get_object_value(size_t index) const { assert(type == _OBJECT); assert(index < msize); return &m[index].v; } 

        union {
            struct { Member* m; size_t msize; }; // member
            struct { Value* e; size_t size; };  // array
            struct { char* s; size_t len; };    // string
            double n;   // number
        };
        Type type;
    };

    struct Context {
        Context(): json(nullptr), stack(nullptr), size(0), top(0) {}

        void init() { json = nullptr; stack = nullptr; size = top = 0; }

        void* context_push(size_t size);
        void* context_pop(size_t size) { assert(top >= size); return stack + (top -= size); }

        const char* json;
        char* stack;
        size_t size, top;
    };

    struct Member {
        char* k; size_t klen;
        Value v;
    };

    Status parse(const char* json);

    void init() { vp->free(); vp.reset(new Value()); c.init(); }

    Type get_type() const { return vp->type; }

    void   set_null()                { vp->set_null(); } 

    int    get_boolean() const       { return vp->get_boolean(); }
    void   set_boolean(int b)        { vp->set_boolean(b); }

    double get_number() const        { return vp->get_number(); }
    void   set_number(double d)      { vp->set_number(d); }

    const char* get_string() const   { return vp->get_string(); }
    size_t get_string_length() const { return vp->get_string_length(); }
    void   set_string(const char* s, size_t len) { vp->set_string(s, len); }

    size_t get_array_size() const    { return vp->get_array_size(); }
    Value* get_array_element(size_t index) const
                                     { return vp->get_array_element(index); } 

    size_t get_object_size() const { return vp->get_object_size(); }
    const char* get_object_key(size_t index) const { return vp->get_object_key(index); }
    size_t get_object_key_length(size_t index) const { return vp->get_object_key_length(index); }
    Value* get_object_value(size_t index) const { return vp->get_object_value(index); }

private:
    void EXPECT(char ch) {
        assert(*c.json == (ch));
        ++c.json;
    }
    
    void   parse_whitespace(); 
    Status parse_literal(const char *target, Type type);
    Status parse_value();
    Status parse_number();
    Status parse_string_raw(char** ps, size_t *plen);
    Status parse_string();
    const char* parse_hex4(const char* p, unsigned* u);
    void encode_utf8(unsigned u);
    Status parse_array();
    Status parse_object();

    std::unique_ptr<Value> vp;
    Context c;
};

}

#endif  /*  LEPTJSON_H__  */
