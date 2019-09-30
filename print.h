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

struct printOPT {
    ino_t i_node;
    char *f_mode;
    nlink_t h_links;
    char *user_name;
    char *groupname;
    off_t f_size;
    char *month;
    int day;
    int hour;
    int minute;
    char *f_name;
};

struct printOPT initialize_options_null();
void display_out(struct printOPT options, char *pathname);
void initialize_default_options(struct printOPT *options);