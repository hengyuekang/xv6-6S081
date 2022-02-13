#include "kernel/types.h"
#include "user/user.h"

void redirect(int opt, int pipe[])
{
    close(opt);     // close std
    dup(pipe[opt]); // pipe to std
    // communicate with pipe,a process use it either to write or to read
    close(pipe[0]);
    close(pipe[1]);
}
void filter(int prime)
{
    int num;
    while (read(0, &num, 4)) // in the end,all process block here
    {
        if (num % prime != 0)
        {
            write(1, &num, sizeof(num));
        }
    }
    close(1); // if no data and still write end,process will block in read;close write end in time to avoid this
    // inform next process to close
}
void wait_prime()
{
    int pid[2]; // curr to next process
    pipe(pid);
    int prime;
    if (read(0, &prime, sizeof(prime))) // sleep if no data or opening write end
    {

        printf("prime %d\n", prime); // print to 1(default stdout)

        if (fork() == 0)
        {
            redirect(0, pid);
            wait_prime(); // create next process
            exit(0);
        }
        else
        {
            redirect(1, pid);
            filter(prime);
            wait(0); // keep the pipeline complete to let process exit after wait_prime()
            //generate process and print prime one by one,wait for exiting orderly(look like a single process from shell)
        }
    }
}
int main(int argc, char *argv[])
{
    int p[2]; // main to filter
    pipe(p);

    if (fork() == 0)
    {
        redirect(0, p); // child read from curr pipe and write to the next pipe
        wait_prime();
        exit(0);
    }
    else
    {
        redirect(1, p);
        for (int i = 2; i < 36; i++)
        {
            write(1, &i, sizeof(i)); // all numbers to pipeline
        }
        close(1); // in the end,close the first write end to help close all in pipeline;unblock process left to right
        wait(0);  // wait for all outputs to be printed;wait:only if one of its child exits,it coutinues
        exit(0);
    }
    exit(0);
}