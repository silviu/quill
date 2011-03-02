#ifndef COMMON_H__
#define COMMON_H__

#include <string>
#include <string.h>

using namespace std;

struct arg_struct {
	string arg1;
	string arg2;
	string arg3;
	string arg4;
};

struct argu_struct {
	char* arg1;
	char* arg2;
};

extern const char* prompt_line;

void prompt();
void perros(const char* s);

int readall(int fd, char * buf, size_t len);
int read_to_char(int fd, string & dest, int needle);
int readln(int fd, string & dest);

int writeall(int fd, const char * buf, size_t len);
int writeall(int fd, const string & str);
int writeln(int fd, const string & s_);

int connect_to(const char* host, const char* port);
int bind_to(const char* host, const char* port);
void insert_fd(fd_set &s, int &fdmax, int fd);
void copy_fdset(fd_set &source, fd_set &dest);

int make_base64(const string &ascii, string &b_64);
int decode_base64(const string &b_64, string &ascii);
#endif //COMMON_H__
