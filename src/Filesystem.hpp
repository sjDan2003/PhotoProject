#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class Filesystem
{
private:

    static constexpr unsigned int CHUNK_SIZE = 32768;

    enum ErrorCodes
    {
        NO_ERROR             =  0,
        SOURCE_FILE_OPEN_ERR = -1,
        DEST_FILE_OPEN_ERR   = -2,
        SOURCE_FILE_READ_ERR = -3,
        DEST_FILE_READ_ERR   = -4
    };

public:
    static const unsigned int GetFileSize(std::ifstream &infile);
    static const unsigned int GetFileSize(const fs::path file_path);
    static const          int CopyFile(const fs::path &source_file, const fs::path &destination_file);
    static const          int Verify(const fs::path &source_file, const fs::path &destination_file);
};