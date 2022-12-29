/**
* @file main.cpp
* @brief The main file of the program
* @author Dan Fettke
* @date 6-5-2021
*/

#include <iostream>
#include "ExifParser/ExifParser.hpp"
#include "Filesystem.hpp"
#include <filesystem>
#include <sstream>
#include <fstream>

namespace fs = std::filesystem;

/**
 * This is the main function
 */
int main()
{
    fs::path source_root_path("/mnt/chromeos/SMB/8d17eb5d7a9837aa07c6e1aadb2568ee144069f5d1d8fb032b1c1e60db4b13cd/Samsung SM-G960U1 Camera Backup/");
    fs::path destination_root_path("/mnt/chromeos/SMB/22927c5b089cbcbbeeeed9e3591fdcfd54f9ea40143ed0f26fde0fb8f5f2c92f/My Pictures/Camera Photos");
    std::cout << "Root path is a path " << fs::is_directory(source_root_path) << std::endl;
    if (fs::is_directory(source_root_path))
    {

        for (const fs::directory_entry& source_image : fs::directory_iterator(source_root_path))
        {
            if (source_image.path().extension() == ".jpg")
            {
                std::cout << "    " << source_image.path() << '\n';
                cExifParser NewParser(source_image.path().string());
                const tm &PhotoDateTime = NewParser.GetDateTime();
                std::cout << "Photo's date and time is " << asctime(&PhotoDateTime) << std::endl;
                std::stringstream date_folder;
                date_folder << (PhotoDateTime.tm_year + 1900) << '/' << (PhotoDateTime.tm_mon + 1) << '-' << PhotoDateTime.tm_mday << '-' << (PhotoDateTime.tm_year + 1900);
                fs::path destination_path = destination_root_path;
                destination_path /= date_folder.str();
                std::cout << destination_path << std::endl;
                std::cout << "Folder exists " << fs::exists(destination_path) << std::endl;

                if (!fs::exists(destination_path))
                {
                   try
                   {
                       fs::create_directories(destination_path);
                   }
                   catch(const fs::filesystem_error& e)
                   {
                       std::cerr << e.what() << '\n';
                       break;
                   }

                }
                destination_path /= source_image.path().filename();
                std::cout << destination_path << std::endl;
                std::cout << "File exists " << fs::exists(source_image.path()) << ' ' << fs::exists(destination_path) << std::endl;
                std::cout << "Source file is regular file " << fs::is_regular_file(source_image.path()) << " Filesize " << fs::file_size(source_image.path()) << std::endl;
                if (Filesystem::CopyFile(source_image.path(), destination_path) == 0)
                {
                    if (Filesystem::Verify(source_image.path(), destination_path) == 0)
                    {
                        std::cout << "Image copied successfully" << std::endl;
                    }
                    else
                    {
                        std::cout << "Could not verify the images" << std::endl;
                    }
                }
                else
                {
                    std::cout << "Could not copy the image" << std::endl;
                }
            }
            break;
        }
    }
}
