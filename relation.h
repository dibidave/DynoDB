#ifndef RELATION_H
#define RELATION_H

#include <QObject>

#include "class.h"

class Relation : public Class
{
    Q_OBJECT
public:
    Relation(quint32 id, QString name, QList<Class*> classes);

private:
    QList<Class*> classes_;
};

#endif // RELATION_H
