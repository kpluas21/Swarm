/*******************************************************************************************
 *
 *   Swarm
 *   A top down shooting game featuring non-stop waves of enemies!
 *
 *   This game has been created using raylib (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
 *
 *   Copyright (c) 2021 Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include "..\..\raylib\src\raylib.h"
#include "..\..\raylib\src\raymath.h"

#include "Globals.h"
#include "Bullet.h"
#include "Structs.h"
#include "Enemy.h"

#include <stdlib.h>
#include <assert.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Local Function Declarations
//----------------------------------------------------------------------------------

Entity *initPlayer();
PowerUp *generatePowerup();

void loadResources();
void renderScreen(Entity *player, Entity **bullets, Entity **enemies, PowerUp *powerup, int frame);
void unloadResources();

void playerMovementInput(Entity *player);

void renderPowerup(PowerUp *powerup);
void changePowerup(PowerUp *powerup);

void renderHUD(Entity *player, int frame, int currentScore);
void resetGame(Entity *player, Entity **bullets, Entity **enemies, int *frame, int *prevScore);

int checkCollisions(
    Entity **enemies,
    Entity **bullets,
    Entity *player,
    PowerUp *powerup,
    int *score);

void cleanupEntities(Entity **bullets, Entity **enemies, Entity *player, PowerUp *PowerUp);

Vector2 createVector2(int x, int y);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialize critical system components
    InitWindow(screenWidth, screenHeight, windowTitle);
    SetTargetFPS(60);
    InitAudioDevice();

    loadResources();

    // Variables
    int frame = 0;
    int previousScore = 0;
    int enemyTimer = 50;

    // Entity initialization
    Entity *player = initPlayer();
    Entity **bullets = initBullets();
    Entity **enemies = initEnemies();
    PowerUp *powerup = generatePowerup();

    GameScreen currentScreen = LOGO;

    PlayMusicStream(backgroundSong);

    //Cursor functions
    HideCursor();

    while (!WindowShouldClose())
    {
        UpdateMusicStream(backgroundSong);

        // UPDATE LOOP
        switch (currentScreen)
        {

        case LOGO:
        {
            frame++;
            if (frame > 120)
            {
                currentScreen = TITLE;
            }
        }
        break;
        case TITLE:
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                currentScreen = GAMEPLAY;
            }
        }
        break;
        case GAMEPLAY:
        {
            //get mouse position for the cursor
            mousePos = GetMousePosition();

            // Pause function
            if (IsKeyPressed(KEY_SPACE))
            {
                currentScreen = PAUSE;
                break;
            }
            // Input 1 frame
            playerMovementInput(player);

            // Update the players vector
            playerV = createVector2(player->body.x, player->body.y);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                createBullet(bullets, playerV, createVector2(GetMouseX(), GetMouseY()));
            }

            if ((currentScore % 5 == 0 && currentScore > 0) && currentScore != previousScore)
            { // Every five kills will increase the max number of enemies possible on screen at
                CURRENT_MAX_ENEMIES++;
                enemyTimer--;
                previousScore = currentScore;
            }
            if (frame % enemyTimer == 0)
            { // every nth frame, where n is enemyTimer, create an enemy
                generateNewEnemy(enemies, playerV);
            }

            if ((frame % 300 == 0) && frame > 0)
            {
                // If the powerup is still on screen and has not been grabbed, shuffle its
                // effect and position
                if (powerup->isActive)
                { // Change the values
                    changePowerup(powerup);
                }
                else
                { // if the powerup is NOT active and the 300th frame has passed, make it active again
                    powerup->isActive = true;
                    changePowerup(powerup);
                }
            }
            // Update 1 frame
            updateBullets(bullets);
            updateEnemies(enemies, playerV);

            // Check collisions 1 frame
            checkBulletCollisions(bullets);

            player->health -= checkCollisions(enemies, bullets, player, powerup, &currentScore);
            if (player->health == 0)
            {
                currentScreen = ENDING;
            }

            frame++;
            break;
        }
        case PAUSE:
        {
            if (IsKeyPressed(KEY_SPACE))
            {
                currentScreen = GAMEPLAY;
                break;
            }
            break;
        }
        case ENDING:
        {
            if (IsKeyPressed(KEY_Y))
            {
                // Restart game
                resetGame(player, bullets, enemies, &frame, &previousScore);
                currentScreen = GAMEPLAY;
            }
            else if (IsKeyPressed(KEY_N))
            {
                goto EXIT;
            }
            break;
        }
        break;
        default:
            break;
        }

        // RENDER LOOP
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            switch (currentScreen)
            {
            case LOGO:
            {
                float textWidth = MeasureText(logoString, 25);
                ClearBackground(BLACK);
                DrawText(logoString, (screenWidth - textWidth) / 2, screenHeight / 2, 25, WHITE);
            }
            break;
            case TITLE:
            {
                char *title = "SWARM\n\n\n\n\n\nClick the Mouse to Begin";
                float textWidth = MeasureText(title, 40);
                ClearBackground(BLACK);
                DrawText(title, (screenWidth - textWidth) / 2, screenHeight / 2, 40, WHITE);
            }
            break;
            case GAMEPLAY:
            {
                renderScreen(player, bullets, enemies, powerup, frame);
            }
            break;
            case PAUSE:
            { // Same as gameplay except with added faded rectangle
                renderScreen(player, bullets, enemies, powerup, frame);
                DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 155});
            }
            break;
            case ENDING:
            {
                DrawTexture(floorTexture, 0, 0, RAYWHITE);
                DrawText(TextFormat("Game Over\n\n\n\nScore: %d", currentScore), screenWidth / 2 - 100, screenHeight / 2, 40, BLACK);
                DrawText(TextFormat("Try again? Y/N"), screenWidth / 2 - 100, screenHeight / 2 - 150, 40, BLACK);
            }
            break;
            default:
            {
            }
            break;
            }
        }

        EndDrawing();
    }

EXIT:
    // CLEAN UP
    cleanupEntities(bullets, enemies, player, powerup);
    unloadResources();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

/**
 * @brief Renders 1 frame of gameplay to the screen.
 *
 * @param player
 * @param bullets
 * @param enemies
 * @param powerup
 * @param frame
 */
