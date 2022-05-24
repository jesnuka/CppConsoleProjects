using namespace std;

#include "ConsoleEngine.h"

class Demo : public ConsoleEngine
{
public:
	Demo()
	{

	}

protected:
	virtual bool OnUserCreate()
	{
		return true;
	}

	virtual bool OnUserUpdate(float elapsedTime)
	{
		return true;
	}

};

int main()
{
	Demo game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();
}