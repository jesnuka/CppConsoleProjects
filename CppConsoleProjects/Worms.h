#pragma once

// Based on Console Worms by @Javidx9 at OneLoneCoder.com 
/*
	License
	~~~~~~~
	One Lone Coder Console Game Engine  Copyright (C) 2018  Javidx9
	This program comes with ABSOLUTELY NO WARRANTY.
	This is free software, and you are welcome to redistribute it
	under certain conditions; See license for details.
	Original works located at:
	https://www.github.com/onelonecoder
	https://www.onelonecoder.com
	https://www.youtube.com/javidx9

	GNU GPLv3
	https://github.com/OneLoneCoder/videos/blob/master/LICENSE
*/

#include "ConsoleEngine.h"
using namespace std;

class PhysicsObject
{
public:
	float posX = 0.0f;
	float posY = 0.0f;
	float velocityX = 0.0f;
	float velocityY = 0.0f;
	float accelX = 0.0f;
	float accelY = 0.0f;
	float friction = 0.8f;

	// Number of times the object can bounce, before dead flag is set
	// Default, -1, is for infinite bounces
	int bouncesBeforeDeath = -1;
	bool dead = false;

	// For drawing the sprite, get latest traveled direction
	int dir = 1;

	float radius = 4.0f;
	// Stable objects do not move, and don't need to have more calculations
	bool stable = false;

	PhysicsObject(float x = 0.0f, float y = 0.0f)
	{
		posX = x;
		posY = y;
	}

	virtual void Draw(ConsoleEngine *engine, float offsetX, float offsetY, bool smallScale = false) = 0;
	virtual int DeathAction() = 0;
	virtual bool Damage(float dmg) { return true; };
};

class Debris : public PhysicsObject
{
public:
	Debris(float x = 0.0f, float y = 0.0f) : PhysicsObject(x, y)
	{
		// Have initial velocity for debris in a circular direction
		velocityX = 10.0f * cosf((float)rand() / (float)RAND_MAX * 2.0F * M_PI);
		velocityY = 10.0f * sinf((float)rand() / (float)RAND_MAX * 2.0F * M_PI);
		radius = 1.0f;
		friction = 0.8f;
		dead = false;
		bouncesBeforeDeath = 5;
	}

	virtual void Draw(ConsoleEngine* engine, float offsetX, float offsetY, bool smallScale = false)
	{
		// Draw wireframe model
		// Position is offset by camera, atan2f to get the angle using velocities, Scale is done using radius to see the bounding radius
		engine->WireFrame(model, posX - offsetX, posY - offsetY, atan2f(velocityY, velocityX), smallScale ? 0.5f : radius, PIXEL_FULL, FG_DARK_GREEN);
	}
	
	virtual int DeathAction()
	{
		return 0;
	}

private:
	// Share model across models of the same class
	static vector<pair<float, float>> model;
};

class Worm : public PhysicsObject
{
public:
	Worm(float x = 0.0f, float y = 0.0f) : PhysicsObject(x, y)
	{
		radius = 3.5f;
		friction = 0.2f;
		dead = false;
		bouncesBeforeDeath = -1;

		dir = 1;

		// TODO: Add color selection for worms
		if (wormSprite == nullptr)
			wormSprite = new ConsoleSprite(L"wormStand.spr");
		if (tombSprite == nullptr)
			tombSprite = new ConsoleSprite(L"tomb1.spr");
	}

	virtual void Draw(ConsoleEngine* engine, float offsetX, float offsetY, bool smallScale = false)
	{
		if (isPlayable)
		{
			switch (dir)
			{
			case 1:
			{
				engine->DrawPartialSprite(posX - offsetX - radius, posY - offsetY - radius, wormSprite, 0, 0, 8, 8);
				break;
			}
			case -1:
			{
				engine->DrawPartialSprite(posX - offsetX - radius, posY - offsetY - radius, wormSprite, 0, 0, 8, 8, true);
				break;
			}
			default:
				break;
			}

			// Draw the worm healthbar

			for (int i = 0; i < 11 * health; i++)
			{
				engine->Draw(posX - 5 + i - offsetX, posY + 5 - offsetY, PIXEL_FULL, teamColor);
				engine->Draw(posX - 5 + i - offsetX, posY + 6 - offsetY, PIXEL_FULL, teamColor);
			}
		}
		else
		{
			// Draw Tombstone
			engine->DrawPartialSprite(posX - offsetX - radius, posY - offsetY - radius, tombSprite, 0, 0, 8, 8, true);
		}
		
			
	}

	virtual int DeathAction()
	{
		return 0;
	}

	virtual bool Damage(float dmg)
	{
		health -= dmg;
		if (health <= 0)
		{
			health = 0.0f;
			isPlayable = false;
		}
		return health > 0;
	}

public:
	float shootAngle = 0.0f;
	float health = 1.0f;
	int team = 0;
	Color teamColor = FG_BLUE;
	bool isPlayable = true;

private:
	// Share model across models of the same class
	static vector<pair<float, float>> model;
	static ConsoleSprite *wormSprite;
	static ConsoleSprite *tombSprite;
};

// A Team of Worms
class WormTeam
{
public:
	vector<Worm *> members;
	int currentMemberIndex = 0;
	int teamSize = 0;

	// Is the Team defeated
	bool isTeamAlive()
	{
		bool allDefeated = true;
		for (auto w : members)
			if (w->health > 0.0f)
				allDefeated = false;
		return !allDefeated;
	}

	Worm* GetNextMember()
	{
		// Return a pointer to the next valid member in the team (Not dead)
		do
		{
			currentMemberIndex += 1;
			if (currentMemberIndex >= teamSize) currentMemberIndex = 0;
		} while (members[currentMemberIndex]->health <= 0.0f);
		return members[currentMemberIndex];
	}

