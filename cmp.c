#include "cmp.h"

int sortLexographical(const FTSENT **fileEntryPointer, const FTSENT **fileEntryPointerTwo) {
    const FTSENT *file1 = *fileEntryPointer;
    const FTSENT *file2 = *fileEntryPointerTwo;
    return strcmp(file1->fts_name, file2->fts_name);
}

int sortReverseLexographical(const FTSENT **fileEntryPointer, const FTSENT **fileEntryPointerTwo) {
    const FTSENT *file1 = *fileEntryPointer;
    const FTSENT *file2 = *fileEntryPointerTwo;
    return -1 * strcmp(file1->fts_name, file2->fts_name);
}

int sortFileSizeDescending(const FTSENT **fileEntryPointer, const FTSENT **fileEntryPointerTwo) {
    struct stat *stat_file1, *stat_file2;
    const FTSENT *file1 = *fileEntryPointer;
    const FTSENT *file2 = *fileEntryPointerTwo;
    stat_file1 = file1->fts_statp;
    stat_file2 = file2->fts_statp;
    if(stat_file1->st_size > stat_file2->st_size)
        return -1;
    else if(stat_file1->st_size == stat_file2->st_size)
        return 0;
    else 
        return 1;
}

int sortFilesByModifiedTime(const FTSENT **fileEntryPointer, const FTSENT **fileEntryPointerTwo) {
    struct stat *stat_file1, *stat_file2;
    const FTSENT *file1 = *fileEntryPointer;
    const FTSENT *file2 = *fileEntryPointerTwo;
    stat_file1 = file1->fts_statp;
    stat_file2 = file2->fts_statp;
    if(stat_file1->st_mtime > stat_file2->st_mtime) {
        return -1;
    } else if(stat_file1->st_mtime == stat_file2->st_mtime) {
        return sortLexographical(fileEntryPointer, fileEntryPointerTwo);
    } else {
        return 1;
    }
}

int sortFilesByAccessTime(const FTSENT **fileEntryPointer, const FTSENT **fileEntryPointerTwo) {
    struct stat *stat_file1, *stat_file2;
    const FTSENT *file1 = *fileEntryPointer;
    const FTSENT *file2 = *fileEntryPointerTwo;
    stat_file1 = file1->fts_statp;
    stat_file2 = file2->fts_statp;
    if(stat_file1->st_atimespec.tv_sec > stat_file2->st_atimespec.tv_sec) {
        return -1;
    } else if(stat_file1->st_atimespec.tv_sec == stat_file2->st_atimespec.tv_sec){
        return sortLexographical(fileEntryPointer, fileEntryPointerTwo);
    } else {
        return 1;
    }
}