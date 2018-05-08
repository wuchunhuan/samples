#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include "msgq.h"

int msgq::Initialize(int iKey, bool bCreate)
{
    int iFlag = 0666;
    iFlag |= bCreate ? IPC_CREAT : 0;

    if((m_MsgQId = msgget(iKey, iFlag)) == -1)
    {
        printf("msgget() Failed: %s", strerror(errno));
        return -1;
    }

    // Set QBytes
    struct msqid_ds buf;
    int iRetCode = msgctl(m_MsgQId, IPC_STAT, (struct msqid_ds *)&buf);
    if(iRetCode == -1)
    {
        printf("msgctl(IPC_STAT) Failed: %s", strerror(errno));
        return -1;
    }

    buf.msg_qbytes = QBYTES_NUM;
    iRetCode = msgctl(m_MsgQId, IPC_SET, (struct msqid_ds *)&buf);
    if(iRetCode == -1)
    {
        printf("msgctl(IPC_SET) Failed: %s", strerror(errno));
        return -1;
    }

    m_Key      = iKey;
    m_Flag     = iFlag;
    m_Init     = true;

    return 0;
}

int msgq::PutMsg(long lType, const char* pBuf, int iBufLen, bool bWait /* = false */)
{
    if(!m_Init)
    {
        printf("%s", "Not Init");
        return -1;
    }

    if(pBuf == NULL || iBufLen <= 0)
    {
        printf("%s", "Invalid Buffer");
        return -1;
    }

    m_MsgBuf.lType = lType;
    memcpy(m_MsgBuf.sBuf, pBuf, iBufLen);

    int msgflg = bWait ? 0 : IPC_NOWAIT;
    if((msgsnd(m_MsgQId, (struct msgbuf*)&m_MsgBuf, iBufLen, msgflg)) ==-1)
    {
        printf("msgsnd() Failed: %s", strerror(errno));
        return -1;
    }

    return 0;
}

int msgq::GetMsg(long lType, char* pBuf, int& iBufLen, bool bWait /* = false */)
{
    if(!m_Init)
    {
        printf("%s", "Not Init");
        return -1;
    }

    int msgflg = bWait ? 0 : IPC_NOWAIT;
    m_MsgBuf.lType = lType;
    int iRetCode = msgrcv(m_MsgQId, (struct msgbuf*)&m_MsgBuf, MAX_MSG_SIZE, lType, msgflg);
    if(iRetCode < 0)
    {
        if(errno != ENOMSG)
        {
            printf("msgrcv() Failed: %s", strerror(errno));
        }

        return -1;
    }

    if(iBufLen < iRetCode)                      // 空间不足
    {
        printf("msgrcv() Failed: Not Enough Buffer! MsgLen[%d] RecvBufLen[%d]", iRetCode, iBufLen);
        return -1;
    }

    memcpy(pBuf, m_MsgBuf.sBuf, iRetCode);
    iBufLen = iRetCode;

    return 0;
}

int msgq::GetAllMsg( char* pBuf, int& iBufLen, long & lType, bool bWait /* = false */)
{
    if(!m_Init)
    {
        printf("%s", "Not Init");
        return -1;
    }

    int msgflg = bWait ? 0 : IPC_NOWAIT;
    int iRetCode = msgrcv(m_MsgQId, (struct msgbuf*)&m_MsgBuf, MAX_MSG_SIZE, 0, msgflg);
    if(iRetCode < 0)
    {
        if(errno != ENOMSG)
        {
            printf("msgrcv() Failed: %s", strerror(errno));
        }

        return -1;
    }

    if(iBufLen < iRetCode)                      // 空间不足
    {
        printf("msgrcv() Failed: Not Enough Buffer! MsgLen[%d] RecvBufLen[%d]", iRetCode, iBufLen);
        return -1;
    }

    lType = m_MsgBuf.lType ;
    memcpy(pBuf, m_MsgBuf.sBuf, iRetCode);
    iBufLen = iRetCode;

    return 0;
}


int msgq::PeekMsg(long lType)
{
    if(!m_Init)
    {
        printf("%s", "Not Init");
        return -1;
    }

    int iRetCode = msgrcv(m_MsgQId, NULL, 0, lType,  IPC_NOWAIT);
    if(-1 == iRetCode)
    {
        if(errno == E2BIG)
            return 0;
    }

    return -1;
}

int msgq::GetStat(int& iCBytes, int& iQNum)
{
    if(!m_Init)
    {
        printf("%s", "Not Init");
        return -1;
    }

    struct msqid_ds buf;
    int iRetCode = msgctl(m_MsgQId, IPC_STAT, (struct msqid_ds *)&buf);
    if(iRetCode == -1)
    {

        printf("msgctl(IPC_STAT) Failed: %s", strerror(errno));
        return -1;
    }

    iCBytes = buf.__msg_cbytes;
    iQNum = buf.msg_qnum;

    return 0;
}

