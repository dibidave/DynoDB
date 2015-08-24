#include <QString>
#include <QTextStream>

#include <QtSql>

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
        else
        {
            cout << "Error parsing predicate." << endl;
        }

        predicate->deleteLater();
    }

    return 0;
}
