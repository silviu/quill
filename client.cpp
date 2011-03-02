#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netdb.h>
#include <errno.h>
#include <ctime>
#include <list>
#include <vector>
#include "common.h"
#include "base64.h"


#define MAX_BACKLOG 10
#define PROTO_START_MSG "msg from "
#define PROTO_START_FILE "file from "
#define PING "<PING>\n"
#define USR_CMD_MSG_COUNT 2
#define USR_CMD_READ 1
#define USR_CMD_SEND 3
#define USR_CMD_FILE_SEND 4
#define USR_CMD_FILE_READ 5

using namespace std;

const char* prompt_line = "client> ";

fd_set client_fds;
int fdmax = 0;

struct user_info {
	user_info(const string &h, const string &p, int &f) : host(h), port(p), fd(f) {}
	user_info() : host("<none>"), port("-1"), fd(-1) {}
	string host;
	string port;
	int fd;
	vector<string> msg;
	vector<string> files;
	vector<string> flname;
};

map<string, user_info> user_list;

/** List all the users */
void list_users()
{
	int i;
	map<string, user_info>::iterator it;
	for(it = user_list.begin(), i = 0; it != user_list.end(); ++it, i++)
		cout << it->first << endl << flush;
}

/** Respond to the server ping */
int respond_to_ping(int fd)
{
	int rc = writeln(fd, "<RSP>");
	if (rc == -1) {
		perros("writeln() in respond_to_ping()");
		return -1;
	}
	return 0;
}

/** Updates a certain user's fd with the new_fd */
int update_user_fd(string user, int new_fd)
{
	map<string, user_info>::iterator it;
	it = user_list.find(user);
	if (it->second.fd == -1 && it != user_list.end())
		it->second.fd = new_fd;
	return 0;
}

/** Checks if the user is already connected
 * if not the connection is made and the message is sent.
 * Otherwise the message is sent to the socket speciffied
 * in the "user_info"
 */
int send_msg_or_file(user_info &user, string client_name,
		string msg, const string what)
{

    string to_send, msg_start;
	for (unsigned int i = 0; i < msg.length(); i++)
		if (msg[i] == '\n')
			msg[i] = '\01'; 
	if (what == PROTO_START_MSG)
		msg_start = PROTO_START_MSG;
	else
		msg_start = PROTO_START_FILE;
	msg_start.append(client_name);
	to_send.append(msg_start);
	to_send.append(" ");
	to_send.append(msg);


	int connfd;
	if (user.fd == -1) {
		connfd = connect_to(user.host.c_str(), user.port.c_str());
		if (connfd == -1) {
			perros("connect_to() in send_msg()");
			return -1;
		}
		insert_fd(client_fds, fdmax, connfd);
		user.fd = connfd;
	}
	else
		connfd = user.fd;
	int rc = writeln(connfd, to_send);
	if (rc == -1) {
		perros("writeln() in send_msg()");
		return -1;
	}
	return connfd;
}

/** Searches for a user in the map 
 * and adds the message to it's message vector 
 */
int add_msg(string user, string msg)
{
	map<string, user_info>::iterator it;
	it = user_list.find(user);
	if ( it != user_list.end())
		it->second.msg.push_back(msg);
	return 0;
}

int add_file(string user, string file)
{
	map<string, user_info>::iterator it;
	it = user_list.find(user);
	if ( it != user_list.end())
		it->second.files.push_back(file);
	return 0;
}
int add_file_name(string user, string filename)
{
	map<string, user_info>::iterator it;
	it = user_list.find(user);
	if ( it != user_list.end())
		it->second.flname.push_back(filename);
	return 0;
}

/** Reads a message from it's peer */
int read_msg(int fd)
{
	string line, what, from, who, msg;
	int rc = readln(fd, line);
	if (rc == -1) {
		perros("readln() in read_msg()");
		return -1;
	}
	stringstream ss (stringstream::in|stringstream::out);
	ss << line;
	ss >> what >> from >> who;
	if (what == "msg") {
		char* mesaj = (char*) malloc(line.size() - (what.size() + from.size() + who.size()));
		ss.getline(mesaj, (line.size() - (what.size() + from.size() + who.size())));
		for(unsigned int i = 0; i < strlen(mesaj); i++) {
            if (mesaj[i] == '\01')
				mesaj[i] = '\n';
		}
		msg = string(mesaj);
		/** Erase the first \01 */
		msg.erase(0, 2);
		/** Erase the .\n */
		msg.erase(msg.size()-2, 2);

		add_msg(who, msg);
		cout << "\nA fost primit mesaj din partea utilizatorului " 
			 << who << "...." << endl << flush;
		prompt();
		update_user_fd(who, fd);
	}
	else if (what == "file") {
		string file;
		char* fisier = (char*) malloc(line.size() - (what.size() + from.size() + who.size()));
		ss.getline(fisier, (line.size() - (what.size() + from.size() + who.size())));
		for(unsigned int i = 0; i < strlen(fisier); i++) {
			if (fisier[i] == '\01')
				fisier[i] = '\n';
		}
		file = string(fisier);
		string filename, decoded_file;
		decode_base64(file, decoded_file);
        stringstream si (stringstream::in|stringstream::out);
		si << decoded_file;
		si >> filename;

		decoded_file.erase(0, filename.length());

		add_file(who, decoded_file);
		add_file_name(who, filename);
		cout << "\nA fost primit un fișier din partea utilizatorului " 
			 << who << "...." << endl << flush;
		prompt();
		update_user_fd(who, fd);
	}

	return 0;
}

