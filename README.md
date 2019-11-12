# Implementation of LS(1) command
This is a simple implementation of the ls command on NetBSD

## SYNOPSIS
ls [ −AacdFfhiklnqRrSstuw] [file . . .]

## Description
For each operand that names a file of a type other than directory, ls displays its name as well as any
requested, associated information. For each operand that names a file of type directory, ls displays the
names of files contained within that directory, as well as any requested, associated information.

If no operands are given, the contents of the current directory are displayed. If more than one operand is
given, non-directory operands are displayed first; directory and non-directory operands are sorted separately
and in lexicographical order.


## Problems Encountered
1. It was very difficult for me to understand FTS(3). I spent a lot of time
    understanding from the man page. 

2. printing pretty in the terminal in -l with proper spacing took a lot of time
    in refactoring my code. At this stage I made my code very complicated and it 
    was really hard for me debug further.

3. In some senarios my program throws segmentation fault like for 
    example: ./ls -l file1 file2 dir1 dir2 nowhere. I had fixed that 
    issue before but after implementing several options I made the codebase
    very complicated, this is perhaps I did't modularize it properly. I will
    keep all this issues in mind and make sure my further assignments 
    are proper.