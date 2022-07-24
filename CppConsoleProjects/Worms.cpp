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

#include "Worms.h"

vector<pair<float, float>> DefineDummy()
{
	vector<pair<float, float>> model;

	// Produce list of points in an unit circle
	model.push_back({ 0.0f, 0.0f });
	for (int i = 0; i < 10; i++)
		model.push_back({ cosf(i / 9.0f * 2.0f * M_PI), sinf(i / 9.0f * 2.0f * M_PI) });

	return model;
}

vector<pair<float, float>> DefineDebris()
{
	// Draw a wireframe square
	vector<pair<float, float>> model;
	model.push_back({0.0f, 0.0f});
	model.push_back({1.0f, 0.0f});
	model.push_back({1.0f, 1.0f});
	model.push_back({0.0f, 1.0f});

	return model;
}vector<pair<float, float>> DefineMissile()
{
	// Draw missile shaped model
	vector<pair<float, float>> model;
	model.push_back({ 0.0f, 0.0f });
	model.push_back({ 1.0f, 1.0f });
	model.push_back({ 2.0f, 1.0f });
	model.push_back({ 2.5f, 0.0f });
	model.push_back({ 2.0f, -1.0f });
	model.push_back({ 1.0f, -1.0f });
	model.push_back({ 0.0f, 0.0f });
	model.push_back({ -1.0f, -1.0f });
	model.push_back({ -2.5f, -1.0f });
	model.push_back({ -2.0f, 0.0f });
	model.push_back({ -2.5f, 1.0f });
	model.push_back({ -1.0f, 1.0f });

	// Scale points to make the shape be unit sized
	for (auto& v : model)
	{
		v.first /= 2.5f; 
		v.second /= 2.5f;
	}

	return model;
}

ConsoleSprite* Worm::wormSprite = nullptr;
ConsoleSprite* Worm::tombSprite = nullptr;

// Factory function for creation
vector<pair<float, float>> Missile::model = DefineMissile();

// Factory function for creation
vector<pair<float, float>> Debris::model = DefineDebris();

// Factory function for creation
vector<pair<float, float>> Dummy::model = DefineDummy();

void Worms::CreateMap()
{
	// Create 1D Perlin Noise
	float *surface = new float[mapWidth];
	float *noiseSeed = new float[mapWidth];

	// Use Rand() to generate noise
	for (int i = 0; i < mapWidth; i++)
		noiseSeed[i] = (float)rand() / (float)RAND_MAX;

	// Start and End of terrain are up the screen, as Perlin Noise has fixed intervals and ranges from 0 to 1
	noiseSeed[0] = 0.5f;
	PerlinNoise1D(mapWidth, noiseSeed, 8, 2.0f, surface);

	// Put values on map based on the surface
	// These values are either 1 or 0, depending on if there is ground or not
	for (int i = 0; i < mapWidth; i++)
	{
		for (int j = 0; j < mapHeight; j++)
		{
			if (j >= surface[i] * mapHeight) 
				map[j * mapWidth + i] = 1;	// Set as land
			else
				map[j * mapWidth + i] = 0;	// Set as sky

		}
	}

	// Delete the two allocated arrays
	delete[] surface;
	delete[] noiseSeed;
}

void Worms::PerlinNoise1D(int count, float *seed, int octaves, float bias, float *output)
{
	// 1D Perlin Noise
	for (int x = 0; x < count; x++)
	{
		float noise = 0.0f;
		float scaleAcc = 0.0f;
		float scale = 1.0f;

		for (int o = 0; o < octaves; o++)
		{
			// Divide pitch by two
			int pitch = count >> o;
			// Get sample point, division and multiplication are because of the loss of information caused by integers
			int sample1 = (x / pitch) * pitch;
			// Loop around, allow for tesselation
			int sample2 = (sample1 + pitch) % count;

			float blend = (float)(x - sample1) / (float)pitch;

			float sample = (1.0f - blend) * seed[sample1] + blend * seed[sample2];

			scaleAcc += scale;
			noise += sample * scale;

			// Scale is divided as the value decreases each octave
			scale = scale / bias;
		}

		// Scale to seed range
		output[x] = noise / scaleAcc;
	}
}

