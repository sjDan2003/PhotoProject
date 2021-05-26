//
//
//

#include <string>
#include <vector>

class cAppBase
{
protected:
public:
    cAppBase() {};
    ~cAppBase() {};

    const bool DoesAppMarkerExist(const std::vector<unsigned char>::iterator &ReadBufferIter, const unsigned char MarkerNumber);
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