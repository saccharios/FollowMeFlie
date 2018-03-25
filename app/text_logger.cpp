#include "text_logger.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdio>

std::string TextLogger::_file_name = "console_logger.txt";

TextLogger::TextLogger() :
    _sstream()
{
    remove(_file_name.c_str());
}

//void TextLogger::Write( std::stringstream const & strm )
//{
//	*_ostream << strm.str();
//}
//void TextLogger::WriteString( std::string const & str )
//{
//	*_ostream << str;
//}

void TextLogger::WriteToFile()
{
    _sstream.swap();
    std::ofstream myfile;
    myfile.open (_file_name, std::ios::out | std::ios::app);
    myfile << _sstream.side_b().str();
    myfile.close();

    _sstream.side_b().str(std::string()); // Delets content
    _sstream.side_b().clear(); // Delets errorbit
}
