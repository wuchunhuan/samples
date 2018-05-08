//boost库引用文件
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
//标准库
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace boost::archive::iterators;

//Base64编码
bool Base64Encode(string * outPut, const string & inPut);
//Base64解码
bool Base64Decode(string * outPut, const string & inPut);
int main(void)
{

    cout<<"***************************************"<<endl;
    cout<<"*       boost库Base64编码解码         *"<<endl;
    cout<<"***************************************"<<endl;

    string inputstr = "http://blog.csdn.net/nk_wang/article/details/44536269/\n#@$%^^&**(*(())+=";
    //Base64编码
    string Output;
    string Input;
    cout<<"Base64编码前："<<inputstr<<endl;
    Base64Encode(&Output,inputstr);
    cout<<"Base64编码后："<<Output<<endl;
    //Base64解码
    cout<<"Base64解码前："<<Output<<endl;
    Base64Decode(&Input,Output);
    cout<<"Base64解码后："<<Input<<endl;
    return 0;
}

bool Base64Encode(string * outPut, const string & inPut)
{
    typedef base64_from_binary<transform_width<string::const_iterator,6,8> > Base64EncodeIter;

    stringstream  result;
    copy(Base64EncodeIter(inPut.begin()),
         Base64EncodeIter(inPut.end()),
         ostream_iterator<char>(result));

    size_t Num = (3 - inPut.length() % 3) % 3;
    for (size_t i = 0; i < Num; i++)
    {
        result.put('=');
    }
    *outPut = result.str();
    return outPut->empty() == false;
}
bool Base64Decode(string * outPut, const string & inPut)
{
    typedef transform_width<binary_from_base64<string::const_iterator>,8,6> Base64DecodeIter;

    stringstream result;
    try
    {
        copy(Base64DecodeIter(inPut.begin()),
             Base64DecodeIter(inPut.end()),
             ostream_iterator<char>(result));
    }
    catch (...)
    {
        return false;
    }
    *outPut = result.str();
    return outPut->empty() == false;
}
