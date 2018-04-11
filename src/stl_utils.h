#pragma once
#include <algorithm>
#include "text_logger.h"

namespace STLUtils
{


template<typename T>
static bool  VectorContains(std::vector<T> const & vect, T element)
{
    //        textLogger << "using const lvalue ref\n";
    return std::find(vect.begin(), vect.end(), element) != vect.end();
}

template<typename T>
static bool  VectorContains(std::vector<T> && vect, T element)
{
    //        textLogger << "using rvalue ref\n";
    return std::find(vect.begin(), vect.end(), element) != vect.end();
}



template<typename T>
T & IteratorToElement(std::vector<T> && vect, T element, bool & isValid)
{
    auto it = std::find(vect.begin(), vect.end(), element);
    isValid = (it != vect.end());
    return  *it;
}



template<typename T>
T  & ElementForID(std::vector<T> && vect, int id, bool & isValid)
{
    //        textLogger << "using rvalue ref\n";
    typename std::vector<T>::iterator it = std::find_if(vect.begin(), vect.end(), [=](auto const & element){return element.id == id;});
    isValid =( it != vect.end());
    return  *it;
}
template<typename T>
T & ElementForID(std::vector<T> & vect, int id, bool & isValid)
{
    //        textLogger << "using lvalue ref\n";
    typename std::vector<T>::iterator it = std::find_if(vect.begin(), vect.end(), [=](auto const & element){return element.id == id;});
    isValid =( it != vect.end());
    return  *it;
}
template<typename T>
T  & ElementForName(std::vector<T> && vect, std::string name, bool & isValid)
{
    //        textLogger << "using rvalue ref\n";
    typename std::vector<T>::iterator it = std::find_if(vect.begin(), vect.end(), [=](auto const & element){return element.name == name;});
    isValid =( it != vect.end());
    return  *it;
}
template<typename T>
T & ElementForName(std::vector<T> & vect, std::string name, bool & isValid)
{
    //        textLogger << "using lvalue ref\n";
    typename std::vector<T>::iterator it = std::find_if(vect.begin(), vect.end(), [=](auto const & element){return element.name == name;});
    isValid =( it != vect.end());
    return  *it;
}

template<typename T>
void PrintVect(std::vector<T> const vect)
{
    textLogger << "Vector contains " << vect.size() << " elements:\n";
    for(auto const & e : vect)
    {
        textLogger << e << "\n";
    }
}



}





