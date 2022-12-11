#include "headers.h"
#include "pwd.h"
#include "cd.h"
#include "ls.h"
#include "history.h"
#include "discover.h"
#include "pinfo.h"
#include "456.h"

void die(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

char Back_ground[50][MAX_L];
int pid_b[50];
int current_bg;

char Prev_dir[MAX_L];
char USER_G[MAX_L];
char SYSTEM_G[MAX_L];
char CUR_DIR_G[MAX_L];
char HOME_DIR[MAX_L];
int current;
int change;

int stdin_;
int stdout_;

int ifd, ofd;

char History[20][MAX_L];

void pipe_handler(char COMMAND_L[MAX_L]);

void init_shell()
{
    clear();
}

int pipe_flag(char string[MAX_L])
{
    for (int i = 0; i < strlen(string); i++)
    {
        if (string[i] == '|')
        {
            return (1);
        }
    }
    return (0);
}

void remove_bg(int pos)
{
    for (int i = pos; i < current_bg - 1; i++)
    {
        pid_b[i] = pid_b[i + 1];
        strcpy(Back_ground[i], Back_ground[i + 1]);
    }
    current_bg--;
}

void reset_io()
{
    if (ifd)
    {
        close(ifd);
        dup2(stdin_, 0);
        close(stdin_);
    }
    if (ofd)
    {
        close(ofd);
        dup2(stdout_, 1);
        close(stdout_);
    }
}

void status_bg()
{
    int wstatus;
    int pid = waitpid(-1, &wstatus, WNOHANG);

    if (pid > 0)
    {
        int pos = 0;
        while (pid_b[pos] != pid)
            pos++;

        if (WIFSTOPPED(wstatus))
            return;

        fprintf(stderr, "\n%s with PID %d exited ", Back_ground[pos], pid_b[pos]);

        if (WIFEXITED(wstatus))
            fprintf(stderr, "normally\n");
        else
            fprintf(stderr, "abnormally\n");

        remove_bg(pos);
    }
}

int which_command(char string[MAX_L])
{
    if (strcmp("echo", string) == 0)
        return 0;

    if (strcmp("cd", string) == 0)
        return 1;

    if (strcmp("pwd", string) == 0)
        return 2;

    if (strcmp("clear", string) == 0)
        return 3;

    if (strcmp(";", string) == 0)
        return 4;

    if (strcmp("ls", string) == 0)
        return 5;

    if (strcmp("history", string) == 0)
        return 6;

    if (strcmp("discover", string) == 0)
        return 7;

    if (strcmp("&", string) == 0)
        return 8;

    if (strcmp("pinfo", string) == 0)
        return 9;

    if (strstr(string, "|") != NULL)
        return 10;

    if (strcmp("jobs", string) == 0)
        return 11;

    if (strcmp("bg", string) == 0)
        return 12;

    if (strcmp("fg", string) == 0)
        return 13;

    if (strcmp("sig", string) == 0)
        return 14;

    return MAX_L;
}

int IO_redirection(char *final)
{

    char ALL_COMMANDS[20][MAX_L];

    int no_of_s = 0;

    char *SPLIT = strtok(final, "\t\n ");

    char Each_String[MAX_L][MAX_L];

    while (SPLIT != NULL)
    {
        strcpy(Each_String[no_of_s], SPLIT);
        no_of_s++;

        SPLIT = strtok(NULL, "\t\n ");
    }

    strcpy(final, "\0");
    int i = 0;
    while (i < no_of_s && Each_String[i][0] != '<' && Each_String[i][0] != '>')
    {
        strcat(final, Each_String[i]);
        strcat(final, " ");
        i++;
    }

    for (i = 0; i < no_of_s; i++)
    {
        if (strcmp(Each_String[i], "<") == 0)
        {
            if (i == no_of_s - 1)
            {
                printf("Error");
                return -1;
            }

            ifd = open(Each_String[i + 1], O_RDONLY);
        }
        else if (strcmp(Each_String[i], ">>") == 0)
        {
            if (i == no_of_s - 1)
            {
                printf("Error");
                return -1;
            }

            ofd = open(Each_String[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
        }
        else if (strcmp(Each_String[i], ">") == 0)
        {
            if (i == no_of_s - 1)
            {
                printf("Error");
                return -1;
            }

            ofd = open(Each_String[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
    }

    if (ifd < 0 || ofd < 0)
    {
        perror("Error");
        return -1;
    }

    if (ifd)
    {
        stdin_ = dup(0);
        dup2(ifd, 0);
    }
    if (ofd)
    {
        stdout_ = dup(1);
        dup2(ofd, 1);
    }
}

void command_handler(char COMMAND_L[MAX_L])
{

    char *retsemi = strstr(COMMAND_L, ";;");
    char *retand = strstr(COMMAND_L, "&&");
    if (retsemi != NULL || COMMAND_L[0] == ';')
    {
        printf("bash: syntax error near unexpected token ; \n");
        return;
    }

    if (COMMAND_L[0] == '&' || retand != NULL)
    {
        printf("bash: syntax error near unexpected token & \n");
        return;
    }

    //////////////////////////
    char Temp_COMMAND[MAX_L] = {'\0'};
    strcpy(Temp_COMMAND, COMMAND_L);
    int t_lenght = 0;

    for (int i = 0; i < strlen(COMMAND_L); i++)
    {
        if (COMMAND_L[i] != ';' && COMMAND_L[i] != '&')
        {
            Temp_COMMAND[t_lenght] = COMMAND_L[i];
            t_lenght++;
        }
        else if (COMMAND_L[i] == ';')
        {
            Temp_COMMAND[t_lenght] = ' ';
            t_lenght++;
            Temp_COMMAND[t_lenght] = ';';
            t_lenght++;
            Temp_COMMAND[t_lenght] = ' ';
            t_lenght++;
        }
        else if (COMMAND_L[i] == '&')
        {
            Temp_COMMAND[t_lenght] = ' ';
            t_lenght++;
            Temp_COMMAND[t_lenght] = '&';
            t_lenght++;
            Temp_COMMAND[t_lenght] = ' ';
            t_lenght++;
        }
    }

    if (Temp_COMMAND[t_lenght - 1] == ' ')
        Temp_COMMAND[t_lenght - 1] = '\0';

    Temp_COMMAND[t_lenght] = '\0';
    strcpy(COMMAND_L, Temp_COMMAND);

    /////////////////////////

    char ALL_COMMANDS[20][MAX_L];

    int no_of_s = 0;

    char *SPLIT = strtok(COMMAND_L, "\t\n ");

    char Each_String[MAX_L][MAX_L];

    while (SPLIT != NULL)
    {
        strcpy(Each_String[no_of_s], SPLIT);
        no_of_s++;

        SPLIT = strtok(NULL, "\t\n ");
    }

    for (int i = 0; i < no_of_s - 1; i++)
    {
        if (strcmp(Each_String[i], Each_String[i + 1]) == 0 && strcmp(Each_String[i], ";") == 0)
        {
            printf("bash: syntax error near unexpected token ; \n");
            return;
        }
    }

    for (int i = 0; i < no_of_s - 1; i++)
    {
        if (strcmp(Each_String[i], Each_String[i + 1]) == 0 && strcmp(Each_String[i], "&") == 0)
        {
            printf("bash: syntax error near unexpected token & \n");
            return;
        }
    }

    for (int i = 0; i < no_of_s - 1; i++)
    {
        if (strcmp(Each_String[i + 1], ";") == 0 && strcmp(Each_String[i], "&") == 0)
        {
            printf("bash: syntax error near unexpected token ; \n");
            return;
        }
    }

    for (int i = 0; i < no_of_s - 1; i++)
    {
        if (strcmp(Each_String[i + 1], "&") == 0 && strcmp(Each_String[i], ";") == 0)
        {
            printf("bash: syntax error near unexpected token & \n");
            return;
        }
    }

    char final[MAX_L];
    char final2[MAX_L];
    int restorei;
    for (int i = 0; i < no_of_s; i++)
    {
        restorei = i;
        strcpy(final, "\0");
        while (i < no_of_s && (Each_String[i][0] != ';' && Each_String[i][0] != '&'))
        {
            strcat(final, Each_String[i]);
            strcat(final, " ");
            i++;
        }

        if (which_command(final) == 10)
        {
            if (IO_redirection(final) == -1)
                reset_io();
            pipe_handler(final);
            reset_io();
        }
        else
        {
            i = restorei;
            switch (which_command(Each_String[i]))
            {
            case 0:
                strcpy(final, "\0");
                i++;
                while (i < no_of_s && (Each_String[i][0] != ';' && Each_String[i][0] != '&'))
                {
                    strcat(final, Each_String[i]);
                    strcat(final, " ");
                    i++;
                }
                if (IO_redirection(final) == -1)
                {
                    reset_io();
                    continue;
                }
                printf("%s\n", final);
                break;
            case 1:
                strcpy(final, "\0");
                while (i < no_of_s && (Each_String[i][0] != ';' && Each_String[i][0] != '&'))
                {
                    strcat(final, Each_String[i]);
                    strcat(final, " ");
                    i++;
                }
                if (IO_redirection(final) == -1)
                {
                    reset_io();
                    continue;
                }
                cd_function_new(final);
                break;
            case 2:
                strcpy(final, "\0");
                strcat(final2, Each_String[i]);
                i++;
                while (i < no_of_s && (Each_String[i][0] != ';' && Each_String[i][0] != '&'))
                {
                    strcat(final, Each_String[i]);
                    strcat(final, " ");
                    strcat(final2, Each_String[i]);
                    strcat(final2, " ");
                    i++;
                }
                if (IO_redirection(final2) == -1)
                {
                    reset_io();
                    continue;
                }
                pwd_function();
                break;
            case 3:
                strcpy(final, "\0");
                strcat(final2, Each_String[i]);
                i++;
                while (i < no_of_s && (Each_String[i][0] != ';' && Each_String[i][0] != '&'))
                {
                    strcat(final, Each_String[i]);
                    strcat(final, " ");
                    strcat(final2, Each_String[i]);
                    strcat(final2, " ");
                    i++;
                }
                if (IO_redirection(final2) == -1)
                {
                    reset_io();
                    continue;
                }
                init_shell();
                break;
            case 4:
                break;
            case 5:
                strcpy(final, "\0");
                while (i < no_of_s && (Each_String[i][0] != ';' && Each_String[i][0] != '&'))
                {
                    strcat(final, Each_String[i]);
                    strcat(final, " ");
                    i++;
                }
                if (IO_redirection(final) == -1)
                {
                    reset_io();
                    continue;
                }
                ls_function(final);
                break;
            case 6:
                strcpy(final, "\0");
                while (i < no_of_s && (Each_String[i][0] != ';' && Each_String[i][0] != '&'))
                {
                    strcat(final, Each_String[i]);
                    strcat(final, " ");
                    i++;
                }
                if (IO_redirection(final) == -1)
                {
                    reset_io();
                    continue;
                }
                print_history_();
                break;
            case 7:
                strcpy(final, "\0");
                int discover_args = 0;
                while (i < no_of_s && (Each_String[i][0] != ';' && Each_String[i][0] != '&'))
                {
                    strcat(final, Each_String[i]);
                    strcat(final, " ");
                    i++;
                    discover_args++;
                }
                if (IO_redirection(final) == -1)
                {
                    reset_io();
                    continue;
                }
                if (discover_args > 4)
                    printf("discover: too many arguments\n");
                else
                    discover_f(final);
            case 8:
                break;
            case 9:
                strcpy(final, "\0");
                while (i < no_of_s && (Each_String[i][0] != ';' && Each_String[i][0] != '&'))
                {
                    strcat(final, Each_String[i]);
                    strcat(final, " ");
                    i++;
                }
                if (IO_redirection(final) == -1)
                {
                    reset_io();
                    continue;
                }
                pinfo_f(final);
                break;
            case 10:
                // pipehandle
            case 11:
                strcpy(final, "\0");
                i++;
                while (i < no_of_s && (Each_String[i][0] != ';' && Each_String[i][0] != '&'))
                {
                    strcat(final, Each_String[i]);
                    strcat(final, " ");
                    i++;
                }
                if (IO_redirection(final) == -1)
                {
                    reset_io();
                    continue;
                }
                jobs(final);
                break;
            case 12:
                strcpy(final, "\0");
                while (i < no_of_s && (Each_String[i][0] != ';' && Each_String[i][0] != '&'))
                {
                    strcat(final, Each_String[i]);
                    strcat(final, " ");
                    i++;
                }
                if (IO_redirection(final) == -1)
                {
                    reset_io();
                    continue;
                }
                bg_(final);
                break;

            case 13:
                strcpy(final, "\0");
                while (i < no_of_s && (Each_String[i][0] != ';' && Each_String[i][0] != '&'))
                {
                    strcat(final, Each_String[i]);
                    strcat(final, " ");
                    i++;
                }
                if (IO_redirection(final) == -1)
                {
                    reset_io();
                    continue;
                }
                fg_(final);
                break;

            case 14:
                strcpy(final, "\0");
                while (i < no_of_s && (Each_String[i][0] != ';' && Each_String[i][0] != '&'))
                {
                    strcat(final, Each_String[i]);
                    strcat(final, " ");
                    i++;
                }
                if (IO_redirection(final) == -1)
                {
                    reset_io();
                    continue;
                }
                sig(final);
                break;

            case MAX_L:
                strcpy(final, "\0");
                char **args;
                args = malloc(no_of_s);
                int j = 0;
                while (i < no_of_s && Each_String[i][0] != ';' && Each_String[i][0] != '&')
                {
                    strcat(final, Each_String[i]);
                    strcat(final, " ");
                    i++;
                }

                if (IO_redirection(final) == -1)
                {
                    reset_io();
                    continue;
                }

                char *split = strtok(final, "\t\n ");

                while (split != NULL)
                {
                    args[j] = strdup(split);
                    j++;
                    split = strtok(NULL, "\t\n ");
                }
                args[j] = NULL;

                int f;

                if (i < no_of_s && Each_String[i][0] == '&')
                    f = exvpbg(args, j);
                else
                    f = exvpfg(args, j);

                for (int ij = 0; ij < j; ij++)
                    free(args[ij]);

                free(args);

                if (f < 0)
                    printf("command not found\n");
                break;
            }
        }
        reset_io();
    }
}

void pipe_handler(char COMMAND_L[MAX_L])
{
    int Stdin_;
    int Stdout_;

    int no_of_s = 0;
    int no_of_p = 0;

    char *SPLIT = strtok(COMMAND_L, "\t\n ");

    char Each_String[MAX_L][MAX_L];

    while (SPLIT != NULL)
    {
        strcpy(Each_String[no_of_s], SPLIT);
        no_of_s++;
        SPLIT = strtok(NULL, "\t\n ");
    }

    for (int i = 0; i < no_of_s; i++)
    {
        if (strcmp(Each_String[i], "|") == 0)
        {
            no_of_p++;
        }
    }

    int fds[no_of_p][2];
    int errorcode;

    for (int i = 0; i < no_of_p; i++)
    {
        if (pipe(fds[i]))
        {
            printf("Unable to pipe\n");
            errorcode = -1;
            return;
        }
    }

    char final[MAX_L];
    char final2[MAX_L];

    int pipecount = 0;
    int pipefork;

    Stdin_ = dup(0);
    Stdout_ = dup(1);

    for (int i = 0; i < no_of_s; i++)
    {

        if (pipecount < no_of_p)
        {
            dup2(fds[pipecount][1], 1);
        }
        else
        {
            dup2(Stdout_, STDOUT_FILENO);
        }
        if (pipecount > 0)
        {
            close(fds[pipecount - 1][1]);
            dup2(fds[pipecount - 1][0], 0);
        }

        switch (which_command(Each_String[i]))
        {
        case 0:
            strcpy(final, "\0");
            strcpy(final2, Each_String[i]);
            i++;
            while (i < no_of_s && (Each_String[i][0] != '|' && Each_String[i][0] != '&'))
            {
                strcat(final, Each_String[i]);
                strcat(final, " ");
                strcat(final2, Each_String[i]);
                strcat(final2, " ");
                i++;
            }

            printf("%s\n", final);
            break;
        case 1:
            strcpy(final, "\0");
            while (i < no_of_s && (Each_String[i][0] != '|' && Each_String[i][0] != '&'))
            {
                strcat(final, Each_String[i]);
                strcat(final, " ");
                i++;
            }

            cd_function_new(final);
            break;
        case 2:
            strcpy(final, "\0");
            strcat(final2, Each_String[i]);
            i++;
            while (i < no_of_s && (Each_String[i][0] != '|' && Each_String[i][0] != '&'))
            {
                strcat(final, Each_String[i]);
                strcat(final, " ");
                strcat(final2, Each_String[i]);
                strcat(final2, " ");
                i++;
            }

            pwd_function();
            break;
        case 3:
            strcpy(final, "\0");
            strcat(final2, Each_String[i]);
            i++;
            while (i < no_of_s && (Each_String[i][0] != '|' && Each_String[i][0] != '&'))
            {
                strcat(final, Each_String[i]);
                strcat(final, " ");
                strcat(final2, Each_String[i]);
                strcat(final2, " ");
                i++;
            }

            init_shell();
            break;
        case 4:
            break;
        case 5:
            strcpy(final, "\0");
            while (i < no_of_s && (Each_String[i][0] != '|' && Each_String[i][0] != '&'))
            {
                strcat(final, Each_String[i]);
                strcat(final, " ");
                i++;
            }

            ls_function(final);
            break;
        case 6:
            strcpy(final, "\0");
            while (i < no_of_s && (Each_String[i][0] != '|' && Each_String[i][0] != '&'))
            {
                strcat(final, Each_String[i]);
                strcat(final, " ");
                i++;
            }

            print_history_();
            break;
        case 7:
            strcpy(final, "\0");
            int discover_args = 0;
            while (i < no_of_s && (Each_String[i][0] != '|' && Each_String[i][0] != '&'))
            {
                strcat(final, Each_String[i]);
                strcat(final, " ");
                i++;
                discover_args++;
            }

            if (discover_args > 4)
                printf("discover: too many arguments\n");
            else
                discover_f(final);
        case 8:
            break;
        case 9:
            strcpy(final, "\0");
            while (i < no_of_s && (Each_String[i][0] != '|' && Each_String[i][0] != '&'))
            {
                strcat(final, Each_String[i]);
                strcat(final, " ");
                i++;
            }

            pinfo_f(final);
            break;

        case 11:
            strcpy(final, "\0");
            i++;
            while (i < no_of_s && (Each_String[i][0] != '|' && Each_String[i][0] != '&'))
            {
                strcat(final, Each_String[i]);
                strcat(final, " ");
                i++;
            }
            jobs(final);
            break;
        case 12:
            strcpy(final, "\0");
            while (i < no_of_s && (Each_String[i][0] != '|' && Each_String[i][0] != '&'))
            {
                strcat(final, Each_String[i]);
                strcat(final, " ");
                i++;
            }
            bg_(final);
            break;

        case 13:
            strcpy(final, "\0");
            while (i < no_of_s && (Each_String[i][0] != '|' && Each_String[i][0] != '&'))
            {
                strcat(final, Each_String[i]);
                strcat(final, " ");
                i++;
            }
            fg_(final);
            break;

        case 14:
            strcpy(final, "\0");
            while (i < no_of_s && (Each_String[i][0] != '|' && Each_String[i][0] != '&'))
            {
                strcat(final, Each_String[i]);
                strcat(final, " ");
                i++;
            }

            sig(final);
            break;

        case MAX_L:
            strcpy(final, "\0");
            char **args;
            args = malloc(no_of_s);
            int j = 0;
            while (i < no_of_s && Each_String[i][0] != '|' && Each_String[i][0] != '&')
            {
                strcat(final, Each_String[i]);
                strcat(final, " ");
                i++;
            }

            char *split = strtok(final, "\t\n ");

            while (split != NULL)
            {
                args[j] = strdup(split);
                j++;
                split = strtok(NULL, "\t\n ");
            }
            args[j] = NULL;

            int f;

            if (i < no_of_s && Each_String[i][0] == '&')
                f = exvpbg(args, j);
            else
                f = exvpfg(args, j);

            for (int ij = 0; ij < j; ij++)
                free(args[ij]);

            free(args);

            if (f < 0)
                printf("command not found\n");
            break;
        }

        if (pipecount > 0)
        {
            close(fds[pipecount - 1][0]);
        }
        close(fds[pipecount][1]);
        pipecount++;
    }

    dup2(Stdin_, STDIN_FILENO);
}

int main()
{
    init_shell();
    change = 0;
    current_bg = 0;

    dir_status(USER_G, SYSTEM_G, HOME_DIR);
    strcpy(Prev_dir, HOME_DIR);

    char *COMMAND_LIST[MAX_L];

    int i = -1;

    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    char *COMMAND_L = calloc(1, MAX_L);

    read_history_();
    char *inp = malloc(sizeof(char) * 500);
    char *out = malloc(sizeof(char) * 500);
    char c;

    while (1)
    {
        dir_status(USER_G, SYSTEM_G, CUR_DIR_G);
        print_(USER_G, SYSTEM_G);

        if (fgets(COMMAND_L, MAX_L, stdin) == NULL)
        {
            init_shell();
            exit(0);
        }
        COMMAND_L[strlen(COMMAND_L) - 1] = '\0';
        i++;
        if (COMMAND_L[0] != '\0')
        {
            add_history_(COMMAND_L);
            command_handler(COMMAND_L);
            update_history_();
        }
        signal(SIGCHLD, status_bg);
    }

    // while (1)
    // {
    //     dir_status(USER_G, SYSTEM_G, CUR_DIR_G);
    //     print_(USER_G, SYSTEM_G);

    //     setbuf(stdout, NULL);
    //     enableRawMode();
    //     memset(inp, '\0', 100);
    //     memset(out, '\0', 100);
    //     int pt = 0;
    //     while (read(STDIN_FILENO, &c, 1) == 1)
    //     {

    //         if (iscntrl(c))
    //         {
    //             if (c == 10)
    //             {
    //                 inp[pt] = '\0';
    //                 break;
    //             }
    //             else if (c == 27)
    //             {
    //                 char buf[3];
    //                 buf[2] = 0;
    //                 if (read(STDIN_FILENO, buf, 2) == 2)
    //                 { // length of escape code
    //                     printf("\rarrow key: %s", buf);
    //                 }
    //             }
    //             else if (c == 127)
    //             { // backspace
    //                 if (pt > 0)
    //                 {
    //                     if (inp[pt - 1] == 9)
    //                     {
    //                         for (int i = 0; i < 7; i++)
    //                         {
    //                             printf("\b");
    //                         }
    //                     }
    //                     inp[--pt] = '\0';
    //                     printf("\b \b");
    //                 }
    //             }
    //             else if (c == 9)
    //             {
    //                 auto_complete(inp, out);
    //                 strcpy(inp, out);
    //                 pt = strlen(out);
    //                 print_(USER_G, SYSTEM_G);
    //                 printf("%s", inp);
    //             }
    //             else if (c == 4)
    //             {
    //                 printf("\n");
    //                 exit(0);
    //             }
    //             else
    //             {
    //                 printf("%d\n", c);
    //             }
    //         }
    //         else
    //         {
    //             inp[pt++] = c;
    //             printf("%c", c);
    //         }
    //     }
    //     disableRawMode();
    //     strcpy(COMMAND_L, inp);
    //     //  printf("%s\n", inp);
    //     // COMMAND_L[strlen(COMMAND_L) - 1] = '\0';
    //     i++;
    //     if (COMMAND_L[0] != '\0')
    //     {
    //         add_history_(COMMAND_L);
    //         command_handler(COMMAND_L);
    //         update_history_();
    //     }
    //     printf("\n");
    //     signal(SIGCHLD, status_bg);
    // }

    return (0);
}