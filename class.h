#ifndef CLASS_H
#define CLASS_H

#include <QString>

#include "gibly.h"

class Class : public Gibly
{
    Q_OBJECT
public:

    Class(quint32 id, QString name);
    ~Class();

    QString const& getName() const;

private:
    QString name_;
};

#endif // CLASS_H
