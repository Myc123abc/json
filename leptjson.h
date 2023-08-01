#ifndef LEPTJSON_H__
#define LEPTJSON_H__

#include <cassert>

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
        PARSE_NUMBER_TOO_BIG
    };

    struct Value {
        double n;
        Type type;
    };


    Status parse(const char* json);
    Type get_type() { return v.type; }
    double get_number() { assert(v.type == _NUMBER); return v.n; }

    void clear() { v.type = _FALSE; }

private:
    void EXPECT(char ch) {
        assert(*c.json == (ch));
        ++c.json;
    }
    
    void parse_whitespace(); 
    Status parse_literal(const char *target, Type type);
    Status parse_value();
    Status parse_number();

    struct Context {
        const char* json;
    };

    Value v{ 0.0, _FALSE };
    Context c;
};

}

#endif  /*  LEPTJSON_H__  */
