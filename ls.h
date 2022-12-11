#ifndef _LS_H
#define _LS_H

int file_type(struct dirent *d);
void print_file_status(struct dirent *d);
void ls_l(char *s);
int ls_function(char *s);
int which_ls(char *s);
void print_file_name(struct dirent *d);
int file_type(struct dirent *d);

#endif