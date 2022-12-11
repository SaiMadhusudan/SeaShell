#include "headers.h"
#include "pinfo.h"

extern char Back_ground[50][MAX_L];
extern int pid_b[50];

void pinfo_f(char *s)
{
    if (s[strlen(s) - 1] == ' ')
    {
        s[strlen(s) - 1] = '\0';
    }

    char l[10];
    sprintf(l, " %d", getpid());

    if (strcmp(s, "pinfo") == 0)
    {
        strcat(s, l);
    }

    if (s[strlen(s) - 1] == ' ')
    {
        s[strlen(s) - 1] = '\0';
    }

    char status;
    int memory;
    char path[1024];
    int pgrp = -1;
    int tpgid = -1;

    char statfile[1000] = "/proc/";
    char exefile[1000] = "/proc/";
    strcat(statfile, &s[6]);
    strcat(statfile, "/stat");
    strcat(exefile, &s[6]);
    strcat(exefile, "/exe");

    FILE *file = fopen(statfile, "r");

    if (file)
    {
        fscanf(file, "%*d %*s %c %*d %d %*d %*d %d %*u %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %d",
               &status, &pgrp, &tpgid,
               &memory);
    }
    else
    {
        printf("%s\n", statfile);
        printf("Process does not exist\n");
        return;
    }

    printf("pid : %s\n", s);

    printf("process status : %c%c \nmemory : %.0f\n", status, pgrp == tpgid ? '+' : ' ', (memory / (double)1024));

    int pathLen = readlink(exefile, path, 1023);
    printf("Execuatble : %s\n", path);
}