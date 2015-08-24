#ifndef DATATYPES_H
#define DATATYPES_H

enum BuiltInDataType {
    INVALID_DATA_TYPE = 0,
    GIBLY = 1,
    CLASS = 2,
    INSTANCE = 3,
    NAME = 4,
    HAS_TABLE = 5,
    TYPE = 6,
    SUBCLASS = 7,
    LITERAL = 500,
    BOOL = 501,
    INTEGER = 502,
    DATETIME = 503,
    STRING = 504,
    MAX_LITERAL = 505,
    MAX_DATA_TYPE = 1000
};

#endif // DATATYPES_H

