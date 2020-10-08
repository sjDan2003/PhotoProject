#include "ExifParser.hpp"
#include <fstream> // For ifstream
#include <iostream>

cExifParser::cExifParser()
{

}

cExifParser::cExifParser(const std::string ImageFileName)
{
    ParseExifData(ImageFileName);
}

cExifParser::~cExifParser()
{
    
}

const bool cExifParser::DoesStartOfImageExist()
{
    return true;
}

void cExifParser::ParseExifData(const std::string ImageFileName)
{
    std::ifstream ImageFileStream(ImageFileName.c_str(), std::ifstream::binary);

    if (ImageFileStream.is_open())
    {
        std::cout<<"Found Image\n";
        ImageFileStream.close();
    }
    else
    {
        std::cout<<"Could not find image\n";
    }
    
}