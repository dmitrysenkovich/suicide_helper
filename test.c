#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>


#define BUDDIES_COUNT 32


/*
 * "Still alive" is printed if suddenly
 * our buddy is alive after suicide.
 */
void make_suicide(void)
{
    int fd = open("/dev/suicide", O_RDWR);
    if (fd < 0)
        printf("Buddy %d: %s\n", (int)getpid(), strerror(errno));

    puts("Still alive");
    exit(0);
}


int main()
{
    pid_t buddies[BUDDIES_COUNT];
    for (int i = 0; i < BUDDIES_COUNT; i++)
    {
        /* don't want to burden code with error code checking */
        buddies[i] = fork();
        if (buddies[i] != 0)
            continue;
        else
        {
            printf("I am buddy %d, parent %d\n", (int)getpid(), (int)getppid());
            make_suicide();
        }
    }

    for (int i = 0; i < BUDDIES_COUNT; i++)
        waitpid(buddies[i], NULL, 0);

    puts("All buddies are done!");

    printf("And me %d is trying to suicide..\n", (int)getpid());
    make_suicide();
    return 0;
}
