#include "dynodb.h"

DynoDB::DynoDB(QString hostName,
                   QString databaseName,
                   QString userName,
                   QString password)
{
    database = QSqlDatabase::addDatabase("QMYSQL");
    database.setHostName(hostName);
    database.setDatabaseName(databaseName);
    database.setUserName(userName);
    database.setPassword(password);

    valid = database.open();

    if(!valid)
    {
        return;
    }

    valid = initialize();
}

bool DynoDB::isValid() const
{
    return valid;
}

bool DynoDB::addPredicate(QString predicate)
{
    if(predicate.isEmpty())
    {
        return false;
    }

    QStringList predicateList = predicate.split(" ");

    if(predicateList.size() < 1)
    {
        return false;
    }

    QString className = predicateList.at(0);

    if(!hasClass(className))
    {
        if(!addClass(className))
        {
            return false;
        }
    }

    if(predicateList.size() == 2)
    {
        QString instanceName = predicateList.at(1);

        if(!addInstance(className, instanceName))
        {
            return false;
        }
    }

    return true;
}

bool DynoDB::initialize()
{
    QString checkGibliesTableStatement =
            "SELECT Id, Description, IsClass \
            FROM Giblies \
            WHERE Id = 0";

    database.exec(checkGibliesTableStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        QString createGibliesTableStatement =
                "CREATE TABLE Giblies (\
                Id INT AUTO_INCREMENT PRIMARY KEY,\
                Description VARCHAR(255),\
                IsClass BOOLEAN,\
                HasTable BOOLEAN)";

        database.exec(createGibliesTableStatement);

        if(database.lastError().type() != QSqlError::NoError)
        {
            return false;
        }
    }

    return true;
}

bool DynoDB::hasClass(QString className)
{
    QString checkGibliesStatement =
            "SELECT Id \
            FROM Giblies \
            WHERE Description LIKE '%1' \
            AND IsClass = 1";

    checkGibliesStatement = checkGibliesStatement.arg(className);

    QSqlQuery checkGibliesQuery = database.exec(checkGibliesStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    return checkGibliesQuery.size() > 0;
}

bool DynoDB::addClass(QString className)
{
    if(hasClass(className))
    {
        return false;
    }

    QString addClassStatement =
            "INSERT INTO Giblies (Description, IsClass, HasTable) \
            VALUES ('%1', 1, 0)";
    addClassStatement = addClassStatement.arg(className);

    database.exec(addClassStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    return true;
}

bool DynoDB::addInstance(QString className, QString instanceName)
{
    if(!hasTable(className))
    {
        if(!makeClassTable(className))
        {
            return false;
        }
    }

    QString addGiblyStatement =
            "INSERT INTO Giblies (Description, IsClass, HasTable) \
            VALUES ('%1', 0, 0)";

    addGiblyStatement = addGiblyStatement.arg(instanceName);

    QSqlQuery addGiblyQuery = database.exec(addGiblyStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    int giblyId = addGiblyQuery.lastInsertId().toInt();

    QString addInstanceStatement =
            "INSERT INTO %1 (Id) VALUES (%2)";

    addInstanceStatement = addInstanceStatement.arg(className)
            .arg(giblyId);

    database.exec(addInstanceStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    return true;
}

bool DynoDB::hasTable(QString className)
{
    QString getClassStatement =
            "SELECT HasTable FROM Giblies WHERE Description LIKE '%1'";

    getClassStatement = getClassStatement.arg(className);

    QSqlQuery getClassQuery = database.exec(getClassStatement);

    if(getClassQuery.size() > 0)
    {
        getClassQuery.first();
        return getClassQuery.value(0).toBool();
    }

    return false;
}

bool DynoDB::makeClassTable(QString className)
{
    QString createClassTableStatement =
            "CREATE TABLE %1 (\
            Id INT PRIMARY KEY)";

    createClassTableStatement = createClassTableStatement.arg(className);

    database.exec(createClassTableStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    QString updateGibliesTableStatement =
            "UPDATE Giblies SET HasTable = 1 \
            WHERE Description LIKE '%1' AND IsClass = 1";

    updateGibliesTableStatement = updateGibliesTableStatement.arg(className);

    database.exec(updateGibliesTableStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    return true;
}
