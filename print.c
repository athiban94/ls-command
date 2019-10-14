#include "print.h"

void display_out(struct printOPT options, struct perttyPrint pPrint) {

    // printf("Max size : (%llu, %llu) ", pPrint.__max_f_size, options.f_size);
    
    if(options.i_node != 0){
        calculateAndAddSpacesLongType(pPrint.__max_i_node, options.i_node);
        printf("%llu ", options.i_node);
    }

    if(options.display_blocks){
        calculateAndAddSpacesLongType(pPrint.__max_blocks_used, options.blocks_used);
        printf("%lli ", options.blocks_used);
    }

    if(options.h_blocksize != NULL) {
        calculateAndAddSpacesCharType(pPrint.__max_h_blocksize, options.h_blocksize);
        printf("%s ", options.h_blocksize);
    }

    if(options.f_mode != NULL)
        printf("%s ", options.f_mode);
    if(options.h_links != 0) {
        calculateAndAddSpacesLongType(pPrint.__max_h_links, options.h_links);
        printf("%hu ", options.h_links);
    }

    if(options.display_uid_gid) {
        printf("%lu ", options.uid);
        calculateAndAddSpacesLongType(pPrint.__max_uid, options.uid);
    }
    
    if(options.display_uid_gid) {
        printf("%lu ", options.gid);
        calculateAndAddSpacesLongType(pPrint.__max_gid, options.gid);
    }
    
    if(options.user_name != NULL)
        printf("%s ", options.user_name);
    
    if(options.groupname != NULL)
        printf("%s  ", options.groupname);

    if(options.isSpecialFile) {
        printf("%d, ", options.major_number);
    }

    if(options.isSpecialFile) {
        printf("%d ", options.minor_number);
    }

    if(options.display_file_size) {
        calculateAndAddSpacesLongType(pPrint.__max_f_size, options.f_size);
        printf("%lld ", options.f_size);
    }
    if(options.h_filesize != NULL) {
        calculateAndAddSpacesCharType(pPrint.__max_h_filesize, options.h_filesize);
        printf("%s ", options.h_filesize);
    }
    if(options.month != NULL)
        printf("%s ", options.month);
    if(options.day != 0){
        calculateAndAddSpacesLongType(pPrint.__max_day, options.day);
        printf("%d ", options.day);
    }
    if(options.display_time)
        printf("%02d:", options.hour);
    
    if(options.display_time)
        printf("%02d ", options.minute);
    
    if(options.f_name != NULL)
        printf("%s", options.f_name);
    if(options.sym_link != NULL)
        printf(" -> %s", options.sym_link);

    printf("\n");
}

struct printOPT initialize_options_null() {
    struct printOPT opt;
    opt.i_node              = 0;
    opt.blocks_used         = 0;
    opt.h_blocksize         = NULL;
    opt.f_mode              = NULL;
    opt.h_links             = 0;
    opt.user_name           = NULL;
    opt.groupname           = NULL;
    opt.major_number        = 0;
    opt.minor_number        = 0;
    opt.f_size              = 0;
    opt.h_filesize          = NULL;
    opt.month               = NULL;
    opt.day                 = 0;
    opt.hour                = 0;
    opt.minute              = 0;
    opt.f_name              = NULL;
    opt.sym_link            = NULL;
    opt.display_uid_gid     = false;
    opt.display_file_size   = false;
    opt.display_blocks      = false;
    opt.isSpecialFile       = false;
    opt.display_time        = false;
    return opt;
}
struct perttyPrint defaultPrettyPrint() {
    struct perttyPrint pPrint;

    pPrint.totalBytesUsed       = 0;
    pPrint.__max_i_node         = 0;
    pPrint.__max_blocks_used    = 0;
    pPrint.__max_h_blocksize    = NULL;
    pPrint.__max_f_mode         = NULL;
    pPrint.__max_h_links        = 0;
    pPrint.__max_uid            = 0;
    pPrint.__max_gid            = 0;
    pPrint.__max_user_name      = NULL;
    pPrint.__max_groupname      = NULL;
    pPrint.__max_f_size         = 0;
    pPrint.__max_h_filesize     = NULL;
    pPrint.__max_month          = NULL;
    pPrint.__max_day            = 0;
    pPrint.__max_hour           = 0;
    pPrint.__max_minute         = 0;
    pPrint.__max_f_name         = NULL;
    pPrint.__max_sym_link       = NULL;
    pPrint.display_total        = false;
    return pPrint;
}

void calculateAndAddSpacesLongType(unsigned long long maxInt, unsigned long long minInt){
    if(maxInt > minInt) {
        int max_digits = getNumberOfDigits(maxInt);
        int min_digits = getNumberOfDigits(minInt);
        int n = max_digits - min_digits;
        for (int i = 0; i < n; i++) {
            printf(" ");
        }
    }
}

void calculateAndAddSpacesCharType(char *maxChar , char *minChar) {
    if(maxChar != NULL) {
        int max = strlen(maxChar);
        int min = strlen(minChar);
        int n = max - min;
        for (int i = 0; i < n; i++)
        {
            printf(" ");
        }
    }
}


int getNumberOfDigits(unsigned long long num) { 
    int count = 0;
    if(num == 0) {
        return 1;
    } else {
        while(num != 0) {
            num /= 10;
            ++count;
        }
        return count;
    }
}