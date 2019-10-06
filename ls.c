#include "ls.h"

int main(int argc, char *argv[])
{
    int c;
    opterr = 0;
    int n_err = 0, n_nondir = 0, n_dir = 0;
    struct stat sb;
    char **arguments;
    argumentC = argc;

    while ((c = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1)
    {
        switch (c)
        {
            case 'a':
                a_flag = 1;
                break;
            case 'A':
                A_flag = 1;
                break;
            case 'c':
                c_flag = 1;
                break;
            case 'd':
                d_flag = 1;
                break;
            case 'F':
                F_flag = 1;
                break;
            case 'f':
                f_flag = 1;
                break;
            case 'h':
                h_flag = 1;
                break;
            case 'l':
                l_flag = 1;
                break;
            case 'i':
                i_flag = 1;
                break;
            case '?':
                fprintf(stderr, "ls: unknown option `-%c'.\n", optopt);
                fprintf(stderr, "usage: ls [ −AacdFfhiklnqRrSstuw] [file . . .]\n");
                exit(EXIT_FAILURE);
            default:
                abort();
        }
    }

    if(optind == argc) 
    {
        arguments = malloc(1 * sizeof(char*));
        arguments[0] = ".";
        traverse_FTS(arguments);
        // traverseFiles(".");
    }
    else if (optind == argc - 1)
    {
        if (stat(argv[optind], &sb) == -1) 
        {
            fprintf(stderr, "ls: %s: %s\n", argv[optind], strerror(errno));
            exit(EXIT_FAILURE);
        }
        else if(S_ISDIR(sb.st_mode)) 
        {
            arguments = malloc(1 * sizeof(char*));
            arguments[0] = argv[optind];
            // traverseFiles(argv[optind]);
            traverse_FTS(arguments);
        }
        else 
        {
            printFiles(argv[optind]);
        }
    }
    else
    {
        for (int i = optind; i < argc; i++)
        {
            if (stat(argv[i], &sb) == -1)
                n_err++;
            else if (S_ISDIR(sb.st_mode))
                n_dir++;
            else
                n_nondir++;
        }
        handleMuliplePaths(n_err, n_dir, n_nondir, argc, argv);
    }
    exit(EXIT_SUCCESS);
}

/**
 * Handling muliple pathnames in ./ls <options> file1 file2 ...
 * */
void handleMuliplePaths(int n_ferr, int n_dir, int n_nondir, int argc, char **argv)
{
    struct stat sb;
    struct f_error f_err[n_ferr];
    struct f_dir f_directories[n_dir];
    struct f_non_dir f_nondir[n_nondir];
    int i_ferr = 0, i_ndir = 0, i_nondir = 0;

    for (int i = optind; i < argc; i++)
    {
        if (stat(argv[i], &sb) == -1)
        {
            f_err[i_ferr].f_error_path = argv[i];
            i_ferr++;
        }
        else if (S_ISDIR(sb.st_mode))
        {
            f_directories[i_ndir].f_dirname = argv[i];
            i_ndir++;
        }
        else
        {
            f_nondir[i_nondir].f_non_dirname = argv[i];
            i_nondir++;
        }
    }

    if (i_ferr > 0)
        print_errors_args(f_err, i_ferr);
    if (i_nondir > 0)
        print_non_directories(f_nondir, i_nondir);
    if (i_ndir > 0)
        traverseDirs(f_directories, i_ndir);
}

/**
 * This function prints corresponding errors in the 
 * the shell for args - that does not exit in the file
 * system
 * */
void print_errors_args(struct f_error f_error[], int len)
{
    struct stat sb;
    char *file;
    for (int i = 0; i < len; i++)
    {
        file = f_error[i].f_error_path;
        if (stat(file, &sb) == -1)
        {
            printf("ls: %s : %s\n", file, strerror(errno));
        }
    }
}

/**
 * This function prints all the files which
 * are not DirectoryType
 * */
void print_non_directories(struct f_non_dir non_dir[], int len)
{
    for (int i = 0; i < len; i++)
    {
        printf("%s\t", non_dir[i].f_non_dirname);
    }
    printf("\n");
}

/**
 * This function all the directories in the 
 * argv - param
 * */
void traverseDirs(struct f_dir dirs[], int len)
{
    char **args = malloc(len * sizeof(char*));

    for (int i = 0; i < len; i++)
    {
        args[i] = dirs[i].f_dirname;
    }

    traverse_FTS(args);

    // for (int i = 0; i < len; i++)
    // {
    //     printf("%s:\n", dirs[i].f_dirname);
    //     traverseFiles(dirs[i].f_dirname);
    // }
}


/**
 * Function to traverse all the files using
 * FTS(3)
 * */

void traverse_FTS(char **args) {
    FTS *ftsp;
    FTSENT *ftsent;
    FTSENT *children;
    int options = FTS_PHYSICAL | FTS_NOCHDIR;

    if(a_flag)
        options = FTS_PHYSICAL | FTS_NOCHDIR | FTS_SEEDOT;

    if((ftsp = fts_open(args, options, NULL)) == NULL) {
        //double check with professor - not throwing error for invalid
        fprintf(stderr, "ls : %s\n", strerror(errno));
        exit(EXIT_SUCCESS);
    }

    while ((ftsent = fts_read(ftsp)) != NULL)
    {
        if(ftsent->fts_level > 1) {
            fts_set(ftsp, ftsent, FTS_SKIP);
            continue;
        }
        
        children = fts_children(ftsp, 0);
        
        if(children == NULL) {
            continue;
        }

        if(children->fts_level > 1){
            continue;
        }

        if(isDisplayParent()){
            printf("\n");
            printf("%s:\n", children->fts_parent->fts_path);
        }
        while (children != NULL) {
            printf("%s\n", children->fts_name);
            generatePrint(children);
            children = children->fts_link;
        }
    }

    fts_close(ftsp);
    
}

/**
 * This function is used to list all the files(irrespective of the type) based on
 * the options set.
 * */
void traverseFiles(char *dirpath)
{
    DIR				*dp;
	struct dirent	*dirp;
    if ((dp = opendir(dirpath)) == NULL)
        fprintf(stderr, "ls: %s : %s\n", dirpath,strerror(errno));
    while ((dirp = readdir(dp)) != NULL)
		printf("%s\n", dirp->d_name);
    
    closedir(dp);
}

/**
 * This funtion is for long listing. -l option
 * */
void generatePrint(FTSENT *ftsent) {
    struct stat *stat_info;
    struct printOPT options;
    char *mode_str = malloc(10);
    struct passwd *pws;
    struct group *grp;
    time_t m_time;
    struct tm *time_data;
    char *month_list[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    
    /* Getting the stat informnation */
    stat_info = ftsent->fts_statp;
    options = initialize_options_null();

    
    /* i-flag - option */
    if(i_flag)
        options.i_node = stat_info->st_ino;
        
    if(l_flag) {
        strmode(stat_info->st_mode, mode_str);
        options.f_mode = mode_str;

        options.h_links = stat_info->st_nlink;

        pws = getpwuid(stat_info->st_uid);
        options.user_name = pws->pw_name;

        grp = getgrgid(stat_info->st_gid);
        options.groupname = grp->gr_name;

        options.f_size = stat_info->st_size;

        m_time = stat_info->st_mtime;
        time_data = localtime(&m_time);
        options.month = month_list[time_data->tm_mon];
        options.day = time_data->tm_mday;
        options.hour = time_data->tm_hour;
        options.minute = time_data->tm_min;
    }

    display_out(options, ftsent->fts_name);
}

/**
 * This function is for printing files
 * */
void printFiles(char *pathname) {
    struct stat sb;
    struct printOPT options;
    struct passwd *pws;
    struct group *grp;
    int uid;
    char *mode_str = malloc(10);
    time_t m_time;
    struct tm *time_data;
    char *month_list[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    options = initialize_options_null();
    if(stat(pathname, &sb) == -1) {
        fprintf(stderr, "ls: %s: %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(i_flag)
        options.i_node = sb.st_ino;

    if(l_flag) {
        strmode(sb.st_mode, mode_str);
        options.f_mode = mode_str;

        options.h_links = sb.st_nlink;

        pws = getpwuid(sb.st_uid);
        options.user_name = pws->pw_name;

        grp = getgrgid(sb.st_gid);
        options.groupname = grp->gr_name;

        options.f_size = sb.st_size;
        m_time = sb.st_mtime;

        time_data = localtime(&m_time);
        options.month = month_list[time_data->tm_mon];
        options.day = time_data->tm_mday;
        options.hour = time_data->tm_hour;
        options.minute = time_data->tm_min;
    }
    display_out(options, pathname);
}


bool isDisplayParent() {
    if(optind == argumentC || optind == argumentC - 1)
        return false;
    else
        return true;
}