#include "Asteroids.h"


void Asteroids::RestartGame()
{
	// Remove asteroids and bullets
	asteroids.clear();
	bullets.clear();

	// Spawn asteroids
	asteroids.push_back({ 20.0f , 20.0f, 8.0f, -6.0f, (int)16, 0 , false });
	asteroids.push_back({ 80.0f , 20.0f, -6.0f, -4.0f, (int)16, 0 , false });

	// Initialize the Player
	player.x = screenWidth / 2;
	player.y = screenHeight / 2;
	player.xDir = 0;
	player.yDir = 0;
	player.angle = 0;

	player.dead = false;
	score = 0;
}

bool Asteroids::OnUserCreate()
{
	// Player ship, a triangle
	modelShip =
	{
		{0.0f, -5.0f},
		{-2.5f, 2.5f},
		{2.5f, 2.5f},
	};

	int asteroidVertices = 20;
	// Asteroid model

	for (int i = 0; i < asteroidVertices; i++)
	{
		//float radius = 1.0f; // Unit circle
		// Randomize a number between minimum of 0.2 to maximum of 1.2
		// This creates different distance vertices for the asteroid, making it more jagged looking
		float radius = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f;
		float angle = ((float)i / (float)asteroidVertices) * (2 * M_PI);
		modelAsteroid.push_back(make_pair(radius * sinf(angle), radius * cosf(angle)));
	}

	RestartGame();


	return true;
}

