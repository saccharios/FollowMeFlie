#include "types.h"
#include "text_logger.h"

void PrintData(Data const & data)
{
    for(std::size_t i = 0; i < data.size() ; ++i)
    {
        textLogger << "i = " << i << " data = " << static_cast<int>(data.at(i)) << " " << data.at(i)<< "\n";
    }
}