/** Function for the simple "read" command */
int print_msg_nr()
{
	map<string, user_info>::iterator it;
	for (it = user_list.begin(); it != user_list.end(); ++it)
		cout << it->first << ": " << it->second.msg.size() << endl << flush;
	return 0;
}

/** Reads the user_list sent by the server and updates
 * its local copy
 */
int update_user_list(string user_bulk)
{
	string h, p, name;
	map<string, user_info>::iterator it;
	stringstream ss(stringstream::in|stringstream::out);

	ss << user_bulk;
	ss >> name >> h >> p;
	int f = -1;

	user_info* user = new user_info(h, p, f);
	it = user_list.find(name);

	if (it == user_list.end())
		/* if the username is not in the map add it to the user_list */
		user_list.insert(pair<string, user_info>(name, *user));
	else if (( it->second.host != user->host ) || (it->second.port != user->port)) {
		/* if the username already was in the list, but the host/port changed */
		it->second.host = user->host;
		it->second.port = user->port;
		it->second.fd = -1;
	}
	return 0;
}


int run_command_from_server(int fd)
{
	string line;
	int rc = readln(fd, line);
	if (rc == -1) {
		perros("readln in run_command_from_server()");
		return -1;
	}
	if (line == PING) {
		respond_to_ping(fd);
		return 0;
	}
	if (line == "ACK\n")
		return 0;

    update_user_list(line);
	return 0;
}

/** Breaks the command from the user into name, message number or the message */
int break_command(string command, int comm, string &name, string &no, string &msg)
{
	string co;
	stringstream ss(stringstream::in|stringstream::out);
	ss << command;

	if (comm == USR_CMD_READ) {
		ss >> co >> name >> no;
		return 0;
	}
	if (comm == USR_CMD_MSG_COUNT) {
		ss >> co;
		return 0;
	}
	if (comm == USR_CMD_SEND) {
		ss >> co >> name;
		char* mesaj = (char*) malloc(command.size() - (name.size() + co.size()));
		ss.getline(mesaj, (command.size() - (name.size() + co.size())));
		msg = string(mesaj);
	}
	if (comm == USR_CMD_FILE_SEND) {
		ss >> co >> name >> no;
		return 0;
	}
	if (comm == USR_CMD_FILE_READ) {
		ss >> co >> name >> no;
		return 0;
	}
	return 0;
}


/** Returns the "user_info" for a certain name */
user_info* get_info_for_user(const string & name)
{
	map<string, user_info>::iterator it;
	it = user_list.find(name);
	if (it != user_list.end())
		return &it->second;
	return NULL;
}


/** Returns the type of the command (read,read [user][no],send) */
int get_command_type(string command)
{
	string comm, user, no;
	stringstream ss (stringstream::in|stringstream::out);
	ss << command;
	ss >> comm >> user >> no;
	if (comm == "read" && user != "" && no != "")
		return USR_CMD_READ;
	else if (comm == "read" && user == "" && no == "")
		return USR_CMD_MSG_COUNT;
	else if (comm == "send" && user != "")
		return USR_CMD_SEND;
	else if (comm == "fsend" && user != "" && no != "")
		return USR_CMD_FILE_SEND;
	else if (comm == "fread" && user != "" && no != "")
		return USR_CMD_FILE_READ;
	return 0;
}

/** Prints the message from the user "name" 
 *  and with the nr "no" 
 */
int print_specific_msg(string name, int no)
{
	map<string, user_info>::iterator it;
	it = user_list.find(name);
	if ( it == user_list.end()) {
		perros("The username you entered does not exist.");
		return -1;
	}
	if ((unsigned int) no >= it->second.msg.size()) {
		perros("message number requested bigger than available msgs number.");
		return -1;
	}
	cout << it->second.msg[no] << endl;
	/** Erase the message from the vector 
	 * after it has been viewed */
	it->second.msg.erase(it->second.msg.begin() + no);
	return 0;
}

