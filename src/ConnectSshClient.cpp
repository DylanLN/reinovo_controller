#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include "../include/reinovo_controller/ConnectSshClient.h"
using namespace std;
//构造函数
ConnectSshClient::ConnectSshClient()
{
    hostname = (const char *)"192.168.3.6";

    username = "ln";//strUser.c_str();
    password = "root";//strPwd.c_str();

    cout << "user : " << username << endl;
    cout << "url : " << hostname << endl;
    cout << "strpassword : " << password << endl;

    bytecount = 0;
    commandline = (const char *)"\n";
    char *exitsignal = (char *)"none";
    hostaddr = inet_addr(hostname);
}
ConnectSshClient::ConnectSshClient(string users,string url,string strpassword,string host)
{

   username = users.c_str();
   hostname = url.c_str();
   password = strpassword.c_str();

    cout << "user : " << username << endl;
    cout << "url : " << hostname << endl;
    cout << "strpassword : " << password << endl;

    bytecount = 0;
    commandline = (const char *)"\n";
    char *exitsignal = (char *)"none";
    hostaddr = inet_addr(hostname);
}

//析构函数
ConnectSshClient::~ConnectSshClient()
{
    if(channel){
        libssh2_channel_close(channel);
        libssh2_channel_free(channel);
        channel = NULL;
    }
    libssh2_session_disconnect(session,"Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);
    close(sock);
    libssh2_exit();
}

//连接ssh
bool ConnectSshClient::ConnectSsh()
{
    //SOCKET 连接
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        return false;
    }
    sin.sin_family = AF_INET;
    //port 22
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = hostaddr;
    if(::connect(sock,(struct sockaddr*)&sin, sizeof(sin)) != 0){
        fprintf(stderr,"failed to connect!\n");
        return  false;
    }

    //创建一个会话实例并启动它。
    session = libssh2_session_init();
    if(!session)
        return false;
    int sessionHandShake = libssh2_session_handshake(session,sock);
    if (sessionHandShake != 0)
        return false;

    fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
    fprintf(stderr, "Fingerprint: ");
    for(int i = 0; i < 20; i++) {
        fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
    }
    fprintf(stderr, "\n");

    if (libssh2_userauth_password(session, username, password))
    {
        fprintf(stderr, "\tAuthentication by password failed!\n");
        return false;
    }
    printf("requesting shell\n");

    channel = libssh2_channel_open_session(session);
    if (!channel)
    {
        cout << "channel error" <<endl;
        return false;
    }
    libssh2_channel_setenv(channel, "FOO", "bar");

    if (libssh2_channel_request_pty(channel, "vanilla") != 0)
    {
        cout << "vanilla error" <<endl;
        return false;
    }
    if (libssh2_channel_shell(channel))
    {
        cout << "shell error" <<endl;
        return false;
    }

    char buffer[0x4000] = {};
    for (int i = 0;i < 10; i++)
    {
        int rc = libssh2_channel_read(channel,buffer,0x4000);
        if (rc > 0){
            cout << buffer << endl;
        }else if (rc < 0){
            break;
        }
        string str = buffer;
        string struser = username;
        struser += "@";
        if (str.find(struser) != string::npos)
        {
            break;
        }
    }
    return true;
}

//写入
bool ConnectSshClient::Sshwrite(string data)
{
    commandline = data.c_str();
    libssh2_channel_write(channel,commandline,strlen(commandline));
    return true;
}

//读取
string ConnectSshClient::SshRead()
{
    char buffer[0x4000] = {};
    int rc = libssh2_channel_read(channel,buffer,0x4000);
    cout << "rc : " << rc << endl;
    string str = buffer;
    return str;
}
