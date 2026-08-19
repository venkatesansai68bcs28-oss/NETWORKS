#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define R 10
#define C 7

int main(int argc,char *argv[])
{
    int s,c,rows,ep=-1,eq=-1;
    struct sockaddr_in sa,ca;
    socklen_t l=sizeof(ca);
    char d[R][C+1];
    int rp[R],cp[C];

    if(argc!=2) return 1;

    s=socket(AF_INET,SOCK_STREAM,0);
    if(s<0) return 1;

    sa.sin_family=AF_INET;
    sa.sin_addr.s_addr=htonl(INADDR_ANY);
    sa.sin_port=htons(atoi(argv[1]));

    if(bind(s,(struct sockaddr*)&sa,sizeof(sa))<0) return 1;
    listen(s,1);

    printf("2D Parity Server Waiting...\n");

    c=accept(s,(struct sockaddr*)&ca,&l);
    if(c<0) return 1;

    recv(c,&rows,sizeof(rows),0);
    recv(c,d,sizeof(d),0);
    recv(c,rp,sizeof(rp),0);
    recv(c,cp,sizeof(cp),0);

    printf("\nReceived Data:\n");

    for(int i=0;i<rows;i++)
    {
        for(int j=0;j<C;j++)
            printf("%c ",d[i][j]);
        printf("| %d\n",rp[i]);
    }

    printf("Column Parity: ");
    for(int j=0;j<C;j++)
        printf("%d ",cp[j]);

    for(int i=0;i<rows;i++)
    {
        int n=0;
        for(int j=0;j<C;j++)
            if(d[i][j]=='1') n++;

        if(n%2!=rp[i]) ep=i;
    }

    for(int j=0;j<C;j++)
    {
        int n=0;
        for(int i=0;i<rows;i++)
            if(d[i][j]=='1') n++;

        if(n%2!=cp[j]) eq=j;
    }

    if(ep!=-1 && eq!=-1)
    {
        printf("\n\nError at Row %d, Column %d\n",
               ep+1,eq+1);

        d[ep][eq]=(d[ep][eq]=='0')?'1':'0';

        printf("Corrected Bit: %c\n",d[ep][eq]);
    }
    else if(ep==-1 && eq==-1)
        printf("\n\nNo Error Detected\n");
    else
        printf("\n\nUnresolvable Error\n");

    printf("\nFinal Data:\n");

    for(int i=0;i<rows;i++)
    {
        for(int j=0;j<C;j++)
            printf("%c ",d[i][j]);
        printf("\n");
    }

    close(c);
    close(s);

    return 0;
}