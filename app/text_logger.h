#pragma once
#include <QObject>
#include <sstream>
#include "math/double_buffer.h"
class TextLogger : public QObject
{
    // Periodically writes data to a text file
    Q_OBJECT
public:
    TextLogger();

    //void operator<< (std::ostream const & stream, TextLogger & textLogger)

    template<typename T> void operator<< (T t){this->Write(t);}
    void operator<< (std::stringstream const & strm){this->Write(strm);}

public slots:
    void WriteToFile();


private:
    static std::string _file_name;
    Double_Buffer_Bidirectional<std::stringstream> _sstream;

    template<typename T> void Write(T t) {_sstream.side_a() << t;}
    void Write(std::stringstream const & strm) {_sstream.side_a() << strm.str();}

};


static TextLogger textLogger;
