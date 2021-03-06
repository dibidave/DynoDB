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
        // See if this is a subpredicate
        qint16 openParenthesesIndex = predicateString.indexOf("(", cursorIndex);

        // See if we can find an open curly brace
        qint16 openCurlyBraceIndex = predicateString.indexOf("{", cursorIndex);

        // Find the next comma
        qint16 nextCursorIndex = predicateString.indexOf(",", cursorIndex);

        // If there is an open parentheses and it shows up before the next comma,
        // then this is the start of a subpredicate
        if(openParenthesesIndex != -1
                && (openParenthesesIndex < nextCursorIndex || nextCursorIndex == -1))
        {
            // Look for a closing parentheses
            nextCursorIndex = predicateString.indexOf(")", cursorIndex);

            // If there isn't a closing parentheses, then this is an invalid statement
            if(nextCursorIndex == -1)
            {
                return;
            }

            // Find the next comma
            nextCursorIndex = predicateString.indexOf(",", nextCursorIndex);
        }
        // If there is a curly brace and it shows up before the next comma,
        // then this is the start of a set
        else if(openCurlyBraceIndex != -1
                && (openCurlyBraceIndex < nextCursorIndex || nextCursorIndex == -1))
        {
            // Look for a closing curly brace
            nextCursorIndex = predicateString.indexOf("}", cursorIndex);

            // If there isn't a closing curly brace, then this is an invalid statement
            if(nextCursorIndex == -1)
            {
                return;
            }

            // Find the next comma
            nextCursorIndex = predicateString.indexOf(",", nextCursorIndex);
        }

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


bool Predicate::isQuery() const
{

    // Loop through predicate elements and check if they are queries
    quint64 predicateElementIndex;

    for (predicateElementIndex = 0; predicateElementIndex < predicateElements_.size(); predicateElementIndex++)
    {
        if (predicateElements_.at(predicateElementIndex)->isQuery())
        {
            return true;
        }

    }

    return false;



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



