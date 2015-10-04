#include <QString>
#include <QTextStream>

#include <QtSql>
#include <QPair>

#include "dynodb.h"
#include "predicate.h"

int main(int argc, char *argv[])
{
    QTextStream cout (stdout);
    QTextStream cin (stdin);

    if(argc < 2)
    {
        cout << "Must supply database password in command line.\n";
        return -1;
    }

    DynoDB dynoDB("127.0.0.1",
                      "testdb",
                      "root",
                      argv[1]);

    if(!dynoDB.isValid())
    {
        cout << "Error opening databse.\n";
        return -1;
    }
    else
    {
        cout << "Connected to database successfully!" << endl;
    }

    while(true)
    {
        QString input = cin.readLine();

        if(input == "quit")
        {
            break;
        }

        Predicate* predicate = new Predicate(input);

        if(predicate->isValid())
        {
            if(predicate->isQuery())
            {
                QList<QPair<quint32, QMap<QString, QString>>> queryResults = dynoDB.processQuery(predicate);

                for(qint32 queryResultIndex = 0; queryResultIndex < queryResults.size();
                    queryResultIndex++)
                {
                    cout << queryResults.at(queryResultIndex).first << ":\n";

                    for(QMap<QString, QString>::iterator attributeElement = queryResults[queryResultIndex].second.begin();
                        attributeElement != queryResults[queryResultIndex].second.end(); attributeElement++)
                    {
                        cout << "\t" << attributeElement.key() << ": " << attributeElement.value() << "\n";
                    }
                }
            }
            else
            {
                quint32 id = dynoDB.addPredicate(predicate);

                if(id == 0)
                {
                    cout << "Error adding predicate." << endl;
                }
                else
                {
                    cout << "Created Gibly Id " << id << " parsed as : " << predicate->toString() << endl;
                }
            }
        }
        else
        {
            cout << "Error parsing predicate." << endl;
        }

        predicate->deleteLater();
    }

    return 0;
}
