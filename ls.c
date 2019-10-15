#include "ls.h"

int main(int argc, char *argv[])
{
    int c;
    int n_err = 0;
    int n_nondir = 0;
    int n_dir = 0;
    struct stat sb;
    char **arguments;
    argumentC = argc;
    compar = &sortLexographical;
    uid_t userId;
    opterr = 0;
    
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
                compar = NULL;
                break;
            case 'h':
                h_flag = 1;
                break;
            case 'k':
                k_flag = 1;
                break;
            case 'l':
                l_flag = 1;
                break;
            case 'n':
                n_flag = 1;
                l_flag = 1;
                break;
            case 'i':
                i_flag = 1;
                break;
            case 'r':
                r_flag = 1;
                compar = &sortReverseLexographical;
                break;
            case 'R':
                R_flag = 1;
                break;
            case 's':
                s_flag = 1;
                break;
            case 'S':
                S_flag = 1;
                compar = &sortFileSizeDescending;
                break;
            case 't':
                compar = &sortFilesByModifiedTime;
                break;
            case 'u':
                compar = &sortFilesByAccessTime;
                break;
            case 'q':
                q_flag = 1;
                w_flag = 0;
                break;
            case 'w':
                w_flag = 1;
                q_flag = 0;
                break;
            case '?':
                fprintf(stderr, "ls: unknown option `-%c'.\n", optopt);
                fprintf(stderr, "usage: ls [ −AacdFfhiklnqRrSstuw] [file . . .]\n");
                exit(EXIT_FAILURE);
            default:
                abort();
        }
    }

    if(isatty(fileno(stdout))) {
        if(!w_flag) {
            q_flag = 1;
        }
    } else {
        w_flag  = 1;
    }

    /* Setting the -A flag if the root is running the process*/
    userId = geteuid();
    if(userId == 0) {
        A_flag = 1;
    }

    if(k_flag) {
        blockSIZE = 1024;
    } else  {
        getBlocksAllocated();
    }

    if(optind == argc) 
    {
        arguments = malloc(1 * sizeof(char*));
        arguments[0] = "./";
        traverse_FTS(arguments);
        // free(arguments);
    }
    else if (optind == argc - 1)
    {
        if (lstat(argv[optind], &sb) == -1) 
        {
            fprintf(stderr, "ls: %s: %s\n", argv[optind], strerror(errno));
            exit(EXIT_FAILURE);
        }
        else if(S_ISDIR(sb.st_mode)) 
        {
            arguments = malloc(1 * sizeof(char*));
            if(strcmp(argv[optind], ".") == 0)
                arguments[0] = "./";
            else if(strcmp(argv[optind], "..") == 0)
                arguments[0] = "../";
            else 
                arguments[0] = argv[optind];
            traverse_FTS(arguments);
            // free(arguments);
        }
        else 
        {
            arguments = malloc(1 * sizeof(char*));
            arguments[0] = argv[optind];
            parseArgFiles(arguments);
            // free(arguments);
        }
    }
    else
    {
        int i;
        for (i = optind; i < argc; i++)
        {
            if (lstat(argv[i], &sb) == -1)
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
    int i;
    struct stat sb;
    int i_ferr = 0;
    int i_ndir = 0;
    int i_nondir = 0;
    struct f_error f_err[n_ferr];
    struct f_dir f_directories[n_dir];
    struct f_non_dir f_nondir[n_nondir];

    for (i = optind; i < argc; i++)
    {
        if (lstat(argv[i], &sb) == -1)
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

    if(d_flag) {
        if (i_ferr > 0)
            print_errors_args(f_err, i_ferr);
        if (i_nondir > 0 || i_ndir > 0)
            traverseDOption(f_nondir, i_nondir, f_directories, i_ndir);
    } else {
        if (i_ferr > 0)
            print_errors_args(f_err, i_ferr);
        if (i_nondir > 0)
            print_non_directories(f_nondir, i_nondir);
        if (i_ndir > 0)
            traverseDirs(f_directories, i_ndir);
    }

}

/**
 * This function prints corresponding errors in the 
 * shell for args - that does not exit in the file
 * system
 * */
void print_errors_args(struct f_error f_error[], int len)
{
    int i;
    struct stat sb;
    char *file;
    for (i = 0; i < len; i++)
    {
        file = f_error[i].f_error_path;
        if (stat(file, &sb) == -1)
        {
            fprintf(stderr,"ls: %s : %s\n", file, strerror(errno));
        }
    }
}

/**
 * This function prints all the files which
 * are not DirectoryType
 * */
void print_non_directories(struct f_non_dir non_dir[], int len)
{
    int i;
    char **args = malloc(len * sizeof(char*));
    for (i = 0; i < len; i++)
    {
        args[i] = non_dir[i].f_non_dirname;
    }
    parseArgFiles(args);
    // free(args);
}

/**
 * This function all the directories in the 
 * argv - param
 * */
void traverseDirs(struct f_dir dirs[], int len)
{
    int i;
    char **args = malloc(len * sizeof(char*) + 1);
    for (i = 0; i < len; i++)
    {
        args[i] = dirs[i].f_dirname;
    }

    // for (i = 0; i < len; i++)
    // {
    //     printf("%s \n", args[i]);
    // }
    

    traverse_FTS(args);
    // free(args);
}


/**
 * Function to traverse all the files using
 * FTS(3)
 * */
void traverse_FTS(char **args) {
    FTS *ftsp;
    FTSENT *ftsent;
    FTSENT *children;
    int options;
    struct perttyPrint pPrint;
    pPrint = defaultPrettyPrint();


    options = FTS_PHYSICAL | FTS_NOCHDIR;
    if(a_flag) {
        options = FTS_PHYSICAL | FTS_NOCHDIR | FTS_SEEDOT;
        A_flag = 1;
    }


    if((ftsp = fts_open(args, options, compar)) == NULL) {
        fprintf(stderr, "ls : %s\n", strerror(errno));
        exit(EXIT_SUCCESS);
    }

    while ((ftsent = fts_read(ftsp)) != NULL)
    {
        if(ftsent->fts_level > 1 && !R_flag) {
            fts_set(ftsp, ftsent, FTS_SKIP);
            continue;
        }
        
        children = fts_children(ftsp, 0);
        
        if(children == NULL) {
            continue;
        }

        if(children->fts_level > 1 && !R_flag) {
            continue;
        }

        if(isDisplayParent() && print_Flag == 1) {
                printf("\n");
                printf("%s:\n", children->fts_parent->fts_path);
        }

        if(isDisplayTotal(&pPrint)) {
            printf("total %lu\n", pPrint.totalBytesUsed);
        }
        while (children != NULL) {
            if(print_Flag == 1) {
                if(strncmp(children->fts_name, ".", 1) == 0) {
                    if(A_flag) {
                        generatePrint(children, pPrint);
                    }
                } else {
                    generatePrint(children, pPrint);
                }
                
            } else {
                getPrintDetails(children, &pPrint);
            }
            children = children->fts_link;
            
            if(print_Flag == 1 && children == NULL) {
                pPrint = defaultPrettyPrint();
            }
        }

        if(print_Flag == 0) {
           
            if((fts_set(ftsp, ftsent, FTS_AGAIN)) == 0) {
                print_Flag = 1;
            } else {
                fprintf(stderr, "fts_set FTS_AGAIN error : %s", strerror(errno));
            }
        } else {
            print_Flag = 0;
        }
    }

    fts_close(ftsp);
    
}

/**
 * This funtion is for long listing. -l option
 * */
void generatePrint(FTSENT *ftsent, struct perttyPrint pPrint) {
    struct stat *stat_info;
    struct printOPT options;
    char *mode_str = malloc(10);
    time_t time;
    char *path = malloc(ftsent->fts_pathlen + ftsent->fts_namelen + 1);
    char buf[PATH_MAX];
    ssize_t len;
    char *filename_F = malloc(ftsent->fts_namelen + 2);
    blkcnt_t file_blocks;
    
    if(ftsent->fts_level == 0) {
        strcpy(path, ftsent->fts_path);
    } else {
        strcpy(path, ftsent->fts_path);
        strcat(path, ftsent->fts_name);
    }
    
    /* Getting the stat informnation */
    stat_info = ftsent->fts_statp;
    options = initialize_options_null();

    if(s_flag) {
        if(h_flag) {
            options.h_blocksize = generateHumanReadableSize(512 * stat_info->st_blocks);
        } else  {
            file_blocks = calculateBlockSize(stat_info->st_blocks);
            options.display_blocks = true;
            options.blocks_used = file_blocks;
        }
    }
    
    /* i-flag - option */
    if(i_flag)
        options.i_node = stat_info->st_ino;
        
    if(l_flag) {
        options.display_file_size = true;
        options.display_time = true;

        strmode(stat_info->st_mode, mode_str);
        options.f_mode = mode_str;

        options.h_links = stat_info->st_nlink;

        if(n_flag) {
            /* -n option when enabled displaying uid and gid */
            options.display_uid_gid = true;
            options.uid = stat_info->st_uid;
            options.gid = stat_info->st_gid;
        } else {
            options.user_name = getUserNameByUserId(stat_info->st_uid);
            options.groupname = getGroupNameByGroupId(stat_info->st_gid);
        }

        if(S_ISBLK(stat_info->st_mode) || S_ISCHR(stat_info->st_mode)) {
            options.major_number = major(stat_info->st_rdev);
            options.minor_number = minor(stat_info->st_rdev);
            options.isSpecialFile = true;
            options.display_file_size = false;
            
        }

        options.f_size = stat_info->st_size;

        if(h_flag) {
            options.display_file_size = false;
            if(!S_ISBLK(stat_info->st_mode) && !S_ISCHR(stat_info->st_mode)) {
                options.h_filesize = generateHumanReadableSize(stat_info->st_size);
            }
        }

        if(c_flag) {
            time = stat_info->st_ctime;
        } else if(t_flag) {
            time = stat_info->st_mtime;
        } else {
            time = stat_info->st_atime;
        }
        options.month = getMonth(time);
        options.day = getDay(time);
        options.hour = getHour(time);
        options.minute = getMinute(time);

        if (ftsent->fts_info == FTS_SL)
        {
            if ((len = readlink(path, buf, sizeof(buf)-1)) == -1) {
                fprintf(stderr, "ls: %s: %s\n", ftsent->fts_name, strerror(errno));
            } else {
                buf[len] = '\0';
                options.sym_link = buf;
            }
        }
    }

    if(F_flag) {
        strcpy(filename_F, ftsent->fts_name);
        if(S_ISDIR(stat_info->st_mode))
            strcat(filename_F, "/");
        if(S_ISSOCK(stat_info->st_mode))
            strcat(filename_F, "=");
        if(S_ISFIFO(stat_info->st_mode))
            strcat(filename_F, "|");
        if(S_ISLNK(stat_info->st_mode))
            strcat(filename_F, "@");
        if(S_ISREG(stat_info->st_mode)){
            if(stat_info->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                strcat(filename_F, "*");
            }
        }
        if(ftsent->fts_info == FTS_W) {
            strcat(filename_F, "%");
        }
        
        options.f_name = filename_F;
    } else {
        options.f_name = ftsent->fts_name;
    }
    
    options.f_name = checkNonPrintChars(options.f_name);
    
    display_out(options, pPrint);
    free(path);
    free(mode_str);
    // free(filename_F);
}



/**
 * Function to get month 
 * */
char* getMonth(time_t time) {
    struct tm *time_data;
    char *month_list[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    time_data = localtime(&time);
    return month_list[time_data->tm_mon];

}

/**
 * This function retuns username with
 * uid_t passed as a parameter
 * */
char* getUserNameByUserId(uid_t uid) {
    struct passwd *pws;
    int n = numOfDigits(uid);
    char *usrName = malloc(n + 1);
    char *result;
    pws = getpwuid(uid);
    if(pws == NULL) {
        sprintf(usrName, "%u", uid);
        result = strdup(usrName);
        free(usrName);
        return result;
    } else {
        return pws->pw_name;
    }
}


/**
 * This function returns groupname with 
 * g_id passed as a parameter
 * */
char* getGroupNameByGroupId(gid_t gid) {
    struct group *grp;
    int n = numOfDigits(gid);
    char *grpName = malloc(n + 1);
    char *result;
    grp = getgrgid(gid);
    if(grp == NULL) {
        sprintf(grpName, "%u", gid);
        result = strdup(grpName);
        free(grpName);
        return result;
    } else {
        return grp->gr_name;
    }
}


/**
 * Function to get day
 * */
int getDay(time_t time) {
    struct tm *time_data;
    time_data = localtime(&time);
    return time_data->tm_mday;
}

/**
 * Function to get hour
 * */
int getHour(time_t time) {
    struct tm *time_data;
    time_data = localtime(&time);
    return time_data->tm_hour;
}

/**
 * Funtion to get minute
 * */
int getMinute(time_t time) {
    struct tm *time_data;
    time_data = localtime(&time);
    return time_data->tm_min;
}

/**
 * This function decides weather to display 
 * parent directory - in case of multiple arguments 
 * in the cli.
 * */
bool isDisplayParent() {
    if (R_flag)
        return true;
    else if(optind == argumentC || optind == argumentC - 1)
        return false;
    else {
        return true;
    }
}

/**
 * This function is called when -d option is set.
 * */
void traverseDOption(struct f_non_dir f_non_dir[], int n_files, struct f_dir f_dir[], int n_dirs) {
    int j = 0;
    int i;
    int totalSizeArgs = n_files + n_dirs;
    char **d_arguments = malloc(totalSizeArgs * sizeof(char*));
    for (i = 0; i < n_files; i++)
    {
        d_arguments[j] = f_non_dir[i].f_non_dirname;
        j++;
    }

    for (i = 0; i < n_dirs; i++)
    {
        d_arguments[j] = f_dir[i].f_dirname;
        // if(d_arguments[j][strlen(d_arguments[j]) - 1] == '/') {
        //     d_arguments[j][strlen(d_arguments[j]) - 1] = '\0';
        // }
        j++;
    }

    // for ( i = 0; i < totalSizeArgs; i++)
    // {
    //     printf("%s \n", d_arguments[i]);
    // }
    
    
    parseArgFiles(d_arguments);
    // free(d_arguments);
}

void parseArgFiles(char **args) {
    int options;
    FTS *ftsp;
    FTSENT *ftsent;
    struct perttyPrint pPrint;
    pPrint = defaultPrettyPrint();
    
    options = FTS_PHYSICAL | FTS_NOCHDIR;
    if(a_flag) {
        options = FTS_PHYSICAL | FTS_NOCHDIR | FTS_SEEDOT;
        A_flag = 1;
    }

    if((ftsp = fts_open(args, options, compar)) == NULL) {
        fprintf(stderr, "ls : %s\n", strerror(errno));
        exit(EXIT_SUCCESS);
    }

    while((ftsent = fts_read(ftsp)) != NULL) {
        if (ftsent->fts_level > 0) {
            fts_set(ftsp, ftsent, FTS_SKIP);
            continue;
        } 
        else if (ftsent->fts_info == FTS_DP) {
            continue;
        }
        generatePrint(ftsent, pPrint);
    }
    fts_close(ftsp);

}


/**
 * This function allocates the blockSIZE varible 
 * which is defined in ls.h, and which is then furthur
 * used to calculate the number of file system 
 * blocks actually used by each file
 * */
void getBlocksAllocated() {
    unsigned int i;
    int len;
    char *e_blockValue;
    bool inValidBlocksize = false;
    e_blockValue = getenv("BLOCKSIZE");
    if(e_blockValue == NULL) {
        blockSIZE = 512;
    }
    
    len = strlen(e_blockValue);
    if(len == 1) {
        blockSIZE = 1;
    }

    for (i = 0; i < strlen(e_blockValue); i++)
    {
        char ch =  e_blockValue[i];
        if(isalpha(ch)){
            switch (ch)
            {
                case 'k':
                case 'K':
                    blockSIZE *= 1024;
                    if(i != strlen(e_blockValue) -1) {
                        inValidBlocksize = true;
                    }
                    break;
                case 'M':
                case 'm':
                    blockSIZE *= 1024 * 1024;
                    if(i != strlen(e_blockValue) -1) {
                        inValidBlocksize = true;
                    }
                    break;
                case 'G':
                case 'g':
                    blockSIZE *= 1024 * 1024 * 1024;
                    if(i != strlen(e_blockValue) -1) {
                        inValidBlocksize = true;
                    }
                    break;
                default:
                    inValidBlocksize = true;
                    break;
            }
            if(inValidBlocksize){
                break;
            }
        } else if(isdigit(ch)) {
            blockSIZE = (blockSIZE * 10) + (long)(ch - '0');
        } else {
            inValidBlocksize = true;
            break;
        }
    }
    if(inValidBlocksize) {
        fprintf(stderr, "ls: %s: unknown blocksize\n", e_blockValue);
        fprintf(stderr, "ls: maximum blocksize is 1G\n");
        fprintf(stderr, "ls: %s: minimum blocksize is 512\n", e_blockValue);
        blockSIZE = 512;
    } else {
        if(blockSIZE < 512) {
            fprintf(stderr, "ls: %ld: minimum blocksize is 512\n", blockSIZE);
            blockSIZE = 512;
        }
        else if(blockSIZE >= 512 && blockSIZE <= 1024 * 1024 * 1024){

        }
        else {
            fprintf(stderr, "ls: %ld: maximum blocksize is 1G\n", blockSIZE);
            blockSIZE = 1024 * 1024 * 1024;
        }
    }
}


/**
 * This fucntion calculates the number of file 
 * system blocks actually used by each file,
 * */
blkcnt_t calculateBlockSize(blkcnt_t blocks) {
    blkcnt_t ret_blockSize;
    ret_blockSize = (unsigned long) (blocks / (blockSIZE / 512));
    return ret_blockSize;
}


/**
 * This is a function which generates human 
 * readable size format
 * */
char* generateHumanReadableSize(off_t size) {
    
    int i = 0;
    char *return_str_size;
    char  *result;
    int n;
    float acc_Size = size;
    char *units[] = {"B", "K", "M", "G", "T"};
    double num;
    n = numOfDigits(size);
    return_str_size = malloc(n + 2);
    
    while (size >= 1024) {
        size /= 1024;
        i++;
    }
    
    if(i == 0) {
        sprintf(return_str_size, "%ld%s", size, units[i]);
    } else {
        if(strcmp(units[i], "K") == 0) {
            num = (double)acc_Size / (double)1024;
            if(num > 9) {
                int size_temp = (int) (round(num));
                sprintf(return_str_size, "%d%s",  size_temp, units[i]);
            } else {
                sprintf(return_str_size, "%.1f%s", num, units[i]);
            }

        } else if(strcmp(units[i], "M") == 0) {
            num = (double)acc_Size / (double)(1024 * 1024);
            if(num > 9) {
                int size_temp = (int) (round(num));
                sprintf(return_str_size, "%d%s",  size_temp, units[i]);
            } else {
                sprintf(return_str_size, "%.1f%s", num, units[i]);
            }
        } else {
            num = (double)acc_Size / (double)(1024 * 1024 * 1024);
            if(num > 9) {
                int size_temp = (int) (round(num));
                sprintf(return_str_size, "%d%s",  size_temp, units[i]);
            } else {
                sprintf(return_str_size, "%.1f%s", num, units[i]);
            }
        }
    }
    result = strdup(return_str_size);
    free(return_str_size);
    return result;
}

int numOfDigits(long num) {
    int count = 1;
    while(num != 0) {
        num /= 10;
        ++count;
    }
    return count;
}

/**
 * This function checks for non-printatble characters in the 
 * string and replaces it with '?'
 * */
char* checkNonPrintChars(char* filename){
    unsigned int i;
    for (i = 0; i < strlen(filename); i++)
    {
        if(!isprint(filename[i])) {
            if(q_flag) {
                filename[i] = '?';
            } else if(w_flag) {
            } else {
            }
        }
    }
    return filename;
}

void getPrintDetails(FTSENT *ftsent, struct perttyPrint *pPrint) {
    struct stat *stat_info;
    time_t time;
    stat_info = ftsent->fts_statp;


    if(stat_info->st_ino  >= pPrint->__max_i_node) {
        pPrint->__max_i_node = stat_info->st_ino;
    }
    
    if(!pPrint->display_total) {
        pPrint->display_total = true;
    }
    pPrint->totalBytesUsed += calculateBlockSize(stat_info->st_blocks);
    
    if(stat_info->st_size >= pPrint->__max_f_size) {
        pPrint->__max_f_size = stat_info->st_size;
    }

    if(pPrint->__max_h_filesize == NULL){
        pPrint->__max_h_filesize = generateHumanReadableSize(stat_info->st_size);
    } else{
        if(strlen(generateHumanReadableSize(stat_info->st_size)) >= strlen(pPrint->__max_h_filesize)) {
            pPrint->__max_h_filesize =  generateHumanReadableSize(stat_info->st_size);
        }
    }

    if(pPrint->__max_user_name == NULL) {
        pPrint->__max_user_name = getUserNameByUserId(stat_info->st_uid);
    } else {
        if(strlen(getUserNameByUserId(stat_info->st_uid)) >= strlen(pPrint->__max_user_name)) {
            pPrint->__max_user_name = getUserNameByUserId(stat_info->st_uid);
        }
    }

    if(pPrint->__max_groupname == NULL) {
        pPrint->__max_groupname = getGroupNameByGroupId(stat_info->st_gid);
    } else {
        if(strlen(getGroupNameByGroupId(stat_info->st_gid)) >= strlen(pPrint->__max_groupname)){
            pPrint->__max_groupname = getGroupNameByGroupId(stat_info->st_gid);
        }
    }

    if(stat_info->st_uid >= pPrint->__max_uid) {
        pPrint->__max_uid = stat_info->st_uid;
    }

    if(stat_info->st_gid >= pPrint->__max_gid) {
        pPrint->__max_gid =  stat_info->st_gid;
    }

    if(c_flag) {
        time = stat_info->st_ctime;
    } else if(t_flag) {
        time = stat_info->st_mtime;
    } else {
        time = stat_info->st_atime;
    }

    if(getDay(time) >= pPrint->__max_day) {
        pPrint->__max_day = getDay(time);
    }

    if(calculateBlockSize(stat_info->st_blocks) >= pPrint->__max_blocks_used) {
        pPrint->__max_blocks_used = stat_info->st_blocks;
    }

    if(pPrint->__max_h_blocksize == NULL) {
        pPrint->__max_h_blocksize = generateHumanReadableSize(512 * stat_info->st_blocks);
    } else {
        if(strlen(generateHumanReadableSize(512 * stat_info->st_blocks)) >= strlen(pPrint->__max_h_blocksize)){
            pPrint->__max_h_blocksize = generateHumanReadableSize(512 * stat_info->st_blocks);
        }
    }

    if(stat_info->st_nlink >= pPrint->__max_h_links) {
        pPrint->__max_h_links = stat_info->st_nlink;
    }
}

bool isDisplayTotal(struct perttyPrint *pPrint) {
    if((s_flag  || l_flag || n_flag) && pPrint->display_total) {
        return true;
    }
    return false;
}
