#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <syscall.h>
#include <stdbool.h>
#include <time.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define clear() printf("\033[H\033[J")
#define MAX_L 500