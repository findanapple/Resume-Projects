#pragma once

#include <cstring>
#define	_DEF_UDP_PORT 12345
#define	_DEF_BUF_SIZE 4096

//协议头
#define _DEF_PROTOCAL_BASE (1000)
//上线
#define _DEF_PROTOCAL_ONLINE_RQ (_DEF_PROTOCAL_BASE+1)
#define _DEF_PROTOCAL_ONLINE_RS (_DEF_PROTOCAL_BASE+2)
//下线
#define _DEF_PROTOCAL_OFFLINE_RQ (_DEF_PROTOCAL_BASE+3)
//聊天
#define _DEF_PROTOCAL_CHAT_RQ (_DEF_PROTOCAL_BASE+4)
//传文件
#define _DEF_PROTOCAL_FILEINFO_RQ (_DEF_PROTOCAL_BASE+5)

#define _DEF_NAME_SIZE (50)
#define _DEF_CONTENT_SIZE (1024)

//请求结构体
struct STRU_ONLINE {
    STRU_ONLINE() :nType(_DEF_PROTOCAL_ONLINE_RQ) {
        memset(szName,0, _DEF_NAME_SIZE);
    }
    int nType;
    //long ip;
    char szName[_DEF_NAME_SIZE];
};

struct STRU_OFFLINE_RQ
{
    STRU_OFFLINE_RQ() :nType(_DEF_PROTOCAL_ONLINE_RQ) {}
    int nType;
};

struct STRU_CHAT_RQ
{
    STRU_CHAT_RQ() :nType(_DEF_PROTOCAL_ONLINE_RQ) {
        memset(szContent, 0, _DEF_CONTENT_SIZE);
    }
    int nType;
    //long ip;
    char szContent[_DEF_CONTENT_SIZE];
};
