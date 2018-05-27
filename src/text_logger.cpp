#include "text_logger.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <chrono>
TextLogger textLogger;
std::string TextLogger::_file_name = "console_logger.txt";

TextLogger::TextLogger() :
    _sstream()
{
}
void TextLogger::Init()
{
    remove(_file_name.c_str());
    std::ofstream myfile;
    myfile.open (_file_name, std::ios::out | std::ios::trunc);
    std::stringstream initMsg;
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    initMsg<< "Program execution started on " << std::ctime(&now) << std::endl;
    myfile << initMsg.str();
    myfile.close();
}
void TextLogger::WriteToFile()
{
    _sstream.swap();
    std::ofstream myfile;
    myfile.open (_file_name, std::ios::out | std::ios::app);
    myfile << _sstream.side_b().str();
    myfile.close();

    _sstream.side_b().str(std::string()); // Deletes content
    _sstream.side_b().clear(); // Deletes errorbit
}
