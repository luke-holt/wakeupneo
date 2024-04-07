#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

enum { WHITE=1, GREEN=2 };

static void wakeupneo(void);
static void shiftlines(void);
static void putlines(void);
static void clearlines(void);
static void run(void);
static char randch(void);

WINDOW *stdscr;
static int width, height;
static char *lines;
static int *threadlen;

int
main(int argc, char *argv[])
{	
	stdscr = initscr();
    curs_set(0);
    getmaxyx(stdscr, height, width);
    start_color();
    init_pair(WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);

    lines = ecalloc(height + 2, width);
    threadlen = ecalloc(width, sizeof(*threadlen));
    clearlines(); // set to ' '
    run();
    endwin();
    free(lines);
    free(threadlen);
    curs_set(1);
	return 0;
}

void
run(void)
{
    wakeupneo();
    for (;;) {
        bool newthread = (rand());
        if (newthread) {
            int len = rand() % height;
            int pos = rand() % width;
            threadlen[pos] += len;
        }
        for (int i = 0; i < width; i++) {
            if (threadlen[i] == 0) continue;
            threadlen[i]--;
            lines[i] = randch();
        }
        shiftlines();
        putlines();
        refresh();
        usleep(100000);
    }
}

char
randch(void)
{
    char *printable = "!\"#$$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    return printable[rand() % (strlen(printable)-1)];
}

void
shiftlines(void)
{
    for (int i = height; i > 0; i--)
        strncpy(&lines[i*width], &lines[(i-1)*width], width);
    memset(lines, ' ', width);
}

void
putlines(void)
{
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++) {
            int a;
            char *l;
            l = &lines[(i+1)*width+j];
            a = (' ' == l[width]) ? WHITE : GREEN; // char in next row is ' '
            attron(COLOR_PAIR(a));
            mvaddch(i, j, *l);
            attroff(COLOR_PAIR(a));
        }
}

void
clearlines(void)
{
    memset(lines, ' ', height*width);
}

void
wakeupneo(void)
{
    char msg[] = "Wake up Neo...";
    attron(COLOR_PAIR(GREEN));
    move(1, 2);
    for (int i = 0; i < strlen(msg); i++) {
        addch(msg[i]);
        refresh();
        usleep(150000);
    }
    sleep(1);
    attroff(COLOR_PAIR(GREEN));
}

