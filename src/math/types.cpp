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
    stream << "id = " << static_cast<int>(element.id)<< ", name = " << element.name << ", type = " << static_cast<int>(element.type)<< ", value = "<< element.value << "\n";
    return stream;
}

std::ostream & operator << (std::ostream& stream, Data const & data)
{
    for(auto const & e : data)
    {
        stream <<  static_cast<int>(e) << " ";
    }
    return stream;
}

Point3f operator-(Point3f const & a, Point3f const & b)
{
    Point3f c;
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    return c;
}
