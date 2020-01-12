#ifndef MSG_H__
#define MSG_H__

#define MSG_INFO "INFO"
#define MSG_ERROR "ERROR"
#define MSG_WARN "WARN"
#define MSG_SUCCESS "SUCCESS"
#define MSG_DATA "DATA"
#define MSG_IN_DATA "INDATA"
#define MSG_OUT_DATA "OUTDATA"

void one_msg(char* msg_type, char* msg);
void two_msg(char* msg_type, char* msg1, char* msg2);

#endif // MSG_H__
