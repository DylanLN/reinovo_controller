#ifndef CONNECTSSHCLIENT_H
#define CONNECTSSHCLIENT_H

/*
    Include
*/

#include <string>
#include "libssh2_config.h"
#include <libssh2.h>
#include <libssh2_sftp.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

using namespace std;
#define BUFSIZE 32000

class ConnectSshClient
{
public:
    ConnectSshClient();
    ConnectSshClient(string users,string url,string strpassword,string host);
    bool ConnectSsh();
    bool Sshwrite(string data);
    string SshRead();
    ~ConnectSshClient();

private:
    const char *hostname;
    const char *commandline;
    const char *username;
    const char *password;
    unsigned long hostaddr;
    int sock;
    struct sockaddr_in sin;
    const char *fingerprint;
    LIBSSH2_SESSION *session;
    LIBSSH2_CHANNEL *channel;
    int rc;
    int exitcode;
    char *exitsignal;
    int bytecount;
    size_t len;
    LIBSSH2_KNOWNHOSTS *nh;
    int type;
};

#endif
