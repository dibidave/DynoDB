#include "relation.h"

Relation::Relation(quint32 id, QString name, QList<Class*> classes)
    : Class(id, name)
{
    classes_ = classes;
}

