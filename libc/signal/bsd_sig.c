#define __USE_BSD_SIGNAL

#include <signal.h>

#undef signal

/* The `sig' bit is set if the interrupt on it
 * is enabled via siginterrupt (). */
extern sigset_t _sigintr;

__sighandler_t
__bsd_signal (int sig, __sighandler_t handler)
{
  int ret;
  struct sigaction action, oaction;
  action.sa_handler = handler;
  __sigemptyset (&action.sa_mask);
  if (!__sigismember (&_sigintr, sig)) {
#ifdef SA_RESTART
    action.sa_flags = SA_RESTART;
#else
    action.sa_flags = 0;
#endif
  }
  else {
#ifdef SA_INTERRUPT
    action.sa_flags = SA_INTERRUPT;
#else
    action.sa_flags = 0;
#endif
  }
  ret = __sigaction (sig, &action, &oaction); 
  return (ret == -1) ? SIG_ERR : oaction.sa_handler;
}
