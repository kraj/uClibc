#include <errno.h>
#include <sys/msg.h>
#include "ipc.h"


#ifdef L_msgctl
/* Message queue control operation.  */
int msgctl (int msqid, int cmd, struct msqid_ds *buf)
{
    return __ipc(IPCOP_msgctl ,msqid ,cmd ,0 ,buf);
}
#endif


#ifdef L_msgget
/* Get messages queue.  */
int msgget (key_t key, int msgflg)
{
    return __ipc(IPCOP_msgget ,key ,msgflg ,0 ,0);
}
#endif


struct new_msg_buf{
    struct msgbuf * oldmsg;
    long int r_msgtyp;       /* the fifth arg of __ipc */
};
/* Receive message from message queue.  */


#ifdef L_msgrcv
int msgrcv (int msqid, void *msgp, size_t msgsz,
	long int msgtyp, int msgflg)
{
    struct new_msg_buf temp;

    temp.r_msgtyp = msgtyp;
    temp.oldmsg = msgp;
    return __ipc(IPCOP_msgrcv ,msqid ,msgsz ,msgflg ,&temp);
}
#endif



#ifdef L_msgsnd
/* Send message to message queue.  */
int msgsnd (int msqid, const void *msgp, size_t msgsz, int msgflg)
{
    return __ipc(IPCOP_msgsnd, msqid, msgsz, msgflg, (void *)msgp);
}
#endif

