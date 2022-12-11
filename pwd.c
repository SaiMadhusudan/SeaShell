#include "pwd.h"
#include "headers.h"
#include "stdlib.h"

extern char Prev_dir[MAX_L];
extern char USER_G[MAX_L];
extern char SYSTEM_G[MAX_L];
extern char CUR_DIR_G[MAX_L];
extern char HOME_DIR[MAX_L];

extern int change;
void longestCommonPrefix(int n, char ar[n][MAX_L], char *final);

bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

int file_typepwd(struct dirent *d)
{
    struct stat statbuf;
    int status = stat(d->d_name, &statbuf);

    if (statbuf.st_mode & S_IXUSR && d->d_type != DT_DIR)
        return (2);

    if (d->d_type == DT_DIR)
        return (0);

    if (d->d_type == DT_REG)
        return (1);
}

int check_subdir()
{
    if (strlen(CUR_DIR_G) >= strlen(HOME_DIR))
    {
        if (strncmp(CUR_DIR_G, HOME_DIR, strlen(HOME_DIR)) == 0)
            return 1;
        else
            return 0;
    }
    return (0);
}

void pwd_function()
{
    getcwd(CUR_DIR_G, MAX_L);
    printf("%s\n", CUR_DIR_G);
}

void print_(char *USER, char *SYSTEM)
{
    char PRINT_DIR[MAX_L];
    getcwd(CUR_DIR_G, MAX_L);
    strcpy(PRINT_DIR, "\0");

    if (check_subdir() == 1)
    {
        //     for (int i = strlen(HOME_DIR); i < strlen(CUR_DIR_G); i++)
        //     {
        //         PRINT_DIR[i - strlen(HOME_DIR)] = CUR_DIR_G[i];
        //     }
        strcpy(PRINT_DIR, &CUR_DIR_G[strlen(HOME_DIR)]);
    }
    else
    {
        strcpy(PRINT_DIR, CUR_DIR_G);
    }

    printf("\033[1;32m");
    printf("<%s@%s:", USER, SYSTEM);
    printf("\033[0m");
    printf("\033[0;34m");
    printf("~%s", PRINT_DIR);
    printf("\033[0;32m");
    printf(">");
    printf("\033[0m");
}

void dir_status(char *USER, char *SYSTEM, char *CUR_DIR)
{
    getlogin_r(USER, MAX_L);
    getcwd(CUR_DIR, MAX_L);
    gethostname(SYSTEM, MAX_L);
}

void only_space(char *S, char *F)
{
    strcpy(F, S);
    char address[MAX_L];
    strcpy(address, CUR_DIR_G);

    struct dirent **d;
    int count = scandir(address, &d, NULL, alphasort);

    for (int i = 0; i < count; i++)
    {
        if (d[i]->d_name[0] != '.')
        {
            if (file_typepwd(d[i]) == 0)
            {
                printf("%s/\n", d[i]->d_name);
            }
            else
            {
                printf("%s\n", d[i]->d_name);
            }
        }
    }
    return;
}

void longestCommonPrefix(int n, char ar[MAX_L][MAX_L], char *final)
{

    // If size is 0, return empty string
    if (n == 0)
    {
        strcpy(final, "");
        return;
    }

    if (n == 1)
    {
        strcpy(final, ar[0]);
        return;
    }

    // Find the minimum length from
    // first and last string
    int en = strlen(ar[0]);
    for (int i = 0; i < n; i++)
    {
        if (en < strlen(ar[i]))
        {
            en = strlen(ar[i]);
        }
    }

    // Now the common prefix in first and
    // last string is the longest common prefix
    char first[MAX_L];
    strcpy(first, ar[0]);

    char last[MAX_L];
    strcpy(last, ar[n - 1]);

    int i = 0;
    while (i < en && first[i] == last[i])
        i++;

    strncpy(final, first, i);
}

