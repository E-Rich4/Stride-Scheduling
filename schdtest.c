#include "types.h"
#include "user.h"

#define P_LOOP_CNT 0x10000000
#define C_LOOP_CNT 0x20000000
#define G_LOOP_CNT 0x30000000


unsigned int avoid_optm = 0; // a variable used to avoid compiler optimization
void do_parent(void)
{
    unsigned int cnt = 0;
    unsigned int tmp = 0;

    while(cnt < P_LOOP_CNT)
    {
        tmp += cnt;
        cnt ++;
    }
    
    avoid_optm = tmp;
}


void do_child(void)
{
    unsigned int cnt = 0;
    unsigned int tmp = 0;

    while(cnt < C_LOOP_CNT)
    {
        tmp += cnt;
        cnt ++;
    }
    
    avoid_optm = tmp;
}

void do_grand_child(void)
{
    unsigned int cnt = 0;
    unsigned int tmp = 0;

    while(cnt < G_LOOP_CNT)
    {
        tmp += cnt;
        cnt ++;
    }
    
    avoid_optm = tmp;
}

void two_fork_test()
{
    printf(1, "TWO FORK TEST\n");
    printf(1, "parent has 10 tickets\n");
    printf(1, "parent pid: %d\n\n", getpid());
    int pid = 0;
    //int parent_pid = getpid();

    pid = fork();
    if (pid < 0)
    {
        printf(1, "fork() failed!\n");
        exit();
    }
    else if (pid == 0) // child
    {
        sleep(1);
        do_child();
    }
    else // parent
    {
        // Transfer tickets
        int transfer_amount = 40;
        transfer_tickets(pid, transfer_amount);
        do_parent();
        if (wait() < 0)
        {
            printf(1, "wait() failed!\n");
        }
        wait();
    }
	
	printf(1, "\n");
}

void three_fork_test()
{
    printf(1, "\n\nTHREE FORK TEST\n");
    printf(1, "parent has 3 tickets, child has 53, GC has 43\n");
    printf(1, "parent pid: %d\n\n", getpid());
    int pid = 0;
    //int parent_pid = getpid();

    pid = fork();
    if (pid < 0)
    {
        printf(1, "fork() failed!\n");
        exit();
    }
    else if (pid == 0) // child
    {
        pid = fork();
        if (pid < 0) {
            printf(1, "fork() 2 failed! \n");
            exit();
        }
        else if (pid == 0) {
            sleep(2);
            do_grand_child();
        }
        else {
            int transfer_amount = 10;
            transfer_tickets(pid, transfer_amount);
            do_child();
            if (wait() < 0) {
                printf(1, "wait() 2 failed! \n");
            }
        }
        
    }
    else // parent
    {
        // Transfer tickets
        sleep(1);
        int transfer_amount = 30;
        transfer_tickets(pid, transfer_amount);
        do_parent();
        if (wait() < 0)
        {
            printf(1, "wait() failed!\n");
        }
    }
	
	printf(1, "\n");
}

int
main(int argc, char *argv[])
{
    enable_sched_trace(1);
    set_sched(1);

    two_fork_test();
    three_fork_test();

    set_sched(0);
    enable_sched_trace(0);
    
    exit();
}
