#include "ExifParser.hpp"
#include <fstream>  // For ifstream
#include <iostream> // For cout

const bool cAppBase::DoesAppMarkerExist(const std::vector<unsigned char>::iterator &ReadBufferIter, const unsigned char MarkerNumber)
{
    if ((*ReadBufferIter == 0xFF) && (*(ReadBufferIter + 1) == MarkerNumber))
    {
        return true;
    }
    else
    {
        return false;
    }
}

const unsigned int cApp0::ParseApp(const std::vector<unsigned char>::iterator &ReadBufferIter)
{
    unsigned int TotalBytesRead = (*ReadBufferIter << 8) + *(ReadBufferIter + 1);
    std::cout << "App0 length in bytes is " << TotalBytesRead << "\n";
    // The next four bytes should be JFIF0

    return TotalBytesRead;
}

const unsigned int cApp1::ParseApp(const std::vector<unsigned char>::iterator &ReadBufferIter)
{
    unsigned int TotalBytesRead = (*ReadBufferIter << 8) + *(ReadBufferIter + 1);
    std::cout << "App1 length in bytes is " << TotalBytesRead << "\n";

    return TotalBytesRead;
}

cExifParser::cExifParser(const std::string ImageFileName) : App0(), App1()
{
    ParseExifData(ImageFileName);
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

void cExifParser::ParseExifData(const std::string ImageFileName)
{
    std::ifstream ImageFileStream(ImageFileName, std::ifstream::binary);

    if (ImageFileStream.is_open())
    {
        std::vector<unsigned char> ReadBuffer(READ_BUFFER_LENGTH_BYTES);
        ImageFileStream.read(reinterpret_cast<char *>(&ReadBuffer[0]), READ_BUFFER_LENGTH_BYTES);
        std::vector<unsigned char>::iterator ExifIter = ReadBuffer.begin();
        if (ImageFileStream && DoesStartOfImageExist(ExifIter))
        {
            std::cout << "Found valid SOI marker\n";
            std::advance(ExifIter, MARKER_LENGTH_BYTES);
            if (App0.DoesAppMarkerExist(ExifIter, cApp0::MARKER_NUMBER))
            {
                std::cout << "Found APP0\n";
                std::advance(ExifIter, MARKER_LENGTH_BYTES);
                const unsigned int BytesRead = App0.ParseApp(ExifIter);
                std::advance(ExifIter, BytesRead);
            }
            if (App1.DoesAppMarkerExist(ExifIter, cApp1::MARKER_NUMBER))
            {
                std::cout << "Found APP1\n";
                std::advance(ExifIter, MARKER_LENGTH_BYTES);
                const unsigned int BytesRead = App1.ParseApp(ExifIter);
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
        std::cout << "Could not find image\n";
    }

}