#pragma once

// Based on Console FPS by @Javidx9 at OneLoneCoder.com 

class ImprovedConsoleFPS : ConsoleEngine
{
protected:

	virtual bool OnUserCreate();

	virtual bool OnUserUpdate(float elapsedTime);

};