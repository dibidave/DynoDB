#include "predicate.h"

#include "predicateelement.h"

Predicate::Predicate(QString predicateString)
{
    isValid_ = false;

    qint16 cursorIndex = predicateString.indexOf("(");

    // If there is no open parentheses, this is invalid
    if(cursorIndex == -1)
    {
        return;
    }

    QString predicateElementString = predicateString.mid(0, cursorIndex);
    predicateElementString = predicateElementString.trimmed();

    PredicateElement* predicateElement = new PredicateElement(predicateElementString);

    if(!predicateElement->isValid())
    {
        delete predicateElement;
        return;
    }

    // The first element shouldn't be a literal
    if(predicateElement->isLiteral())
    {
        return;
    }

    predicateElements_.append(predicateElement);

    // Skip over the parentheses
    cursorIndex++;

    while(true)
    {
        // Find the next comma
        qint16 nextCursorIndex = predicateString.indexOf(",", cursorIndex);

        // If we didn't find a comma, let's look for a closing parentheses
        if(nextCursorIndex == -1)
        {
            nextCursorIndex = predicateString.indexOf(")", cursorIndex);

            // If no closing parentheses, bad format!
            if(nextCursorIndex == -1)
            {
                return;
            }

            predicateElementString = predicateString.mid(cursorIndex, nextCursorIndex - cursorIndex);
            predicateElementString = predicateElementString.trimmed();

            if(predicateElementString.isEmpty())
            {
                break;
            }

            predicateElement = new PredicateElement(predicateElementString);

            if(!predicateElement->isValid())
            {
                delete predicateElement;
                return;
            }

            predicateElements_.append(predicateElement);

            break;
        }

        predicateElementString = predicateString.mid(cursorIndex, nextCursorIndex - cursorIndex);
        predicateElementString = predicateElementString.trimmed();

        // Empty predicates are not allowed
        if(predicateElementString.isEmpty())
        {
            return;
        }

        predicateElement = new PredicateElement(predicateElementString);

        if(!predicateElement->isValid())
        {
            delete predicateElement;
            return;
        }

        predicateElements_.append(predicateElement);

        cursorIndex = nextCursorIndex + 1;
    }

    isValid_ = true;
}

Predicate::~Predicate()
{
    while(!predicateElements_.isEmpty())
    {
        predicateElements_.takeLast()->deleteLater();
    }
}

bool Predicate::isValid() const
{
    return isValid_;
}

quint16 Predicate::getNumElements() const
{
    return predicateElements_.size();
}

PredicateElement const* Predicate::getElement(quint16 index) const
{
    if(index > getNumElements())
    {
        return 0;
    }

    return predicateElements_.at(index);
}

QString Predicate::toString() const
{
    if(predicateElements_.size() < 1)
    {
        return QString();
    }

    QString predicateString;

    predicateString.append(predicateElements_.at(0)->toString());

    predicateString.append("(");

    for(qint16 predicateElementIndex = 1; predicateElementIndex < predicateElements_.size() - 1;
        predicateElementIndex++)
    {
        predicateString.append(predicateElements_.at(predicateElementIndex)->toString());
        predicateString.append(", ");
    }

    if(predicateElements_.size() > 1)
    {
        predicateString.append(predicateElements_.last()->toString());
    }

    predicateString.append(")");

    return predicateString;
}
