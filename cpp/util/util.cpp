#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <libgen.h>
#include <string.h>
#include <signal.h>
#include "util.h"

std::string timestr( time_t tmt, const char * pFmt )
{
    char szTime[ 32 ] = {0};
    struct tm p;
    time_t lTime = tmt;
    if ( lTime == 0 )
    {
        lTime = time( ( time_t * ) NULL );
    }
    memcpy( &p, localtime( &lTime ), sizeof( p ) ); /*取得当地时间*/
    strftime( szTime, sizeof( szTime ),pFmt, &p);
    return szTime;
}

time_t tmstamp(const char * szTime,const char * pFmt)
{
    if (szTime == NULL)
    {
        return -1;
    }
    struct tm stTm;
    bzero(&stTm, sizeof(stTm));
    if (!strptime(szTime,pFmt,&stTm))
    {
        return -1;
    }
    return  mktime(&stTm);
}



double BankerRound(double f)
{
    const uint64_t integer = (uint64_t)f; // 整数部分
    const uint64_t decimals = (uint64_t)(f * 100) % 100; // 小数部分（小数点后两位）
    const uint64_t f3 = uint64_t(f * 1000) % 10;  // 小数点后第3位

    //Debug("f3=[%lu], integer=[%lu], decimals=[%lu]", f3, integer, decimals);

    if(f3 < 5)             // 小数点后第3位若为零~四则舍掉
    {
        return (double)integer + (double)decimals / 100;  // 整数部分 + 小数部分

    }
    else if(f3 > 5)        // 若为六~九则进位
    {
        return (double)integer + (double)(decimals + 1) / 100;  // 整数部分 + 小数部分（＋1为进位）
    }
    else/* f3 == 5 */
    {
        const uint64_t f4 = uint64_t(f * 10000) % 10;  // 小数点后第4位
        //Debug("f4=[%lu]", f4);

        if(0 != f4) // 五后（小数点后第4位）不为零就进位；
        {
            return (double)integer + (double)(decimals + 1) / 100;  // 整数部分 + 小数部分（＋1为进位）
        }
        else/* 0 == f4 */   // 五后（小数点后第4位）为零，则看五前（小数点后第2位）
        {
            const uint64_t f2 = uint64_t(f * 100) % 10;  // 小数点后第2位
            //Debug("f2=[%lu]", f2);


            if(f2 % 2 == 0) // 五前为偶应舍去
            {
                return (double)integer + (double)decimals / 100;  // 整数部分 + 小数部分
            }
            else // 五前为奇要进一
            {
                return (double)integer + (double)(decimals + 1) / 100;  // 整数部分 + 小数部分（＋1为进位）
            }
        }
    }

    return f;
}





inline void sig_term( int signo )
{
    if ( signo == SIGTERM ) /* catched signal sent by kill(1) command */
    {
        //BOSS_INFO( "进程收到信号退出" );
        exit( 0 );
    }
}

void daemon_init( sighandler_t termfun)
{
    //int iMaxFileOpen;
    //char sDir[ 256 ] = {0};

    //getcwd( sDir, sizeof( sDir ) );
    //iMaxFileOpen = getdtablesize();

    if ( fork() != 0 )
    {
        exit( 0 );
    }
    if ( setsid() < 0 )
    {
        exit( 1 );
    }
    if ( fork() != 0 )
    {
        exit( 0 );
    }
    //chdir( sDir );
    umask( 0 );
    signal( SIGINT, SIG_IGN );
    signal( SIGHUP, SIG_IGN );
    signal( SIGQUIT, SIG_IGN );
    signal( SIGPIPE, SIG_IGN );
    signal( SIGTTOU, SIG_IGN );
    signal( SIGTTIN, SIG_IGN );
    signal( SIGUSR1, SIG_IGN );
    signal( SIGUSR2, SIG_IGN );
    signal( SIGCHLD, SIG_IGN );

    if ( !termfun )
    {
        termfun = &sig_term;
    }
    signal( SIGTERM, termfun );
}