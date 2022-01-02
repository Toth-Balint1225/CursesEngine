#ifndef ENGINE_H
#define ENGINE_H

#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/curses.h>

#include <locale.h>

#include <iostream>
#include <chrono>
#include <string>


/* settings:
 * CBREAK | RAW | ECHO | NOECHO | CURSOR | NOCURSOR | RESIZE | DEBUG | FULLSCREEN | KEYPAD
 */

namespace tui {

#define CBREAK     0x001
#define RAW        0x002
#define ECHO       0x004
#define NOECHO     0x008
#define CURSOR     0x010
#define NOCURSOR   0x020
#define RESIZE     0x040
#define DEBUG      0x080
#define FULLSCREEN 0x100
#define KEYPAD	   0x200
#define POLL_KEYS  0x400

/* The engine handles the creation and operation */
class Engine 
{
private:
	int boundX, boundY;
	int winPosX, winPosY;
	bool fullScreen;
protected:
	int height,width;
	WINDOW* win;
public:
	Engine();
	void createWindow(int width,int height);
	void start(uint32_t mode);
protected:
	virtual bool onCreate() = 0;
	virtual bool onUpdate(float dt) = 0;
	virtual bool onDestroy();
	virtual void onKeypress(wint_t c);


	void clear(wchar_t c = L' ');

	void putChar(int posX, int posY, wchar_t c);
	void putChar(float posX, float posY, wchar_t c);
	void putString(int posX, int posY, const std::wstring& str);

	void line(int startX, int startY, int endX, int endY, wchar_t c = L'#');

	void rect(int startX, int startY, int lengthX, int lengthY, wchar_t c = L'#');
	void rectFill(int startX, int startY, int lengthX, int lengthY, wchar_t c = L'#');

	void circle(int originX, int originY, int radius, wchar_t c = L'#');
	void circleFill(int originX, int originY, int radius, wchar_t c = L'#');

private:
	void syncWindow();
	void pollKey();
};

}

#endif //ENGINE_H

#ifdef ENGINE_IMPL
#ifndef _ENGINE_IMPL
#define _ENGINE_IMPL


namespace tui {

Engine::Engine() : win(NULL)
{}

void Engine::createWindow(int _width,int _height)
{
	width = _width;
	height = _height;
}


void Engine::start(uint32_t mode)
{
	setlocale(LC_ALL,"en_EN.UTF-8");
	initscr();

	if (mode & CBREAK)
		cbreak();
	if (mode & RAW)
		raw();
	if (mode & ECHO)
		echo();
	if (mode & NOECHO)
		noecho();
	if (mode & CURSOR)
		curs_set(1);
	if (mode & NOCURSOR)
		curs_set(0);
	if (mode & KEYPAD)
		keypad(stdscr,TRUE);

	refresh();

	fullScreen = false;
	if (mode & FULLSCREEN)
	{
		fullScreen = true;
		getmaxyx(stdscr,boundY,boundX);
		height = boundY-2;
		width = boundX-2;
	} 

	// always center the window
	win = newwin(height+2,width+2,0,0);
	box(win,0,0);
	syncWindow();

	if (!onCreate())
	{
		onDestroy();
		return;
	}

	bool resize = false;
	if (mode & RESIZE)
		resize = true;

	bool debug = false;
	if (mode & DEBUG)
		debug = true;

	bool pollKeyboard = false;
	if (mode & POLL_KEYS)
		pollKeyboard = true;

	wrefresh(win);

	std::chrono::system_clock::time_point t1
		= std::chrono::system_clock::now();
	std::chrono::system_clock::time_point t2
		= std::chrono::system_clock::now();

	nodelay(stdscr,TRUE);

	bool active(true);
	while (active)
	{
		if (resize)
			syncWindow();

		t1 = t2;
		t2 = std::chrono::system_clock::now();
		std::chrono::duration<float> dt = t2 - t1;

		if (pollKeyboard)
			pollKey();

		active = onUpdate(dt.count());

		if (debug)
			mvwprintw(win,0,1,"height:%d,width:%d",height,width);

		wrefresh(win);
	}

	bool destruction = onDestroy();
	delwin(win);
	endwin();
	std::cout << destruction << std::endl;
}


bool Engine::onDestroy()
{
	return true;
}

void Engine::onKeypress(wint_t c)
{}


/* poll boundary and reposition / resize the window */
void Engine::syncWindow()
{
	int boundPrevX = boundX, boundPrevY = boundY;
	getmaxyx(stdscr,boundY,boundX);
	if (boundPrevX == boundX && boundPrevY == boundY)
		return;

	wborder(win,' ',' ',' ',' ',' ',' ',' ',' ');
	wrefresh(win);

	if (fullScreen)
	{
		height = boundY - 2;
		width = boundX - 2;
	}
	else
	{
		winPosX = (boundX / 2) - (width / 2);
		winPosY = (boundY / 2) - (height / 2);

		mvwin(win,winPosY,winPosX);
	}
	box(win,0,0);
	wrefresh(win);
}

void Engine::pollKey()
{
	wint_t c;
	get_wch(&c);
	if (c != 0)
	{
		mvprintw(0,0,"Key pressed: %lc",c);
		onKeypress(c);
	}
}

void Engine::clear(wchar_t c)
{
	for (int x=0;x<width;x++)
		for (int y=0;y<height;y++)
			putChar(x,y,c);
}

void Engine::putChar(int posX, int posY, wchar_t c)
{
	mvwprintw(win,posY+1,posX+1,"%lc",c);
}

void Engine::putChar(float posX, float posY, wchar_t c)
{
	mvwprintw(win,((int)posY)+1,((int)posX)+1,"%lc",c);
}

void Engine::putString(int posX, int posY, const std::wstring& str)
{
	mvwprintw(win,posY+1,posX+1,"%ls",str.c_str());
}


void Engine::line(int startX, int startY, int endX, int endY, wchar_t c)
{}


void Engine::rect(int startX, int startY, int lengthX, int lengthY, wchar_t c)
{}

void Engine::rectFill(int startX, int startY, int lengthX, int lengthY, wchar_t c)
{}


void Engine::circle(int originX, int originY, int radius, wchar_t c)
{}

void Engine::circleFill(int originX, int originY, int radius, wchar_t c)
{}

}

#endif // _ENGINE_IMPL
#endif // ENGINE_IMPL
