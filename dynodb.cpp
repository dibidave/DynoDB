#include "dynodb.h"

#include "datatypes.h"
#include "predicate.h"
#include "predicateelement.h"

DynoDB* DynoDB::instance_ = 0;

DynoDB* DynoDB::getInstance()
{
    return instance_;
}

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

    if(valid)
    {
        instance_ = this;
    }
}

bool DynoDB::isValid() const
{
    return valid;
}

quint32 DynoDB::addPredicate(Predicate* predicate)
{
    quint32 id = createGibly();

    if(id == 0)
    {
        return id;
    }

    PredicateElement const* firstPredicateElement = predicate->getElement(0);

    // If we are defining a new class
    if(firstPredicateElement->getId() == CLASS)
    {
        // This must be a binary predicate
        if(predicate->getNumElements() != 2)
        {
            deleteGibly(id);
            return 0;
        }

        PredicateElement const* namePredicateElement = predicate->getElement(1);

        // The second predicate must be a literal
        if(!namePredicateElement->isLiteral())
        {
            deleteGibly(id);
            return 0;
        }

        bool didCreateClass = createClass(id, namePredicateElement->toString());

        if(!didCreateClass)
        {
            deleteGibly(id);
            return 0;
        }
    }
    // Otherwise if we are defining a new literal
    else if(firstPredicateElement->getId() > LITERAL &&
            firstPredicateElement->getId() < MAX_DATA_TYPE)
    {
        // This must be a binary predicate
        if(predicate->getNumElements() != 2)
        {
            deleteGibly(id);
            return 0;
        }

        PredicateElement const* namePredicateElement = predicate->getElement(1);

        // The second predicate must be a literal
        if(!namePredicateElement->isLiteral())
        {
            deleteGibly(id);
            return 0;
        }

        bool didCreateLiteral = createLiteral(id, namePredicateElement->toString(),
                                              firstPredicateElement->getId());

        if(!didCreateLiteral)
        {
            deleteGibly(id);
            return 0;
        }
    }
    // Otherwise if we are defining a literal relation
    else if(isLiteral(firstPredicateElement->getId()))
    {
        // This must be a ternary predicate
        if(predicate->getNumElements() != 3)
        {
            deleteGibly(id);
            return 0;
        }

        PredicateElement const* giblyPredicateElement = predicate->getElement(1);

        if(giblyPredicateElement->isLiteral())
        {
            deleteGibly(id);
            return 0;
        }

        PredicateElement const* literalPredicateElement = predicate->getElement(2);

        if(!literalPredicateElement->isLiteral())
        {
            deleteGibly(id);
            return 0;
        }

        // TODO: Probably a cleaner way to do this than deleting the gibly right after creating it
        deleteGibly(id);

        bool addedRelation = addRelation(giblyPredicateElement->getId(), firstPredicateElement->getId(), literalPredicateElement->getLiteral());

        if(!addedRelation)
        {
            deleteGibly(id);
            return 0;
        }
    }
    // Otherwise if we are creating an instance of a class
    else if(isClass(firstPredicateElement->getId()))
    {
        // If this is just a class instantiation
        if (predicate->getNumElements() == 1)
        {
            if(!hasTable(firstPredicateElement->getId()))
            {
                bool createdTable = makeClassTable(firstPredicateElement->getId());

                if(!createdTable)
                {
                    deleteGibly(id);
                    return 0;
                }
            }

            bool createdInstance = createInstance(firstPredicateElement->getId(), id);

            if(!createdInstance)
            {
                deleteGibly(id);
                return 0;
            }
        }
        // Otherwise this is a relationship being defined
        else
        {
            QList<quint32> classIds;
            QList<quint32> giblyIds;
            for (qint8 remainingPredicateIndex = 1; remainingPredicateIndex < predicate->getNumElements(); remainingPredicateIndex++)
            {
                if (predicate->getElement(remainingPredicateIndex)->isLiteral())
                {
                    //TODO: clean this up in the future because it is kind of a waste
                    deleteGibly(id);
                    return 0;
                }
                else
                {
                    giblyIds.append(predicate->getElement(remainingPredicateIndex)->getId());
                    quint32 classId = getClass(giblyIds.last());
                    if (!classId)
                    {
                        //TODO: clean this up in the future because it is kind of a waste
                        deleteGibly(id);
                        return 0;
                    }
                    else
                    {
                        classIds.append(classId);
                    }

                }
            }
            if (!relationTypeExists(predicate->getElement(0)->getId(),classIds))
            {
                registerRelationType(predicate->getElement(0)->getId(),classIds);
            }

            if(!addRelation(id, firstPredicateElement->getId(), giblyIds))
            {
                deleteGibly(id);
                return 0;
            }
        }
    }
    else
    {
        deleteGibly(id);
        return 0;
    }

    return id;
}

