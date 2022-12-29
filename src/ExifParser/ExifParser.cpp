/**
* @file ExifParser.hpp
* @brief Source code for the ExifParser library
* @author Daniel Fettke
* @date 6-5-2021
*/

#include "ExifParser.hpp"
#include <fstream>  // For ifstream
#include <iostream> // For cout
#include <string>   // For std::string
#include <iomanip>  // For resetiosflags
#include <cstdio>   // For sscanf
#include <ctime>    // For tm struct

/**
 * @brief Determines if the App Marker Exists
 *
 * @param[in] ReadBufferIter
 * @param[in] MarkerNumber
 *
 * @return True if the app marker exists.
 *         False if it does not.
 */
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

const unsigned short cAppBase::ConvertEndian16(const unsigned short Value)
{
    return (((Value & 0x0000FF00) << 8)  |
            ((Value & 0x00FF0000) >> 8));
}

const unsigned int   cAppBase::ConvertEndian32(const unsigned int   Value)
{
    return (((Value & 0x000000FF) << 24) |
            ((Value & 0x0000FF00) << 8)  |
            ((Value & 0x00FF0000) >> 8)  |
            ((Value & 0xFF000000) >> 24));
}

/**
 * @brief Reads two bytes from the EXIF data, taking endianess into account.
 *
 * @param[in] ReadBufferIter Iterator to the EXIF data
 *
 * @return Two byte value from the EXIF data
 */
const unsigned short cAppBase::ReadTwoBytes(const std::vector<unsigned char>::iterator &ReadBufferIter)
{
    unsigned short Ret = 0;
    if (mLittleEndian)
    {
        Ret = (*(ReadBufferIter + 1) << 8) | *ReadBufferIter;
    }
    else
    {
        Ret = (*ReadBufferIter << 8) | *(ReadBufferIter + 1);
    }
    return Ret;
}

/**
 * @brief Reads four bytes from the EXIF data, taking endianess into account.
 *
 * @param[in] ReadBufferIter Iterator to the EXIF data
 *
 * @return Four byte value from the EXIF data
 */
const unsigned int cAppBase::ReadFourBytes(const std::vector<unsigned char>::iterator &ReadBufferIter)
{
    unsigned int Ret = 0;
    if (mLittleEndian)
    {
        Ret = (*(ReadBufferIter + 3) << 24) | (*(ReadBufferIter + 2) << 16) | (*(ReadBufferIter + 1) << 8) | *ReadBufferIter;
    }
    else
    {
        Ret = ((*ReadBufferIter << 24) | (*(ReadBufferIter + 1) << 16) | (*(ReadBufferIter + 2) << 8) | *(ReadBufferIter + 3));
    }
    return Ret;
}

/**
 * @brief Parses information for App0 data
 *
 * @param[in] ReadBufferIter Iterator to the EXIF data
 *
 * @return The bytes read by this parsing function.
 */
const unsigned int cApp0::ParseApp(const std::vector<unsigned char>::iterator &ReadBufferIter)
{
    const unsigned int TotalBytesRead = (*ReadBufferIter << 8) + *(ReadBufferIter + 1);
    std::cout << "App0 length in bytes is " << TotalBytesRead << "\n";
    // The next four bytes should be JFIF0

    return TotalBytesRead;
}

/**
 * @brief Determines the endianess based on the EXIF header data
 *
 * @pre App1Iter starts at the beginning of the endianess datat
 *
 * @param[in] App1Iter Iterator to the EXIF data
 *
 * @return None
 */
void cApp1::GetEndianess(const std::vector<unsigned char>::iterator &App1Iter)
{
    unsigned char EndianValue[ENDIAN_LENGTH];
    EndianValue[0] = *App1Iter;
    EndianValue[1] = *(App1Iter + 1);
    if ((EndianValue[0] == LITTLE_ENDIAN_TAG) && (EndianValue[1] == LITTLE_ENDIAN_TAG))
    {
        // std::cout << "Little Endian" << std::endl;
        mLittleEndian = true;
    }
    else if ((EndianValue[0] == BIG_ENDIAN_TAG) && (EndianValue[1] == BIG_ENDIAN_TAG))
    {
        // std::cout << "Big Endian" << std::endl;
        mLittleEndian = false;
    }
    else
    {
        // std::cout << "Invalid Endian marker\n" << std::endl;
    }
}

/**
 * @brief Parses all of the IFDs in the EXIF data
 *
 * @pre App1Iter starts at the beginning of the IFD data
 *
 * @param[in] App1Iter Iterator to the EXIF data
 *
 * @return None
 */
void cApp1::GetTiffTagList(std::vector<unsigned char>::iterator &App1Iter)
{
    for (TiffTagStruct &CurrIfd : mIfdList)
    {
        CurrIfd.Tag = ReadTwoBytes(App1Iter);
        std::advance(App1Iter, 2);
        CurrIfd.Type = ReadTwoBytes(App1Iter);
        std::advance(App1Iter, 2);
        CurrIfd.Count = ReadFourBytes(App1Iter);
        std::advance(App1Iter, 4);
        CurrIfd.Offset = ReadFourBytes(App1Iter);
        std::advance(App1Iter, 4);
    }
}

void cApp1::ParseIfdData()
{

}

