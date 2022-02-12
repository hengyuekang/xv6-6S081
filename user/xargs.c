#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"
#define LINE 5
int readstr(char *s)
{
    while (1)
    {
        //in case of no data but continue to read
        if(read(0, s, sizeof(char)) == 0)
        {
            exit(0);
        }
        if (*s == ' ')
        {
            *s = '\0';
            break;
        }
        if (*s == '\n')
        {
            *s = '\0';
            return LINE;
        }
        s++;
    }
    return 1;
}
int readarg(int argc, char *argvs[])
{
    char s[512];
    int res;
    int tmp = argc;
    while ((res = readstr(s)) != 0)
    {
        if (tmp > MAXARG)
        {
            fprintf(2, "too many arguments to command in xargs\n");
            exit(1);
        }
        if(strcmp(s," ")==0)
        {
            exit(0);
        }
        // printf("read buffer-arg:%s\n",s);
        strcpy(argvs[tmp++], s);
        // printf("after read buffer-arg:%s\n",argvs[tmp]);
        memset(s, 0, sizeof(s));
        if (res == LINE)
        {
            // printf("a read line end!\n");
            break;
        }
    }
    // printf("all arg read from input is %d\n",tmp);
    argvs[tmp] =0;//help exec to end reading argvs
    // todo:a limited implementation,to make it more flexible,use a char[maxarg][maxsize] to buffer for argvs and dynamically teminate argvs for evec
    return tmp;
}
int main(int argc, char *argv[])
{
    if (argc > MAXARG)
    {
        fprintf(2, "too much arguments to xargs\n");
        exit(1);
    }
    char *argvs[MAXARG];
    for (int i = 0; i < MAXARG; i++)
    {
        argvs[i] = (char *)malloc(sizeof(char) * 32);
    }

    for (int i = 1; i < argc; i++)
    {
        strcpy(argvs[i - 1], argv[i]);
    }
    int res;
    while ((res = readarg(argc, argvs)) != 0)
    {

        if (fork() == 0)
        {
            // printf("pcocess %d is running:argvs is %s:\n", getpid(), argvs[3]);
            // for (int i = 0; i < res; i++)
            // {
            //     printf("%s ", argvs[i]);
            // }
            exec(argvs[0], argvs);
            // printf("process %d done!\n", getpid());
        }
        else
        {
            wait(0);
        }
    }
    for (int i = 0; i < MAXARG; i++)
    {
        free(argvs[i]);
    }
    exit(0);
}