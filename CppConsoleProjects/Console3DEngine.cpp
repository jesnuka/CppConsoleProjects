#define _USE_MATH_DEFINES
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <chrono>
#include <vector>
#include <utility>
#include <algorithm>
using namespace std;

// Settings
int screenWidth = 120;
int screenHeight = 40;
bool paused = false;

// Player
float playerX = 8.0f;
float playerY = 8.0f;
float playerAngle = 0.0f;

float playerRotationSpeed = 2.0f;
float playerMovementSpeed = 5.0f;

float FOV = M_PI / 4.0;
float depth = 16.0f;

//Map
int mapHeight = 16;
int mapWidth = 16;
int mapMode = 0;

wstring CreateMap(int width, int height, int mapMode);

int main()
{
    // Screen Buffer
    wchar_t* screen = new wchar_t[screenWidth * screenHeight];
    HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(console);
    DWORD dwVBytesWritten = 0;

    // Generate map
    // Map mode 0 = Empty map with only walls
    // Map mode 1 = Map with one long corridor
    wstring map = CreateMap(mapWidth, mapHeight, 1);


    // Frame time
    auto tp1 = chrono::steady_clock::now();
    auto tp2 = chrono::steady_clock::now();

    // Loop Gameplay
    while(!paused) 
    {
        // Frame time
        auto tp2 = chrono::steady_clock::now();
        chrono::duration<float> timeDifference = tp2 - tp1;
        tp1 = tp2;
        float elapsedTime = timeDifference.count();

        // Player Controls

        // Rotate player Left
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
        {
            playerAngle -= playerRotationSpeed * elapsedTime;
        }
        // Rotate player right
        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
        {
            playerAngle += playerRotationSpeed * elapsedTime;
        }

        // Move player forward
        if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
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
        if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
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
        if (GetAsyncKeyState((unsigned short)'E') & 0x8000)
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
        if (GetAsyncKeyState((unsigned short)'Q') & 0x8000)
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
                    if(map[rayY * mapWidth + rayX] == '#')
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
                    screen[y * screenWidth + x] = ' ';
                }
                else if(y > ceilingDistance && y <= floorDistance)
                {
                    // Cell is part of wall
                    screen[y * screenWidth + x] = shadeColor;
                }
                else
                {
                    // Cell is part of floor
                    // Shade it based on distance from player
                   // screen[y * screenWidth + x] = '_';

                    float floorAmount = 1.0f - (((float)y - screenHeight / 2.0f) / (screenHeight / 2.0f));

                    if (floorAmount < 0.25)
                        screen[y * screenWidth + x] = '#';
                    else if (floorAmount < 0.5)
                        screen[y * screenWidth + x] = 'x';
                    else if (floorAmount < 0.75)
                        screen[y * screenWidth + x] = '-';
                    else if (floorAmount < 0.9)
                        screen[y * screenWidth + x] = '.';
                    else
                        screen[y * screenWidth + x] = ' ';


                }
            }
        }

        // Display player coordinates, angle and current FPS
        swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f, FPS=%3.2f ", playerX, playerY, playerAngle, (1.0f/elapsedTime));

        // Draw and display map
        for (int mapX = 0; mapX < mapWidth; mapX++)
        {
            for (int mapY = 0; mapY < mapHeight; mapY++)
            {
                // Draw map, offset by 1 to not cover the other stats
                screen[(mapY + 1)*screenWidth + mapX] = map[mapY * mapWidth + mapX];
            }
        }

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

        screen[(int)(playerY + 1) * screenWidth + (int)playerX] = playerDirection;


        // Set last character as escape character
        screen[screenWidth * screenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(console, screen, screenWidth * screenHeight, { 0,0 }, &dwVBytesWritten);

    }
}

wstring CreateMap(int width, int height, int mapMode)
{
    wstring map;

    switch (mapMode)
    {
        case 0:
            // Create empty map
           for (int i = 0; i < height; i++)
            {
               for (int j = 0; j < width; j++)
               {
                   if (i == 0 || i == (height - 1))
                       map += L"#";
                   else
                   {
                       if(j == 0 || j == (width - 1))
                           map += L"#";
                       else
                           map += L".";
                   }

               }
            }
            break;
        case 1:
            // Create empty map
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    if (i == 0 || i == (height - 1))
                        map += L"#";
                    else
                    {
                        if (j == 0 || j == (width - 1))
                            map += L"#";
                        else
                            map += L".";
                    }

                }
            }

            // Add some walls
            for (int i = 0; i < width - 2; i++)
            {
                map[3 * mapWidth + i] = L'#';
                map[7 * mapWidth + i] = L'#';
                map[11 * mapWidth + i] = L'#';
            }

            for (int i = 2; i < width; i++)
            {
                map[5 * mapWidth + i] = L'#';
                map[9 * mapWidth + i] = L'#';
                map[13 * mapWidth + i] = L'#';
            }

            break;
        default:
            // Create empty map
           for (int i = 0; i < height; i++)
            {
               for (int j = 0; j < width; j++)
               {
                   if (i == 0 || i == (height - 1))
                       map += L"#";
                   else
                   {
                       if(j == 0 || j == (width - 1))
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