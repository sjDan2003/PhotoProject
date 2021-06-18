/**
* @file main.cpp
* @brief The main file of the program
* @author Dan Fettke
* @date 6-5-2021
*/

#include <iostream>
#include "ExifParser/ExifParser.hpp"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

/**
 * This is the main function
 */
int main()
{
    cExifParser NewParser("DSC00667.jpg");
    std::string photo_backup_path = "/mnt/chromeos/SMB/8d17eb5d7a9837aa07c6e1aadb2568ee144069f5d1d8fb032b1c1e60db4b13cd/Samsung SM-G960U1 Camera Backup/";
    fs::path backup_path(photo_backup_path);
    std::cout << "Root path is a path " << fs::is_directory(backup_path) << std::endl;
    if (fs::is_directory(backup_path))
    {
        // fs::directory_iterator it(backup_path);
        // for (; it != fs::directory_iterator(); ++it)
        //     std::cout << *it << '\n';
        for (fs::directory_entry& x : fs::directory_iterator(backup_path))
        {
            if (fs::extension(x) == ".jpg")
            {
                std::cout << "    " << x.path() << '\n';
                cExifParser NewParser(x.path().string());
            }
        }
    }

}