int file_to_string(string name, string &file, string &filename)
{
	FILE *fp;
	long len;
	char *buf;
	const char* filename_c = basename(name.c_str());
	filename = string(filename_c);
	file.append(filename);
	file.append(" ");
	fp=fopen(name.c_str(),"rb");
	if (fp == NULL) {
		cout << "Fișierul " << filename << " nu exista." << endl;
		return -1;
	}
	fseek(fp,0,SEEK_END); //go to end
	len=ftell(fp); //get position at end (length)
	fseek(fp,0,SEEK_SET); //go to begining.
	buf=(char *)malloc(len); //malloc buffer
	int ret = fread(buf,len,1,fp); //read into buffer
	if (ret == -1)
		printf("fread\n");
    file.append(string(buf));
	fclose(fp);
	return 0;
}

int list_files()
{
	map<string, user_info>::iterator it;
	for (it = user_list.begin(); it != user_list.end(); ++it) {
		int file_nr = it->second.files.size();
		if (file_nr != 0) {
			vector <string>::iterator v_it;
			for(int i = 0; i < file_nr; i++)
				cout << it->first << " '" << it->second.flname[i] << "'"<< endl;
		}
	}
	return 0;
}

int write_to_disk(string file_content, string filename, string download_path)
{
	FILE* fp;
	string path_to_file;
	path_to_file.append(download_path);
	path_to_file.append("/");
	path_to_file.append(filename);
	const char* path = path_to_file.c_str();
	const char* file_content_c = file_content.c_str();
	fp = fopen(path, "w+");
	if (fp == NULL) {
		cout << "Folderul " << download_path << " nu exista." << endl;
		return -1;
	}
	fputs(file_content_c, fp);
	fclose(fp);
	return 0;
}

int print_file (string username, string filename, string download_path)
{
	map <string, user_info>::iterator it;
	for (it = user_list.begin(); it != user_list.end(); ++it)
		if (it->first == username)
			for (unsigned int i = 0; i < it->second.flname.size(); i++)
				if ( it->second.flname[i] == filename) {
					write_to_disk(it->second.files[i], filename, download_path);
					cout << it->second.files[i] << endl << flush;
				}
	return 0;
}

void send_message(void* args) 
{
	struct argu_struct *argu = (struct argu_struct *)args;
	
	string message ="\01";
	message.append(argu->arg2);
	message.append("\01\01");

	user_info* user = new user_info();
	user = get_info_for_user("silviu");
	int rc = send_msg_or_file(*user, "luther", message, PROTO_START_MSG);
		if (rc == -1) {
			perros("send_msg() in run_command_from_user()");
			return;
		}
}

/** Runs commands from the client user */
int run_command_from_user(int fd, string client_name, string download_path)
{
	string command;
	getline(cin, command);
	if (command == "")
		prompt();
	else if (command == "list") {
		list_users();
		prompt();
	}
	else if (command == "flist") {
		list_files();
		prompt();
	}
	else if (command == "quit") {
		close(fd);
		exit(EXIT_SUCCESS);
	}
	else {
		string name, msg, no;
		int comm = get_command_type(command);
		break_command(command, comm, name, no, msg);
		if (comm == USR_CMD_SEND) {
			user_info* user = new user_info();
			user = get_info_for_user(name);
			string line;
			do {
				getline(cin, line);
				if(!cin)
					return -1;
				msg.append("\01");
				msg.append(line);
			} while(line.compare(".")) ;
			int rc = send_msg_or_file(*user, client_name, msg, PROTO_START_MSG);
			if (rc == -1) {
				perros("send_msg() in run_command_from_user()");
				return -1;
			}
			prompt();
		}
		else if (comm == USR_CMD_MSG_COUNT) {
			print_msg_nr();
			prompt();
		}
		else if (comm == USR_CMD_READ) {
			int nr = atoi(no.c_str());
			print_specific_msg(name, nr - 1);
			prompt();
		}
		else if (comm == USR_CMD_FILE_SEND) {
			user_info* user = new user_info();
			user = get_info_for_user(name);
			string file, filename;
			file_to_string(no, file, filename);
			string coded_file;
			make_base64(file, coded_file);
			int rc = send_msg_or_file(*user, client_name, coded_file, PROTO_START_FILE);
			if (rc == -1) {
				cout << "Fisierul " << filename <<" nu a putut fi trimis" << endl;
				return -1;
			}
			else 
				cout << "Fisierul " << filename << " a fost trimis" <<endl;
			prompt();
		}
		else if (comm == USR_CMD_FILE_READ) {
			string username = name;
			string filename = no;
			print_file(username, filename, download_path);
			prompt();
		}
		else {
			cout << command << ": command not found" << endl;
			prompt();
		}
	}
	return 0;
}

