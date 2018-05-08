#ifndef PROJECT_MSGQ_H
#define PROJECT_MSGQ_H

#include <stdint.h>
#include <string>

const uint32_t MAX_MSG_SIZE = 1024 * 1024;                  // MsgQ消息的最大值, 1M

const unsigned long QBYTES_NUM = 10 * 1024 * 1024;          // max number of bytes allowed on queue, 10M

class msgq
{
public:
    msgq() : m_MsgQId(0), m_Key(0), m_Flag(0), m_Init(false)
    {
    }

    ~msgq()
    {
    }

public:
    int Initialize(int iKey, bool bCreate);

    int PutMsg(long lType, const char* pBuf, int iBufLen, bool bWait = false);

    int GetMsg(long lType, char* pBuf, int& iBufLen, bool bWait = false);

    int GetAllMsg( char* pBuf, int& iBufLen, long & lType, bool bWait = false);

    int PeekMsg(long lType);

    int GetStat(int& iCBytes, int& iQNum);

private:
    typedef struct tagMsgBuf
    {
        long   lType;
        char   sBuf[MAX_MSG_SIZE];
    } MsgBuf_T;

private:
    int         m_MsgQId;          // Msg id

    int         m_Key;             // Msg Key

    int         m_Flag;            // Msg Flag

    bool        m_Init;

    MsgBuf_T    m_MsgBuf;

};


#endif //PROJECT_MSGQ_H
