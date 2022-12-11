#include "headers.h"
#include "cd.h"
#include <pwd.h>
#include <grp.h>

extern char Prev_dir[MAX_L];
extern char USER_G[MAX_L];
extern char SYSTEM_G[MAX_L];
extern char CUR_DIR_G[MAX_L];
extern char HOME_DIR[MAX_L];

int file_type(struct dirent *d);
void print_file_name(struct dirent *d);

void print_file_status(struct dirent *d)
{
    struct stat statbuf;
    int status = stat(d->d_name, &statbuf);

    char months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char symbols[3] = "rwx";
    int permission = 511;

    struct tm *t;
    t = localtime(&statbuf.st_mtime);

    int m = t->tm_mon;     // month number 0 - 11
    int date = t->tm_mday; // date
    int h = t->tm_hour;    // hours
    int mi = t->tm_min;    // mins
    int size = statbuf.st_size;

    if (d->d_type == DT_DIR)
        printf("d");
    else
        printf("-");

    permission = permission & statbuf.st_mode;
    int moder = 256;
    for (int i = 0; i < 9; i++)
    {
        printf("%c", (permission & moder) ? (symbols[i % 3]) : '-');
        moder = moder / 2;
    }

    printf(" %ld ", statbuf.st_nlink);
    printf("%s %s ", getpwuid(statbuf.st_uid)->pw_name, getgrgid(statbuf.st_gid)->gr_name);

    printf("%7d %s %d %d:%d ", size, months[m], date, h, mi);
    print_file_name(d);
}

void print_file_statusn(char *str)
{
    struct stat statbuf;
    int status = stat(str, &statbuf);

    char months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char symbols[3] = "rwx";
    int permission = 511;

    struct tm *t;
    t = localtime(&statbuf.st_mtime);

    int m = t->tm_mon;     // month number 0 - 11
    int date = t->tm_mday; // date
    int h = t->tm_hour;    // hours
    int mi = t->tm_min;    // mins
    int size = statbuf.st_size;

    printf("-");

    permission = permission & statbuf.st_mode;
    int moder = 256;
    for (int i = 0; i < 9; i++)
    {
        printf("%c", (permission & moder) ? (symbols[i % 3]) : '-');
        moder = moder / 2;
    }

    printf(" %ld ", statbuf.st_nlink);
    printf("%s %s ", getpwuid(statbuf.st_uid)->pw_name, getgrgid(statbuf.st_gid)->gr_name);

    printf("%7d %s %d %d:%d %s\n", size, months[m], date, h, mi, str);
}

void lsd(char *s)
{
    struct dirent **d;
    int count = scandir(s, &d, NULL, alphasort);

    for (int i = 0; i < count; i++)
    {
        if (d[i]->d_name[0] != '.')
            print_file_name(d[i]);

        free(d[i]);
    }

    free(d);
}

void ls_ad(char *s)
{
    struct dirent **d;
    int count = scandir(s, &d, NULL, alphasort);

    for (int i = 0; i < count; i++)
    {
        print_file_name(d[i]);
        free(d[i]);
    }

    free(d);
}

void ls_ld(char *s)
{
    struct stat statbuf;
    int total = 0;
    struct dirent **d;
    int count = scandir(s, &d, NULL, alphasort);

    for (int i = 0; i < count; i++)
    {
        if (d[i]->d_name[0] != '.')
        {
            int status = stat(d[i]->d_name, &statbuf);
            total += statbuf.st_blocks;
        }
    }

    printf("total %d\n", total / 2);

    for (int i = 0; i < count; i++)
    {
        if (d[i]->d_name[0] != '.')
            print_file_status(d[i]);

        free(d[i]);
    }

    free(d);
}

void ls_lad(char *s)
{
    struct stat statbuf;
    int total = 0;
    struct dirent **d;
    int count = scandir(s, &d, NULL, alphasort);

    for (int i = 0; i < count; i++)
    {
        int status = stat(d[i]->d_name, &statbuf);
        total += statbuf.st_blocks;
    }

    printf("total %d\n", total / 2);

    for (int i = 0; i < count; i++)
    {
        print_file_status(d[i]);
        free(d[i]);
    }

    free(d);
}

int which_flag(char *s)
{
    if (strstr(s, " -l") != NULL && strstr(s, " -a") != NULL)
        return 3;

    if (strstr(s, " -la") != NULL || strstr(s, " -al") != NULL)
        return 3;

    if (strstr(s, " -a") != NULL)
        return 1;

    if (strstr(s, " -l") != NULL)
        return 2;

    return 0;
}

