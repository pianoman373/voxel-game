#include "Path.hpp"

#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
#include <sstream>
#include "tinydir.h"

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <stdio.h>
#endif

// https://stackoverflow.com/questions/236129/the-most-elegant-way-to-iterate-the-words-of-a-string
template<typename Out>
static void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty())
            *(result++) = item;
    }
}
// ------------------------------------------------------------------------------------------------

Path::Path() {
    this->absolute = true;
    drive = 0;
    isFolder = true;
}

Path::Path(bool absolute) {
    this->absolute = absolute;
    drive = 0;
    isFolder = true;
}

Path::Path(const char *raw): Path(std::string(raw)) {}

Path::Path(const std::string &raw) {
    size_t i = 0;

    //check if absolute
    if (raw[0] == '/' || raw[0] == '\\') {
        //Unix style absolute

        absolute = true;
        drive = 0;
        i = 1;
    }
    else if (raw[1] == ':' && (raw[2] == '/' || raw[2] == '\\')) {
        //Windows style absolute

        absolute = true;
        drive = raw[0];
        i = 3;
    }
    else {
        absolute = false;
        drive = 0;
    }

    if (raw[raw.size()-1] == '/' || raw[raw.size()-1] == '\\') {
        isFolder = true;
    }
    else {
        isFolder = false;
    }


    std::string currentElement = "";
    while (i < raw.size()) {
        char c = raw[i];

        if (c == '/' || c == '\\') {
            if (currentElement.size() > 0) {
                if (currentElement.compare(".")) { // Never a need to store ./

                    if (!currentElement.compare("..")) { //if a .. is found do some fancy stuff
                        if (data.size() == 0) {
                            if (!absolute) { //No logic in having .. at the beginning of an absolute path
                                data.push_back(currentElement);
                            }
                        }
                        else {
                            if (data.back().compare("..")) { //don't pop back if the element above is ..
                                data.pop_back();
                            }
                            else {
                                data.push_back(currentElement);
                            }
                        }
                    }
                    else {
                        data.push_back(currentElement);
                    }

                }
                currentElement = "";
            }
        }
        else {
            currentElement += c;
        }

        i++;
    }
    if (currentElement.size() > 0) {
        if (!currentElement.compare(".")) { // Never a need to store ./
            isFolder = true;
        }
        else if (!currentElement.compare("..")) { //Really doesn't make much sense to do this, but need to account for it anyways
            data.pop_back();
            isFolder = true;        }
        else {
            data.push_back(currentElement);
        }
    }
}

std::string Path::toString() const {
    if (!absolute && data.size() == 0) {
        return "./";
    }

    std::string ret;

    if (absolute) {
        if (drive) {
            // Windows style

            ret += drive;
            ret += ":/";
        }
        else {
            //Unix style

            ret += "/";
        }
    }

    for (size_t i = 0; i < data.size(); i++) {
        ret += data[i];

        if (i != data.size() - 1 || isFolder) {
            ret += "/";
        }
    }

    return ret;
}

bool Path::isAbsolute() const {
    return absolute;
}

bool Path::pointsToFolder() const {
    return isFolder;
}

bool Path::hasParent() const {
    return data.size() > 0;
}

Path Path::getParent() const {
    Path p = *this;
    p.isFolder = true;

    if (absolute) {
        if (p.data.size() > 0) {
            p.data.pop_back();
        }
    }
    else {
        if (p.data.size() > 0) {
            if (!p.data.back().compare("..")) {
                p.data.push_back("..");
            }
            else {
                p.data.pop_back();
            }
        }
        else {
            p.data.push_back("..");
        }
    }

    return p;
}

