/**
* @file ExifParser.hpp
* @brief Header file for the ExifParser library
* @author Daniel Fettke
* @date 6-5-2021
*/

#include <string>
#include <vector>

class cAppBase
{
protected:
    bool mLittleEndian;
public:
    cAppBase() : mLittleEndian(true) {};
    ~cAppBase() {};

    const bool DoesAppMarkerExist(const std::vector<unsigned char>::iterator &ReadBufferIter, const unsigned char MarkerNumber);
    const unsigned short ConvertEndian16(const unsigned short Value);
    const unsigned int   ConvertEndian32(const unsigned int   Value);
    const unsigned short ReadTwoBytes(const std::vector<unsigned char>::iterator &ReadBufferIter);
    const unsigned int   ReadFourBytes(const std::vector<unsigned char>::iterator &ReadBufferIter);
    virtual const unsigned int ParseApp(const std::vector<unsigned char>::iterator &ReadBufferIter) = 0;
};

class cApp0 : public cAppBase
{
private:
public:

    static const unsigned char MARKER_NUMBER = 0xE0;

    const unsigned int ParseApp(const std::vector<unsigned char>::iterator &ReadBufferIter);
};

class cApp1 : public cAppBase
{
private:

    struct IfdStruct
    {
        unsigned short Tag;    //< The type of information to read. See 4.6.4
        unsigned short Type;   //< The type of data to read. See 4.6.2
        unsigned int   Count;  //< The number of values to read
        unsigned int   Offset; //< The offset of the data to read from the start of the header
    };
    // Lengths to advance the iterator by
    static constexpr unsigned char APP_DATA_SIZE_LENGTH = 2;
    static constexpr unsigned char ENDIAN_LENGTH        = 2;

    // Other constants
    static constexpr unsigned char LITTLE_ENDIAN_TAG = 0x49;
    static constexpr unsigned char BIG_ENDIAN_TAG    = 0x4D;

public:

    static const unsigned char MARKER_NUMBER = 0xE1;

    const unsigned int ParseApp(const std::vector<unsigned char>::iterator &ReadBufferIter);
};

class cExifParser
{

private:

    static constexpr unsigned int MAX_APPLICATION_DATA_LENGTH_BYTES = 0xFFFF;
    static constexpr unsigned int MARKER_LENGTH_BYTES = 0x02;
    static constexpr unsigned int NUMBER_OF_MARKERS_TO_READ = 3; // SOI, APP0 and APP1
    static constexpr unsigned int NUMBER_OF_DATA_REGIONS_TO_READ = 2; // APP0 and APP1
    static constexpr unsigned int READ_BUFFER_LENGTH_BYTES = (MARKER_LENGTH_BYTES * NUMBER_OF_MARKERS_TO_READ) +
                                                             (MAX_APPLICATION_DATA_LENGTH_BYTES * NUMBER_OF_DATA_REGIONS_TO_READ);

    cApp0 App0;
    cApp1 App1;

    const bool DoesStartOfImageExist(const std::vector<unsigned char>::iterator &ReadBufferIter);

public:
    cExifParser() : App0(), App1() {};
    cExifParser(const std::string ImageFileName);
    ~cExifParser() {};

    void ParseExifData(const std::string ImageFileName);

};