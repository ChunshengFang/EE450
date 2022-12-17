#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include "clientA.h"

using namespace std;

const std::string localhost = "127.0.0.1";
const std::string central_port_A = "25779";

int main(int argc, char *argv[])
{
    int client_fd = init_tcp_connect(localhost,central_port_A);
    printf("The client is up and running.\n");
    //send the username A to central
    if (send(client_fd, argv[1], strlen(argv[1]), 0) < 0)
    {
        fprintf(stderr,"clientA send error\n");
        return 1;
    }

    printf("The client sent %s to the Central server.\n", argv[1]);
    //receive the message and output
    size_t read_bytes;
    char buf[512];
    if ((read_bytes = recv(client_fd, buf, 512, 0)) < 0)
    {
        fprintf(stderr,"clientA recv error\n");
        return 1;
    }
    string data(buf,read_bytes);
    //receive the central's data
    //if contains #,means no error
    if(data.find("#")!=string::npos)
    {
        //split the usernames and gap
        vector<string> datas = split(data,"#");
        vector<string> names = split(datas[0],"!");
        printf("Found compatibility for %s and %s:\n",names[0].c_str(),names[names.size()-1].c_str());
        for(size_t i = 0;i < names.size();++i)
        {
            printf("%s",names[i].c_str());
            if(i != names.size()-1)
            {
                printf(" --- ");
            }
        }
        printf("\n");
        printf("Compatibility score: %s\n",datas[1].substr(0,4).c_str());
    }
    else
    {   //handle the error
        vector<string> usernames = split(data,"!");
        printf("Found no compatibility between %s and %s\n",usernames[0].c_str(),usernames[1].c_str());
    }

    close(client_fd);
}
