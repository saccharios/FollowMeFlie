#include "types.h"
#include "text_logger.h"

void PrintData(Data const & data)
{
    for(std::size_t i = 0; i < data.size() ; ++i)
    {
        textLogger << "i = " << i << " data = " << static_cast<int>(data.at(i)) << " " << data.at(i)<< "\n";
    }
}


std::ostream & operator << (std::ostream& stream, TOCElement const & element)
{
    stream << "id = " << static_cast<int>(element.id)<< " name = " << element.name << " type = " << static_cast<int>(element.type)<< " value = "<< element.value << "\n";
    return stream;
}