void Worms::Explosion(float worldX, float worldY, float radius)
{
	// Draw a circle of "Sky", so 0, to create a hole in the terrain
	// This is done using Bresenham Circle Algorithm as a lambda function

	auto CircleBresenham = [&](int xc, int yc, int r)
	{
		// Taken from wikipedia
		int x = 0;
		int y = r;
		int p = 3 - 2 * r;
		if (!r) 
			return;

		auto drawline = [&](int sx, int ex, int ny)
		{
			for (int i = sx; i < ex; i++)
			{
				if (ny >= 0 && ny < mapHeight && i >= 0 && i < mapWidth)
					map[ny * mapWidth + i] = 0; // Turns area to 0 in the bitmap, making it be drawn as sky

			}
		};

		while (y >= x)
		{
			// Modified to draw scan-lines instead of edges
			drawline(xc - x, xc + x, yc - y);
			drawline(xc - y, xc + y, yc - x);
			drawline(xc - x, xc + x, yc + y);
			drawline(xc - y, xc + y, yc + x);
			if (p < 0) 
				p += 4 * x++ + 6;
			else 
				p += 4 * (x++ - y--) + 10;
		}
	};

	// Create a hole in the terrain
	CircleBresenham(worldX, worldY, radius);
	

	// Cause a shockwave to move other objects around using pythagoras theorem
	for (auto& o : objects)
	{
		float dx = o->posX - worldX;
		float dy = o->posY - worldY;
		float distance = sqrt(dx*dx + dy*dy);

		// Make sure we don't divide by 0
		if (distance < 0.00001f)
			distance = 0.0001f;

		// If close enough, push them away according to how close they are
		if (distance < radius)
		{
			o->velocityX = (dx / distance) * radius;
			o->velocityY = (dy / distance) * radius;
			o->stable = false;
		}
	}

	// Launch debris
	for (int i = 0; i < (int)radius; i++)
		objects.push_back(unique_ptr<Debris>(new Debris(worldX, worldY)));
}

bool Worms::OnUserCreate()
{
	// Create and initialize the map
	map = new unsigned char[mapWidth * mapHeight];
	memset(map, 0, mapWidth * mapHeight * sizeof(unsigned char));


	// Initialize Game State variables
	GameState = GS_RESET;
	NextState = GS_RESET;

    return true;
}

