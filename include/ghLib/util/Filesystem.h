/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

// Some filesystem helper functions because I can't have std::experimental::filesystem :(
// This have terrible error checking. I could give in and use Boost...

#include <sys/stat.h>
#include <string>
#include <sstream>
#include <fstream>

namespace ghLib {
namespace Util {
namespace FS {

bool create_directory(std::string path);
bool create_directories(std::string path);
bool exists(std::string path);
void rename(std::string path, std::string newPath);
bool is_directory(std::string path);
std::string parent_path(std::string path);
std::string basename(std::string path);

}
}
}