void only_name(char *S, char *F, char *search_name)
{
    printf("here\n");
    strcpy(F, S);
    struct dirent **d;
    int count = scandir(CUR_DIR_G, &d, NULL, alphasort);

    int count_all = 0;

    char pre_strings[count][MAX_L];
    int index[count];

    for (int i = 0; i < count; i++)
    {
        if (prefix(search_name, d[i]->d_name) == 1)
        {
            strcpy(pre_strings[count_all], d[i]->d_name);
            if (file_typepwd(d[i]) == 0)
            {
                strcat(pre_strings[count_all], "/");
            }
            index[count_all] = i;
            count_all++;
        }
    }

    //////////////////////////////
    if (count_all == 1)
    {
        strcpy(F, pre_strings[0]);
        return;
    }
    //////////////////////////////
    // getting best prefix
    char final[MAX_L];
    longestCommonPrefix(count_all, pre_strings, final);

    if (strcmp(search_name, final) == 0)
    {
        for (int i = 0; i < count; i++)
        {
            if (d[index[i]]->d_name[0] != '.')
            {
                if (file_typepwd(d[index[i]]) == 0)
                {
                    printf("%s/\n", d[index[i]]->d_name);
                }
                else
                {
                    printf("%s\n", d[index[i]]->d_name);
                }
            }
        }
        strcpy(F, S);
        return;
    }
    else
    {
        strcpy(search_name, final);
    }
    strcpy(F, search_name);
    return;
}

void auto_complete(char *S, char *F)
{
    printf("\33[2K");
    printf("\r");

    char search[MAX_L] = {'\0'}; // What to search
    char search_name[MAX_L] = {'\0'};
    char address[MAX_L] = {'\0'}; // Where to search

    char temp[MAX_L];

    int firstspace;

    for (int i = strlen(S) - 1; i > -1; i--)
    {
        if (S[i] == ' ')
        {
            firstspace = i;
            strcpy(temp, &S[i + 1]);

            if (i == strlen(S) - 1)
            {
                only_space(S, F);
                return;
            }
            break;
        }
    }
    strcpy(F, temp);
    ///////temp is going
    //////searching for name
    //////searching for address
    int firstslash;

    for (int i = strlen(temp) - 1; i > -1; i--)
    {
        if (temp[i] == '/')
        {
            firstslash = i;
            strcpy(search_name, &temp[i + 1]);
            strncpy(address, temp, i + 1);
            break;
        }

        if (i == 0)
        {
            strcpy(search_name, temp);
            only_name(S, F, search_name);
            return;
        }
    }

    ////////////address
    ////////////searchname

    struct dirent **d;
    int count = scandir(address, &d, NULL, alphasort);

    int count_all = 0;

    char pre_strings[count][MAX_L];
    int index[count];

    for (int i = 0; i < count; i++)
    {
        if (prefix(search_name, d[i]->d_name) == 1)
        {
            strcpy(pre_strings[count_all], d[i]->d_name);
            if (file_typepwd(d[i]) == 0)
            {
                strcat(pre_strings[count_all], "/");
            }
            index[count_all] = i;
            count_all++;
        }
    }

    //////////////////////////////
    if (count_all == 1)
    {
        strcat(address, pre_strings[0]);
        strncpy(F, S, firstspace + 1);
        strcat(F, address);
        return;
    }
    //////////////////////////////
    // getting best prefix
    char final[MAX_L];
    longestCommonPrefix(count_all, pre_strings, final);

    if (strcmp(search_name, final) == 0)
    {
        for (int i = 0; i < count; i++)
        {
            if (d[index[i]]->d_name[0] != '.')
            {
                if (file_typepwd(d[index[i]]) == 0)
                {
                    printf("%s/\n", d[index[i]]->d_name);
                }
                else
                {
                    printf("%s\n", d[index[i]]->d_name);
                }
            }
        }
        return;
    }
    else
    {
        strcpy(search_name, final);
    }

    strcat(address, search_name);
    strncpy(F, S, firstspace + 1);
    strcat(F, address);

    return;
}
