using namespace std;

#include "ConsoleEngine.h"

class Demo : public ConsoleEngine
{
public:
	Demo()
	{

	}

private:
	float playerX = 0.0f;
	float playerY = 0.0f;

protected:
	virtual bool OnUserCreate()
	{
		playerX = 10.0f;
		playerY = 10.0f;

		return true;
	}

	virtual bool OnUserUpdate(float elapsedTime)
	{
		/*for (int i = 0; i < screenWidth; i++)
		{
			for (int j = 0; j < screenHeight; j++)
				// Test to draw # signs with random colors on the whole screen
				Draw(i, j, L'#', rand() % 16);
		}*/

		/*if (keys[VK_LEFT].isHeld)
			playerX -= 10 * elapsedTime;
		if (keys[VK_RIGHT].isHeld)
			playerX += 10 * elapsedTime;
		if (keys[VK_UP].isHeld)
			playerY -= 10 * elapsedTime;
		if (keys[VK_DOWN].isHeld)
			playerY += 10 * elapsedTime;

		// Empty screen
		Fill(0, 0, screenWidth, screenHeight, L' ', 0);

		// Draw player at coordinates
		//Draw(playerX, playerY, L'P', rand() % 16);
		// Or larger player
		Fill(playerX, playerY, playerX + 3, playerY + 3, 0x2588, rand() % 16);*/

		Fill(0, 0, screenWidth, screenHeight, L' ', 0);
		// Draw mouse cursor
		//Draw(mousePositionX - 1, mousePositionY);
		/*Draw(mousePositionX, mousePositionY);
		Draw(mousePositionX+1, mousePositionY);
		Draw(mousePositionX, mousePositionY-1);
		Draw(mousePositionX, mousePositionY+1); */
		playerX = mousePositionX;
		playerY = mousePositionY;
		Fill(playerX, playerY, playerX + 3, playerY + 3, 0x2588, rand() % 16);

		return true;
	}

};

int main()
{
	Demo game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();
	/*Demo test;
	test.ConstructConsole(128,80, 12,10);
	test.Start();*/

	return 0;
}