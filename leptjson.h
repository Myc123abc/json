#ifndef LEPTJSON_H__
#define LEPTJSON_H__

namespace leptjson {
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


    Status parse(Value *v, const char *json);
    Type get_type(const Value *v);
}

#endif  /*  LEPTJSON_H__  */
