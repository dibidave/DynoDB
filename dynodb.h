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

    QList<Relation*> getRelations(quint32 giblyId);
    QList<Predicate*> getPredicates(Relation* relation, quint32 giblyId);

    Class* getClass(QString name);

    bool isValid() const;

private:

    static DynoDB* instance_;

    bool initialize();
    bool createInstance(quint32 classId, quint32 instanceId);
    bool hasTable(quint32 classId);
    bool makeClassTable(quint32 classId);
    quint32 createGibly();
    bool deleteGibly(quint32 id);
    bool createClass(quint32 id, QString className);
    bool isClass(quint32 id);
    bool isLiteral(quint32 id);
    bool createLiteral(quint32 id, QString literalName, quint32 typeId);
    bool addRelationship(quint32 id, quint32 literalId, QVariant literal);
    quint32 getClass(quint32 id);
    BuiltInDataType getLiteralType(quint32 id);
    bool hasColumn(quint32 classId, quint32 columnId);
    bool addColumn(quint32 classId, quint32 columnId, BuiltInDataType dataType);

    bool relationshipTypeExists(quint32 classId, quint32 literalId);
    bool registerRelationshipType(quint32 classId, quint32 literalId, bool isColumn = true);

    bool valid;
    QSqlDatabase database;
};

#endif // DYNO_DB_H