	Color GetTeamColor(int i)
	{
		switch (i)
		{
			case 0:
			{
				return FG_BLUE;
				break;
			}
			case 1:
			{
				return FG_RED;
				break;
			}
			case 2:
			{
				return FG_GREEN;
				break;
			}
			case 3:
			{
				return FG_YELLOW;
				break;
			}
			default:
			{
				return FG_BLUE;
				break;
			}
		}
	}

};

class Missile : public PhysicsObject
{
public:
	Missile(float x = 0.0f, float y = 0.0f, float _velocityX = 0.0f, float _velocityY = 0.0f) : PhysicsObject(x, y)
	{
		velocityX = 10.0f * cosf((float)rand() / (float)RAND_MAX * 2.0F * M_PI);
		velocityY = 10.0f * sinf((float)rand() / (float)RAND_MAX * 2.0F * M_PI);
		radius = 2.5f;
		friction = 0.5f;
		velocityX = _velocityX;
		velocityY = _velocityY;
		bouncesBeforeDeath = 1;
		dead = false;
	}

	virtual void Draw(ConsoleEngine* engine, float offsetX, float offsetY, bool smallScale = false)
	{
		// Draw wireframe model
		// Position is offset by camera, atan2f to get the angle using velocities, Scale is done using radius to see the bounding radius
		engine->WireFrame(model, posX - offsetX, posY - offsetY, atan2f(velocityY, velocityX), smallScale ? 0.5f : radius, PIXEL_FULL, FG_YELLOW);
	}

	virtual int DeathAction()
	{
		return 20; // Big radius explosion
	}

private:
	// Share model across models of the same class
	static vector<pair<float, float>> model;
};

class Dummy : public PhysicsObject
{
public:
	Dummy(float x= 0.0f, float y = 0.0f) : PhysicsObject(x,y)
	{

	}

	virtual void Draw(ConsoleEngine *engine, float offsetX, float offsetY, bool smallScale = false)
	{
		// Draw wireframe model of dummy, 
		// Position is offset by camera, atan2f to get the angle using velocities, Scale is done using radius to see the bounding radius
		engine->WireFrame(model, posX - offsetX, posY - offsetY, atan2f(velocityY, velocityX), smallScale ? 0.5f : radius, PIXEL_FULL, FG_WHITE);
	}
	

	virtual int DeathAction()
	{
		return 0; 
	}

private:
	// Share model across models of the same class
	static vector<pair<float, float>> model;
};

class Worms : public ConsoleEngine
{
public:
	Worms()
	{
		appName = L"Worms";
	}

private:

	// Map
	int mapWidth = 1024;
	int mapHeight = 512;

	// Camera
	float cameraPosX = 0.0f;
	float cameraPosY = 0.0f;

	float cameraTargetPosX = 0.0f;
	float cameraTargetPosY = 0.0f;

	float mapScrollSpeed = 200.0f;

	float turnTimeMax = 30.0f;
	float turnTime = turnTimeMax;

	float angleRotationSpeed = 1.5f;

	// List of all the physics objects, as pointers because they are abstract
	// Subclasses can therefore also be added to this list
	// These are unique_ptr:s to allow them to be deleted when they go out of scope
	list<unique_ptr<PhysicsObject>> objects;

	// The worm that is under player control
	PhysicsObject* objectUnderControl = nullptr;
	PhysicsObject* cameraTrackingObject = nullptr;

	// Boolean flags used by the state machine
	bool zoomOut = false;
	bool fireWeapon = false;
	bool chargingWeapon = false;

	bool gameIsStable = false;

	bool allowPlayerControl = false;
	bool allowComputerControl = false;
	bool playerHasFired = false;
	bool showCountdown = false;

	float chargeLevel = 0.0f;

	// Store teams to this vector
	vector<WormTeam> teamVector;
	int currentTeam = 0;
	// Gamemode 0 == PlayerVsComputer, Gamemode 1 == PlayerVsPlayer
	bool gameMode = 0;

	// Variables used by the Computer to determine actions and behavior
	bool Com_Jump = false;
	bool Com_AimLeft = false;
	bool Com_AimRight = false;
	bool Com_ChargeWeapon = false;

	float Com_TargetAngle = 0.0f;
	float Com_TargetChargeLevel = 0.0f;
	float Com_SafePosition = 0.0f;
	Worm* Com_TargetWorm = nullptr;
	float Com_TargetX = 0.0f;
	float Com_TargetY = 0.0f;



	unsigned char* map = nullptr;

	void CreateMap();
	void PerlinNoise1D(int nCount, float* fSeed, int nOctaves, float fBias, float* fOutput);
	void Explosion(float worldX, float worldY, float radius);

	// State machine for the state of the game
	enum GAMESTATE
	{
		GS_RESET = 0,
		GS_GENERATE_TERRAIN = 1,
		GS_GENERATING_TERRAIN,
		GS_ALLOCATE_UNITS,
		GS_ALLOCATING_UNITS,
		GS_START_PLAY,
		GS_CAMERA_MODE,
		GS_GAME_OVER1
	} GameState, NextState;

	enum COMPUTERSTATE
	{
		COM_ASSESS_ENVIRONMENT = 0,
		COM_MOVE,
		COM_CHOOSE_TARGET,
		COM_POSITION_FOR_TARGET,
		COM_AIM,
		COM_FIRE,
	} ComputerGameState, ComputerNextState;


protected:

	virtual bool OnUserCreate();

	virtual bool OnUserUpdate(float elapsedTime);
};