QList<Relation*> DynoDB::getRelations(quint32 giblyId)
{
    //QString checkRelationTableStatement = "SELECT Id FROM `8` WHERE ";

    return QList<Relation*>();
}

QList<Predicate*> DynoDB::getPredicates(Relation* relation, quint32 giblyId)
{
    return QList<Predicate*>();
}

Class* DynoDB::getClass(QString name)
{
    //Return corresponding class with id matching the input string
    QString checkClassTableStatement = "SELECT Id FROM `2` WHERE `4` = '%1'";
    checkClassTableStatement = checkClassTableStatement.arg(name);
    QSqlQuery getClassId = database.exec(checkClassTableStatement);
    if (getClassId.next())
    {
        quint32 foundClassId = getClassId.value(0).toInt();
        Class* foundClass = new Class(foundClassId,name);
        return foundClass;
    }
    else return 0;
}

bool DynoDB::initialize()
{
    QString checkGibliesTableStatement =
            "SELECT Id "
            "FROM `%1` "
            "WHERE Id = 0";

    checkGibliesTableStatement = checkGibliesTableStatement.arg(GIBLY);

    database.exec(checkGibliesTableStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        QString createGibliesTableStatement =
                "CREATE TABLE `%1` ("
                "Id INT AUTO_INCREMENT PRIMARY KEY) AUTO_INCREMENT=%2";

        createGibliesTableStatement = createGibliesTableStatement
                .arg((quint16)GIBLY)
                .arg((quint16)MAX_DATA_TYPE);

        database.exec(createGibliesTableStatement);

        if(database.lastError().type() != QSqlError::NoError)
        {
            return false;
        }

        QString createClassTableStatement =
                "CREATE TABLE `%1` ("
                "Id INT PRIMARY KEY, "
                "`%2` VARCHAR(255), "
                "`%3` BOOLEAN DEFAULT false)";

        createClassTableStatement = createClassTableStatement
                .arg((quint16)CLASS)
                .arg((quint16)NAME)
                .arg((quint16)HAS_TABLE);

        database.exec(createClassTableStatement);

        if(database.lastError().type() != QSqlError::NoError)
        {
            return false;
        }

        QString createLiteralTableStatement =
                "CREATE TABLE `%1` ("
                "Id INT PRIMARY KEY, "
                "`%2` VARCHAR(255), "
                "`%3` INT)";

        createLiteralTableStatement = createLiteralTableStatement
                .arg(LITERAL)
                .arg(NAME)
                .arg(TYPE);

        database.exec(createLiteralTableStatement);

        if(database.lastError().type() != QSqlError::NoError)
        {
            return false;
        }

        QString createInstanceTableStatement =
                "CREATE TABLE `%1` ("
                "Id INT PRIMARY KEY, "
                "`%2` INT)";

        createInstanceTableStatement = createInstanceTableStatement
                .arg(INSTANCE)
                .arg(CLASS);

        database.exec(createInstanceTableStatement);

        if(database.lastError().type() != QSqlError::NoError)
        {
            return false;
        }

        QString createRelationTableStatement =
                "CREATE TABLE `%1` ("
                "Id INT, "
                "`%2` INT,"
                "`%3` BOOL,"
                "`%4` INT,"
                "`%5` INT)";

        createRelationTableStatement = createRelationTableStatement
                .arg(RELATION)
                .arg(CLASS)
                .arg(IS_COLUMN)
                .arg(GROUP)
                .arg(DIRECTION);

        database.exec(createRelationTableStatement);

        if(database.lastError().type() != QSqlError::NoError)
        {
            return false;
        }

        quint16 builtInDataTypeIndex = 0;
        BuiltInDataType builtInDataType = BuiltInDataTypeValues[builtInDataTypeIndex];

        while(builtInDataType != MAX_DATA_TYPE)
        {
            QString insertGiblyStatement =
                    "INSERT INTO `%1` (Id) "
                    "VALUES (%2)";

            insertGiblyStatement = insertGiblyStatement
                    .arg(GIBLY)
                    .arg(builtInDataType);

            database.exec(insertGiblyStatement);

            if(database.lastError().type() != QSqlError::NoError)
            {
                return false;
            }

            if(!createClass(builtInDataType, BuiltInDataTypeNames[builtInDataType]))
            {
                return false;
            }

            builtInDataType = BuiltInDataTypeValues[++builtInDataTypeIndex];
        }

        if(!createLiteral(NAME, BuiltInDataTypeNames[NAME], STRING))
        {
            return false;
        }

        if(!createLiteral(HAS_TABLE, BuiltInDataTypeNames[HAS_TABLE], BOOL))
        {
            return false;
        }

        if(!createLiteral(IS_COLUMN, BuiltInDataTypeNames[IS_COLUMN], BOOL))
        {
            return false;
        }

//        if(!registerRelationType(NAME,CLASS))
//        {
//            return false;
//        }

//        if(!registerRelationType(NAME,LITERAL))
//        {
//            return false;
//        }

//        if(!registerRelationType(HAS_TABLE,CLASS))
//        {
//            return false;
//        }

//        if(!registerRelationType(IS_COLUMN,RELATION))
//        {
//            return false;
//        }

//        if(!registerRelationType(CLASS,RELATION))
//        {
//            return false;
//        }

//        if(!registerRelationType(CLASS,INSTANCE))
//        {
//            return false;
//        }

    }

    return true;
}

