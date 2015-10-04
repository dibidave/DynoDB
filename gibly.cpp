#include "gibly.h"

Gibly::Gibly(quint32 id)
    : QObject()
{
    id_ = id;
}

quint32 Gibly::getId() const
{
    return id_;
}

QMap<QString, QString> Gibly::getAttributes() const
{
    return QMap<QString, QString>();
}
