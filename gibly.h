#ifndef GIBLY_H
#define GIBLY_H

#include <QObject>
#include <QString>
#include <QMap>

class Gibly : public QObject
{
    Q_OBJECT
public:
    Gibly(quint32 id);

    quint32 getId() const;
    QMap<QString, QString> getAttributes() const;

private:
    quint32 id_;
};

#endif // GIBLY_H
