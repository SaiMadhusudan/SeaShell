#ifndef _PWDN_H
#define _PWDN_H

int check_subdir();
void pwd_function();
void print_(char *USER, char *SYSTEM);
void dir_status(char *USER, char *SYSTEM, char *CUR_DIR);
void auto_complete(char *S, char *F);

#endif