int ls_function(char *s)
{
    int s_n = 0;
    char temp[MAX_L];
    strcpy(temp, s);

    char *SPLIT = strtok(temp, " ");

    char dirs[MAX_L][MAX_L];

    while (SPLIT != NULL)
    {
        if (strcmp(SPLIT, "-l") != 0 && strcmp(SPLIT, "-a") != 0 && strcmp(SPLIT, "-la") != 0 && strcmp(SPLIT, "-al") != 0 && strcmp(SPLIT, "ls"))
        {

            strcpy(dirs[s_n], SPLIT);
            if (dirs[s_n][0] == '~')
            {
                char temp[MAX_L];
                strcpy(temp, HOME_DIR);
                strcat(temp, &dirs[s_n][1]);
                strcpy(dirs[s_n], temp);
            }
            s_n++;
        }
        SPLIT = strtok(NULL, " ");
    }
    // printf("%d\n", s_n);
    // printf("%d\n", which_flag(s));
    switch (which_flag(s))
    {
    case 0:
        if (s_n == 0)
        {
            lsd(CUR_DIR_G);
            return 1;
        }
        else
        {
            for (int i = 0; i < s_n; i++)
            {
                int fd = open(dirs[i], O_RDONLY);
                DIR *dir = opendir(dirs[i]);

                if (fd < 0)
                {
                    printf("ls: cannot access '%s': No such file or directory\n", dirs[i]);
                    continue;
                }

                if (dir != NULL)
                {
                    if (s_n > 1)
                    {
                        printf("%s:\n", dirs[i]);
                    }
                    closedir(dir);
                    lsd(dirs[i]);
                    continue;
                }

                if (fd >= 0)
                {
                    close(fd);
                    printf("%s\n", dirs[i]);
                    continue;
                }
            }
        }
        break;

    case 1:
        if (s_n == 0)
        {
            ls_ad(CUR_DIR_G);
            return 1;
        }
        else
        {
            for (int i = 0; i < s_n; i++)
            {
                int fd = open(dirs[i], O_RDONLY);
                DIR *dir = opendir(dirs[i]);

                if (fd < 0)
                {
                    printf("ls: cannot access '%s': No such file or directory\n", dirs[i]);
                    continue;
                }

                if (dir != NULL)
                {
                    if (s_n > 1)
                    {
                        printf("%s:\n", dirs[i]);
                    }
                    ls_ad(dirs[i]);
                    closedir(dir);
                    continue;
                }

                if (fd >= 0)
                {
                    close(fd);
                    printf("%s\n", dirs[i]);
                    continue;
                }
            }
        }
        break;

    case 2:
        if (s_n == 0)
        {
            ls_ld(CUR_DIR_G);
            return 1;
        }
        else
        {
            for (int i = 0; i < s_n; i++)
            {
                int fd = open(dirs[i], O_RDONLY);
                DIR *dir = opendir(dirs[i]);

                if (fd < 0)
                {
                    printf("ls: cannot access '%s': No such file or directory\n", dirs[i]);
                    continue;
                }

                if (dir != NULL)
                {
                    if (s_n > 1)
                    {
                        printf("%s:\n", dirs[i]);
                    }
                    ls_ld(dirs[i]);
                    closedir(dir);
                    continue;
                }

                if (fd >= 0)
                {
                    close(fd);
                    print_file_statusn(dirs[i]);
                    continue;
                }
            }
        }
        break;

    case 3:

        if (s_n == 0)
        {
            ls_lad(CUR_DIR_G);
            return 1;
        }
        else
        {
            for (int i = 0; i < s_n; i++)
            {
                int fd = open(dirs[i], O_RDONLY);
                DIR *dir = opendir(dirs[i]);

                if (fd < 0)
                {
                    printf("ls: cannot access '%s': No such file or directory\n", dirs[i]);
                    continue;
                }

                if (dir != NULL)
                {
                    if (s_n > 1)
                    {
                        printf("%s:\n", dirs[i]);
                    }
                    ls_lad(dirs[i]);
                    closedir(dir);
                    continue;
                }

                if (fd >= 0)
                {
                    close(fd);
                    print_file_statusn(dirs[i]);
                    continue;
                }
            }
        }
        break;
    }
}

void print_file_name(struct dirent *d)
{
    if (file_type(d) == 0)
    {
        printf("\e[1;34m%s\e[0m\n", d->d_name);
        return;
    }

    if (file_type(d) == 1)
    {
        printf("%s\n", d->d_name);
        return;
    }

    if (file_type(d) == 2)
    {
        printf("\e[1;32m%s\e[0m\n", d->d_name);
        return;
    }
}

int file_type(struct dirent *d)
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