Path Path::appendFolder(const std::string &folder) const {

    if (!folder.compare("..")) {
        return getParent();
    }

    Path p = *this;

    if (!folder.compare(".")) {
        return p;
    }

    std::string folderCopy = folder;

    std::replace(folderCopy.begin(), folderCopy.end(), '\\', '_');
    std::replace(folderCopy.begin(), folderCopy.end(), '/', '_');

    if (p.data.size() > 0) {
        if (p.isFolder) {
            p.data.push_back(folderCopy);
        }
        else {
            p.data[p.data.size()-1] = folderCopy;

        }
    }
    else {
        p.data.push_back(folderCopy);
    }
    p.isFolder = true;

    return p;
}

Path Path::appendFile(const std::string &file) const {
    if (!file.compare("..")) {
        return getParent();
    }

    Path p = *this;

    if (!file.compare(".")) {
        return p;
    }

    std::string fileCopy = file;

    std::replace(fileCopy.begin(), fileCopy.end(), '\\', '_');
    std::replace(fileCopy.begin(), fileCopy.end(), '/', '_');

    if (p.data.size() > 0) {
        if (p.isFolder) {
            p.data.push_back(fileCopy);
        }
        else {
            p.data[p.data.size()-1] = fileCopy;
        }
    }
    else {
        p.data.push_back(fileCopy);
    }
    p.isFolder = false;

    return p;
}

Path Path::appendPath(const Path &path) const {
    Path p = *this;

    if (!p.isFolder && p.data.size() > 0) {
        p.data.pop_back();
    }

    for (size_t i = 0; i < path.data.size(); i++) {
        std::string d = path.data[i];

        if (!d.compare("..")) {
            p = p.getParent();
        }
        else {
            p.data.push_back(path.data[i]);
        }
    }
    p.isFolder = path.isFolder;

    return p;
}

Path Path::relativeTo(const Path &path) const {
    if (this->absolute != path.absolute) {
        return *this;
    }

    Path right = path;
    if (!right.isFolder) {
        right = right.getParent();
    }

    // recenter both paths. e.g foo/bar/test.txt and foo/bar/files/something.txt
    // become test.txt and files/something.txt
    Path recenteredLeft(this->absolute);
    recenteredLeft.isFolder = this->isFolder;
    Path recenteredRight(right.absolute);

    size_t i = 0;
    while (i < this->data.size() && i < right.data.size()) {
        if (this->data[i] == right.data[i]) {

        }
        else {
            break;
        }

        i++;
    }
    recenteredLeft.data = std::vector<std::string>(this->data.begin() + i, this->data.end());
    recenteredRight.data = std::vector<std::string>(right.data.begin() + i, right.data.end());

    Path p(false);
    p.isFolder = this->isFolder;

    //however many folders are in recenteredRight is how many ..'s we add to the beginning of the path

    for (size_t i = 0; i < recenteredRight.data.size(); i++) {
        p.data.push_back("..");
    }

    //now append recenteredleft

    p.data.insert(p.data.end(), recenteredLeft.data.begin(), recenteredLeft.data.end());

    return p;
}

Path Path::getProgramWorkingDirectory() {
#ifdef _WIN32
    char cCurrentPath[FILENAME_MAX];
    GetCurrentDirectory(sizeof(cCurrentPath), cCurrentPath);

    return Path(std::string(cCurrentPath) + "/");
#else
    char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            return Path(cwd);

        Path p("/");
        return p;
#endif
}

std::vector<Path> Path::listDirectory(const Path &path, bool directoryOnly) {
    std::vector<Path> ret;

    tinydir_dir dir;
    size_t i;
    tinydir_open_sorted(&dir, path.toString().c_str());

    for (i = 0; i < dir.n_files; i++) {
        tinydir_file file;
        tinydir_readfile_n(&dir, &file, i);

        if (file.name[0] == '.') {
            continue;
        }

        if (file.is_dir) {
            ret.push_back(Path(file.name));
        }
        else if (!directoryOnly) {
            ret.push_back(Path(file.name));
        }
    }

    tinydir_close(&dir);

    return ret;
}

std::ostream &operator<<(std::ostream &os, const Path &m) {
    return os << m.toString();
}