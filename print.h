#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

struct printOPT {
    ino_t i_node;
    blkcnt_t blocks_used;
    char *h_blocksize;
    char *f_mode;
    nlink_t h_links;
    uid_t uid;
    gid_t gid;
    char *user_name;
    char *groupname;
    off_t f_size;
    char* h_filesize;
    char *month;
    int day;
    int hour;
    int minute;
    char *f_name;
    char *sym_link;
    bool display_uid_gid;
    bool display_file_size;
    bool display_blocks;
};

struct printOPT initialize_options_null();
void display_out(struct printOPT options, char *pathname);
void initialize_default_options(struct printOPT *options);