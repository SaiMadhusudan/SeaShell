#ifndef _456_H
#define _456_H

int exvpfg(char **args, int n_s);
int exvpbg(char **args, int n_s);
void terminatefg(int signum);
int jobs(char *s);
int bg_(char *s);
int fg_(char *s);
int sig(char *s);
#endif