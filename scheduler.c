#include "headers.h"
//--------------------------------------------------------------------------------------------------
int ALGORITHM;

int main(int argc, char * argv[])
{
    initClk();
    
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
     struct msgbuff Msg_Snd;
     ALGORITHM = atoi(argv[1]);
     int key_id;
     int rec_val, msgq_id;
     key_id = ftok("keyfile", 65);                // CREATE A UNIQUE KEY
     msgq_id = msgget(key_id, 0666 | IPC_CREAT); // CREATE MESSAGE QUEUE THEN RETURN THE ID
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    //destroyClk(true);
}