bool Asteroids::OnUserUpdate(float elapsedTime)
{
	// Check for player death
	if (player.dead)
		RestartGame();

	// Empty screen
	Fill(0, 0, screenWidth, screenHeight, L' ', 0);

	// Stores newly generated asteroids that have been split from a larger one
	vector<gameObject> splitAsteroids;

	// Update and draw asteroids
	for (auto& ast : asteroids)
	{
		// Position = Velocity * Time (+ Old Position)
		ast.x += ast.xDir * elapsedTime;
		ast.y += ast.yDir * elapsedTime;
		ast.angle += 0.5f * elapsedTime;
		WrapCoordinates(ast.x, ast.y, ast.x, ast.y);

		WireFrame(modelAsteroid, ast.x, ast.y, ast.angle, ast.size, PIXEL_FULL, FG_YELLOW);

	}

	// Update and draw bullets
	for (auto& bullet : bullets)
	{
		// Position = Velocity * Time (+ Old Position)
		bullet.x += bullet.xDir * elapsedTime;
		bullet.y += bullet.yDir * elapsedTime;
		WrapCoordinates(bullet.x, bullet.y, bullet.x, bullet.y);
		Draw(bullet.x, bullet.y, PIXEL_FULL, FG_WHITE);
		// TODO: Draw at the tip of player ship

		// Check for collision, slow method but works for small scale
		for (auto& ast : asteroids)
		{
			if (PointInsideCircle(bullet.x, bullet.y, ast.size, ast.x, ast.y))
			{
				// Move bullet off-screen to have it be destroyed in the remove_if check
				bullet.x = -100;

				// Check if asteroid is larger than minimum size to split
				if (ast.size > 4)
				{
					// Randomize two angles for the new split asteroids
					float newAngle1 = ((float)rand() / (float)RAND_MAX) * (2 * M_PI);
					float newAngle2 = ((float)rand() / (float)RAND_MAX) * (2 * M_PI);

					// Create split asteroids. Size is half of the parent asteroid
					splitAsteroids.push_back({ ast.x, ast.y, 10.0f * sinf(newAngle1), 10.0f * cosf(newAngle1), (int)ast.size >> 1, 0.0f, false });
					splitAsteroids.push_back({ ast.x, ast.y, 10.0f * sinf(newAngle2), 10.0f * cosf(newAngle2), (int)ast.size >> 1, 0.0f, false });
				}

				// Set parent asteroid as dead, and move it away from the screen
				ast.dead = true;
				ast.x = -100;
				score += 100;
			}
		}

	}

	// Add newly created splitAsteroids to asteroids vector now, that vectors are not accessed anymore
	for (auto& ast : splitAsteroids)
	{
		asteroids.push_back(ast);
	}

	// Destroy off-screen bullets
	if (bullets.size() > 0)
	{
		// Use "remove_if" - function
		auto i = remove_if(bullets.begin(), bullets.end(), [&](gameObject o)
			{
				// Check if coordinates are off the screen
				return(o.x < 1 || o.y < 1 || o.x >= screenWidth || o.y >= screenHeight);
			});
		// If something to be removed was found, remove it from the vector
		if (i != bullets.end())
			bullets.erase(i);
	}
	// Destroy flagged asteroids
	if (asteroids.size() > 0)
	{
		// Use "remove_if" - function
		auto i = remove_if(asteroids.begin(), asteroids.end(), [&](gameObject o)
			{
				// Check if asteroid is dead
				return(o.dead == true);
			});
		// If something to be removed was found, remove it from the vector
		if (i != asteroids.end())
			asteroids.erase(i);
	}

	// Check if any asteroids are left after 
	if (asteroids.empty())
	{
		// Increase score, and also create two new asteroids away from the player
		score += 1000;

		// TODO: Place away from player velocity direction instead of player.angle, to avoid crashing better
		// Spawn asteroids 90 degrees away from the player
		asteroids.push_back({ 30.0f * sinf(player.angle - (M_PI / 2)),
								30.0f * cosf(player.angle - (M_PI / 2)),
								10.0f * sinf(player.angle),
								10.0f * sinf(player.angle),
								16, 0.0f });
		asteroids.push_back({ 30.0f * sinf(player.angle - (M_PI / 2)),
								30.0f * cosf(player.angle - (M_PI / 2)),
								10.0f * sinf(-player.angle),
								10.0f * sinf(-player.angle),
								16, 0.0f });

	}


	// Player Steering
	if (keys[VK_LEFT].isHeld)
		player.angle -= 5.0f * elapsedTime;
	if (keys[VK_RIGHT].isHeld)
		player.angle += 5.0f * elapsedTime;

	// Player Thrust Movement (Acceleration)
	if (keys[VK_UP].isHeld)
	{
		// Acceleration = Velocity * Time
		// cos is negative, because 0 is at the top, and screenHeight is at the bottom of the screen
		player.xDir += sin(player.angle) * 20.0f * elapsedTime;
		player.yDir += -cos(player.angle) * 20.0f * elapsedTime;

	}
	// Can't go backwards
	if (keys[VK_DOWN].isHeld)
	{
		// Acceleration = Velocity * Time
		// cos is negative, because 0 is at the top, and screenHeight is at the bottom of the screen
		player.xDir -= sin(player.angle) * 20.0f * elapsedTime;
		player.yDir -= -cos(player.angle) * 20.0f * elapsedTime;

	}

	player.x += player.xDir * elapsedTime;
	player.y += player.yDir * elapsedTime;

	WrapCoordinates(player.x, player.y, player.x, player.y);

	// Check for player and asteroid collision
	for (auto& ast : asteroids)
	{
		if (PointInsideCircle(ast.x, ast.y, ast.size, player.x, player.y))
			player.dead = true;
	}


	// Shoot bullets
	if (keys[VK_SPACE].isReleased)
	{
		bullets.push_back({ player.x, player.y, 50.0f * sinf(player.angle), -50.0f * cosf(player.angle), 0, 0, false });
	}


	// Draw the Player Ship
	WireFrame(modelShip, player.x, player.y, player.angle);

	if (score > highScore)
		highScore = score;
	// Draw the Player Score
	DrawString(2, 2, L"HIGHSCORE: " + to_wstring(highScore));
	DrawString(2, 4, L"SCORE: " + to_wstring(score));

	return true;
}