bool DynoDB::createClass(quint32 id, QString name)
{
    QString addClassStatement =
            "INSERT INTO `%1` (Id, `%2`) "
            "VALUES (%3, '%4')";
    addClassStatement = addClassStatement
            .arg(CLASS)
            .arg(NAME)
            .arg(id)
            .arg(name);

    database.exec(addClassStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    return true;
}

bool DynoDB::createLiteral(quint32 id, QString literalName, quint32 typeId)
{
    QString addLiteralStatement =
            "INSERT INTO `%1` (Id, `%2`, `%3`) "
            "VALUES (%4, '%5', %6)";

    addLiteralStatement = addLiteralStatement
            .arg(LITERAL)
            .arg(NAME)
            .arg(TYPE)
            .arg(id)
            .arg(literalName)
            .arg(typeId);

    database.exec(addLiteralStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    return true;
}

bool DynoDB::createInstance(quint32 classId, quint32 instanceId)
{
    QString addGiblyStatement =
            "INSERT INTO `%1` (Id) "
            "VALUES (%2)";

    addGiblyStatement = addGiblyStatement.arg(classId).arg(instanceId);

    database.exec(addGiblyStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    QString addInstanceStatement =
            "INSERT INTO `%1` (Id, `%2`) "
            "VALUES (%3, %4)";

    addInstanceStatement = addInstanceStatement
            .arg(INSTANCE)
            .arg(CLASS)
            .arg(instanceId)
            .arg(classId);

    database.exec(addInstanceStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    return true;
}

bool DynoDB::hasTable(quint32 classId)
{
    QString getClassStatement =
            "SELECT `%1` FROM `%2` WHERE Id = %3";

    getClassStatement = getClassStatement
            .arg(HAS_TABLE)
            .arg(CLASS)
            .arg(classId);

    QSqlQuery getClassQuery = database.exec(getClassStatement);

    if(getClassQuery.size() > 0)
    {
        getClassQuery.first();
        return getClassQuery.value(0).toBool();
    }

    return false;
}

bool DynoDB::makeClassTable(quint32 classId)
{
    QString createClassTableStatement =
            "CREATE TABLE `%1` (\
            Id INT PRIMARY KEY)";

    createClassTableStatement = createClassTableStatement.arg(classId);

    database.exec(createClassTableStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    QString updateClassesTableStatement =
            "UPDATE `%1` SET `%2` = 1 "
            "WHERE Id = %3";

    updateClassesTableStatement = updateClassesTableStatement
            .arg(CLASS)
            .arg(HAS_TABLE)
            .arg(classId);

    database.exec(updateClassesTableStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    return true;
}

quint32 DynoDB::createGibly()
{
    QString addGiblyStatement =
            "INSERT INTO `%1`() VALUES()";

    addGiblyStatement = addGiblyStatement.arg(GIBLY);

    QSqlQuery addGiblyQuery = database.exec(addGiblyStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return INVALID_DATA_TYPE;
    }

    return addGiblyQuery.lastInsertId().toInt();
}

bool DynoDB::deleteGibly(quint32 id)
{
    QString deleteGiblyStatement =
            "DELETE FROM `%1` WHERE Id = %2";

    deleteGiblyStatement = deleteGiblyStatement
            .arg(GIBLY)
            .arg(id);

    database.exec(deleteGiblyStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    return true;
}

bool DynoDB::isClass(quint32 id)
{
    QString isClassStatement =
            "SELECT Id FROM `%1` WHERE Id = %2";

    isClassStatement = isClassStatement
            .arg(CLASS)
            .arg(id);

    QSqlQuery isClassQuery = database.exec(isClassStatement);

    if(isClassQuery.size() == 1)
    {
        return true;
    }

    return false;
}

bool DynoDB::isLiteral(quint32 id)
{
    QString isLiteralStatement =
            "SELECT Id FROM `%1` WHERE Id = %2";

    isLiteralStatement = isLiteralStatement
            .arg(LITERAL)
            .arg(id);

    QSqlQuery isLiteralQuery = database.exec(isLiteralStatement);

    if(isLiteralQuery.size() == 1)
    {
        return true;
    }

    return false;
}

bool DynoDB::addRelation(quint32 id, quint32 literalId, QVariant literal)
{
    quint32 classId = getClass(id);

    if(!relationTypeExists(classId, literalId))
    {
        if(!registerRelationType(literalId, classId))
        {
            return false;
        }
    }

    BuiltInDataType literalType = getLiteralType(literalId);

    if(!hasColumn(classId, literalId))
    {
        bool columnAdded = addColumn(classId, literalId, literalType);

        if(!columnAdded)
        {
            return false;
        }
    }

    QString updateRowStatement =
            "UPDATE `%1` SET `%2` = %3 WHERE Id = %4";

    updateRowStatement = updateRowStatement
            .arg(classId)
            .arg(literalId);

    switch(literalType)
    {
    case BOOL:
        updateRowStatement = updateRowStatement.arg(literal.toBool());
        break;
    case INTEGER:
        updateRowStatement = updateRowStatement.arg(literal.toInt());
        break;
    case DATETIME:
        updateRowStatement = updateRowStatement.arg(literal.toDateTime().toString("'yyyy-MM-dd HH:mm:ss.zzz0'"));
        break;
    case STRING:
        updateRowStatement = updateRowStatement.arg("'" + literal.toString() + "'");
        break;
    default:
        return false;
    }

    updateRowStatement = updateRowStatement.arg(id);

    database.exec(updateRowStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    return true;
}

bool DynoDB::addRelation(quint32 giblyId, quint32 relationId, QList<quint32> giblyIds)
{
    // Check whether this table exists
    if(!hasTable(relationId))
    {
        // If it doesn't, make it
        if(!makeClassTable(relationId))
        {
            return false;
        }
    }

    // Check whether this table has these columns
    for(qint32 giblyIndex = 0; giblyIndex < giblyIds.size(); giblyIndex++)
    {
        quint32 giblyId = giblyIds.at(giblyIndex);
        quint32 classId = getClass(giblyId);

        if(!hasColumn(relationId, classId))
        {
            if(!addColumn(relationId, classId, GIBLY))
            {
                return false;
            }
        }
    }

    QString insertRelationStatement =
            "INSERT INTO `%2` (Id, %3) VALUES (%1, %4)";

    insertRelationStatement = insertRelationStatement.arg(giblyId);
    insertRelationStatement = insertRelationStatement.arg(relationId);

    QString columnNames;
    QString columnValues;

    for(qint32 giblyIndex = 0; giblyIndex < giblyIds.size(); giblyIndex++)
    {
        quint32 giblyId = giblyIds.at(giblyIndex);
        quint32 classId = getClass(giblyId);

        columnNames.append("`%1`");
        columnNames = columnNames.arg(classId);
        columnValues.append("%1");
        columnValues = columnValues.arg(giblyId);

        if(giblyIndex != giblyIds.size() - 1)
        {
            columnNames.append(", ");
            columnValues.append(", ");
        }
    }

    insertRelationStatement = insertRelationStatement.arg(columnNames);
    insertRelationStatement = insertRelationStatement.arg(columnValues);

    database.exec(insertRelationStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    return true;
}

quint32 DynoDB::getClass(quint32 id)
{
    QString getClassStatement =
            "SELECT `%1` FROM `%2` WHERE Id = %3";

    getClassStatement = getClassStatement
            .arg(CLASS)
            .arg(INSTANCE)
            .arg(id);

    QSqlQuery getClassQuery = database.exec(getClassStatement);

    if(getClassQuery.size() > 0)
    {
        getClassQuery.first();
        return getClassQuery.value(0).toInt();
    }

    return 0;
}

BuiltInDataType DynoDB::getLiteralType(quint32 id)
{
    QString getLiteralStatement =
            "SELECT `%1` FROM `%2` WHERE Id = %3";

    getLiteralStatement = getLiteralStatement
            .arg(TYPE)
            .arg(LITERAL)
            .arg(id);

    QSqlQuery getLiteralQuery = database.exec(getLiteralStatement);

    if(getLiteralQuery.size() > 0)
    {
        getLiteralQuery.first();
        return (BuiltInDataType)getLiteralQuery.value(0).toInt();
    }

    return INVALID_DATA_TYPE;
}

bool DynoDB::hasColumn(quint32 classId, quint32 columnId)
{
    QString getColumnStatement =
            "SELECT `%1` FROM `%2` WHERE Id = 0";

    getColumnStatement = getColumnStatement
            .arg(columnId)
            .arg(classId);

    database.exec(getColumnStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    return true;
}

bool DynoDB::addColumn(quint32 classId, quint32 columnId, BuiltInDataType dataType)
{
    QString addColumnStatement =
            "ALTER TABLE `%1` ADD COLUMN `%2` %3";

    addColumnStatement = addColumnStatement
            .arg(classId)
            .arg(columnId);

    switch(dataType)
    {
    case BOOL:
        addColumnStatement = addColumnStatement.arg("BOOL");
        break;
    case INTEGER:
        addColumnStatement = addColumnStatement.arg("INT");
        break;
    case DATETIME:
        addColumnStatement = addColumnStatement.arg("DATETIME(4)");
        break;
    case STRING:
        addColumnStatement = addColumnStatement.arg("VARCHAR(255)");
        break;
    default:
        addColumnStatement = addColumnStatement.arg("INT");
    }

    database.exec(addColumnStatement);

    if(database.lastError().type() != QSqlError::NoError)
    {
        return false;
    }

    return true;
}

bool DynoDB::relationTypeExists(quint32 relationId, quint32 classId)
{
   return  relationTypeExists(relationId, QList<quint32>({classId}));
}

bool DynoDB::relationTypeExists(quint32 relationId, QList<quint32> classIds)
{
    QString checkRelationStatement = "SELECT DISTINCT `%1` FROM `%2` WHERE Id = %3";

    checkRelationStatement = checkRelationStatement
            .arg(GROUP)
            .arg(RELATION)
            .arg(relationId);
    QSqlQuery checkRelationQuery = database.exec(checkRelationStatement);

    while (checkRelationQuery.next())
    {
        quint16 currentGroupId = checkRelationQuery.value(0).toInt();
        QString getRelationStatement = "SELECT `%1` FROM `%2` WHERE Id = %3 AND `%4` = %5 ORDER BY `%6`";
        getRelationStatement = getRelationStatement
                .arg(CLASS)
                .arg(RELATION)
                .arg(relationId)
                .arg(GROUP)
                .arg(currentGroupId)
                .arg(DIRECTION);
        QSqlQuery getRelationQuery = database.exec(getRelationStatement);
        if (getRelationQuery.size() != classIds.size())
        {
            continue;
        }
        else
        {
            bool allMatch = true;
            for (qint32 currentClassIdIndex = 0; currentClassIdIndex < classIds.size(); currentClassIdIndex++)
            {
                getRelationQuery.next();
                quint16 getClassId = getRelationQuery.value(0).toInt();
                if (classIds.at(currentClassIdIndex) != getClassId)
                {
                    allMatch = false;
                    break;
                }
            }
            if (allMatch)
            {
                return true;
            }
        }



    }

    return false;
}


bool DynoDB::registerRelationType(quint32 relationId,quint32 classId)
{
    return registerRelationType(relationId,QList<quint32>({classId}));
}


bool DynoDB::registerRelationType(quint32 relationId, QList<quint32> classIds)
{
    QString largestGroupStatement =
            "SELECT MAX(`%1`) FROM `%2` WHERE Id = %3";

    largestGroupStatement = largestGroupStatement
            .arg(GROUP)
            .arg(RELATION)
            .arg(relationId);

    QSqlQuery largestGroupQuery = database.exec(largestGroupStatement);

    // If this relation doesn't exist already, it's the first one, so the group is 1
    quint32 groupId = 1;

    // If this relation exists already
    if(largestGroupQuery.next())
    {
        // Set the group to be one bigger than the biggest groupId
        groupId = largestGroupQuery.value(0).toInt() + 1;
    }

    for(qint8 classIndex = 0; classIndex < classIds.size(); classIndex++)
    {


        QString registerRelationStatement =
                "INSERT INTO `%1` (Id, `%2`, `%3`,`%4`,`%5`) "
                "VALUES (%6, %7,%8, %9, %10)";

        registerRelationStatement = registerRelationStatement
                .arg(RELATION)
                .arg(CLASS)
                .arg(IS_COLUMN)
                .arg(GROUP)
                .arg(DIRECTION)
                .arg(relationId)
                .arg(classIds.at(classIndex))
                .arg(true)
                .arg(groupId)
                .arg(classIndex+1);

        database.exec(registerRelationStatement);

        if(database.lastError().type() != QSqlError::NoError)
        {
            return false;
        }
    }

    return true;
}
