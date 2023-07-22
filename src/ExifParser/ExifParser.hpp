/**
* @file ExifParser.hpp
* @brief Header file for the ExifParser library
* @author Daniel Fettke
* @date 6-5-2021
*/

#include <string>
#include <vector>
#include <stdint.h>

class cAppBase
{
protected:
    bool mLittleEndian;
public:
    cAppBase() : mLittleEndian(false) {};
    ~cAppBase() {};

    const bool DoesAppMarkerExist(const std::vector<uint8_t>::iterator &ReadBufferIter, const uint16_t app_marker);
    const uint16_t ReadTwoBytes(const std::vector<uint8_t>::iterator &ReadBufferIter);
    const uint32_t ReadFourBytes(const std::vector<uint8_t>::iterator &ReadBufferIter);
    virtual const uint32_t ParseApp(const std::vector<uint8_t>::iterator &ReadBufferIter) = 0;
};

/**
* @brief Application Marker 0 is used for storing JFIF information.
*/
class cApp0 : public cAppBase
{
private:
public:

    static constexpr uint16_t MARKER_NUMBER = 0xFFE0;

    const uint32_t ParseApp(const std::vector<uint8_t>::iterator &ReadBufferIter);
};

/**
 * @brief Application Marker 1 is used to store EXIF metadata.
 */
class cApp1 : public cAppBase
{
private:

    /**
     * @struct Structure to organize Tiff Tag information
     */
    struct TiffTagStruct
    {
        uint16_t Tag;    ///< The type of information to read. See 4.6.4
        uint16_t Type;   ///< The type of data to read. See 4.6.2
        uint32_t Count;  ///< The number of values to read
        uint32_t Offset; ///< The offset of the data to read from the start of the TIFF header
    };
    // Lengths in bytes to advance the iterator by
    static constexpr uint8_t APP_DATA_SIZE_LENGTH = 2;
    static constexpr uint8_t ENDIAN_LENGTH        = 2;
    static constexpr uint8_t TWO_BYTE_LENGTH      = 2;
    static constexpr uint8_t FOUR_BYTE_LENGTH     = 4;
    static constexpr uint8_t EXIF_HEADER_LENGTH   = 6;

    // Tiff tags
    static constexpr uint16_t IFD_DATE_TIME = 0x0132;

    // Other constants
    static constexpr uint16_t LITTLE_ENDIAN_TAG = 0x4949;
    static constexpr uint16_t BIG_ENDIAN_TAG    = 0x4D4D;
    static constexpr uint32_t EXIF_TAG          = 0x45786966; //< Hex representation of EXIF in ASCII
    static constexpr uint16_t TWO_ALPHA_TAG     = 0x2A;
    static constexpr uint8_t  BLANK_BYTE        = 0x00; //< Used to separate one IFD from another
    static constexpr uint8_t  EXPECTED_DATE_TIME_LENGTH = 20;

    std::vector<uint8_t>::iterator start_of_file;
    std::vector<TiffTagStruct> mIfdList;
    tm mDateTime;

    const bool GetEndianess(const std::vector<uint8_t>::iterator &App1Iter);
    const bool VerifyExifHeader(const std::vector<uint8_t>::iterator &App1Iter);
    void GetTiffTagList(std::vector<uint8_t>::iterator &App1Iter);
    void GetTiffTagData(uint32_t HeaderOffsetStart);
    void ParseDateTime(std::vector<uint8_t>::iterator &App1Iter, const uint32_t BytesToParse);

public:

    static constexpr uint16_t MARKER_NUMBER = 0xFFE1;

    cApp1() : mIfdList(), mDateTime() {};
    ~cApp1() {}

    const uint32_t ParseApp(const std::vector<uint8_t>::iterator &read_buffer_iter);
    const tm & GetDateTime() {return mDateTime;}
    void SetStartOfFile(const std::vector<uint8_t>::iterator &new_start_of_file) {start_of_file = new_start_of_file;}
};

class cExifParser
{

private:

    static constexpr uint32_t MAX_APPLICATION_DATA_LENGTH_BYTES = 0xFFFF;
    static constexpr uint32_t SOI_MARKER_LENGTH_BYTES = 2;
    static constexpr uint32_t APP_MARKER_LENGTH_BYTES = 2;
    static constexpr uint32_t NUMBER_OF_MARKERS_TO_READ = 3; // SOI, APP0 and APP1
    static constexpr uint32_t NUMBER_OF_DATA_REGIONS_TO_READ = 2; // APP0 and APP1
    static constexpr uint32_t READ_BUFFER_LENGTH_BYTES = (SOI_MARKER_LENGTH_BYTES * NUMBER_OF_MARKERS_TO_READ) +
                                                         (MAX_APPLICATION_DATA_LENGTH_BYTES * NUMBER_OF_DATA_REGIONS_TO_READ);

    static constexpr uint16_t START_OF_IMAGE_MARKER = 0xFFD8;

    cApp0 App0;
    cApp1 App1;

    const bool DoesStartOfImageExist(const std::vector<uint8_t>::iterator &ReadBufferIter);

public:
    cExifParser() : App0(), App1() {};
    cExifParser(const std::string &ImageFileName);
    ~cExifParser() {};

    void ParseExifData(const std::string &ImageFileName);
    const tm & GetDateTime() {return App1.GetDateTime();}

};