#ifndef CLIENT_H__
#define CLIENT_H__

int connect_to(char* host, int portnum);

int join_room(int roomid);
int add_player(char* nname);

#endif // CLIENT_H__
