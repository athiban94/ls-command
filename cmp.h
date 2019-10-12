#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <fts.h>
static int (*compar) (const FTSENT **, const FTSENT **); 

int sortLexographical(const FTSENT **fileEntryPointer, const FTSENT **fileEntryPointerTwo);
int sortReverseLexographical(const FTSENT **fileEntryPointer, const FTSENT **fileEntryPointerTwo);
int sortFileSizeDescending(const FTSENT **fileEntryPointer, const FTSENT **fileEntryPointerTwo);
int sortFilesByModifiedTime(const FTSENT **fileEntryPointer, const FTSENT **fileEntryPointerTwo);
int sortFilesByAccessTime(const FTSENT **fileEntryPointer, const FTSENT **fileEntryPointerTwo);
