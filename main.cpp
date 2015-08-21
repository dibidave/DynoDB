#include <QString>
#include <QTextStream>

#include <QtSql>

#include "dynodb.h"

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

        if(!dynoDB.addPredicate(input))
        {
            cout << "Error adding predicate." << endl;
        }
    }

    return 0;
}
