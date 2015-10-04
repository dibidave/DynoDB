#ifndef PREDICATE_H
#define PREDICATE_H

#include <QObject>
#include <QList>

class PredicateElement;

class Predicate : public QObject
{
    Q_OBJECT

public:

    // Creates a predicate from a string in the format:
    // [Id]
    // [Id]([Id/Literal0]...[Id/LiteralN])
    // Note: literals cannot contain commas or parentheses
    Predicate(QString predicateString);
    ~Predicate();

    bool isValid() const;
    quint16 getNumElements() const;
    PredicateElement const* getElement(quint16 index) const;

    bool isQuery() const;

    QString toString() const;

private:

    bool isValid_;
    QList<PredicateElement*> predicateElements_;
};

#endif // PREDICATE_H
