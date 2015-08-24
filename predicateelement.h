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
    quint32 getId() const;
    QVariant getLiteral() const;

    QString toString() const;

private:

    bool isValid_;
    bool isLiteral_;
    quint32 id_;
    QVariant literal_;
};

#endif // PREDICATEELEMENT_H
