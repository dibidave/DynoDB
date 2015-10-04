#ifndef PREDICATEELEMENT_H
#define PREDICATEELEMENT_H

#include <QObject>
#include <QVariant>

class Predicate;

class PredicateElement : public QObject
{
    Q_OBJECT

public:
    PredicateElement(QString predicateElementString);
    ~PredicateElement();

    bool isValid() const;
    bool isLiteral() const;
    bool isSet() const;
    bool isQuery() const;
    bool isPredicate() const;
    quint32 getId() const;
    QList<quint32> getIds() const;
    QVariant getLiteral() const;
    Predicate const* getPredicate() const;
    QString toString() const;

private:

    bool isValid_;
    bool isLiteral_;
    bool isSet_;
    bool isQuery_;
    bool isPredicate_;
    quint32 id_;
    QList<quint32> ids_;
    QVariant literal_;
    Predicate* predicate_;
};

#endif // PREDICATEELEMENT_H
