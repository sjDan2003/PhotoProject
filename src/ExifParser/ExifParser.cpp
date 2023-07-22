/**
* @file ExifParser.hpp
* @brief Source code for the ExifParser library
* @author Daniel Fettke
* @date 6-5-2021
*/

#include "ExifParser.hpp"
#include <endian.h> // For endian correction
#include <fstream>  // For ifstream
#include <iostream> // For cout
#include <string>   // For std::string
#include <cstdio>   // For sscanf
#include <ctime>    // For tm struct

/**
 * @brief Determines if the App Marker Exists
 *
 * @param[in] ReadBufferIter Iterator to the EXIF data
 * @param[in] app_marker The app marker to check
 *
 * @return True if the app marker exists.
 *         False if it does not.
 */
const bool cAppBase::DoesAppMarkerExist(const std::vector<uint8_t>::iterator &ReadBufferIter, const uint16_t app_marker)
{
    bool app_marker_exists = false;
    const uint16_t read_app_marker = ReadTwoBytes(ReadBufferIter);
    if (read_app_marker == app_marker)
    {
        app_marker_exists = true;
    }

    return app_marker_exists;
}

/**
 * @brief Reads two bytes from the EXIF data, taking endianess into account.
 *
 * @param[in] ReadBufferIter Iterator to the EXIF data
 *
 * @return Two byte value from the EXIF data
 */
const uint16_t cAppBase::ReadTwoBytes(const std::vector<uint8_t>::iterator &ReadBufferIter)
{
    uint16_t converted_value = 0;
    uint16_t raw_value = *(reinterpret_cast<const std::vector<uint16_t>::iterator &>(ReadBufferIter));
    if (mLittleEndian)
    {
        converted_value = le16toh(raw_value);
    }
    else
    {
        converted_value = be16toh(raw_value);
    }
    return converted_value;
}

/**
 * @brief Reads four bytes from the EXIF data, taking endianess into account.
 *
 * @param[in] ReadBufferIter Iterator to the EXIF data
 *
 * @return Four byte value from the EXIF data
 */
const uint32_t cAppBase::ReadFourBytes(const std::vector<uint8_t>::iterator &ReadBufferIter)
{
    uint32_t converted_value = 0;
    uint32_t raw_value = *(reinterpret_cast<const std::vector<uint32_t>::iterator &>(ReadBufferIter));
    if (mLittleEndian)
    {
        converted_value = le32toh(raw_value);
    }
    else
    {
        converted_value = be32toh(raw_value);
    }
    return converted_value;
}

/**
 * @brief Parses information for App0 data
 *
 * @pre The calling function has advanced the buffer iterator to the start of the App1 data
 *
 * @param[in] ReadBufferIter Iterator pointing to the App0 data
 *
 * @return The bytes read by this parsing function.
 */
