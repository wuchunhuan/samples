#ifndef SAMPLES_FILE_H
#define SAMPLES_FILE_H

#include <string>

namespace fileio {
    int ls(const std::string &sPath, const std::string &sPattern, std::vector<std::string> &vFiles, bool basename=true);

    int grep(const std::string &sPattern, std::vector<std::string> &in, std::vector<std::string> &out);

    int line(const std::string &sPath, std::vector<std::string> &vLines);

    bool fexists(const std::string &sName);

    int cp( const std::string &src, const std::string &dest );

    int fsize(const std::string &sFileName);

    std::string cat(const std::string &sFileName);

    int frename(const std::string &sSrc, const std::string &sDest);

    int fremove(const std::string &sFile);

    time_t flastmodtime(const std::string &sFile);

    bool fchanged(const std::string &sFile, uint32_t dwLastTime);

    int fmkdir( const std::string & sPath );

    const std::string GetBasename(const std::string &sPath);

    const std::string GetDirname(const std::string &sPath);

    bool IsDir(const std::string &sPath);

    bool IsFile(const std::string &sPath);
}


#endif //SAMPLES_FILE_H
