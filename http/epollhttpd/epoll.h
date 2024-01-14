#define EVENT_MAX 1024

void epoll_run(int lfd);
void do_accept(int lfd, int efd);
void do_read(int cfd, int efd);
int get_line(int sock, char *buf, int size);
void http_get_request(const char* line, int cfd);
void send_respond_head(int cfd, int no, const char* desp, const char* type, long len);
const char *get_file_type(const char *name);
void send_file(int cfd, const char* path);
void disconnect(int cfd, int epfd);
void send_dir(int cfd, const char* path);
void encode_str(char* to, int tosize, const char* from);
void decode_str(char *to, char *from);
