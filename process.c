#include "headers.h"
struct scheduler_process_buff {
    
    int get_remaining_time[2];
    int m_type;
};

int remainingtime;

int main(int argc, char *argv[])
{
    initClk();

    remainingtime = atoi(argv[2]); // Set remaining time from command line argument

    key_t CLK_key_id, CLK_msgq_id;
    int CLK_rec_val;
    struct scheduler_process_buff CLK_buf;
    CLK_buf.m_type = 1;

    CLK_key_id = ftok("keyfile", 63);                   // create unique key for clock
    CLK_msgq_id = msgget(CLK_key_id, 0666 | IPC_CREAT); // create message queue for clock

    while (remainingtime > 0) 
    {
        CLK_rec_val = msgrcv(CLK_msgq_id, &CLK_buf, sizeof(&CLK_buf.get_remaining_time), 0, IPC_NOWAIT);

        if (CLK_rec_val != -1)
        {
            remainingtime -= 1; // Decrement remaining time when a clock tick is received
        }
    }

    destroyClk(false); // Cleanup clock resources
    // Notify the scheduler on termination
    kill(getppid(), SIGUSR1); // Send SIGUSR1 signal to scheduler
    return 0;
}
