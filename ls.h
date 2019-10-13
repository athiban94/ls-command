#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include "print.h"
#include "cmp.h"

struct f_error { char *f_error_path; };
struct f_non_dir { char *f_non_dirname; };
struct f_dir { char *f_dirname; };

int argumentC = 0;
int a_flag = 0;
int A_flag = 0;
int c_flag = 0;
int d_flag = 0;
int F_flag = 0;
int f_flag = 0;
int h_flag = 0;
int i_flag = 0;
int k_flag = 0;
int l_flag = 0;
int n_flag = 0;
int q_flag = 0;
int R_flag = 0;
int r_flag = 0;
int S_flag = 0;
int s_flag = 0;
int t_flag = 0;
int u_flag = 0;
int w_flag = 0;

blkcnt_t blockSIZE = 0;
int print_Flag = 0;


void handleMuliplePaths(int, int, int, int, char**);
void print_errors_args(struct f_error [], int size);
void print_non_directories(struct f_non_dir [], int size);
void traverseDirs(struct f_dir [], int size);
void traverseFiles(char*);
void traverse_FTS(char**);
void generatePrint(FTSENT *);
void parseArgFiles(char**);
bool isDisplayParent();
void getBlocksAllocated();
blkcnt_t calculateBlockSize(blkcnt_t blocks);
char* generateHumanReadableSize(off_t);
int numOfDigits(long long num);
void traverseDOption(struct f_non_dir non_dirs[], int n, struct f_dir n_dirs[], int m);
char* checkNonPrintChars(char*);