#include "string_op.h"
#include <string.h>
#include <algorithm>
void split(const std::string & s, const std::string & delim,std::vector<std::string> & vec, int flag )
{
    if ( s.empty() || delim.empty()) return;

    //TLOG_DBG("src [%s]",s.c_str());

    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = std::string::npos;
    do
    {
        pos2 = s.find_first_of(delim,pos1);
        std::string::size_type end = pos2 != std::string::npos ? pos2 : s.size();
        //TLOG_DBG("start:%d,end:%d",pos1,end);

        if (!flag || end > pos1 )
        {
            vec.push_back(s.substr(pos1,end - pos1));
            //TLOG_DBG("split:[%s]",s.substr(pos1,end - pos1).c_str());
        }
        pos1 = pos2 + 1;
    }while ( pos2 != std::string::npos);
    return;
}

const char * index2(const char *haystack, const char *needle)
{
    int len = strlen(haystack);
    const char * ret = NULL;
    int i = 0;
    for ( ; i < len; ++i ) {
        if (index(needle,haystack[i])) {
            ret = haystack + i;
            break;
        }
    }
    return ret;
}

void split(const char * s, const char * delim, std::vector<int>& vec,int iEatEmpSec )
{
    if ( !s ) return;
    const char * pbegin = s;
    std::string stmp;

    const char * p = NULL;
    do {
        p = index2( pbegin , delim );
        if (p) {
            stmp.assign(pbegin,p - pbegin);
        } else {
            stmp.assign(pbegin);
        }

        if (!iEatEmpSec || !stmp.empty()) {
            vec.push_back(atoi(stmp.c_str()));
        }
        pbegin = p + 1;
    }while ( p!= NULL);
    return;
}

std::string map2str(std::map<std::string, std::string> & ssmap)
{
    if ( ssmap.empty() )
    {
        return "";
    }
    std::map<std::string,std::string>::iterator it = ssmap.begin();
    std::string d = it->first + "=" + it->second;
    for ( ++it;it != ssmap.end(); ++it )
    {
        d += "&"+ it->first + "=" + it->second;
    }
    return d;
}

int str2map(std::string & s,std::map<std::string, std::string> & ssmap)
{
    std::vector<std::string> vec;
    split(s, "=&", vec );
    if ( (vec.size() %2) != 0 )//非偶数
    {
        return -1;
    }

    for ( unsigned i = 0;i < vec.size();i+=2 )
    {
        ssmap.insert(make_pair(vec[i],vec[i+1]));
    }
    return 0;
}

const char* ReplaceAll(std::string& str,  const char* lpszPattern,  const char* lpszNewPattern)
{
    const size_t nsize = strlen(lpszNewPattern);
    const size_t psize = strlen(lpszPattern);

    for (size_t pos = str.find(lpszPattern, 0);
         pos != std::string::npos;
         pos = str.find(lpszPattern, pos + nsize))
    {
        str.replace(pos, psize, lpszNewPattern);
    }
    return str.c_str();
}

std::string&  lTrim(std::string &ss)
{
    std::string::iterator   p = std::find_if(ss.begin(),ss.end(),not1(std::ptr_fun(::isspace)));
    ss.erase(ss.begin(),p);
    return  ss;
}

std::string&  rTrim(std::string &ss)
{
    std::string::reverse_iterator  p = std::find_if(ss.rbegin(),ss.rend(),not1(std::ptr_fun(::isspace)));
    ss.erase(p.base(),ss.end());
    return   ss;
}

std::string&  trim(std::string &st)
{
    return lTrim(rTrim(st));
}

std::string&  lTrimCtrl(std::string &ss)
{
    std::string::iterator   p = std::find_if(ss.begin(),ss.end(),not1(std::ptr_fun(::iscntrl)));
    ss.erase(ss.begin(),p);
    return  ss;
}

std::string&  rTrimCtrl(std::string &ss)
{
    std::string::reverse_iterator  p = std::find_if(ss.rbegin(),ss.rend(),not1(std::ptr_fun(::iscntrl)));
    ss.erase(p.base(),ss.end());
    return   ss;
}

std::string&  trimCtrl(std::string &st)
{
    return lTrimCtrl(rTrimCtrl(st));
}

const std::string DelChar(const std::string &str, int c)
{
    std::string s;
    for(uint32_t i=0; i<str.length(); ++i)
    {
        if(c != str[i])
        {
            s += str[i];
        }
    }
    return s;
}