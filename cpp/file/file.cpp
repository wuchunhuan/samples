#include <sys/types.h>
#include <regex.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <vector>
#include <string.h>
#include <libgen.h>
#include "file.h"

int fileio::ls(const std::string &sPath, const std::string &sPattern, std::vector<std::string> &vFiles, bool basename)
{
    DIR *pDir = opendir( sPath.c_str() );
    if ( !pDir )
    {
        return -1;
    }

    regex_t iReg;
    if ( !sPattern.empty() )
    {
        int iRetCode = regcomp( &iReg, sPattern.c_str(), REG_EXTENDED | REG_ICASE );
        if ( iRetCode != 0 )
        {
            closedir( pDir );
            char szErr[1024] = {0};
            regerror(iRetCode, &iReg, szErr, sizeof(szErr));
            return -2;
        }
    }
    struct dirent *pDirEntry = NULL;
    while ( ( pDirEntry = readdir( pDir ) ) != NULL )
    {
        if ( strcmp(pDirEntry->d_name, ".") == 0 || strcmp(pDirEntry->d_name, "..") == 0 )
        {
            continue;
        }
        if ( sPattern.empty() || regexec( &iReg, pDirEntry->d_name, 0, NULL, REG_ICASE ) == 0 )
        {
            if (basename) {
                vFiles.push_back(pDirEntry->d_name );
            } else {
                vFiles.push_back( sPath + "/" + pDirEntry->d_name );
            }
        }
    }
    if ( !sPattern.empty() )
    {
        regfree(&iReg);
    }
    closedir( pDir );
    return 0;
}

int fileio::grep(const std::string &sPattern, std::vector<std::string> &in, std::vector<std::string> &out) {
    regex_t iReg;
    if ( !sPattern.empty() )
    {
        int iRetCode = regcomp( &iReg, sPattern.c_str(), REG_EXTENDED | REG_ICASE );
        if ( iRetCode != 0 )
        {
            char szErr[1024] = {0};
            regerror(iRetCode, &iReg, szErr, sizeof(szErr));
            return -2;
        }
    }

    for (const auto & it : in) {
        if ( sPattern.empty() || regexec( &iReg, it.c_str(), 0, NULL, REG_ICASE ) == 0 ) {
            out.push_back(it);
        }
    }
    if ( !sPattern.empty() )
    {
        regfree(&iReg);
    }
    return 0;
}

int fileio::line(const std::string &sPath, std::vector<std::string> &vLines)
{
    std::ifstream file(sPath.c_str());

    if ( !file )
    {
        return -1;
    }
    vLines.clear();
    vLines.reserve(1000);

    std::string sLine;
    while ( std::getline(file, sLine) )
    {
        vLines.push_back(sLine);
    }
    return 0;
}

bool fileio::fexists(const std::string &sName)
{
    struct stat szBuf;
    if ( stat( sName.c_str(), &szBuf ) != 0 )
    {
        return false;
    }
    return true;
}

int fileio::cp( const std::string &src, const std::string &dest )
{
    int n = fsize(src);
    if ( n < 0 )return -1;

    std::ifstream ifile( src.c_str() );
    std::ofstream ofile( dest.c_str() );
    if ( !ifile || !ofile )
        return -1;

    char *ret = static_cast<char *>(calloc(n + 1, 1));
    ifile.read(ret, n);
    ofile.write (ret, n);
    free(ret);
    ifile.close();
    ofile.close();
    return 0;
}

int fileio::fsize(const std::string &sFileName)
{
    struct stat st;
    int n = stat(sFileName.c_str(), &st);
    if ( 0 == n )
    {
        return st.st_size;
    }
    return -1;
}

std::string fileio::cat(const std::string &sFileName)
{
    int n = fsize(sFileName);
    if ( n <= 0 ) return "";
    char *ret = static_cast<char *>(calloc(n + 1, 1));
    if ( ret == NULL ) return "";
    std::ifstream f;
    f.open(sFileName.c_str());
    f.read(ret, n);
    f.close();
    std::string sContent;
    sContent.assign(ret, n);
    free(ret);
    return sContent;
}

int fileio::frename(const std::string &sSrc, const std::string &sDest)
{
    int iRet = rename(sSrc.c_str(), sDest.c_str());
    if ( iRet != 0 )
    {
        return iRet;
    }
    return 0;
}

int fileio::fremove(const std::string &sFile)
{
    if ( 0 != unlink(sFile.c_str()) )
    {
        return -1;
    }

    return 0;
}

/*
 * 功能：取文件最后修改时间
 * 返回：返回整数的时间数值，出错返0；
 */
time_t fileio::flastmodtime(const std::string &sFile)
{
    struct stat stFile;
    if ( stat(sFile.c_str(), &stFile) == -1 )
    {
        return 0;
    }
    return stFile.st_mtime;
}

//相比dwLastTime时间，文件是否有变化
bool fileio::fchanged(const std::string &sFile, uint32_t dwLastTime)
{
    struct stat st;
    int iRet = stat(sFile.c_str(), &st);
    if ( 0 != iRet )
    {
        return false;
    }

    if ( st.st_mtime > dwLastTime )//最后一次修改时间
    {
        return true;
    }
    return false;
}

int fileio::fmkdir( const std::string & sPath )
{
    char sDir[ PATH_MAX ] = {0};
    int iPos = 0;
    int iCnt = 0;
    const char *ptr;

    const char * sFullName = sPath.c_str();
    while ( true )
    {
        if ( ( ptr = index( sFullName + iPos, '/' ) ) == 0 )
            break;
        iCnt = ptr - sFullName + 1;
        iPos = iCnt;
        bzero( sDir, sizeof( sDir ) );
        strncpy( sDir, sFullName, iCnt );
        if ( mkdir( sDir, 0777 ) < 0 )
        {
            if ( errno != EEXIST )
                return -1;
        }
    }
    return 0;
}

const std::string fileio::GetBasename(const std::string &sPath)
{
    char szBasename[512] = "";
    snprintf(szBasename, sizeof(szBasename)-1, "%s", sPath.c_str());
    return basename(szBasename);
}

const std::string fileio::GetDirname(const std::string &sPath)
{
    char szBasename[512] = "";
    snprintf(szBasename, sizeof(szBasename)-1, "%s", sPath.c_str());
    return dirname(szBasename);
}

// 当前路径是目录时，返回true；
bool fileio::IsDir(const std::string &sPath)
{
    /*
     * 注：
     *      stat() ：如果sPath是一个链接，函数操作的是sPath指向的那个对象；
     *      lstat()：与stat()的唯一不同是，若sPath是链接，lstat()操作的是链接本身；
     */
    struct stat stDir;
    if( stat(sPath.c_str(), &stDir) == -1)
    {
        return false;
    }
    return S_ISDIR(stDir.st_mode) ? true : false;
}

// 当前路径是文件时，返回true；
bool fileio::IsFile(const std::string &sPath)
{
    /*
     * 注：
     *      stat() ：如果sPath是一个链接，函数操作的是sPath指向的那个对象；
     *      lstat()：与stat()的唯一不同是，若sPath是链接，lstat()操作的是链接本身；
     */
    struct stat stDir;
    if( stat(sPath.c_str(), &stDir) == -1)
    {
        return false;
    }
    return S_ISREG(stDir.st_mode) ? true : false;
}