void renderScreen(Entity *player, Entity **bullets, Entity **enemies, PowerUp *powerup, int frame)
{
    DrawTexture(floorTexture, 0, 0, RAYWHITE);

    DrawRectangleRec(player->body, RED);
    renderBullets(bullets);
    renderEnemies(enemies);
    renderPowerup(powerup);

    DrawTextureEx(crosshairTexture, mousePos, 0.0, 3.0, WHITE);
    renderHUD(player, frame, currentScore);
}

//----------------------------------------------------------------------------------
// Local Function Definitions
//----------------------------------------------------------------------------------

/**
 * @brief Loads into memory all the resources such as sounds and images.
 *
 */
void loadResources()
{
    // Asset loading

    // SOUNDS
    gunFx = LoadSound("resources/blaster.mp3");
    impactFx = LoadSound("resources/impact.mp3");
    backgroundSong = LoadMusicStream("resources/ambient.ogg");

    // IMAGES
    floorBackground = LoadImage("resources/ground.png");
    healthTic = LoadImage("resources/health_tic.png");
    
    ImageResize(&floorBackground, screenWidth, screenHeight);
    
    floorTexture = LoadTextureFromImage(floorBackground);
    healthTexture = LoadTextureFromImage(healthTic);

    crosshairImage = LoadImage("resources/crosshair.png");
    crosshairTexture = LoadTextureFromImage(crosshairImage);

    zombieSprite = LoadTexture("resources/zombie.png");


}

/**
 * @brief Unloads all resources
 *
 */
void unloadResources()
{
    // SOUNDS
    UnloadSound(gunFx);
    UnloadSound(impactFx);
    UnloadMusicStream(backgroundSong);

    // IMAGES
    UnloadImage(floorBackground);
    UnloadImage(healthTic);
    UnloadTexture(floorTexture);
    UnloadTexture(healthTexture);
}

/**
 * @brief Initializes the player character into memory
 *
 * @return Entity*
 */
Entity *initPlayer()
{
    Entity *player = MemAlloc(sizeof(Entity));
    if (!player)
    {
        TraceLog(LOG_ERROR, "Error: Unable to initialize player");
        return NULL;
    }

    player->body.height = player->body.width = 25;
    player->body.x = screenWidth / 2;
    player->body.y = screenHeight / 2;

    player->health = 3;
    player->speed = 2.5;

    return player;
}

/**
 * @brief Creates a new powerup to be rendered on-screen
 *
 * @return PowerUp*
 */
PowerUp *generatePowerup()
{ // This should only be called once when the game is initialized.
    PowerUp *newPowerup = MemAlloc(sizeof(PowerUp));
    if (newPowerup == NULL)
    {
        TraceLog(LOG_ERROR, "Error initializing powerup");
        return NULL;
    }

    switch (GetRandomValue(MAXBULLETUP, HEALTHUP))
    {
    case MAXBULLETUP:
        newPowerup->color = BLUE;
        newPowerup->effect = MAXBULLETUP;
        break;
    case ENEMYWIPE:
        newPowerup->color = PURPLE;
        newPowerup->effect = ENEMYWIPE;
        break;
    case INCREASESPEED:
        newPowerup->color = GRAY;
        newPowerup->effect = INCREASESPEED;
        break;
    case PLUS10SCORE:
        newPowerup->color = DARKGREEN;
        newPowerup->effect = PLUS10SCORE;
        break;
    case PLUS50SCORE:
        newPowerup->color = GOLD;
        newPowerup->effect = PLUS50SCORE;
        break;
    case HEALTHUP:
        newPowerup->color = RED;
        newPowerup->effect = HEALTHUP;
    default:
        TraceLog(LOG_INFO, "This shouldn't happen!");
        break;
    }

    newPowerup->position.x = GetRandomValue(50, screenWidth - 50);
    newPowerup->position.y = GetRandomValue(50, screenHeight - 50);
    newPowerup->isActive = true;

    return newPowerup;
}

