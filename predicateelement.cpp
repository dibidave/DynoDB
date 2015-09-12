#include "predicateelement.h"

#include "dynodb.h"

PredicateElement::PredicateElement(QString predicateElementString)
{
    isLiteral_ = false;
    isSet_ = false;
    isValid_ = false;
    bool isInt;

    // Try to convert the string to an integer
    id_ = predicateElementString.toInt(&isInt);

    // If it's not successful, parse it
    if(!isInt)
    {
        // If this is surrounded by curly braces, it might be a set of giblies
        // if(isSurrounded)
            // Split the string by commas, and try to parse each element as an integer
            // Populate the ids_ list with these elements
            // Set isSet_ to true
        // If this isn't surrounded by quotes, it might be a class
        /*else*/ if(predicateElementString.length() < 2 ||
                predicateElementString.at(0) != '\"' ||
                predicateElementString.at(predicateElementString.length() - 1) != '\"')
        {
            // Check to see if a class with this name exists
            Class* dbclass = DynoDB::getInstance()->getClass(predicateElementString);

            if(!dbclass)
            {
                return;
            }

            id_ = dbclass->getId();
        }
        else
        {
            predicateElementString.remove(0,1);
            predicateElementString.remove(predicateElementString.length() - 1,1);

            isLiteral_ = true;
            literal_ = predicateElementString;
        }
    }

    isValid_ = true;
}

PredicateElement::~PredicateElement()
{

}

bool PredicateElement::isValid() const
{
    return isValid_;
}

bool PredicateElement::isLiteral() const
{
    return isLiteral_;
}

bool PredicateElement::isSet() const
{
    return isSet_;
}

quint32 PredicateElement::getId() const
{
    return id_;
}

QList<quint32> PredicateElement::getIds() const
{
    return ids_;
}

QVariant PredicateElement::getLiteral() const
{
    return literal_;
}

QString PredicateElement::toString() const
{
    if(isLiteral())
    {
        return literal_.toString();
    }
    else
    {
        return QString("%1").arg(id_);
    }
}
