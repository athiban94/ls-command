#include "print.h"


int main() {
    struct stat sb;
    struct printOPT options;
    char *pathname = "dir1/hellowo";
    struct passwd *pws;
    struct group *grp;
    int uid;
    char *mode_str;
    time_t m_time;
    struct tm *time_data;
    char *month_list[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    int hr;
    int min;

    options = initialize_options_null();
    if(stat(pathname, &sb) == -1) {
        printf("error getting info of %s : %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE); // Remove it after 
    }

    options.i_node = sb.st_ino;

    strmode(sb.st_mode, mode_str);
    options.f_mode = mode_str;

    options.h_links = sb.st_nlink;

    pws = getpwuid(sb.st_uid);
    options.user_name = pws->pw_name;

    grp = getgrgid(sb.st_gid);
    options.groupname = grp->gr_name;

    options.f_size = sb.st_size;
    m_time = sb.st_mtime;

    // options.mtime = sb.st_mtime;
    
    time_data = localtime(&m_time);
    options.month = month_list[time_data->tm_mon];
    options.day = time_data->tm_mday;
    options.hour = time_data->tm_hour;
    options.minute = time_data->tm_min;

    display_out(options, pathname);
}

void display_out(struct printOPT options, char* pathname) {

    char *month_list[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    if(options.i_node != 0)
        printf("%llu ", options.i_node);
    if(options.f_mode != NULL)
        printf("%s ", options.f_mode);
    if(options.h_links != 0)
        printf("%hu ", options.h_links);
    if(options.user_name != NULL)
        printf("%s ", options.user_name);
    if(options.groupname != NULL)
        printf("%s ", options.groupname);
    if(options.f_size != 0)
        printf("%lld ", options.f_size);
    if(options.month != NULL)
        printf("%s ", options.month);
    if(options.day != 0)
        printf("%d ", options.day);
    if(options.hour != 0)
        printf("%d:", options.hour);
    if(options.minute != 0)
        printf("%d ", options.minute);
    printf("%s", pathname);

    printf("\n");
}

struct printOPT initialize_options_null() {
    struct printOPT opt;
    opt.i_node = 0;
    opt.f_mode = NULL;
    opt.h_links = 0;
    opt.user_name = NULL;
    opt.groupname = NULL;
    opt.f_size = 0;
    opt.month = NULL;
    opt.day = 0;
    opt.hour = 0;
    opt.minute = 0;
    opt.f_name = NULL;
    return opt;
}



// void initialize_options_null(struct printOPT *options) {
//     options->i_node = 0;
//     options->f_mode = NULL;
//     options->h_links = 0;
//     options->user_name = "athibanp";
//     options->groupname = NULL;
//     options->f_size = 0;
//     options->month = NULL;
//     options->day = NULL;
//     options->hour_minute = NULL;
//     options->f_name = NULL;
// }