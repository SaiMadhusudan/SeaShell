#include "headers.h"
#include "discover.h"

extern char CUR_DIR_G[MAX_L];
extern char HOME_DIR[MAX_L];

int which_flagd(char *s)
{
    if (strstr(s, " -d") != NULL && strstr(s, " -f") != NULL)
        return 0;

    if (strstr(s, " -d") != NULL)
        return 1;

    if (strstr(s, " -f") != NULL)
        return 2;

    return 0;
}

int file_typedirent(struct dirent *d)
{
    struct stat statbuf;
    int status = stat(d->d_name, &statbuf);

    if (d->d_type == DT_DIR)
        return (0);

    if (d->d_type == DT_REG)
        return (1);
}

void find_all(char *path, int f_flag, int d_flag)
{
    if (d_flag)
        printf("%s\n", path);
    struct dirent **d;
    int count = scandir(path, &d, NULL, alphasort);

    for (int i = 0; i < count; i++)
    {
        if (strcmp(d[i]->d_name, "..") != 0 && strcmp(d[i]->d_name, ".") != 0)
        {
            char new_path[MAX_L];
            sprintf(new_path, "%s/%s", path, d[i]->d_name);

            if ((file_typedirent(d[i]) == 0))
                find_all(new_path, f_flag, d_flag);
            else if ((file_typedirent(d[i])) == 1 && f_flag)
                printf("%s\n", new_path);
        }

        free(d[i]);
    }

    free(d);
}

void search_all(char *path, int f_flag, int d_flag, char *S_name)
{
    struct dirent **d;
    int count = scandir(path, &d, NULL, alphasort);

    for (int i = 0; i < count; i++)
    {
        if (strcmp(d[i]->d_name, "..") != 0 && strcmp(d[i]->d_name, ".") != 0)
        {
            char new_path[MAX_L];
            sprintf(new_path, "%s/%s", path, d[i]->d_name);

            if ((file_typedirent(d[i]) == 0))
            {
                if (strcmp(d[i]->d_name, S_name) == 0 && d_flag)
                    printf("%s\n", new_path);

                search_all(new_path, f_flag, d_flag, S_name);
            }
            else if ((file_typedirent(d[i])) == 1)
            {
                if (strcmp(d[i]->d_name, S_name) == 0 && f_flag)
                    printf("%s\n", new_path);
            }
        }

        free(d[i]);
    }

    free(d);
}

void discover_f(char *s)
{
    char Target[MAX_L];
    strcpy(Target, s);
    char S_name[MAX_L];
    strcpy(S_name, "\0");

    int j = 0;
    int sflag = 0;

    for (int i = 0; i < strlen(s); i++)
    {
        if (s[i] == '\"')
        {
            i++;
            while (s[i] != '\"')
            {
                S_name[j] = s[i];
                j++;
                i++;
            }
        }
    }
    S_name[j] = '\0';

    char *SPLIT = strtok(Target, " ");
    char T[MAX_L];
    int s_n = 0;
    while (SPLIT != NULL)
    {
        if (strcmp(SPLIT, "-d") != 0 && strcmp(SPLIT, "-f") != 0 && strcmp(SPLIT, "discover") != 0 && SPLIT[0] != '\"')
        {
            strcpy(T, SPLIT);
            if (T[0] == '~')
            {
                T[0] = '.';
               // assert(1==2);
            }
            s_n++;
            break;
        }
        SPLIT = strtok(NULL, " ");
    }

    if (s_n == 0)
    {
        strcpy(T, ".");
    }

    if (j != 0)
        search_all(T, (which_flagd(s) == 0 | which_flagd(s) == 2), (which_flagd(s) == 0 | which_flagd(s) == 1), S_name);
    else
        find_all(T, (which_flagd(s) == 0 | which_flagd(s) == 2), (which_flagd(s) == 0 | which_flagd(s) == 1));
    return;
}