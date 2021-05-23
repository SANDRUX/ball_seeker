#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

typedef struct
{
    bool seeker;
    bool has_ball;
    int height;
} player;

int main()
{
    srand(time(NULL));

    int fd;

    if ((fd = shm_open("OBJ_BALL_SEEKER", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) == -1)
    {
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, sizeof (player) * 4) == -1)
    {
        exit(EXIT_FAILURE);
    }

    player * players = mmap(NULL, sizeof(player) * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (players == MAP_FAILED)
    {
        exit(EXIT_FAILURE);
    }

    int * lost = (int *)malloc(4 * sizeof(int));

    for (int i = 0; i < 4; i++)
    {
        players[i].height = rand() % ((190 - 160 + 1)) + 160;
        players[i].seeker = 0;
        players[i].has_ball = 0;
    }

    players[3].has_ball = 1;
    players[2].seeker = 1;
    lost[2] += 1;

    while (1)
    {
        for (int i = 0; i < 4; i++)
        {
            if (lost[i] == 5)
            {
                //game finished
                return 0;
            }
        }

        for (int i = 0; i < 4; i++)
        {
            if (players[i].has_ball == 1)
            {
                int jump = (rand() % 41) + 10;
                int distance = players[i].height + jump;
                int chosen_player = rand() % 3;
                players[i].has_ball = 0;

                bool status = 0;

                for (int j = 0; j < 4; j++)
                {
                    if (chosen_player == j && players[j].seeker == 1)
                    {
                        players[i].seeker = 1;
                        lost[i] += 1;
                        players[j].seeker = 0;
                        players[j].has_ball = 1;

                        status = 1;
                    }
                }

                bool status2 = 0;

                if (status != 1)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        if (players[j].seeker == 1)
                        {
                            if ((players[j].height + 50) > distance)
                            {
                                players[j].has_ball = 1;
                                players[j].seeker = 0;
                                players[i].seeker = 1;
                                lost[i] += 1;

                                status = 1;
                            }
                        }
                    }
                }

                if (status2 != 1)
                {
                    players[chosen_player].has_ball = 1;
                }
            }
        }

    }

    return 0;
}
