//
//
//

#include <string>
#include <vector>

class cExifParser
{

private:

    static constexpr unsigned int MAX_APPLICATION_DATA_LENGTH_BYTES = 0xFFFF;
    static constexpr unsigned int MARKER_LENGTH_BYTES = 0x02;
    static constexpr unsigned int NUMBER_OF_MARKERS_TO_READ = 3; // SOI, APP0 and APP1
    static constexpr unsigned int NUMBER_OF_DATA_REGIONS_TO_READ = 2; // APP0 and APP1
    static constexpr unsigned int READ_BUFFER_LENGTH_BYTES = (MARKER_LENGTH_BYTES * NUMBER_OF_MARKERS_TO_READ) +
                                                             (MAX_APPLICATION_DATA_LENGTH_BYTES * NUMBER_OF_DATA_REGIONS_TO_READ);

    const bool DoesStartOfImageExist(std::vector<unsigned char> &ReadBuffer);

public:
    cExifParser();
    cExifParser(const std::string ImageFileName);
    ~cExifParser();

    void ParseExifData(const std::string ImageFileName);

};