#pragma once

class Racing : public ConsoleEngine
{
public:
	Racing()
	{
		appName = L"Racing";
	}

private:
	// Racing stats
	// Distance driven around the track
	float distance = 0.0f;

	// Player Car
	float carPositionFloat = 0.0f;

	// Fixed height on the screen
	int carPositionheight = 80;
	int carSpriteWidth = 7;

protected:

	virtual bool OnUserCreate();

	virtual bool OnUserUpdate(float elapsedTime);

};
