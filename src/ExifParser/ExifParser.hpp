//
//
//

#include <string>

class cExifParser
{

private:
    const bool DoesStartOfImageExist();

public:
    cExifParser();
    cExifParser(const std::string ImageFileName);
    ~cExifParser();

    void ParseExifData(const std::string ImageFileName);

};