bool Worms::OnUserUpdate(float elapsedTime)
{
	// Debug Inputs
	/*if (keys[L'M'].isReleased)
		CreateMap();

	if (mouse[2].isReleased)
	{
		Dummy *dummy = new Dummy(mousePositionX + cameraPosX, mousePositionY + cameraPosY);
		objects.push_back(unique_ptr<Dummy>(dummy));
	}
	if (mouse[1].isReleased)
	{
		objects.push_back(unique_ptr<Missile>(new Missile(mousePositionX + cameraPosX, mousePositionY + cameraPosY)));
	}
	if (mouse[0].isReleased)
	{
		//Explosion(mousePositionX + cameraPosX, mousePositionY + cameraPosY, 10.0f);

		Worm* worm = new Worm(mousePositionX + cameraPosX, mousePositionY + cameraPosY);
		objectUnderControl = worm;
		cameraTrackingObject = worm;
		objects.push_back(unique_ptr<Worm>(worm));
	}

	*/

	// Game State handling using a state machine
	switch (GameState)
	{
		case GS_RESET:
		{
			allowPlayerControl = false;
			gameIsStable = false;
			playerHasFired = false;
			showCountdown = false;
			NextState = GS_GENERATE_TERRAIN;
			break;
		}
		case GS_GENERATE_TERRAIN: // Generate the Terrain, so the Map
		{
			zoomOut = true;
			CreateMap();
			gameIsStable = false;
			showCountdown = false;
			NextState = GS_GENERATING_TERRAIN;
			break;
		}
		case GS_GENERATING_TERRAIN: 
		{
			showCountdown = false;
			if(gameIsStable)
				NextState = GS_ALLOCATE_UNITS;
			break;
		}
		case GS_ALLOCATE_UNITS: // Create the Units
		{
			// TODO: Add team selector menu
			int teams = 4;
			int wormsPerTeam = 4;

			// Calculate where each worm should be placed, with the spacing from worm to worm also calculated
			float spacePerTeam = (float)mapWidth / (float)teams;
			float spacePerWorm = (float)spacePerTeam / (wormsPerTeam * 2.0f);

			// Create the teams
			for (int i = 0; i < teams; i++)
			{
				teamVector.emplace_back(WormTeam());
				// Get location of the team on the map
				float teamMiddle = (spacePerTeam / 2.0f) + (i * spacePerTeam);
				for (int j = 0; j < wormsPerTeam; j++)
				{
					// Location of the worm in relation to the team position. Y-position is at the top of the map, as worms are dropped down
					float posX = teamMiddle - ((spacePerWorm * (float)wormsPerTeam) / 2.0f) + (j * spacePerWorm);
					float posY = 0.0f;

					// Add the worms to the teams
					Worm* worm = new Worm(posX, posY);
					worm->team = i;
					worm->teamColor = teamVector[i].GetTeamColor(i);
					objects.push_back(unique_ptr<Worm>(worm));
					teamVector[i].members.push_back(worm);
					teamVector[i].teamSize = wormsPerTeam;
				}

				teamVector[i].currentMemberIndex = 0;
			}

			// Select the first worm for control and camera tracking
			objectUnderControl = teamVector[0].members[teamVector[0].currentMemberIndex];
			cameraTrackingObject = objectUnderControl;
			showCountdown = false;

			NextState = GS_ALLOCATING_UNITS;
			break;
		}
		case GS_ALLOCATING_UNITS:
		{
			// Wait for the Game to prepare units, so everything stops moving before the game starts.
			if (gameIsStable)
			{
				allowPlayerControl = true;
				allowComputerControl = false;
				turnTime = turnTimeMax;
				zoomOut = false;
				NextState = GS_START_PLAY;
			}

			break;
		}
		case GS_START_PLAY:
		{
			showCountdown = true;

 			if(playerHasFired ||turnTime <= 0.0f)
				NextState = GS_CAMERA_MODE;

			break;
		}
		case GS_CAMERA_MODE:
		{
			allowPlayerControl = false;
			allowComputerControl = false;
			showCountdown = false;
			playerHasFired = false;
			chargeLevel = 0.0f;

			if (gameIsStable)
			{
				// Get the next team's worm. If there is none, game is over
				int oldTeam = currentTeam;
				do
				{
					currentTeam += 1;
					currentTeam %= teamVector.size();
				} while (!teamVector[currentTeam].isTeamAlive() && currentTeam != oldTeam);

				// Player vs Computer
				if (gameMode == 0)
				{
					// Player Team
					if (currentTeam == 0)
					{
						allowPlayerControl = true;
						allowComputerControl = false;
					}
					else // Computer Team
					{
						allowPlayerControl = true;
						allowComputerControl = false;
					//	allowPlayerControl = false;
					//	allowComputerControl = true;
					}
				}

				else // Player vs Player
				{

					allowPlayerControl = true;
					allowComputerControl = false;
				}


				// Reset camera back to the object under control
				objectUnderControl = teamVector[currentTeam].GetNextMember();
				cameraTrackingObject = objectUnderControl;
				turnTime = turnTimeMax;
				zoomOut = false;
				NextState = GS_START_PLAY;

				// No new team was found
				if (currentTeam == oldTeam)
				{
					NextState = GS_GAME_OVER1;
				}
			}

			break;
		}
		case GS_GAME_OVER1:
		{
			// TODO ADD
		}
		default:
		{
			break;
		}
	}

	// Decrease the turn timer
	turnTime -= elapsedTime;

	// User Input
	if (allowPlayerControl)
	{
		if (objectUnderControl != nullptr)
		{
			// Only if the object / worm is stable
			if (objectUnderControl->stable)
			{
				// Assume object is a worm
				Worm* worm = (Worm*)objectUnderControl;

				// Move Shooting Angle to the Right
				if (keys[L'D'].isHeld)
				{
					worm->shootAngle += 1.0f * elapsedTime;
					// Wrap angle to back 
					if (worm->shootAngle > M_PI)
						worm->shootAngle -= M_PI * 2.0f;
				}
				// Move Shooting Angle to the Left
				if (keys[L'A'].isHeld)
				{
					worm->shootAngle -= 1.0f * elapsedTime;
					if (worm->shootAngle < -M_PI)
						worm->shootAngle += M_PI * 2.0f;
				}
				// Jump to the cursor direction
				if (keys[L'X'].isPressed)
				{
					float angle = worm->shootAngle;
					objectUnderControl->velocityX = 4.0f * cosf(angle);
					objectUnderControl->velocityY = 8.0f * sinf(angle);
					objectUnderControl->stable = false;
				}

				// Start weapon charging
				if (keys[VK_SPACE].isPressed)
				{
					chargingWeapon = true;
					fireWeapon = false;
					chargeLevel = 0.0f;
				}
				if (keys[VK_SPACE].isHeld)
				{
					if (chargingWeapon)
					{
						// Increase weapon charge while button is held down
						chargeLevel += 0.75f * elapsedTime;
						if (chargeLevel >= 1.0f)
						{
							chargeLevel = 1.0f;
							fireWeapon = true;
						}
					}
				}
				if (keys[VK_SPACE].isReleased)
				{
					if (chargingWeapon)
						fireWeapon = true;

					chargingWeapon = false;
				}

				// Change worm direction
				if (worm->shootAngle > -M_PI/2 && worm->shootAngle <= M_PI/2)
					worm->dir = 1;
				else
					worm->dir = -1;
			}


			// Shoot weapon if fireWeapon is true
			if (fireWeapon)
			{
				Worm* worm = (Worm*)objectUnderControl;

				// Weapon origin
				float originX = worm->posX;
				float originY = worm->posY;

				// Weapon direction
				float dirX = cosf(worm->shootAngle);
				float dirY = sinf(worm->shootAngle);

				// Create the weapon object
				Missile* missile = new Missile(originX, originY, dirX * 40.0f * chargeLevel, dirY * 40.0f * chargeLevel);
				objects.push_back(unique_ptr<Missile>(missile));

				cameraTrackingObject = missile;

				fireWeapon = false;
				chargeLevel = 0.0f;
				chargingWeapon = false;
				playerHasFired = true;

			}
		}
	}

	// Camera moves to the camera target position smoothly
	if (cameraTrackingObject != nullptr)
	{
		cameraTargetPosX = cameraTrackingObject->posX - (screenWidth / 2);
		cameraTargetPosY = cameraTrackingObject->posY - (screenHeight / 2);
		// Interpolate actual camera coordinates

		cameraPosX += (cameraTargetPosX - cameraPosX) * 5.0f * elapsedTime;
		cameraPosY += (cameraTargetPosY - cameraPosY) * 5.0f * elapsedTime;
	}

	// Sides of the screen allow for moving the camera around the map, if not tracking an object
	if (mousePositionX < 5)
		cameraPosX -= mapScrollSpeed * elapsedTime;
	if (mousePositionX > screenWidth - 5)
		cameraPosX += mapScrollSpeed * elapsedTime;
	if (mousePositionY < 5)
		cameraPosY -= mapScrollSpeed * elapsedTime;
	if (mousePositionY > screenHeight - 5)
		cameraPosY += mapScrollSpeed * elapsedTime;

	// Clamp the map boundaries to avoid going of bounds
	if (cameraPosX < 0)
		cameraPosX = 0;
	if (cameraPosX >= mapWidth - screenWidth)
		cameraPosX = mapWidth - screenWidth;
	if (cameraPosY < 0)
		cameraPosY = 0;
	if (cameraPosY >= mapHeight - screenHeight)
		cameraPosY = mapHeight - screenHeight;

	// Perform 10 loops of physics calculation per update
	for (int i = 0; i < 10; i++)
	{
		// Update physics of physics objects
		for (auto &o : objects)
		{
			// Apply Gravity to object
			//o->accelY += 9.81f;
			o->accelY += 2.0f;

			// Update Velocity in respect to acceleration
			o->velocityX += o->accelX * elapsedTime;
			o->velocityY += o->accelY * elapsedTime;

			// Update position according to the velocity
			// However, use potential variables because collision needs to be tested first
			float potentialPosX = o->posX + o->velocityX * elapsedTime;
			float potentialPosY = o->posY + o->velocityY * elapsedTime;

			// Reset acceleration at the end
			o->accelX = 0.0f;
			o->accelY = 0.0f;
			o->stable = false;

			// Check map collision
			float angle = atan2f(o->velocityY, o->velocityX);
			// Response vector is generated using points that collide with the terrain,
			// to then calculate the direction the physics object continues moving in
			float responseX = 0.0f;
			float responseY = 0.0f;
			bool collision = false;

			// Iterate around a semi-circle of the object, as collision only happens in the other side (99% of the time)
			// Step size could be further calculated, to make sure all points are within unit distance from each other
			for (float r = angle - (M_PI / 2.0f); r < angle + (M_PI / 2.0f); r += M_PI / 8.0f)
			{
				float testPosX = (o->radius) * cosf(r) + potentialPosX;
				float testPosY = (o->radius) * sinf(r) + potentialPosY;

				// Clamp points to map size
				if (testPosX < 0.0f)
					testPosX = 0.0f;
				if (testPosX >= mapWidth)
					testPosX = mapWidth - 1;
				if (testPosY < 0.0f)
					testPosY = 0.0f;
				if (testPosY >= mapHeight)
					testPosY = mapHeight - 1;

				// Check for intersection with terrain in that position
				if (map[(int)testPosY * mapWidth + (int)testPosX] != 0)
				{
					// Accumulate collision points to create escape response to object
					responseX += potentialPosX - testPosX;
					responseY += potentialPosY - testPosY;
					collision = true;
				}
			}

			float velocityMagnitude = sqrtf(powf(o->velocityX, 2) + powf(o->velocityY, 2));
			float responseMagnitude = sqrtf(powf(responseX, 2) + powf(responseY, 2));

			if (collision)
			{
				// Object has technically not YET collided, but potentially will, 
				// but is still stopped beforehand to avoid intersection with terrain
				o->stable = true;

				// Calculate reflection vector of the objects velocity
				// Dot product between velocity vector and the normalized response vector
				float dot = o->velocityX * (responseX / responseMagnitude) + o->velocityY * (responseY / responseMagnitude);
				// Calculate new velocity using the reflection equation "r=d-2(d.n)n"
				// Additionally, calculate in the friction coefficient, loss of energy to dampen the response
				o->velocityX = o->friction * (-2.0f * dot * (responseX / responseMagnitude) + o->velocityX);
				o->velocityY = o->friction * (-2.0f * dot * (responseY / responseMagnitude) + o->velocityY);

				// Calculate bounces to determine if dead flag needs to be set
				if (o->bouncesBeforeDeath > 0)
				{
					// If bouncesBeforeDeath is 0, set dead to true
					o->bouncesBeforeDeath -= 1;
					o->dead = o->bouncesBeforeDeath == 0;

					// Object dies
					if (o->dead)
					{
						int action = o->DeathAction();
						if (action > 1)
						{
							Explosion(o->posX, o->posY, action);
							cameraTrackingObject = nullptr;
						}
					}
				}

			}
			else // With no collision, move object to the potential position
			{
				o->posX = potentialPosX;
				o->posY = potentialPosY;
			}

			// When object barely moves anymore, change it to stable
			if (velocityMagnitude < 0.1f)
				o->stable = true;
		}

		// Use lambda function to remove dead physics objects from the list
		// Because of the unique_ptr, once they are removed, they will be deleted as they are out of scope automatically
		objects.remove_if([](unique_ptr<PhysicsObject> &o) {return o->dead; });
	}
	// Empty screen
	Fill(0, 0, screenWidth, screenHeight, L' ', 0);


	// Draw the Landscape
	if (!zoomOut)
	{
		for (int i = 0; i < screenWidth; i++)
		{
			for (int j = 0; j < screenHeight; j++)
			{
				switch (map[(j + (int)(cameraPosY)) * mapWidth + (i + (int)cameraPosX)])
				{
				case 0:
					Draw(i, j, PIXEL_FULL, FG_CYAN);
					break;
				case 1:
					Draw(i, j, PIXEL_FULL, FG_DARK_GREEN);
					break;
				default:
					Draw(i, j, PIXEL_FULL, FG_CYAN);
					break;
				}
			}
		}
	
		// Draw all the objects using each objects own Draw method
		for (auto& o : objects)
		{
			o->Draw(this, cameraPosX, cameraPosY);

			Worm* worm = (Worm*)objectUnderControl;

			// Draw cursor around objectUnderControl worm
			if (o.get() == worm)
			{
				float centerX = worm->posX + 8.0f * cosf(worm->shootAngle) - cameraPosX;
				float centerY = worm->posY + 8.0f * sinf(worm->shootAngle) - cameraPosY;

				Draw(centerX, centerY, PIXEL_FULL, FG_BLACK);
				Draw(centerX + 1, centerY, PIXEL_FULL, FG_BLACK);
				Draw(centerX - 1, centerY, PIXEL_FULL, FG_BLACK);
				Draw(centerX, centerY + 1, PIXEL_FULL, FG_BLACK);
				Draw(centerX, centerY - 1, PIXEL_FULL, FG_BLACK);

				// Draw Charge level of player's weapon
				for (int i = 0; i < 11 * chargeLevel; i++)
				{
					Draw(worm->posX - 5 + i - cameraPosX, worm->posY - 12 - cameraPosY, PIXEL_FULL, FG_GREEN);
					Draw(worm->posX - 5 + i - cameraPosX, worm->posY - 11 - cameraPosY, PIXEL_FULL, FG_RED);
				}
			}

		}
	}
	else // The view is Zoomed Out
	{
		// Draw the whole map on the screen at once, with all the worms visible
		for (int i = 0; i < screenWidth; i++)
		{
			for (int j = 0; j < screenHeight; j++)
			{
				// Rescale the coordinates
				float fx = (float)i / (float)screenWidth * (float)mapWidth;
				float fy = (float)j / (float)screenHeight * (float)mapHeight;

				switch (map[((int)fy) * mapWidth + ((int)fx)])
				{
				case 0:
					Draw(i, j, PIXEL_FULL, FG_CYAN);
					break;
				case 1:
					Draw(i, j, PIXEL_FULL, FG_DARK_GREEN);
					break;
				default:
					Draw(i, j, PIXEL_FULL, FG_CYAN);
					break;
				}
			}
		}
		// Draw all the objects using each objects own Draw method
		for (auto& o : objects)
		{
			o->Draw(this, o->posX-(o->posX / (float)mapWidth) * (float)screenWidth, 
				o->posY - (o->posY / (float)mapHeight) * (float)screenHeight, true);
		}
	}

	// Check for the stability of the game, by making sure all objects have stopped moving
	gameIsStable = true;
	for (auto& o : objects)
	{
		if (!o->stable)
		{
			gameIsStable = false;
			break;
		}
	}

	// Draw Health Bars for Teams ( Total remaining health of all members of the team)
	for (size_t i = 0; i < teamVector.size(); i++)
	{
		float totalHealth = 0.0f;
		float maxHealth = (float)teamVector[i].teamSize;
		for (auto w : teamVector[i].members)
			totalHealth += w->health;

		Fill(4, 4 + i * 4, (totalHealth / maxHealth)* (float)(screenWidth - 8) + 4, 4 + i * 4 + 3, PIXEL_FULL, teamVector[i].GetTeamColor(i));
	}

	// Update our Game State
	GameState = NextState;

    return true;
}