/** Accepts a new connection from another client */
int accept_new_peer(int bfd, fd_set &client_fds, int &fdmax)
{
	int connfd;
	struct sockaddr_storage addr;
	socklen_t len = sizeof(addr);
	connfd = accept(bfd, (sockaddr *)&addr, &len);
	if (connfd == -1) {
		perros("accept()");
		return -1;
	}
	insert_fd(client_fds, fdmax, connfd);
   return connfd;	
}

/** Binds to a random port on which it will then listen */
int bind_to_random_port(string &host, string & port)
{    
	struct sockaddr_storage addr;
	socklen_t len = sizeof(addr);

	int bfd = bind_to(NULL, "0");
	int rc = getsockname(bfd, (sockaddr*)&addr, &len);
	if (rc == -1) {
		perros("getsockname in bind_to_random_port()");
		return -1;
	}
	char addr_host[NI_MAXHOST], addr_serv[NI_MAXSERV];
	rc = getnameinfo( (sockaddr*) &addr, len, addr_host, NI_MAXHOST, addr_serv, NI_MAXSERV, NI_NUMERICSERV|NI_NUMERICHOST);
	if (rc != 0) {
		fprintf(stderr, "getnameinfo: %s\n", gai_strerror(rc));
		return -1;
	}
	host.assign(addr_host);
	port.assign(addr_serv);
	return bfd;
}
 

void make_connection(void* args)
{
	struct arg_struct *argu = (struct arg_struct *)args;

	cout << "AAAAAAAAAAAAAAAAA" << endl;
	/*
	if (argc < 4) {
		perror("main: Too few arguments. Usage: ./client username host port");
		exit(EXIT_FAILURE);
	}
*/
 	string host, port;
	int bfd = bind_to_random_port(host, port);
	if (bfd == -1) {
		perros("bind_to_random_port returned -1 in main");
		return;
	}

	int rc = listen(bfd, MAX_BACKLOG);
	if ( rc == -1) {
		perros("listen in main");
		return;
	}

    string name = argu->arg1;
	string server_host = argu->arg2;
	string server_port = argu->arg3;
	string client_download = argu->arg4;
	
    string client_name = string(name);
	string download = string(client_download);
	/* Connect to the server. Get the dile descriptor. */
	int cfd = connect_to(server_host.c_str(), server_port.c_str());
    
	string to_send;
	to_send.append(name); to_send.append(" ");
	to_send.append(host); to_send.append(" ");
	to_send.append(port);

    /* Send user info to the server */
	rc = writeln(cfd, to_send);
	if (rc == -1) {
		perros("writeln in main");
		close(cfd);
	}

	/* Wait for the response ACK/NACK */
	string response;
	rc = readln(cfd, response);

	if (rc == -1) {
		perror("recv: in readln while waiting for ACK/NACK\n");
		close(cfd);
		return;
	}

	if (response == "NACK\n") {
		cout << "Autentificare nu a reusit... exista deja client cu numele \"" 
			 << name << "\"" <<endl << flush;
		close(cfd);
		return;
	}
	cout << "Autentificarea a reusit" << endl << flush;	

	FD_ZERO(&client_fds);
    /* stdin is selectable */
	insert_fd(client_fds, fdmax, STDIN_FILENO);
    /* the "listen" socket is added to the socket set */
	insert_fd(client_fds, fdmax, bfd);
	/* the server connected fd is added to the socket set */
	insert_fd(client_fds, fdmax, cfd);
	prompt();
	for(;;) {
		fd_set tmp_fds;
		int selected_fds = 0;
		copy_fdset(client_fds, tmp_fds);

		selected_fds = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);

		if (selected_fds == -1) {
			perros("ERROR in select");
			exit(EXIT_FAILURE);
		}

		for(int fd = 0, i = 0; fd <= fdmax && i < selected_fds; fd++) {
			if (!FD_ISSET(fd, &tmp_fds))
				continue;
			i++;
			if (fd == cfd) {
				/** If the selected socket is that of the 
				 *  connection with the server. */  
				int rc = run_command_from_server(cfd);
				if (rc == -1)
					FD_CLR(cfd, &client_fds);
			} else if (fd == STDIN_FILENO) {
				/** If the selected socket is the console
				 * run a command from the client user. */
				run_command_from_user(fd, client_name, download);
			}
			else if (fd == bfd) {
				/** If the selected socket is the one 
				 *  the clients is litening on then
				 *  a new client is trying to connect. */
				accept_new_peer(bfd, client_fds, fdmax);
			}
			else {
				/** Else it must be a connection with another 
				 * client. Read the messages it sends. */
				int rc = read_msg(fd);
				if (rc == -1)
					FD_CLR(fd, &client_fds);
			}
		}
	}
	return;
}
