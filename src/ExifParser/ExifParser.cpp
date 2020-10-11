#include "ExifParser.hpp"
#include <fstream>  // For ifstream
#include <iostream> // For cout

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

const bool cExifParser::DoesStartOfImageExist(const std::vector<unsigned char>::iterator &ReadBufferIter)
{
    if ((*ReadBufferIter == 0xFF) && (*(ReadBufferIter + 1) == 0xD8))
    {
        return true;
    }
    else
    {
        return false;
    }
}

const bool cExifParser::DoesApp0MarkerExist(const std::vector<unsigned char>::iterator &ReadBufferIter)
{
    if ((*ReadBufferIter == 0xFF) && (*(ReadBufferIter + 1) == 0xE0))
    {
        return true;
    }
    else
    {
        return false;
    }
}

const unsigned int cExifParser::ParseApp0(const std::vector<unsigned char>::iterator &ReadBufferIter)
{
    unsigned int TotalBytesRead = (*ReadBufferIter << 8) + *(ReadBufferIter + 1);
    std::cout << "App0 length in bytes is " << TotalBytesRead << "\n";
    return TotalBytesRead;
}

const bool cExifParser::DoesApp1MarkerExist(const std::vector<unsigned char>::iterator &ReadBufferIter)
{
    if ((*ReadBufferIter == 0xFF) && (*(ReadBufferIter + 1) == 0xE1))
    {
        return true;
    }
    else
    {
        return false;
    }
}

const unsigned int cExifParser::ParseApp1(const std::vector<unsigned char>::iterator &ReadBufferIter)
{
    unsigned int TotalBytesRead = (*ReadBufferIter << 8) + *(ReadBufferIter + 1);
    std::cout << "App1 length in bytes is " << TotalBytesRead << "\n";
    return TotalBytesRead;
}

void cExifParser::ParseExifData(const std::string ImageFileName)
{
    std::ifstream ImageFileStream(ImageFileName.c_str(), std::ifstream::binary);

    if (ImageFileStream.is_open())
    {
        std::vector<unsigned char> ReadBuffer(READ_BUFFER_LENGTH_BYTES);
        ImageFileStream.read(reinterpret_cast<char *>(&ReadBuffer[0]), READ_BUFFER_LENGTH_BYTES);
        std::vector<unsigned char>::iterator ExifIter = ReadBuffer.begin();
        if (ImageFileStream && DoesStartOfImageExist(ExifIter))
        {
            std::cout << "Found valid SOI marker\n";
            std::advance(ExifIter, MARKER_LENGTH_BYTES);
            if (DoesApp0MarkerExist(ExifIter))
            {
                std::cout << "Found APP0\n";
                std::advance(ExifIter, MARKER_LENGTH_BYTES);
                const unsigned int BytesRead = ParseApp0(ExifIter);
                std::advance(ExifIter, BytesRead);
            }
            if (DoesApp1MarkerExist(ExifIter))
            {
                std::cout << "Found APP1\n";
                std::advance(ExifIter, MARKER_LENGTH_BYTES);
                const unsigned int BytesRead = ParseApp1(ExifIter);
                std::advance(ExifIter, BytesRead);
            }
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