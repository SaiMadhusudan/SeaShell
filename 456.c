#include "headers.h"
#include "456.h"

extern char Back_ground[50][MAX_L];
extern int pid_b[50];
extern int current_bg;

char curr[50];
int b, e;
int fork_return;

void add_bg_pid(char *s, int p);

void terminatefg(int signum)
{
    kill(fork_return, SIGINT);
    printf("\n");
    return;
}

void ctrlz(int signum)
{
    kill(fork_return, SIGSTOP);
    add_bg_pid(curr, fork_return);
    printf("\n");
    return;
}

typedef struct job_data
{
    int job_number;
    char status[15];
    char name[20];
    int pid;

} job_d;

void remove_bg456(int pos)
{
    if (pos == 0)
    {
        current_bg--;
        return;
    }
    for (int i = pos; i < current_bg - 1; i++)
    {
        pid_b[i] = pid_b[i + 1];
        strcpy(Back_ground[i], Back_ground[i + 1]);
    }
    current_bg--;
}

int bg_(char *s)
{
    int job_number = atoi(&s[3]) - 1;

    if (kill(pid_b[job_number], SIGCONT) < 0)
    {
        perror("Error");
        return -1;
    }

    return 0;
}

int fg_(char *s)
{
    // signal(SIGINT, terminatefg);
    // signal(SIGTSTP, ctrlz);
    int status;
    int job_number = atoi(&s[3]) - 1;

    setpgid(pid_b[job_number], getpgid(0));

    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

   tcsetpgrp(0, pid_b[job_number]);

    if (kill(pid_b[job_number], SIGCONT) < 0)
        perror("Error");

    remove_bg456(job_number);

    waitpid(pid_b[job_number], &status, WUNTRACED);

    tcsetpgrp(0, getpgid(0));

    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);

    if (WIFSTOPPED(status) || WIFEXITED(status))
        return 0;
}

int sig(char *s)
{

    int job_n;
    int signal_n;
    char Each_String[2][MAX_L];
    int n = 0;

    char *SPLIT = strtok(s, "\t\n ");
    SPLIT = strtok(NULL, "\t\n ");
    while (SPLIT != NULL)
    {
        strcpy(Each_String[n], SPLIT);
        n++;
        SPLIT = strtok(NULL, "\t\n ");
        if (n == 2)
        {
            break;
        }
    }

    job_n = atoi(Each_String[0]) - 1;
    signal_n = atoi(Each_String[1]);

    if (kill(pid_b[job_n], signal_n) < 0)
    {
        perror("Error");
    }
}

int which_flagd456(char *s)
{
    if (strstr(s, " -r") != NULL)
        return 0;

    if (strstr(s, " -s") != NULL)
        return 1;

    return -1;
}

int cmpfunc(const void *a, const void *b)
{
    job_d *A = (job_d *)a;
    job_d *B = (job_d *)b;

    return strcmp(A->name, B->name);
}

int jobs(char *s)
{
    int flag = which_flagd456(s);

    job_d all_jobs[50];

    for (int i = 0; i < current_bg; i++)
    {
        char status;
        char pidnumber[1000] = {'\0'};
        int memory;
        char path[1024];
        int pgrp = -1;
        int tpgid = -1;

        char statfile[10000] = "/proc/";
        sprintf(pidnumber, "%d", pid_b[i]);
        strcat(statfile, pidnumber);
        strcat(statfile, "/stat");

        FILE *file = fopen(statfile, "r");

        if (file)
        {
            fscanf(file, "%*d %*s %c %*d %d %*d %*d %d %*u %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %d",
                   &status, &pgrp, &tpgid,
                   &memory);
        }

        strcpy(all_jobs[i].status, status == 'T' ? "Stopped" : "Running");
        all_jobs[i].job_number = i + 1;
        all_jobs[i].pid = pid_b[i];
        strcpy(all_jobs[i].name, Back_ground[i]);
    }

    qsort(all_jobs, current_bg, sizeof(job_d), cmpfunc);

    for (int i = 0; i < current_bg; i++)
    {
        if (flag == -1)
        {
            printf("[%d] %s %s [%d]\n", all_jobs[i].job_number, all_jobs[i].status,
                   all_jobs[i].name, all_jobs[i].pid);
        }
        else if (flag == 0)
        {
            if (strcmp(all_jobs[i].status, "Running") == 0)
            {
                printf("[%d] %s %s [%d]\n", all_jobs[i].job_number, all_jobs[i].status,
                       all_jobs[i].name, all_jobs[i].pid);
            }
        }
        else if (flag == 1)
        {
            if (strcmp(all_jobs[i].status, "Stopped") == 0)
            {
                printf("[%d] %s %s [%d]\n", all_jobs[i].job_number, all_jobs[i].status,
                       all_jobs[i].name, all_jobs[i].pid);
            }
        }
    }
}

void add_bg_pid(char *s, int p)
{
    pid_b[current_bg] = p;
    strcpy(Back_ground[current_bg], s);
    current_bg++;
}

int exvpfg(char **args, int n_s)
{
    b = time(NULL);
    fork_return = fork();
    int w, wstatus;
    int f = 0;
    strcpy(curr, args[0]);
    signal(SIGINT, terminatefg);
    signal(SIGTSTP, ctrlz);
    if (fork_return == 0)
    {
        f = execvp(args[0], args);
        if (f == -1)
        {
            return -1;
        }
    }
    else
    {
        w = waitpid(fork_return, &wstatus, WUNTRACED);
        e = time(NULL);
        if (e - b > 1)
            printf("it took %d s\n", e - b);
    }
    return 0;
}

int exvpbg(char **args, int n_s)
{
    int fork_return = fork();
    int f;
    int w, wstatus;
    if (fork_return == 0)
    {
        setpgid(0, 0);
        f = execvp(args[0], args);
        if (f == -1)
        {
            exit(-1);
        }
    }
    else
    {
        add_bg_pid(args[0], fork_return);
        w = waitpid(fork_return, &wstatus, WNOHANG);
        printf("[%d]   %d\n", current_bg, fork_return);
    }

    return 0;
}
