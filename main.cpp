#define ENGINE_IMPL
#include "engine.h"

class Demo : public tui::Engine
{
private:
	float x,y;
	float dx,dy;
	float ddx,ddy;
protected:
	bool onCreate() override
	{
		x = 0.0f;
		y = height-1;
		dx = 8.0f;
		dy = -8.0f;
		ddx = 0.0f;
		ddy = 1.0f;
		return true;
	}

	bool onUpdate(float dt) override
	{
		dx += ddx*dt;
		dy += ddy*dt;
		x += dx*dt;
		y += dy*dt;
		if (x >= width || y >= height)
			return false;
		putChar(x,y,L'O');
		return true;
	}
};

class Ping : public tui::Engine
{
private:
	float ballX, ballY;
	float ballDX, ballDY;
protected:
	bool onCreate() override
	{
		ballX = 5;
		ballY = 5;
		ballDX = 4.0f;
		ballDY = 6.0f;
		return true;
	}

	bool onUpdate(float dt) override
	{
		clear();

		ballX += ballDX*dt;
		ballY += ballDY*dt;

		if (ballX < 0 || ballX >= width-1.0f)
			ballDX *= -1.0f;
		if (ballY < 0 || ballY >= height-1.0f)
			ballDY *= -1.0f;

		putChar(ballX,ballY,'O');
		return true;
	}

	void onKeypress(wint_t c) override
	{
		if (c == L'j')
			ballDY *= -1.0f;
	}
};


int main()
{
	/*
	Demo d;
	//d.createWindow(20,10);
	d.start(DEBUG | CBREAK | NOECHO | NOCURSOR | FULLSCREEN);
	*/

	//*
	Ping p;
	p.createWindow(20,10);
	p.start(CBREAK | NOECHO | NOCURSOR | RESIZE | DEBUG | POLL_KEYS);
	//*/
	return 0;
}
