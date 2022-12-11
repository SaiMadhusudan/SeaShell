#include "headers.h"
#include "pwd.h"
#include "cd.h"

extern char Prev_dir[MAX_L];
extern char USER_G[MAX_L];
extern char SYSTEM_G[MAX_L];
extern char CUR_DIR_G[MAX_L];
extern const char HOME_DIR[MAX_L];
extern int change;

void cd_function_new(char s[MAX_L])
{
    int s_n = 0;
    char temp[MAX_L];
    strcpy(temp, s);

    char *SPLIT = strtok(temp, " ");

    char dirs[10][MAX_L];

    while (SPLIT != NULL)
    {
        strcpy(dirs[s_n], SPLIT);
        s_n++;
        SPLIT = strtok(NULL, " ");
    }

    if (s_n > 2)
    {
        printf("bash: cd: too many arguments\n");
        return;
    }
    else if (s_n == 1)
    {
        strcpy(Prev_dir, CUR_DIR_G);
        int ch = chdir(HOME_DIR);
    }
    else
    {
        cd_function(dirs[1]);
    }
}

void cd_function(char s[MAX_L])
{

    if (strcmp("-", s) == 0)
    {
        if (change == 0)
        {
            printf("bash: cd: OLDPWD not set\n");
            return;
        }
        printf("%s\n", Prev_dir);
        int ch = chdir(Prev_dir);
        strcpy(Prev_dir, CUR_DIR_G);
        if (ch != 0)
            printf("bash: cd: %s: No such file or directory\n", s);
    }
    else if (strcmp("~", s) == 0)
    {
        strcpy(Prev_dir, CUR_DIR_G);
        int ch = chdir(HOME_DIR);
    }
    else if (s[0] == '~')
    {
        strcpy(CUR_DIR_G, HOME_DIR);
        strcat(CUR_DIR_G, &s[1]);

        int ch = chdir(CUR_DIR_G);
        if (ch != 0)
            printf("bash: cd: %s: No such file or directory here \n", s);
        return;
    }
    else if (s[0] == ';')
    {
        strcpy(Prev_dir, CUR_DIR_G);
        int ch = chdir(HOME_DIR);
        if (ch != 0)
            printf("bash: cd: %s: No such file or directory\n", s);
        return;
    }
    else
    {
        int ch = chdir(s);
        if (ch != 0)
        {
            printf("bash: cd: %s: No such file or directory\n", s);
            return;
        }
        else
            strcpy(Prev_dir, CUR_DIR_G);
    }
    getcwd(CUR_DIR_G, MAX_L);
    change = 1;
}
