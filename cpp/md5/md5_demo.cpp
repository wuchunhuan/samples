//github下载最新的OpenSSL：https://github.com/openssl/openssl
//
//在linux解压压缩包
//
//安装OpenSSL
//./config  --prefix=/usr/local --openssldir=/usr/local/ssl
//make && make install
//./config shared --prefix=/usr/local --openssldir=/usr/local/ssl
//make clean
//make && make install
//
//用ln将需要的so文件链接到/usr/lib或者/lib这两个默认的目录下面
//ln -s /where/you/install/lib/*.so /usr/lib
//sudo ldconfig

//int MD5_Init(MD5_CTX *c);
//初始化MD5上下文结构

//int MD5_Update(MD5_CTX *c, const void *data, size_t len);
//刷新MD5，将文件连续数据分片放入进行MD5刷新。

//int MD5_Final(unsigned char *md, MD5_CTX *c);
//产生最终的MD5数据

//unsigned char *MD5(const unsigned char *d, size_t n, unsigned char *md);
//直接产生字符串的MD5


#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <openssl/md5.h>

using namespace std;

#define MAXDATABUFF 1024
#define MD5LENTH 16

int main(int arc,char *arv[])
{
    string strFilePath = arv[1];
    ifstream ifile(strFilePath.c_str(),ios::in|ios::binary);    //打开文件
    unsigned char MD5result[MD5LENTH];
    do
    {
        if (ifile.fail())   //打开失败不做文件MD5
        {
            cout<<"open file failure!so only display string MD5!"<<endl;
            break;
        }
        MD5_CTX md5_ctx;
        MD5_Init(&md5_ctx);

        char DataBuff[MAXDATABUFF];
        while(!ifile.eof())
        {
            ifile.read(DataBuff,MAXDATABUFF);   //读文件
            int length = ifile.gcount();
            if(length)
            {
                MD5_Update(&md5_ctx,DataBuff,length);   //将当前文件块加入并更新MD5
            }
        }
        MD5_Final(MD5result,&md5_ctx);  //获取MD5
        cout<<"file MD5:"<<endl;
        for(int i = 0; i < MD5LENTH; i++)  //将MD5以16进制输出
            cout<< hex <<(int)MD5result[i];
        cout<<endl;
    }while(false);

    MD5((const unsigned char*)strFilePath.c_str(),strFilePath.size(),MD5result);    //获取字符串MD5
    cout<<"string MD5:"<<endl;
    for(int i = 0; i < MD5LENTH; i++)
        cout << hex << (int)MD5result[i];
    cout<<endl;
    return 0;
}