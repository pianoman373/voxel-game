#pragma once

#include <string>
#include <vector>

/**
 * Stores a given file path in a cross-platform manner that can be manipulated easier than a traditional string path
 * and can be converted to a string version that may be used on both Windows and Unix systems.
 *
 * The Path object constructor is designed to handle any kind of path format and store it in the path object as an
 * abstract list of the directory structure and the path's metadata (relative or absolute, Windows drive letter, etc).
 * Behaviour of this class is designed to be the same regardless of with OS is running the executable. To do this, the
 * toString function outputs a path string in a format that can be used on both Unix and Windows systems, as long as
 * the path is not absolute. If the input string used to construct the Path object is absolute (starting with a "/" on
 * Unix or a "X:/" on Windows) then it will retain whichever style was used to construct it when converted back to
 * string form.
 */
class Path {
private:
    std::vector<std::string> data;

    bool isFolder;
    bool absolute;
    char drive;

public:
    operator std::string() const
    {
        return toString();
    }

    Path();


    /**
     * Constructs an empty Path object that points to either the filesystem root, or a relative root (known as "./")
     *
     * @param absolute Whether to create an absolute or relative Path object.
     */
    Path(bool absolute);



    Path(const char *raw);

    /**
     * Constructs a Path object using the given string. The input string can be a path pointing to a file or
     * a folder, and may also be a relative or absolute path. Both Unix and Windows style paths are valid, and forward
     * or backslashes can be used interchangably in each style.
     *
     * Note: For a file path to be considered a directory, it must end in a backslash or forward slash. If a
     * file path ends with any name it is assumed that it is pointing to a file of that name.
     *
     * @param raw Input string to be stored.
     *
     * Examples:
     * 
     * @code
     * Path("C:\foo\bar.txt");
     * Path("/foo/bar/");
     * Path("foo/bar.txt");
     * @endcode
     *
     */
    Path(const std::string &raw);

    /**
     * Returns a formatted string version of this path using the following formatting rules:
     *     - A "/" is always used for path separators even on Windows.
     *     - If a Windows drive path was specified on construction, the path will begin with "X:/" where X = the drive path.
     *     - A relative path may have any number of "../" symbols at the beginning but only at the beginning.
     *     - If the path references a folder, the output will end in a "/".
     *
     * @return Formatted string version of this path.
     */
    std::string toString() const;

    /**
     * If this path was constructed from an absolute path (Beginning in either "/" or "X:/") then it is considered
     * an absolute path.
     *
     * @return Whether this path object is referencing an absolute path or not.
     */
    bool isAbsolute() const;

    /**
     * Path objects may only point to either a folder or a file. A path that ends in any name is considered to point to
     * a file (excluding paths that end in "." or ".."). Paths that end in a slash always point to a folder.
     *
     * @return True if this path points to a folder, false if it points to a file.
     */
    bool pointsToFolder() const;

    /**
     * @return True if this this path does not equal the root directory or ./
     */
    bool hasParent() const;

    /**
     * Returns the parent folder of whichever folder/file this path links to. If this path points to the root
     * directory, or ./ then it will return a copy of itself.
     *
     * @return The parent folder of this path.
     */
    Path getParent() const;

    /**
     * Adds a folder to the end of this path. If this path points to a file, then the appended folder will replace the
     * file at the end of the path. Any forward or backslashes in the folder name will be converted to underscores. If
     * the given string is "." then the function will return a copy of this Path object. Passing the string ".." is
     * equivalent to calling getParent().
     *
     * @param folder Folder name to append.
     * @return Modified path.
     */
    Path appendFolder(const std::string &folder) const;

    /**
     * Adds a file to the end of this path. If this path already points to a file, then the appended file will replace the
     * file at the end of the path. Any forward or backslashes in the file name will be converted to underscores. If
     * the given string is "." then the function will return a copy of this Path object. Passing the string ".." is
     * equivalent to calling getParent().
     *
     * @param file File name to append.
     * @return Modified path.
     */
    Path appendFile(const std::string &file) const;

    /**
     * Appends the specified path to the end of this path and returns the result. If this Path object references a file,
     * the directory containing the file will be used. The path which is appended to the end will be considered as a
     * relative path for the purposes of this function, even if it is marked as an absolute path.
     *
     * @param path The path to append to this one.
     * @return The combination of this path and the argument path.
     */
    Path appendPath(const Path &path) const;

    /**
     * Returns the path pointing to the same place that this path currently points to, but from the perspective of the
     * given path. The returned path can be thought of as the path you would need to type in a terminal to be able
     * to access this one from the working directory of the argument path.
     *
     * Examples:
     *
     * @code
     * "/usr/share/resource.txt" relative to "/usr/bin/"  is "../share/resource.txt"
     * "../foo/bar.txt" relative to "folder/" is "../../foo/bar.txt"
     * "C:/folder1/folder2/folder3/file.txt" relative to "C:/folder1/resource.txt" is "folder2/folder3/file.txt"
     * @endcode
     *
     * @param path Path to be considered the working directory of the relative return path.
     * @return Path to this referenced object from the perspective of the argument path.
     */
    Path relativeTo(const Path &path) const;

    /**
     * Returns a Path pointing to this process's working directory.
     *
     * @return This process's working directory.
     */
    static Path getProgramWorkingDirectory();

    /**
     * Returns a list of paths pointing to the files and folders contained within the given path.
     *
     * @param path Path to search.
     *
     * @param directoryOnly Forces the output to be directories only.
     */
    static std::vector<Path> listDirectory(const Path &path, bool directoryOnly=false);

    static void createDirectory(const Path &path);
};

std::ostream &operator<<(std::ostream &os, const Path &m);