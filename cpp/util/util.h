#ifndef SAMPLES_TMP_H
#define SAMPLES_TMP_H

#include <time.h>
#include <string>
#include <map>

/**
* 由 tmt 得到 yyyy-mm-dd hh:mi:ss 格式的时间
*/
std::string timestr( time_t tmt = 0,const char * pFmt = "%Y-%m-%d %H:%M:%S");


/**
 * 返回字符串对应的时间戳
 *
 * @param szTime 时间字符串  yyyy-mm-dd hh:mi:ss
 *
 * @return 时间戳
 */
time_t tmstamp(const char * szTime,const char * pFmt = "%Y-%m-%d %H:%M:%S");




/*
 *   银行家舍入算法:
 *
 *   即：小数点后保留2位，小数点后第3位若为零~四则舍掉，若为六~九则进位；小数点后第3位若为五则分三种情况：
 *   五后（小数点后第4位）不为零就进位；
 *   五后（小数点后第4位）为零，则看五前（小数点后第2位），五前为偶应舍去，五前为奇要进一。
 *
 *   举例：
 *   2.3340 → 2.33（小数点后第3位若为零~四则舍掉）
 *   2.3360 → 2.34（小数点后第3位若为六~九则进位）
 *   2.3151 → 2.32（小数点后第3位若为五，五后不为零就进位）
 *   2.3250 → 2.32（五后为零看前一位奇偶，五前为偶应舍去）
 *   2.3350 → 2.34（五前为奇要进一）
 */
double BankerRound(double f);




//默认收到kill -9 使用上面的函数
typedef void ( *sighandler_t ) ( int );
void daemon_init( sighandler_t termfun = NULL );

#endif //SAMPLES_TMP_H
