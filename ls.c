#include "ls.h"

int main(int argc, char *argv[])
{
    int c;
    opterr = 0;
    int n_err = 0, n_nondir = 0, n_dir = 0;
    struct stat sb;

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
        traverseFiles(".");
    }
    else if (optind == argc - 1)
    {
        traverseFiles(argv[optind]);
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
    for (int i = 0; i < len; i++)
    {
        printf("%s:\n", dirs[i].f_dirname);
        traverseFiles(dirs[i].f_dirname);
    }
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
    {
        fprintf(stderr, "ls: %s : %s\n", dirpath,strerror(errno));
    }
    while ((dirp = readdir(dp)) != NULL)
    {
		printf("%s\n", dirp->d_name);
    }
    
    closedir(dp);
}