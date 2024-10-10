#include "types.h"
#include "user.h"

//ask for scheduler type
//forks and waits with print

void RR_test(void) {

}

void stride_reset_test(void) {
    int pid, pid2, pid3;
    printf(1, "ticket reset count test\n");
    printf(1, "//////////////\n\n");
    //1 process, 100 tickets
    printf(1, "start, %d tickets\n", tickets_owned(getpid()));

    printf(1, "fork called:");
    pid = fork();
    //2 processes, 50 tickets each
    if (pid < 0) {
        printf(1, "fork() failed (%d)\n", pid);
        exit();
    }
    if (pid > 0) {
        printf(1, "\n\tparent: %d tickets", tickets_owned(pid));
        wait();
    }
    if (pid == 0) {
        printf(1, "\n\tchild: %d tickets", tickets_owned(pid));
        exit();
    }

    if (pid > 0) {
        printf(1, "\n\nfork 2:");
    }

    printf(1, "\n\tparent: %d tickets", tickets_owned(pid));
    pid = fork();
    if (pid == 0) {
        printf(1, "\n\t%child 1, d tickets", tickets_owned(pid));
        pid2 = fork();
        if (pid2 == 0) {
            printf(1, "\n\t%grandchild, d tickets", tickets_owned(pid2));
            pid3 = fork();
            if (pid3 == 0) {
                printf(1, "\n\tgreatGC, %d tickets", tickets_owned(pid3));
                exit();
            }
            else if (pid3 > 0) {
                printf(1, "\n\tGC, %d tickets", tickets_owned(pid3));
                sleep(1);
                exit();
            }
        }
        else if (pid2 > 0) {
            sleep(1);
            printf(1, "%Child, d tickets", tickets_owned(pid));
            wait();
        }
    }
    else if (pid > 0) {
        wait();
        printf(1, "parent, %d tickets", tickets_owned(pid));
    }

    wait();
    wait();


    //return 0;

    //fork();

}

void stride_transfer_test()
{
    int pid;
    int parent_pid = getpid();
    int result;

    printf(1, "\n\n\nticket transfer test");
    printf(1, "\n//////////////\n");

    // Test: Transfer negative number of tickets
    result = transfer_tickets(12345, -5);
    if (result == -1)
        printf(1, "Correctly returned -1 for negative ticket transfer\n");
    else
        printf(1, "Error: Expected -1, got %d\n", result);

    // Fork a child process
    pid = fork();
    if (pid < 0)
    {
        printf(1, "Fork failed!\n");
        exit();
    }
    else if (pid == 0)
    {
        // Child process does nothing significant
        sleep(50);
        exit();
    }
    else
    {
        // Parent process

        // Get the number of tickets parent has
        int parent_tickets = tickets_owned(parent_pid);

        //printf(1, "parent tickets: %d\n", parent_tickets);
        //printf(1, "pid: %d, getpid: %d, parentpid: %d\n", pid, getpid(), parent_pid);

        // Test: Transfer more tickets than allowed
        result = transfer_tickets(pid, parent_tickets);
        if (result == -2)
            printf(1, "Correctly returned -2 for transferring too many tickets\n");
        else
            printf(1, "Error: Expected -2, got %d\n", result);

        // Test: Transfer tickets to non-existent process
        result = transfer_tickets(99999, 10);
        if (result == -3)
            printf(1, "Correctly returned -3 for non-existent recipient\n");
        else
            printf(1, "Error: Expected -3, got %d\n", result);

        // Test: Transfer tickets to self
        result = transfer_tickets(parent_pid, 10);
        if (result == -4)
            printf(1, "Correctly returned -4 for transferring tickets to self\n");
        else
            printf(1, "Error: Expected -4, got %d\n", result);

        // Test: Successful transfer
        result = transfer_tickets(pid, parent_tickets / 2);
        if (result >= 0)
        {
            printf(1, "Successfully transferred tickets, parent now has %d tickets\n", result);
        }
        else
        {
            printf(1, "Error: Expected successful transfer, got %d\n", result);
        }

        //printf(1, "about to wait\n");
        wait();  // Wait for child to finish
        //printf(1, "finished waiting\n");
    }
    
    //printf(1, "about to exit\n");
    set_sched(0);
    //exit();
}

void stride_seqence_test()
{
    int pid;
    int parent_pid = getpid();
    int i;

    printf(1, "\n\nstride sequence test");
    printf(1, "\n//////////////\n\n\n");

    enable_sched_trace(1);  // Enable scheduling trace

    // Set scheduler to stride
    set_sched(1);

    // Fork a child process
    pid = fork();
    if (pid < 0)
    {
        printf(1, "Fork failed!\n");
        exit();
    }
    else if (pid == 0)
    {
        // Child process
        // Minimal sleep to allow parent to proceed
        sleep(20);

        // Get initial tickets
        int child_tickets = tickets_owned(getpid());
        printf(1, "Child initial tickets: %d\n", child_tickets);

        for (i = 0; i < 10; i++)
        {
            printf(1, "Child working %d\n", i);
            sleep(5);
        }
        printf(1, "\nChild exiting\n\n");
        exit();
    }
    else
    {
        // Parent process

        // Wait until child is RUNNABLE
        sleep(25);  // Ensure child is RUNNABLE

        // Get initial tickets
        int parent_tickets = tickets_owned(parent_pid);
        printf(1, "Parent initial tickets: %d\n", parent_tickets);

        // Transfer all but one ticket to the child
        int transfer_amount = parent_tickets - 1;
        int result = transfer_tickets(pid, transfer_amount);
        if (result >= 0)
        {
            printf(1, "Parent transferred %d tickets to child, now has %d tickets\n", transfer_amount, result);
        }
        else
        {
            printf(1, "Ticket transfer failed, result: %d\n", result);
        }

        // Get updated tickets
        parent_tickets = tickets_owned(parent_pid);
        int child_tickets = tickets_owned(pid);
        printf(1, "Parent tickets after transfer: %d\n", parent_tickets);
        printf(1, "Child tickets after transfer: %d\n", child_tickets);

        for (i = 0; i < 10; i++)
        {
            printf(1, "Parent working %d\n", i);
            sleep(5);
        }
        wait();  // Wait for child to finish
        printf(1, "\nParent exiting\n\n");
    }

    enable_sched_trace(0);  // Disable scheduling trace
    exit();
}

int main(int argc, char *argv[]) {
    int w = 0;

    if (argc < 2)
    {
        printf(1, "Usage: %s 0|1 \n"
                  "\t0: RR scheduling\n"
                  "\t1: Stride scheduling\n", argv[0]);
        exit();
    }

    if (argv[1][0] == '0') {
        w = 0;
        set_sched(w);
        printf(1, "\nRound Robin Scheduling ...\n");
        
    }
    else {
        w = 1;
        set_sched(w);
        printf(1, "\nStride Scheduling ...\n");
        //stride_reset_test();
        stride_transfer_test();
        stride_seqence_test();
    }
    printf(1, "returning");
    return 0;
    
}
