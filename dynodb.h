#ifndef DYNO_DB_H
#define DYNO_DB_H

#include <QtSql>


class DynoDB
{
public:
    DynoDB(QString hostName,
             QString databaseName,
             QString userName,
             QString password);

    bool addPredicate(QString predicate);

    bool isValid() const;

private:

    bool initialize();
    bool hasClass(QString className);
    bool addClass(QString className);
    bool addInstance(QString className, QString instanceName);
    bool hasTable(QString className);
    bool makeClassTable(QString className);

    bool valid;
    QSqlDatabase database;
};

#endif // DYNO_DB_H