/**
 * @brief Parses the date and time from the EXIF header
 *
 * @pre App1Iter starts at the beginning of the time date information
 *
 * @param[in] App1Iter Iterator to the EXIF data
 * @param[in] Count The number of characters to parse
 *
 * @return None
 */
void cApp1::ParseDateTime(std::vector<unsigned char>::iterator &App1Iter, const unsigned int Count)
{
    std::string DateTimeStr;
    char DateTimeChar;
    for (unsigned int Offset = 0; Offset < Count; ++Offset)
    {
        DateTimeChar = static_cast<char>(*(App1Iter + Offset));
        DateTimeStr.push_back(DateTimeChar);
    }
    sscanf(DateTimeStr.c_str(), "%d:%d:%d %d:%d:%d", &mDateTime.tm_year, &mDateTime.tm_mon, &mDateTime.tm_mday,
                                                     &mDateTime.tm_hour, &mDateTime.tm_min, &mDateTime.tm_sec);
    --mDateTime.tm_mon; // EXIF month is ones based, but struct tm expects zero based.
                        // Convert the EXIF month to zero based.
    mDateTime.tm_year -= 1900; // tm_year expects the number of years since 1900, but EXIF data is years since 0 AD
    // std::cout << "Photo's date time is " << asctime(&mDateTime) << std::endl;
}

void cApp1::GetTiffTagData(unsigned int HeaderOffsetStart)
{
    for (const TiffTagStruct &CurrIfd : mIfdList)
    {
        std::cout << std::resetiosflags(std::ios_base::basefield);
        const unsigned int OffsetToData = HeaderOffsetStart + CurrIfd.Offset;
        std::vector<unsigned char>::iterator StartOfDataIter = mStartOfFileIter + OffsetToData;
        switch (CurrIfd.Tag)
        {
            case IFD_DATE_TIME:
            {
                ParseDateTime(StartOfDataIter, CurrIfd.Count);
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

/**
 * @brief Parses information for App1 data
 *
 * @param[in] ReadBufferIter
 *
 * @return The bytes read by this parsing function.
 */
const unsigned int cApp1::ParseApp(const std::vector<unsigned char>::iterator &ReadBufferIter)
{
    // Copy the ReadBufferIter to a local iterator so we don't overwrite the iterator's position
    std::vector<unsigned char>::iterator App1Iter = ReadBufferIter;

    // Get the length of App1
    const unsigned int TotalBytesRead = (*App1Iter << 8) + *(App1Iter + 1);
    // std::cout << "App1 length in bytes is " << TotalBytesRead << "\n";
    std::advance(App1Iter, APP_DATA_SIZE_LENGTH);

    // TODO: Ensure the next six bytes equals EXIF plus 2 spaces
    std::advance(App1Iter, 6);

    const unsigned int HeaderOffsetStart = App1Iter - mStartOfFileIter;
    // std::cout << "Header offset start 0x" << std::hex << HeaderOffsetStart << std::endl;

    // Determine the endianness of the data.
    GetEndianess(App1Iter);
    std::advance(App1Iter, ENDIAN_LENGTH);

    // The next two bytes should be 0x002A
    // const unsigned short TwoAlphaValue = ReadTwoBytes(App1Iter);

    // if (TwoAlphaValue != 0x2A)
    // {
    //     std::cout << "Unexpected value after endian marker " << *App1Iter << std::endl;
    // }
    std::advance(App1Iter, 2);

    // The next four bytes contains the offset of the 0th IFD in bytes
    // According to the standard, if the value is 0x00000008 then the 0th IFD is right after
    // the IFD offset.
    // const unsigned int IfdOffset = ReadFourBytes(App1Iter);
    // std::cout << "Offset to IFD is " << IfdOffset << " bytes" << std::endl;
    std::advance(App1Iter, 4);

    // The next two bytes are the number of IFDs
    const unsigned short NumOfIFDs = ReadTwoBytes(App1Iter);
    // std::cout << "Number of IFDs " << std::dec << NumOfIFDs << std::endl;
    std::advance(App1Iter, 2);

    mIfdList.resize(NumOfIFDs);
    GetTiffTagList(App1Iter);
    GetTiffTagData(HeaderOffsetStart);

    return TotalBytesRead;
}

cExifParser::cExifParser(const std::string ImageFileName) : App0(), App1()
{
    ParseExifData(ImageFileName);
}

/**
 * @brief Checks to make sure the value
 *
 * @param[in] ReadBufferIter
 *
 * @return The bytes read by this parsing function.
 */
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
            // std::cout << "Found valid SOI marker\n";
            std::advance(ExifIter, MARKER_LENGTH_BYTES);
            if (App0.DoesAppMarkerExist(ExifIter, cApp0::MARKER_NUMBER))
            {
                // std::cout << "Found APP0\n";
                std::advance(ExifIter, MARKER_LENGTH_BYTES);
                const unsigned int BytesRead = App0.ParseApp(ExifIter);
                std::advance(ExifIter, BytesRead);
            }
            if (App1.DoesAppMarkerExist(ExifIter, cApp1::MARKER_NUMBER))
            {
                // std::cout << "Found APP1\n";
                std::advance(ExifIter, MARKER_LENGTH_BYTES);
                App1.SetStartOfFile(ReadBuffer.begin());
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