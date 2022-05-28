#include "ColoredConsoleFPS.h"
#include "ConsoleEngine.h"

// Based on Ultimate Console FPS by @Javidx9 at OneLoneCoder.com 


wstring ColoredConsoleFPS::CreateMap(int mapWidth, int mapHeight, int mapMode)
{
    wstring map;

    switch (mapMode)
    {
    case 0:
        // Create empty map
        for (int i = 0; i < mapHeight; i++)
        {
            for (int j = 0; j < mapWidth; j++)
            {
                if (i == 0 || i == (mapHeight - 1))
                    map += L"#";
                else
                {
                    if (j == 0 || j == (mapWidth - 1))
                        map += L"#";
                    else
                        map += L".";
                }

            }
        }
        break;
    case 1:
        // Create empty map
        for (int i = 0; i < mapHeight; i++)
        {
            for (int j = 0; j < mapWidth; j++)
            {
                if (i == 0 || i == (mapHeight - 1))
                    map += L"#";
                else
                {
                    if (j == 0 || j == (mapWidth - 1))
                        map += L"#";
                    else
                        map += L".";
                }

            }
        }

        // Add some walls
        for (int i = 0; i < mapWidth - 2; i++)
        {
            map[3 * mapWidth + i] = L'#';
            map[7 * mapWidth + i] = L'#';
            map[11 * mapWidth + i] = L'#';
        }

        for (int i = 2; i < mapWidth; i++)
        {
            map[5 * mapWidth + i] = L'#';
            map[9 * mapWidth + i] = L'#';
            map[13 * mapWidth + i] = L'#';
        }

        break;
    default:
        // Create empty map
        for (int i = 0; i < mapHeight; i++)
        {
            for (int j = 0; j < mapWidth; j++)
            {
                if (i == 0 || i == (mapHeight - 1))
                    map += L"#";
                else
                {
                    if (j == 0 || j == (mapWidth - 1))
                        map += L"#";
                    else
                        map += L".";
                }

            }
        }
        break;
    }

    return map;
}

bool ColoredConsoleFPS::OnUserCreate()
{
    map = CreateMap(mapWidth, mapHeight, 1);
    return true;
}

