#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

#define BLACK_PAIR 1
#define WHITE_PAIR 2
#define YELLOW_PAIR 3

bool quite = false;

typedef struct Element {
	struct Element *pNext;
	
	int y, x;
} element;

element *head, *tail;
char dir = 2;

struct timeval curr, prev;
int interval = 350;

element *last() {
	element *temp = head;
	while (temp->pNext) {
		temp = temp->pNext;
	}
	return temp;
}

element *penultimate() {
	element *temp = head;
	while (temp->pNext->pNext) {
		temp = temp->pNext;
	}
	return temp;
}

bool checkpos (element* elem) {
	char ch = mvinch (elem->y, elem->x) & A_CHARTEXT;
	if (ch == '!' || ch == '>' || ch == '<' || ch == '=' || ch == '*'
		|| ch == '/' || ch == '\\') return false;
	ch = mvinch (elem->y, elem->x+1) & A_CHARTEXT;
	if (ch == '!' || ch == '>' || ch == '<' || ch == '=' || ch == '*'
		|| ch == '/' || ch == '\\') return false;
	return true;
}

void spawnBerry () {
	int x, y;
	element *temp;
_start:
	y = rand()%20 + 1;
	x = 2*(rand()%30) + 2;

	temp = head;

	while (temp) {
		if (y == temp->y && x == temp->x) {
			goto _start;
		}
		temp = temp->pNext;
	}

	mvprintw (y, x, "@@");
}

void drawSnake () {
	element *pElement = head;
	for (int i = 0; pElement; i++) {
		if (i%2 == 0) attron (COLOR_PAIR(YELLOW_PAIR));
		else attron (COLOR_PAIR(WHITE_PAIR));
		mvprintw (pElement->y, pElement->x, "**");
		pElement = pElement->pNext;
	}
	attron (COLOR_PAIR(BLACK_PAIR));
}

void init () {
	initscr();
	noecho();
	raw();
	curs_set(0);

	if (!has_colors()) {
		endwin();
		exit (1);
	}

	start_color();
	init_pair (WHITE_PAIR, COLOR_WHITE, COLOR_WHITE);
	init_pair (YELLOW_PAIR, COLOR_YELLOW, COLOR_YELLOW);
	init_pair (BLACK_PAIR, COLOR_WHITE, COLOR_BLACK);

	attron (COLOR_PAIR(BLACK_PAIR));

	for (int i = 2; i < 62; i++) mvaddch (0, i, '=');
	for (int i = 0; i < 22; i++) {
		mvaddch (i, 0, '<');
		mvaddch (i, 1, '!');
		mvaddch (i, 62, '!');
		mvaddch (i, 63, '>');
	}
	for (int i = 2; i < 62; i++) mvaddch (21, i, '=');
	for (int i = 1; i < 21; i++)
		for (int j = 2; j < 62; j += 2)
			mvaddch (i, j, '.');


	head = malloc (sizeof(element));
	head->y = 10;
	head->x = 28;
	
	head->pNext = malloc (sizeof(element));
	head->pNext->y = 10;
	head->pNext->x = 30;

	head->pNext->pNext = malloc (sizeof(element));
	head->pNext->pNext->y = 10;
	head->pNext->pNext->x = 32;

	head->pNext->pNext->pNext = malloc (sizeof(element));
	head->pNext->pNext->pNext->y = 10;
	head->pNext->pNext->pNext->x = 34;

	tail = head->pNext->pNext->pNext;
	tail->pNext = NULL;

	drawSnake();
	spawnBerry();
	gettimeofday (&prev, NULL);
	srand (prev.tv_sec);
	refresh();
}

void run () {
	char ch = getch();
	switch (ch) {
		case 'd':
			dir = 0;
			break;
		case 'w':
			dir = 1;
			break;
		case 'a':
			dir = 2;
			break;
		case 's':
			dir = 3;
			break;
		case 'x':
			quite = true;
	}

	gettimeofday (&curr, NULL);
	if ((curr.tv_sec - prev.tv_sec)*1000 + (curr.tv_usec - prev.tv_usec)/1000 >= interval) {
		element *newElement = malloc (sizeof(element));
		element *pen = penultimate();
		element *lst = last();
		pen->pNext = NULL;
		switch (dir) {
			case 0:
				newElement->y = head->y;
				newElement->x = head->x+2;
				break;
			case 1:
				newElement->y = head->y-1;
				newElement->x = head->x;
				break;
			case 2:
				newElement->y = head->y;
				newElement->x = head->x-2;
				break;
			case 3:
				newElement->y = head->y+1;
				newElement->x = head->x;
		}
		if (!checkpos(newElement))
			quite = true;
		char ch = mvinch (newElement->y, newElement->x) & A_CHARTEXT;
		if (ch == '@') {
			pen->pNext = lst;
			spawnBerry();
		}
		else {
			mvprintw (lst->y, lst->x, ". ");
			free(lst);
		}
		newElement->pNext = head;
		head = newElement;
		drawSnake();
		prev = curr;
	}
}

int main () {
	init();
	getch();
	nodelay (stdscr, true);
	while (!quite)
		run();
	curs_set (1);
	nodelay (stdscr, false);
	endwin();
	system ("stty sane");
	return 0;
}
