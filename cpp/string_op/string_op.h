#ifndef PROJECT_STRING_OP_H
#define PROJECT_STRING_OP_H

#include <string>
#include <vector>
#include <map>
/**
*
* @param s
* @param delim 每个字符都会被当成分隔符
* @param iFlag  iFlag|1:eat flag @see Split_flag_t
*/
void split(const std::string & s, const std::string & delim,std::vector<std::string> & vec,int flag = 0);
void split(const char * s, const char * delim,std::vector<int>& vec,int iEatEmpSec=0);

//用于将map里面的明值对变换成 a=XX&b=XX的形式
std::string map2str(std::map<std::string,std::string> & ssmap);

int str2map(std::string & s,std::map<std::string,std::string> & ssmap);

//用lpszNewPattern替换字符串中所有的lpszPattern
const char* ReplaceAll(std::string& str,  const char* lpszPattern,  const char* lpszNewPattern);

std::string&  lTrim(std::string &ss);

std::string&  rTrim(std::string &ss);
//去掉空格
std::string&  trim(std::string &st);

std::string&  lTrimCtrl(std::string &ss);

std::string&  rTrimCtrl(std::string &ss);
//去掉控制字符
std::string&  trimCtrl(std::string &st);

// 删掉串中所有指定的字符
const std::string DelChar(const std::string &str, int c);
#endif //PROJECT_STRING_OP_H