bool ColoredConsoleFPS::OnUserUpdate(float elapsedTime)
{
    // Rotate player Left
    if (keys[VK_LEFT].isHeld || keys[0x41].isHeld)
    {
        playerAngle -= playerRotationSpeed * elapsedTime;
    }
    // Rotate player right
    if (keys[VK_RIGHT].isHeld || keys[0x44].isHeld)
    {
        playerAngle += playerRotationSpeed * elapsedTime;
    }

    // Move player forward
    if (keys[VK_UP].isHeld || keys[0x57].isHeld)
    {
        playerX += cosf(playerAngle) * playerMovementSpeed * elapsedTime;
        playerY += sinf(playerAngle) * playerMovementSpeed * elapsedTime;

        // Collision detection, check if next tile is wall or not, by rounding player coordinates to integer
        if (map[(int)playerY * mapWidth + (int)playerX] == '#')
        {
            playerX -= cosf(playerAngle) * playerMovementSpeed * elapsedTime;
            playerY -= sinf(playerAngle) * playerMovementSpeed * elapsedTime;
        }
    }

    //Move player backward
    if (keys[VK_DOWN].isHeld || keys[0x53].isHeld)
    {
        playerX -= cosf(playerAngle) * playerMovementSpeed * elapsedTime;
        playerY -= sinf(playerAngle) * playerMovementSpeed * elapsedTime;

        // Collision detection, check if next tile is wall or not, by rounding player coordinates to integer
        if (map[(int)playerY * mapWidth + (int)playerX] == '#')
        {
            playerX += cosf(playerAngle) * playerMovementSpeed * elapsedTime;
            playerY += sinf(playerAngle) * playerMovementSpeed * elapsedTime;
        }
    }

    // Strafe player right
    if (keys[0x51].isHeld || (keys[VK_RIGHT].isHeld && keys[VK_SHIFT].isHeld))
    {
        playerY += cosf(playerAngle) * playerMovementSpeed * elapsedTime;
        playerX += sinf(playerAngle) * playerMovementSpeed * elapsedTime;

        // Collision detection, check if next tile is wall or not, by rounding player coordinates to integer
        if (map[(int)playerY * mapWidth + (int)playerX] == '#')
        {
            playerY -= cosf(playerAngle) * playerMovementSpeed * elapsedTime;
            playerX -= sinf(playerAngle) * playerMovementSpeed * elapsedTime;
        }
    }

    // Strafe player left
    if (keys[0x45].isHeld || (keys[VK_LEFT].isHeld && keys[VK_SHIFT].isHeld))
    {
        playerY -= cosf(playerAngle) * playerMovementSpeed * elapsedTime;
        playerX -= sinf(playerAngle) * playerMovementSpeed * elapsedTime;

        // Collision detection, check if next tile is wall or not, by rounding player coordinates to integer
        if (map[(int)playerY * mapWidth + (int)playerX] == '#')
        {
            playerY += cosf(playerAngle) * playerMovementSpeed * elapsedTime;
            playerX += sinf(playerAngle) * playerMovementSpeed * elapsedTime;
        }
    }


    for (int x = 0; x < screenWidth; x++)
    {
        // Calculate FOV / 2 and sum the current part of the screenWidth
        float rayAngle = (playerAngle - FOV / 2.0f) + ((float)(x / (float)screenWidth) * FOV);

        float distToWall = 0.0f;
        bool wallHit = false;
        bool cellBoundary = false; // Has cell edge been found

        float playerViewX = cosf(rayAngle);
        float playerViewY = sinf(rayAngle);

        // Raycast to wall, when we reach a "Wall" block == #, we stop, OR if depth value is reached
        while (!wallHit && distToWall < depth)
        {
            distToWall += 0.1f;

            int rayX = (int)(playerX + playerViewX * distToWall);
            int rayY = (int)(playerY + playerViewY * distToWall);

            // Out of Bounds check
            if (rayX >= mapWidth || rayY >= mapHeight || rayX < 0 || rayY < 0)
            {
                // Set distance to maximum
                wallHit = true;
                distToWall = depth;
            }
            else
            {
                // Check for wall block
                if (map[rayY * mapWidth + rayX] == '#')
                {
                    wallHit = true;

                    // Contains distance and dot, of the four corners of a block
                    vector<pair<float, float>> corners;

                    // Loop through 4 corners
                    for (int k = 0; k < 2; k++)
                    {
                        for (int l = 0; l < 2; l++)
                        {
                            // Create vector of the corner
                            float vecx = (float)rayX + k - playerX;
                            float vecy = (float)rayY + l - playerY;

                            // Distance to corner
                            float dist = sqrt(vecx * vecx + vecy * vecy);

                            // Dot of unit vector of the corner and ray being cast
                            float dot = (playerViewX * vecx / dist) + (playerViewY * vecy / dist);

                            corners.push_back(make_pair(dist, dot));
                        }
                    }

                    // Find closest corners from vector by sorting from closest to farthest
                    // Compare pair distances using the lambda function
                    sort(corners.begin(), corners.end(), [](const pair<float, float>& a, const pair<float, float>& b) {return a.first < b.first; });

                    float minAngle = 0.005f;
                    // Check if ray angles are close enough, which means ray hits boundary of the cell, the edge

                    // Two closest corners is enough
                    if (acos(corners[0].second) < minAngle)
                        cellBoundary = true;
                    if (acos(corners[1].second) < minAngle)
                        cellBoundary = true;
                    // if (acos(corners[2].second) < minAngle)
                     //     cellBoundary = true;
                }
            }

        }

        // Calculate ceiling and floor distance
        int ceilingDistance = (float)(screenHeight / 2.0) - screenHeight / (float)distToWall;
        int floorDistance = screenHeight - ceilingDistance;

        // Shading
        short shadeColor = ' ';

        // ASCII Shaded Characters, change depending on distance to wall

        if (distToWall <= depth / 4.0f) // Close to wall
            shadeColor = 0x2588;
        else if (distToWall < depth / 3.0f)
            shadeColor = 0x2593;
        else if (distToWall < depth / 2.0f)
            shadeColor = 0x2592;
        else if (distToWall < depth)
            shadeColor = 0x2591;
        else                        // Far away from wall, don't draw
            shadeColor = ' ';

        if (cellBoundary)
            shadeColor = ' ';

        // Draw column
        for (int y = 0; y < screenHeight; y++)
        {
            // Cell is part of the ceiling
            if (y < ceilingDistance)
            {
                Draw(x, y, L' ');
            }
            else if (y > ceilingDistance && y <= floorDistance)
            {
                // Cell is part of wall
                Draw(x, y, shadeColor);
            }
            else
            {
                // Cell is part of floor
                // Shade it based on distance from player
               // screen[y * screenWidth + x] = '_';

                float floorAmount = 1.0f - (((float)y - screenHeight / 2.0f) / (screenHeight / 2.0f));

                if (floorAmount < 0.25)
                    Draw(x, y, L'#');
                else if (floorAmount < 0.5)
                    Draw(x, y, L'x');
                else if (floorAmount < 0.75)
                    Draw(x, y, L'-');
                else if (floorAmount < 0.9)
                    Draw(x, y, L'.');
                else
                    Draw(x, y, L' ');
            }
        }
    }
    // Draw map, offset by 1 to not cover the other stats
    for (int mapX = 0; mapX < mapWidth; mapX++)
        for (int mapY = 0; mapY < mapHeight; mapY++)
            Draw(mapX, (mapY + 1), map[mapY * mapWidth + mapX]);

    // Draw and display player direction and location on the map
    wchar_t playerDirection = ' ';

    if (cosf(playerAngle) < sinf(playerAngle) && cosf(playerAngle) > sinf(playerAngle + M_PI))
        playerDirection = 'v';
    else if (sinf(playerAngle + M_PI) > cosf(playerAngle) && sinf(playerAngle + M_PI) > cosf(playerAngle + M_PI))
        playerDirection = '^';
    else if (cosf(playerAngle + M_PI) > sinf(playerAngle) && cosf(playerAngle + M_PI) > sinf(playerAngle))
        playerDirection = '<';
    else
        playerDirection = '>';

    Draw(playerX, (int)(playerY + 1), playerDirection);

    return true;
}
