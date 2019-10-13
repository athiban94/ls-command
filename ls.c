#include "ls.h"

int main(int argc, char *argv[])
{
    int c;
    opterr = 0;
    int n_err = 0, n_nondir = 0, n_dir = 0;
    struct stat sb;
    char **arguments;
    argumentC = argc;
    compar = &sortLexographical;
    uid_t userId;
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
    userId = getuid();
    if(userId == 0) {
        A_flag = 1;
    }

    if(s_flag) {
        if(k_flag) {
            blockSIZE = 1024;
        } else  {
            getBlocksAllocated();
        }
    }

    if(optind == argc) 
    {
        arguments = malloc(1 * sizeof(char*));
        arguments[0] = "./";
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
            // arguments = malloc(1 * sizeof(char*));
            // arguments[0] = argv[optind];
            arguments = malloc(1 * sizeof(char*));
            if(argv[optind] == ".")
                arguments[0] = "./";
            else if(argv[optind] == "..")
                arguments[0] = "../";
            else 
                arguments[0] = argv[optind];
            // traverseFiles(argv[optind]);
            traverse_FTS(arguments);
        }
        else 
        {
            arguments = malloc(1 * sizeof(char*));
            arguments[0] = argv[optind];
            parseArgFiles(arguments);
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
    char **args = malloc(len * sizeof(char*));
    for (int i = 0; i < len; i++)
    {
        // printf("%s\t", non_dir[i].f_non_dirname);
        args[i] = non_dir[i].f_non_dirname;
    }
    parseArgFiles(args);
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

    if(a_flag) {
        options = FTS_PHYSICAL | FTS_NOCHDIR | FTS_SEEDOT;
        A_flag = 1;
    }


    if((ftsp = fts_open(args, options, compar)) == NULL) {
        //double check with professor - not throwing error for invalid
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

        if(children->fts_level > 1 && !R_flag){
            continue;
        }

        if(isDisplayParent()){
            printf("\n");
            printf("%s:\n", children->fts_parent->fts_path);
        }
        while (children != NULL) {
            if(strncmp(children->fts_name, ".", 1) == 0) {
                if(A_flag) {
                    generatePrint(children);
                }
            } else {
                generatePrint(children);
            }
            children = children->fts_link;
        }


        // if((fts_set(ftsp, ftsent, FTS_AGAIN)) == -1) {
        //     fprintf(stderr, " error : %s", strerror(errno));
        // }
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
    char *h_filesize;
    char *h_blocksize;
    struct passwd *pws;
    struct group *grp;
    time_t time;
    struct tm *time_data;
    char *path = malloc(ftsent->fts_pathlen + ftsent->fts_namelen + 1);
    char *month_list[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    if(ftsent->fts_level == 0) {
        strcpy(path, ftsent->fts_path);
    } else {
        strcpy(path, ftsent->fts_path);
        strcat(path, ftsent->fts_name);
    }
    char buf[PATH_MAX];
    ssize_t len;
    char *filename_F = malloc(ftsent->fts_namelen + 2);
    blkcnt_t file_blocks;
    
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
        // printf("block size :: %lli ", file_blocks);
    }
    
    /* i-flag - option */
    if(i_flag)
        options.i_node = stat_info->st_ino;
        
    if(l_flag) {
        options.display_file_size = true;
        strmode(stat_info->st_mode, mode_str);
        options.f_mode = mode_str;

        options.h_links = stat_info->st_nlink;

        if(n_flag) {
            /* -n option when enabled displaying uid and gid */
            options.display_uid_gid = true;
            options.uid = stat_info->st_uid;
            options.gid = stat_info->st_gid;
        } else {
            pws = getpwuid(stat_info->st_uid);
            options.user_name = pws->pw_name;

            grp = getgrgid(stat_info->st_gid);
            options.groupname = grp->gr_name;
        }

        options.f_size = stat_info->st_size;

        if(h_flag){
            options.display_file_size = false;
            options.h_filesize = generateHumanReadableSize(stat_info->st_size);
        }

        if(c_flag) {
            time = stat_info->st_ctime;
        } else {
            time = stat_info->st_mtime;
        }
        time_data = localtime(&time);
        options.month = month_list[time_data->tm_mon];
        options.day = time_data->tm_mday;
        options.hour = time_data->tm_hour;
        options.minute = time_data->tm_min;

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
            if(stat_info->st_mode & S_IXUSR) {
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
    // printf("path : %s\n", path);
    
    display_out(options, ftsent->fts_name);
    free(path);
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
void traverseDOption(struct f_non_dir f_non_dir[], int n_files, struct f_dir f_dir[], int n_dirs){
    int totalSizeArgs = n_files + n_dirs;
    char **d_arguments = malloc(totalSizeArgs * sizeof(char*));
    int j = 0;
    for (int i = 0; i < n_files; i++)
    {
        d_arguments[j] = f_non_dir[i].f_non_dirname;
        j++;
    }

    for (int i = 0; i < n_dirs; i++)
    {
        d_arguments[j] = f_dir[i].f_dirname;
        j++;
    }
    
    parseArgFiles(d_arguments);
    free(d_arguments);
}

void parseArgFiles(char **args) {
    FTS *ftsp;
    FTSENT *ftsent;
    int options = FTS_PHYSICAL | FTS_NOCHDIR;
    if(a_flag) {
        options = FTS_PHYSICAL | FTS_NOCHDIR | FTS_SEEDOT;
        A_flag = 1;
    }

    if((ftsp = fts_open(args, options, compar)) == NULL) {
        //double check with professor - not throwing error for invalid
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
        // printf("%s \n", ftsent->fts_path);
        generatePrint(ftsent);
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
    char *e_blockValue;
    bool inValidBlocksize = false;
    e_blockValue = getenv("BLOCKSIZE");
    if(e_blockValue == NULL) {
        blockSIZE = 512;
    }
    
    int len = strlen(e_blockValue);
    if(len == 1) {
        blockSIZE = 1;
    }

    for (int i = 0; i < strlen(e_blockValue); i++)
    {
        char ch =  e_blockValue[i];
        long multiplier = 1024;
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
            fprintf(stderr, "ls: %lli: minimum blocksize is 512\n", blockSIZE);
            blockSIZE = 512;
        }
        else if(blockSIZE >= 512 && blockSIZE <= 1024 * 1024 * 1024){
            // do nothing
        }
        else {
            fprintf(stderr, "ls: %lli: maximum blocksize is 1G\n", blockSIZE);
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
    ret_blockSize = blocks / (blockSIZE / 512);
    return ret_blockSize;
}


/**
 * This is a function which generates human 
 * readable size format
 * */
char* generateHumanReadableSize(off_t size) {
    
    int i = 0;
    int n;
    float acc_Size = size;
    char *units[] = {"B", "K", "M", "G", "T"};
    double num;
    n = numOfDigits(size);
    char *return_str_size = malloc(n + 2);
    
    while (size >= 1024) {
        size /= 1024;
        i++;
    }
    
    if(i == 0) {
        sprintf(return_str_size, "%d%s", size, units[i]);
    } else {
        if(units[i] == "K") {
            num = (double)acc_Size / (double)1024;
            if(num > 9) {
                int size = (int) (round(num));
                sprintf(return_str_size, "%d%s",  size, units[i]);
            } else {
                sprintf(return_str_size, "%.1f%s", num, units[i]);
            }

        } else if(units[i] == "M") {
            num = (double)acc_Size / (double)(1024 * 1024);
            if(num > 9) {
                int size = (int) (round(num));
                sprintf(return_str_size, "%d%s",  size, units[i]);
            } else {
                sprintf(return_str_size, "%.1f%s", num, units[i]);
            }
        } else {
            // acc_Size /= (1024 * 1024 * 1024);
            num = (double)acc_Size / (double)(1024 * 1024 * 1024);
            if(num > 9) {
                int size = (int) (round(num));
                sprintf(return_str_size, "%d%s",  size, units[i]);
            } else {
                sprintf(return_str_size, "%.1f%s", num, units[i]);
            }
        }
    }
    char  *result = strdup(return_str_size);
    free(return_str_size);
    return result;
}

int numOfDigits(long long num) {
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
    for (int i = 0; i < strlen(filename); i++)
    {
        if(!isprint(filename[i])) {
            if(q_flag) {
                filename[i] = '?';
            } else if(w_flag) {
                // do not replace
            } else {
                // do nothing
            }
        }
    }
    return filename;
}