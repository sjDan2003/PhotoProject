#include <string.h> // For memcmp
#include "Filesystem.hpp"

/**
 * @brief Gets the size of a file pointed to by an ifstream
 *
 * @pre None
 *
 * @param[in] infile The ifstream object to get the file size from
 *
 * @return If the file is open this function will return the file's size in bytes.
 *         If the file is not open this function will return zero bytes.
 */
const unsigned int Filesystem::GetFileSize(std::ifstream &infile)
{
    if (!infile.is_open())
    {
        return 0;
    }

    infile.seekg(0, infile.end);
    unsigned int length = infile.tellg();
    infile.seekg(0, infile.beg);

    return length;
}

const unsigned int Filesystem::GetFileSize(const fs::path file_path)
{
    return fs::file_size(file_path);
}

const int Filesystem::CopyFile(const fs::path &source_file, const fs::path &destination_file)
{
    std::ifstream infile;
    std::ofstream outfile;

    infile.open(source_file.string(),std::ifstream::binary);
    if (!infile)
    {
        return SOURCE_FILE_OPEN_ERR;
    }

    outfile.open(destination_file.c_str(), std::ofstream::binary);
    if (!outfile)
    {
        infile.close();
        return DEST_FILE_OPEN_ERR;
    }

    const unsigned int file_size = Filesystem::GetFileSize(infile);
    unsigned long curr_byte = 0;
    char buffer[CHUNK_SIZE] = {};

    for(; (curr_byte + CHUNK_SIZE) < file_size; curr_byte += CHUNK_SIZE)
    {
        infile.read(buffer,CHUNK_SIZE);
        if (!infile)
        {
            infile.close();
            outfile.close();
            return SOURCE_FILE_READ_ERR;
        }
        outfile.write(buffer, CHUNK_SIZE);
        if (!outfile)
        {
            infile.close();
            outfile.close();
            return DEST_FILE_READ_ERR;
        }
    }

    if ((curr_byte < file_size) && infile && outfile)
    {
        unsigned long BytesLeft = file_size - curr_byte;
        infile.read(buffer,BytesLeft);
        if (!infile)
        {
            infile.close();
            outfile.close();
            return SOURCE_FILE_READ_ERR;
        }
        outfile.write(buffer, BytesLeft);
        if (!outfile)
        {
            infile.close();
            outfile.close();
            return DEST_FILE_READ_ERR;
        }
    }
    infile.close();
    outfile.close();
    return NO_ERROR;
}

const int Filesystem::Verify(const fs::path &source_file, const fs::path &destination_file)
{
    std::ifstream source_infile;
    std::ifstream dest_infile;

    source_infile.open(source_file.string(),std::ifstream::binary);
    if (!source_infile)
    {
        return SOURCE_FILE_OPEN_ERR;
    }

    dest_infile.open(destination_file.c_str(), std::ofstream::binary);
    if (!dest_infile)
    {
        source_infile.close();
        return DEST_FILE_OPEN_ERR;
    }

    const unsigned int source_file_size = Filesystem::GetFileSize(source_infile);
    const unsigned int dest_file_size   = Filesystem::GetFileSize(dest_infile);

    if (source_file_size != dest_file_size)
    {
        return -1;
    }
    unsigned long curr_byte = 0;
    char source_buffer[CHUNK_SIZE] = {};
    char dest_buffer[CHUNK_SIZE] = {};

    for(; (curr_byte + CHUNK_SIZE) < source_file_size; curr_byte += CHUNK_SIZE)
    {
        source_infile.read(source_buffer,CHUNK_SIZE);
        if (!source_infile)
        {
            source_infile.close();
            dest_infile.close();
            return SOURCE_FILE_READ_ERR;
        }
        dest_infile.read(dest_buffer, CHUNK_SIZE);
        if (!dest_infile)
        {
            source_infile.close();
            dest_infile.close();
            return DEST_FILE_READ_ERR;
        }

        if (memcmp(source_buffer, dest_buffer, CHUNK_SIZE) != 0)
        {
            source_infile.close();
            dest_infile.close();
            return -1;
        }
    }

    if ((curr_byte < source_file_size) && source_infile && dest_infile)
    {
        unsigned long BytesLeft = source_file_size - curr_byte;
        source_infile.read(source_buffer,BytesLeft);
        if (!source_infile)
        {
            source_infile.close();
            dest_infile.close();
            return SOURCE_FILE_READ_ERR;
        }
        dest_infile.read(dest_buffer, BytesLeft);
        if (!dest_infile)
        {
            source_infile.close();
            dest_infile.close();
            return DEST_FILE_READ_ERR;
        }
        if (memcmp(source_buffer, dest_buffer, BytesLeft) != 0)
        {
            source_infile.close();
            dest_infile.close();
            return -1;
        }
    }
    source_infile.close();
    dest_infile.close();
    return NO_ERROR;
}