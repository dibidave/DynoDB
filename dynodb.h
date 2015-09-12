#ifndef DYNO_DB_H
#define DYNO_DB_H

#include <QtSql>

#include "datatypes.h"

class Predicate;
class PredicateElement;

#include "class.h"
#include "relation.h"

class DynoDB
{
public:

    static DynoDB* getInstance();

    DynoDB(QString hostName,
             QString databaseName,
             QString userName,
             QString password);

    quint32 addPredicate(QString predicateString);
    quint32 addPredicate(Predicate* predicate);

    Class* getClass(QString name);

    bool isValid() const;

private:

    static DynoDB* instance_;

    bool initialize();
    bool createInstance(quint32 classId, quint32 instanceId);
    bool hasTable(quint32 classId);
    bool hasTable(quint32 relationId, quint32 relationGroupId);
    bool makeClassTable(quint32 classId);
    bool makeRelationTable(quint32 relationId, quint32 relationGroupId);
    quint32 createGibly();
    bool deleteGibly(quint32 id);
    bool createClass(quint32 id, QString className);
    bool isClass(quint32 id);
    bool isLiteral(quint32 id);
    bool createLiteral(quint32 id, QString literalName, quint32 typeId);
    bool addRelation(quint32 id, quint32 literalId, QVariant literal);
    bool addRelation(quint32 giblyId, quint32 relationId, quint32 relationGroupId, QList<quint32> classIds);
    quint32 getClass(quint32 id);
    BuiltInDataType getLiteralType(quint32 id);
    bool hasColumn(quint32 classId, quint32 columnId);
    bool addColumn(quint32 classId, quint32 columnId, BuiltInDataType dataType);
    bool hasColumn(quint32 relationId, quint32 relationGroupId, quint32 columnId);
    bool addColumn(quint32 relationId, quint32 relationGroupId, quint32 columnId, BuiltInDataType dataType);

    quint32 relationTypeExists(quint32 relationId, quint32 classId);
    quint32 relationTypeExists(quint32 relationId, QList<quint32> classIds, QList<quint32> quantities);
    quint32 registerRelationType(quint32 relationId,quint32 classId);
    quint32 registerRelationType(quint32 relationId, QList<quint32> classIds, QList<quint32> quantities);

    bool valid;
    QSqlDatabase database;
};

#endif // DYNO_DB_H