const uint32_t cApp0::ParseApp(const std::vector<uint8_t>::iterator &ReadBufferIter)
{
    const uint16_t TotalBytesRead = ReadTwoBytes(ReadBufferIter);
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
 * @return True if either a big or little endian tag was found
 *         False otherwise, which may indicate the file was corrupted or the
 *               file uses a standard this program doesn't support.
 */
const bool cApp1::GetEndianess(const std::vector<uint8_t>::iterator &App1Iter)
{
    bool valid_marker = false;
    const uint16_t endianess_value = ReadTwoBytes(App1Iter);
    if (endianess_value == LITTLE_ENDIAN_TAG)
    {
        // std::cout << "Little Endian" << std::endl;
        mLittleEndian = true;
        valid_marker = true;
    }
    else if (endianess_value == BIG_ENDIAN_TAG)
    {
        // std::cout << "Big Endian" << std::endl;
        mLittleEndian = false;
        valid_marker = true;
    }
    else
    {
        // std::cout << "Invalid Endian marker\n" << std::endl;
    }

    return valid_marker;
}

/**
 * @brief Verifies the contents of the EXIF header
 *
 * @pre App1Iter points to the beginning of the EXIF header by the calling function
 *
 * @param App1Iter[in] Iterator to the App1 data
 *
 * @return True if the EXIF header is correct
 *         False otherwise
*/
const bool cApp1::VerifyExifHeader(const std::vector<uint8_t>::iterator &App1Iter)
{
    bool exif_header_valid = false;

    // Copy the ReadBufferIter to a local iterator so we don't overwrite the iterator's position
    std::vector<uint8_t>::iterator exif_iter = App1Iter;

    // The next four bytes should be the word "EXIF" in ASCII
    const uint32_t read_exif_tag = ReadFourBytes(exif_iter);
    if (read_exif_tag == EXIF_TAG)
    {
        std::advance(exif_iter, FOUR_BYTE_LENGTH);

        const uint16_t zero_values = ReadTwoBytes(exif_iter);
        if (zero_values == BLANK_BYTE)
        {
            exif_header_valid = true;
        }
        else
        {
            std::cout << "Expected zero bytes at the end, found " << zero_values << "\n";
        }
    }
    else
    {
        std::cout << "EXIF not found in the header " << read_exif_tag << "\n";
    }

    return exif_header_valid;
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
void cApp1::GetTiffTagList(std::vector<uint8_t>::iterator &App1Iter)
{
    for (TiffTagStruct &CurrIfd : mIfdList)
    {
        CurrIfd.Tag = ReadTwoBytes(App1Iter);
        std::advance(App1Iter, TWO_BYTE_LENGTH);
        CurrIfd.Type = ReadTwoBytes(App1Iter);
        std::advance(App1Iter, TWO_BYTE_LENGTH);
        CurrIfd.Count = ReadFourBytes(App1Iter);
        std::advance(App1Iter, FOUR_BYTE_LENGTH);
        CurrIfd.Offset = ReadFourBytes(App1Iter);
        std::advance(App1Iter, FOUR_BYTE_LENGTH);
    }
}

/**
 * @brief Parses the date and time from the EXIF header
 *
 * @pre App1Iter starts at the beginning of the time date information
 *
 * @param[in] App1Iter Iterator to the EXIF data
 * @param[in] BytesToParse The number of characters to parse
 *
 * @return None
 */
void cApp1::ParseDateTime(std::vector<uint8_t>::iterator &App1Iter, const uint32_t BytesToParse)
{
    if (BytesToParse == EXPECTED_DATE_TIME_LENGTH)
    {
        // Get the expected bytes from the read buffer and push them into a string that will be
        // used to parse the date and time.
        std::string DateTimeStr;
        for (uint32_t Offset = 0; Offset < BytesToParse; ++Offset)
        {
            DateTimeStr.push_back(static_cast<char>(*(App1Iter + Offset)));
        }

        // The EXIF standard expects the final byte to be 0x00, which is a blank byte between
        // the date and time information, and the next field to parse.
        const uint8_t FinalByte = static_cast<uint8_t>(DateTimeStr.at(BytesToParse - 1));
        if (FinalByte == BLANK_BYTE)
        {
            sscanf(DateTimeStr.c_str(), "%d:%d:%d %d:%d:%d", &mDateTime.tm_year, &mDateTime.tm_mon, &mDateTime.tm_mday,
                                                             &mDateTime.tm_hour, &mDateTime.tm_min, &mDateTime.tm_sec);

            --mDateTime.tm_mon; // EXIF month is ones based, but struct tm expects zero based.
                                // Convert the EXIF month to zero based.
            mDateTime.tm_year -= 1900; // tm_year expects the number of years since 1900, but EXIF data is years since 0 AD
            std::cout << "Photo's date time is " << asctime(&mDateTime) << std::endl;
        }
        else
        {
            std::cout << "Expected the final byte to be 0x00, but got " << FinalByte << "\n";
        }
    }
    else
    {
        std::cout << "Date time expects 20 characters to parse but found " << BytesToParse << "\n";
    }


}

void cApp1::GetTiffTagData(uint32_t tiff_header_offset_start)
{
    for (const TiffTagStruct &CurrIfd : mIfdList)
    {
        const uint32_t offset_to_ifd_data = tiff_header_offset_start + CurrIfd.Offset;
        std::vector<uint8_t>::iterator ifd_data_iter = start_of_file + offset_to_ifd_data;
        switch (CurrIfd.Tag)
        {
            case IFD_DATE_TIME:
            {
                ParseDateTime(ifd_data_iter, CurrIfd.Count);
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
 * @pre The calling function has advanced the buffer iterator to the start of the App1 data
 *
 * @param[in] ReadBufferIter Iterator pointing to the start of App1 data
 *
 * @return The bytes read by this parsing function.
 */
const uint32_t cApp1::ParseApp(const std::vector<uint8_t>::iterator &read_buffer_iter)
{
    // Copy the ReadBufferIter to a local iterator so we don't overwrite the iterator's position
    std::vector<uint8_t>::iterator App1Iter = read_buffer_iter;

    // Get the length of App1
    const uint16_t TotalBytesRead = ReadTwoBytes(App1Iter);
    // std::cout << "App1 length in bytes is " << TotalBytesRead << "\n";
    std::advance(App1Iter, APP_DATA_SIZE_LENGTH);

    const bool valid_exif_header = VerifyExifHeader(App1Iter);
    if (!valid_exif_header)
    {
        return TotalBytesRead;
    }
    std::advance(App1Iter, EXIF_HEADER_LENGTH);

    // Determine the endianness of the data.
    const bool valid_endianess = GetEndianess(App1Iter);
    if (!valid_endianess)
    {
        return TotalBytesRead;
    }

    // All IFD offsets are based on the start of the TIFF header.
    // At this point ParseApp has reached the start of the TIFF header so determine
    // and save this offset.
    const uint32_t tiff_header_offset = std::distance(start_of_file, App1Iter);
    std::advance(App1Iter, ENDIAN_LENGTH);

    // The next two bytes should be 0x002A
    const uint16_t read_two_alpha_value = ReadTwoBytes(App1Iter);

    if (read_two_alpha_value != TWO_ALPHA_TAG)
    {
        std::cout << "Unexpected value after endian marker " << *App1Iter << std::endl;
        return TotalBytesRead;
    }
    std::advance(App1Iter, TWO_BYTE_LENGTH);

    // The next four bytes contains the offset of the 0th IFD in bytes
    // According to the standard, if the value is 0x00000008 then the 0th IFD is right after
    // the IFD offset.
    const uint32_t IfdOffset = ReadFourBytes(App1Iter);
    std::cout << "Offset to IFD is " << IfdOffset << " bytes" << std::endl;
    std::advance(App1Iter, FOUR_BYTE_LENGTH);

    // The next two bytes are the number of IFDs
    const uint16_t NumOfIFDs = ReadTwoBytes(App1Iter);
    std::cout << "Number of IFDs " << std::dec << NumOfIFDs << std::endl;
    std::advance(App1Iter, TWO_BYTE_LENGTH);

    mIfdList.resize(NumOfIFDs);
    GetTiffTagList(App1Iter);
    GetTiffTagData(tiff_header_offset);

    return TotalBytesRead;
}

cExifParser::cExifParser(const std::string &ImageFileName) : App0(), App1()
{
    ParseExifData(ImageFileName);
}

/**
 * @brief Checks to make sure the Start of Image (SOI) exists
 *
 * @param[in] ReadBufferIter Iterator which points to the start of the SOI
 *
 * @return True if the first two bytes indicate the SOI tag
 *         False otherwise
 */
const bool cExifParser::DoesStartOfImageExist(const std::vector<uint8_t>::iterator &ReadBufferIter)
{
    bool soi_found = false;
    const uint16_t soi = be16toh(*(reinterpret_cast<const std::vector<uint16_t>::iterator &>(ReadBufferIter)));
    if (soi == START_OF_IMAGE_MARKER)
    {
        soi_found = true;
    }

    return soi_found;
}

/**
* @brief Starting point to parse EXIF data.
*        Reads in the file and calls appropriate functions to parse the contents.
*
* @param[in] ImageFileName The name of the image whose EXIF data needs to be parsed.
*/
void cExifParser::ParseExifData(const std::string &ImageFileName)
{
    std::ifstream ImageFileStream(ImageFileName, std::ifstream::binary);
    std::cout << "Parsing " << ImageFileName << "\n";
    if (ImageFileStream.is_open())
    {
        std::vector<uint8_t> ReadBuffer(READ_BUFFER_LENGTH_BYTES);
        ImageFileStream.read(reinterpret_cast<char *>(&ReadBuffer[0]), READ_BUFFER_LENGTH_BYTES);
        std::vector<uint8_t>::iterator ExifIter = ReadBuffer.begin();
        if (ImageFileStream && DoesStartOfImageExist(ExifIter))
        {
            std::advance(ExifIter, SOI_MARKER_LENGTH_BYTES);
            if (App0.DoesAppMarkerExist(ExifIter, cApp0::MARKER_NUMBER))
            {
                std::cout << "Found APP0\n";
                std::advance(ExifIter, APP_MARKER_LENGTH_BYTES);
                const uint32_t BytesRead = App0.ParseApp(ExifIter);
                std::advance(ExifIter, BytesRead);
            }
            if (App1.DoesAppMarkerExist(ExifIter, cApp1::MARKER_NUMBER))
            {
                std::cout << "Found APP1\n";
                std::advance(ExifIter, APP_MARKER_LENGTH_BYTES);
                App1.SetStartOfFile(ReadBuffer.begin());
                const uint32_t BytesRead = App1.ParseApp(ExifIter);
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