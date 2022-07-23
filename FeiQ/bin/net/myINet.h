#pragma once
#include <winsock2.h>
#include <string>
#include <set>

class InetMediator;
class INet
{
public:
	INet(){}
	virtual ~INet(){}
public:
	virtual bool InitNet()=0;
	virtual bool SendData(long lSendip, char* buf, int nLen)=0;
	virtual void UnInitNet()=0;
    //ip地址转换，long类型
    static std::string GetIpString(long ip){
       SOCKADDR_IN sockaddr;
       sockaddr.sin_addr.S_un.S_addr=ip;
       return inet_ntoa(sockaddr.sin_addr);
    }
    //获取本地ip地址列表
    static std::set<long> GetValidIpList(){
        //获取主机名
        char hostname[100]="";
        std::set<long>setIp;
        if(SOCKET_ERROR!=gethostname(hostname,100)){
            //通过主机名获取ip列表
            int i=0;
            struct hostent* remoteHost=gethostbyname(hostname);
            while(remoteHost->h_addr_list[i]!=0){
                setIp.insert(*(u_long*)remoteHost->h_addr_list[i++]);
            }
            return setIp;
        }else{
            return std::set<long>();
        }
    }
protected:
	virtual bool RecvData()=0;
	InetMediator* m_pMediator;
};

