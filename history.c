#include "headers.h"
#include "history.h"

extern char Prev_dir[MAX_L];
extern char USER_G[MAX_L];
extern char SYSTEM_G[MAX_L];
extern char CUR_DIR_G[MAX_L];
extern char HOME_DIR[MAX_L];

extern int change;

extern int current;

extern char History[20][MAX_L];

void read_history_()
{
    FILE *fp;
    fp = fopen(".history.txt", "r+"); // opening file
    fscanf(fp, "%d", &current);

    for (int i = 0; i < current; i++)
    {
        fscanf(fp, "%s", History[i]);
    }

    fclose(fp);
}

void add_history_(char *s)
{
    if (strcmp(s, History[current - 1]) != 0)
    {
        if (current < 20)
        {
            strcpy(History[current], s);
            current++;
            return;
        }
        for (int i = 0; i < 19; i++)
        {
            strcpy(History[i], History[i + 1]);
        }

        strcpy(History[19], s);
        return;
    }
}

void print_history_()
{
    if (current <= 10)
    {
        for (int i = 0; i < current; i++)
            printf("%s\n", History[i]);
    }
    else
    {
        for (int i = current - 10; i < current; i++)
            printf("%s\n", History[i]);
    }
}

void update_history_()
{
    FILE *fp;
    fp = fopen(".history.txt", "w+"); // opening file

    // printf("printing to file\n");
    fprintf(fp, "%d\n", current);

    for (int i = 0; i < current; i++)
    {
        fprintf(fp, "%s\n", History[i]);
    }
    fclose(fp);
    return;
}