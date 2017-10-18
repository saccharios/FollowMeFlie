#include "toc_parameter.h"


bool TocParameter::ReadAll()
{
    for(TOCElement element : _elements)
    {
        bool success = ReadElement(element);
        if(!success)
        {
            return false;
        }
    }
    return true;
}

bool TocParameter::ReadElement(uint8_t id)
{
    // TODO SF Implement
    // Check if the requested id is in the elemnts list at all;
    return true;
}
bool TocParameter::ReadElement(TOCElement & element)
{
    return ReadElement(element.id);
}


