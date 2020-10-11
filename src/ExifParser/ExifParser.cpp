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

const bool cExifParser::DoesStartOfImageExist(std::vector<unsigned char> &ReadBuffer)
{
    if ((ReadBuffer[0] == 0xFF) && (ReadBuffer[1] == 0xD8))
    {
        return true;
    }
    else
    {
        return false;
    }

}

void cExifParser::ParseExifData(const std::string ImageFileName)
{
    std::ifstream ImageFileStream(ImageFileName.c_str(), std::ifstream::binary);

    if (ImageFileStream.is_open())
    {
        std::vector<unsigned char> ReadBuffer(READ_BUFFER_LENGTH_BYTES);
        ImageFileStream.read(reinterpret_cast<char *>(&ReadBuffer[0]), 2);
        if (ImageFileStream && DoesStartOfImageExist(ReadBuffer))
        {
            std::cout << "Found valid SOI marker\n";
        }
        else
        {
            std::cout << "Error reading the SOI bytes\n";
        }

        ImageFileStream.close();
    }
    else
    {
        std::cout<<"Could not find image\n";
    }

}