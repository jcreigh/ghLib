#include "ghLib/util/Filesystem.h"

namespace ghLib {
namespace Util {
namespace FS {

bool create_directory(std::string path) {
#ifdef _WIN32
	return _mkdir(path.c_str()) == 0;
#else
	return mkdir(path.c_str(), 0755) == 0;
#endif
}

bool create_directories(std::string path) {
	if (is_directory(path)) {
		return true; // We're already a directory
	}
	if (!create_directories(parent_path(path))) {
		return false; // Failed to create one of the parent directories
	}
	return create_directory(path);
}

bool exists(std::string path) {
	struct stat st;
	return (stat(path.c_str(), &st) == 0);
}

void rename(std::string path, std::string newPath) {
	::rename(path.c_str(), newPath.c_str());
}

bool is_directory(std::string path) {
	struct stat st;
	if (stat(path.c_str(), &st) != 0) {
		return false;
	}
	return S_ISDIR(st.st_mode);
}

std::string parent_path(std::string path) {
	size_t last_slash_idx = path.find_last_of("\\/");
	if (last_slash_idx != std::string::npos) {
		return path.substr(0, last_slash_idx);
	}
	return "";
}

std::string basename(std::string path) {
	std::string filename = path;
	size_t last_slash_idx = filename.find_last_of("\\/");
	if (last_slash_idx != std::string::npos) {
		filename.erase(0, last_slash_idx + 1);
	}
	return filename;
}

}
}
}
