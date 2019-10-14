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

struct perttyPrint
{
    unsigned long long  totalBytesUsed; 
    ino_t               __max_i_node;
    blkcnt_t            __max_blocks_used;
    char                *__max_h_blocksize;
    char                *__max_f_mode;
    nlink_t             __max_h_links;
    uid_t               __max_uid;
    gid_t               __max_gid;
    char                *__max_user_name;
    char                *__max_groupname;
    off_t               __max_f_size;
    char                *__max_h_filesize;
    char                *__max_month;
    int                 __max_day;
    int                 __max_hour;
    int                 __max_minute;
    char                *__max_f_name;
    char                *__max_sym_link;
    bool                display_total;
};


struct printOPT {
    ino_t       i_node;
    blkcnt_t    blocks_used;
    char        *h_blocksize;
    char        *f_mode;
    nlink_t     h_links;
    uid_t       uid;
    gid_t       gid;
    char        *user_name;
    char        *groupname;
    off_t       f_size;
    int         major_number;
    int         minor_number;
    char        *h_filesize;
    char        *month;
    int         day;
    int         hour;
    int         minute;
    char        *f_name;
    char        *sym_link;
    bool        display_uid_gid;
    bool        display_file_size;
    bool        display_blocks;
    bool        isSpecialFile;
    bool        display_time;
};

struct printOPT initialize_options_null();
struct perttyPrint defaultPrettyPrint();
void display_out(struct printOPT options, struct perttyPrint pPrint);
void initialize_default_options(struct printOPT *options);
void calculateAndAddSpacesLongType(unsigned long long, unsigned long long);
void calculateAndAddSpacesCharType(char* , char*);
int getNumberOfDigits(unsigned long long);