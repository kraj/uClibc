#ifndef __ERR_H__
#define __ERR_H__

#define warn(X,args...) ({char _buf[128]; sprintf(_buf, X,args); perror(_buf);})
#define err(X,Y,args...) ({char _buf[128]; sprintf(_buf, Y,##args); perror(_buf); exit(X);})

#endif /* __ERR_H__ */
