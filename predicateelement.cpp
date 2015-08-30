#include "predicateelement.h"

#include "dynodb.h"

PredicateElement::PredicateElement(QString predicateElementString)
{
    isValid_ = false;
    bool isInt;

    // Try to convert the string to an integer
    id_ = predicateElementString.toInt(&isInt);

    // If it's successful, we're done
    if(isInt)
    {
        isLiteral_ = false;
    }
    else
    {
        // If this isn't surrounded by quotes, it might be a class
        if(predicateElementString.length() < 2 ||
                predicateElementString.at(0) != '\"' ||
                predicateElementString.at(predicateElementString.length() - 1) != '\"')
        {
            // Check to see if a class with this name exists
            Class* dbclass = DynoDB::getClass(predicateElementString);

            if(!dbclass)
            {
                return false;
            }

            isLiteral_ = false;
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

quint32 PredicateElement::getId() const
{
    return id_;
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
