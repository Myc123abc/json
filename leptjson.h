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
        PARSE_ROOT_NOT_SINGULAR
    };

    struct Value {
        Type type;
    };


    Status parse(const char *json);
    Type get_type() { return v.type; }

    void clear() { v.type = _FALSE; }

private:
    void EXPECT(char ch) {
        assert(*c.json == (ch));
        ++c.json;
    }
    
    void parse_whitespace(); 
    Status parse_null();
    Status parse_true();
    Status parse_false();
    Status parse_value();

    struct Context {
        const char *json;
    };

    Value v = { _FALSE };
    Context c;
};

}

#endif  /*  LEPTJSON_H__  */
