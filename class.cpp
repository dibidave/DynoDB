#include "class.h"

Class::Class(quint32 id, QString name)
    : Gibly(id)
{
    name_ = name;
}

Class::~Class()
{

}

QString const& Class::getName() const
{
    return name_;
}