void playerMovementInput(Entity *player)
{
    if (IsKeyDown(KEY_D) && player->body.x < screenWidth - player->body.width)
        player->body.x += player->speed;
    if (IsKeyDown(KEY_A) && player->body.x > 0)
        player->body.x -= player->speed;
    if (IsKeyDown(KEY_W) && player->body.y > 0)
        player->body.y -= player->speed;
    if (IsKeyDown(KEY_S) && player->body.y < screenHeight - player->body.height)
        player->body.y += player->speed;
}

void renderPowerup(PowerUp *powerup)
{
    if (powerup->isActive)
    {
        // TraceLog(LOG_INFO, "POWERUP RENDERED");
        DrawCircle(powerup->position.x, powerup->position.y, 15, powerup->color);
    }
}

void changePowerup(PowerUp *powerup)
{
    switch (GetRandomValue(MAXBULLETUP, PLUS50SCORE))
    {
    case MAXBULLETUP:
        powerup->color = BLUE;
        powerup->effect = MAXBULLETUP;
        break;
    case ENEMYWIPE:
        powerup->color = PURPLE;
        powerup->effect = ENEMYWIPE;
        break;
    case INCREASESPEED:
        powerup->color = GRAY;
        powerup->effect = INCREASESPEED;
        break;
    case PLUS10SCORE:
        powerup->color = DARKGREEN;
        powerup->effect = PLUS10SCORE;
        break;
    case PLUS50SCORE:
        powerup->color = GOLD;
        powerup->effect = PLUS50SCORE;
        break;
    default:
        TraceLog(LOG_INFO, "This shouldn't happen!");
        break;
    }

    powerup->position.x = GetRandomValue(50, screenWidth - 50);
    powerup->position.y = GetRandomValue(50, screenHeight - 50);
    powerup->isActive = true;
}

void renderHUD(Entity *player, int frame, int currentScore)
{
    for (int i = 0; i < player->health; i++)
    {
        DrawTextureEx(healthTexture, (Vector2){i * 30, 50}, 0.0, 6.0, WHITE);
    }
    DrawText(TextFormat("Score: %d\tFrame: %d\tPlayer Speed: %.1f\t Max Bullets: %d",
                        currentScore, frame, player->speed, CURRENT_MAX_BULLETS),
             screenWidth / 2 - 100, screenHeight - 25, 15, BLUE);
}

void resetGame(Entity *player, Entity **bullets, Entity **enemies, int *frame, int *prevScore)
{
    MemFree(player);
    player = initPlayer();

    for (int i = 0; i < CURRENT_MAX_BULLETS; i++)
    {
        if (bullets[i] != NULL)
        {
            MemFree(bullets[i]);
            bullets[i] = NULL;
        }
    }

    clearEnemies(enemies);

    CURRENT_MAX_BULLETS = 1;
    CURRENT_MAX_ENEMIES = 1;

    currentScore = *prevScore = 0;
    *frame = 0;
}

int checkCollisions(Entity **enemies, Entity **bullets, Entity *player, PowerUp *powerup, int *score)
{
    // Check for powerups first, they may possibly change the state of enemies

    if (powerup->isActive && CheckCollisionCircleRec(powerup->position, 15, player->body))
    {
        switch (powerup->effect)
        {
        case ENEMYWIPE:
            clearEnemies(enemies);
            break;
        case INCREASESPEED:
            player->speed += 0.5;
            break;
        case PLUS10SCORE:
            *score += 10;
            break;
        case PLUS50SCORE:
            *score += 50;
            break;
        case MAXBULLETUP:
            CURRENT_MAX_BULLETS++;
            break;
        case HEALTHUP:
            player->health++;
            break;
        default:
            break;
        }
        powerup->isActive = false;
    }

    for (int i = 0; i < CURRENT_MAX_ENEMIES; i++)
    {
        for (int j = 0; j < CURRENT_MAX_BULLETS; j++)
        {
            if (enemies[i] != NULL && bullets[j] != NULL)
                if (CheckCollisionRecs(enemies[i]->body, bullets[j]->body))
                {
                    MemFree(bullets[j]);
                    MemFree(enemies[i]);
                    enemies[i] = NULL;
                    bullets[j] = NULL;
                    PlaySound(impactFx);
                    *score += 1;
                }
        }

        if (enemies[i] != NULL && CheckCollisionRecs(enemies[i]->body, player->body))
        { // The enemy collided with the player. Triggering a game over
            MemFree(enemies[i]);
            enemies[i] = NULL;
            PlaySound(impactFx);
            return 1;
        }
    }
    return 0;
}

void cleanupEntities(Entity **bullets, Entity **enemies, Entity *player, PowerUp *powerup)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i] != NULL)
        {
            MemFree(bullets[i]);
        }
    }
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i] != NULL)
        {
            MemFree(enemies[i]);
        }
    }

    MemFree(bullets);
    MemFree(enemies);
    MemFree(player);
    MemFree(powerup);
}
