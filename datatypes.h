#ifndef DATATYPES_H
#define DATATYPES_H

#include <QMap>
#include <QString>

enum BuiltInDataType {
    // Maybe gibly should be type 0? That way statements like "Everything is a gibly"
    // are intrinsically true
    INVALID_DATA_TYPE = 0,
    GIBLY = 1,
    CLASS = 2,
    INSTANCE = 3,
    NAME = 4,
    HAS_TABLE = 5,
    TYPE = 6,
    SUBCLASS = 7,
    RELATION = 8,
    IS_COLUMN = 9,
    GROUP = 10,
    DIRECTION = 11,
    QUANTITY = 12,

    // Literal classes
    LITERAL = 500,
    BOOL = 501,
    INTEGER = 502,
    DATETIME = 503,
    STRING = 504,
    MAX_DATA_TYPE = 1000
};

BuiltInDataType const BuiltInDataTypeValues[] =
{
    GIBLY,
    CLASS,
    INSTANCE,
    NAME,
    HAS_TABLE,
    TYPE,
    SUBCLASS,
    RELATION,
    IS_COLUMN,
    GROUP,
    DIRECTION,
    QUANTITY,
    LITERAL,
    BOOL,
    INTEGER,
    DATETIME,
    STRING,
    MAX_DATA_TYPE
};

QMap<BuiltInDataType, QString> const BuiltInDataTypeNames =
{
    {GIBLY, "Gibly"},
    {CLASS, "Class"},
    {INSTANCE, "Instance"},
    {NAME, "Name"},
    {HAS_TABLE, "HasTable"},
    {TYPE, "Type"},
    {SUBCLASS, "Subclass"},
    {RELATION, "Relation"},
    {IS_COLUMN, "IsColumn"},
    {GROUP, "Group"},
    {DIRECTION, "Direction"},
    {QUANTITY, "Quantity"},
    {LITERAL, "Literal"},
    {BOOL, "Bool"},
    {INTEGER, "Integer"},
    {DATETIME, "DateTime"},
    {STRING, "String"}
};

#endif // DATATYPES_H

