#ifndef PREDICATEELEMENT_H
#define PREDICATEELEMENT_H

#include <QObject>
#include <QVariant>

class PredicateElement : public QObject
{
    Q_OBJECT

public:
    PredicateElement(QString predicateElementString);
    ~PredicateElement();

    bool isValid() const;
    bool isLiteral() const;
    bool isSet() const;
    quint32 getId() const;
    QList<quint32> getIds() const;
    QVariant getLiteral() const;

    QString toString() const;

private:

    bool isValid_;
    bool isLiteral_;
    bool isSet_;
    quint32 id_;
    QList<quint32> ids_;
    QVariant literal_;
};

#endif // PREDICATEELEMENT_H
