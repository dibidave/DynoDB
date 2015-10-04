#include "predicateelement.h"

#include "dynodb.h"
#include "predicate.h"

PredicateElement::PredicateElement(QString predicateElementString)
{
    isLiteral_ = false;
    isSet_ = false;
    isValid_ = false;
    isQuery_ = false;
    isPredicate_ = false;
    predicate_ = 0;
    bool isInt;

    quint64 lastElementInPredicateStringIndex = predicateElementString.size()-1;

    // Try to convert the string to an integer
    id_ = predicateElementString.toInt(&isInt);

    // If it's not successful, parse it
    if(!isInt)
    {
        // If the first element is a question mark, this is a query variable
        if(predicateElementString.at(0) == '?')
        {
            isQuery_ = true;
            predicateElementString.remove(0,1);

            if(predicateElementString.isEmpty())
            {
                isValid_ = true;
                return;
            }
        }

        // If this has an open parentheses and ends in a closing parentheses, it might be a predicate
        if (predicateElementString.indexOf("(") != -1 && predicateElementString.at(lastElementInPredicateStringIndex) == ')' )
        {
            isPredicate_ = true;
            predicate_ = new Predicate(predicateElementString);

            if(predicate_->isValid())
            {
                isValid_ = true;
            }

            return;
        }

        // If this is surrounded by curly braces, it might be a set of giblies

        if (predicateElementString.at(0)=='{' && predicateElementString.at(lastElementInPredicateStringIndex) == '}' )
        {
             //remove the first and last elements since they are curly braces
            predicateElementString.remove(lastElementInPredicateStringIndex,1);
            predicateElementString.remove(0,1);
            //split the string by commas
            QStringList predicateElementsStringList = predicateElementString.split(",");
            //iterate through the predicate elements string and parse each element as an integer
            quint16 predicateElementsStringListIndex;
            for (predicateElementsStringListIndex = 0; predicateElementsStringListIndex < predicateElementsStringList.size(); predicateElementsStringListIndex++ )
            {
                ids_.append(predicateElementsStringList.at(predicateElementsStringListIndex).toInt(&isInt));
                if (!isInt) return;

            }
            isSet_ = true;
        }
        else if(predicateElementString.length() < 2 ||
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
    if(predicate_ != 0)
    {
        delete predicate_;
    }
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

bool PredicateElement::isQuery() const
{
    if (isPredicate_)
    {
        if (predicate_->isQuery())
        {
            return true;
        }

    }
    else return isQuery_;

}

bool PredicateElement::isPredicate() const
{

    return isPredicate_;
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
