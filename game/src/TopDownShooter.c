/*******************************************************************************************
 *
 *   raylib game template
 *
 *   Swarm
 *   <Game description>
 *
 *   This game has been created using raylib (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
 *
 *   Copyright (c) 2021 Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
// Window Globals
typedef enum GameScreen
{
    LOGO = 0,
    TITLE,
    GAMEPLAY,
    PAUSE,
    ENDING,
} GameScreen;

static const int screenWidth = 1280;
static const int screenHeight = 720;
static const char *windowTitle = "Swarm";
static const char *logoString = "A GAME BY\nKEVIN PLUAS";

// Player Globals
static int PLAYER_SPEED = 1;

static const int MAX_ENEMIES = 50; // The upper capacity on the number of enemies on screen at once. Should only really be used when MemAllocing the array
static const int MAX_BULLETS = 1;
static int CURRENT_MAX_ENEMIES = 5; // The current capacity. Used for all the other loops.
static Sound gunFx;
static Image floorBackground;

//----------------------------------------------------------------------------------
// Local Structs Declaration
//----------------------------------------------------------------------------------

/**
 * @brief Everything that moves is considered an entity. This includes our player, enemies, and bullets.
 *
 */
typedef struct Entity
{
    int health;
    float speed;
    struct Rectangle body;
    Vector2 direction; // Used for the bullets to calculate their trajectory
} Entity;

//----------------------------------------------------------------------------------
// Local Function Declarations
//----------------------------------------------------------------------------------

void titleScreen();

Entity *initPlayer();
Entity **initBullets();
Entity **initEnemies();

void playerMovementInput(Entity *player);

void createBullet(Entity **bullets, Vector2 playerV, Vector2 mouseV);
void generateNewEnemy(Entity **enemies, Vector2 playerV);
void updateEnemies(Entity **enemies, Vector2 playerV);
void renderEnemies(Entity **enemies);

void updateBullets(Entity **bullets);
void renderBullets(Entity **bullets);
void checkBulletCollisions(Entity **bullets);

int checkCollisions(Entity **enemies, Entity **bullets, Entity *player, int *score);

void cleanupEntities(Entity **bullets, Entity **enemies);

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

    // Asset loading
    gunFx = LoadSound("resources/blaster.mp3");
    floorBackground = LoadImage("resources/ground.png");
    ImageResize(&floorBackground, screenWidth, screenHeight);
    Texture2D texture = LoadTextureFromImage(floorBackground);

    // Variables
    int frame = 0;
    int endingFrame;
    int previousScore = 0;
    int currentScore = 0;

    // Entity initialization
    Entity *player = initPlayer();
    Entity **bullets = initBullets();
    Entity **enemies = initEnemies();

    Vector2 playerV;

    GameScreen currentScreen = LOGO;

    while (!WindowShouldClose())
    {
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
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                currentScreen = GAMEPLAY;
            }
        }
        break;
        case GAMEPLAY:
        {
            //Pause function
            if(IsKeyPressed(KEY_SPACE))
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


            if ((currentScore % 10 == 0 && currentScore > 0) && currentScore != previousScore)
            { // Every ten kills will increase the max number of enemies possible on screen at
                CURRENT_MAX_ENEMIES++;
                previousScore = currentScore;
            }
            if (frame % 30 == 0)
            {
                generateNewEnemy(enemies, playerV);
            }
            // Update 1 frame
            updateBullets(bullets);
            updateEnemies(enemies, playerV);

            // Check collisions 1 frame
            checkBulletCollisions(bullets);
            if (checkCollisions(enemies, bullets, player, &currentScore) == 1)
            {
                currentScreen = ENDING;
                endingFrame = frame;
                break;
            }

            frame++;
            break;
        }
        case PAUSE: 
        {
            if(IsKeyPressed(KEY_SPACE))
            {
                currentScreen = GAMEPLAY;
                break;
            }
            break;
        }
        case ENDING:
        {
            frame++;
            if((frame - endingFrame) >= 240)
            {
                //Goes straight to cleanup, terminating the program.
                goto EXIT;
            }
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
                char*  title = "SWARM\n\n\n\n\n\nClick the Mouse to Begin";
                float textWidth = MeasureText(title, 40);
                ClearBackground(BLACK);
                DrawText(title, (screenWidth - textWidth) / 2, screenHeight / 2, 40, WHITE);
            }
                break;
            case GAMEPLAY:
            {
                DrawTexture(texture, 0, 0, RAYWHITE);
                DrawRectangleRec(player->body, RED);
                renderBullets(bullets);
                renderEnemies(enemies);
                DrawText(TextFormat("Score: %d\tFrame: %d", currentScore, frame), screenWidth / 2 - 50, screenHeight - 25, 15, BLUE);
            }
            break;
            case PAUSE: 
            {//Same as gameplay except with added faded rectangle 
                DrawTexture(texture, 0, 0, RAYWHITE);
                DrawRectangleRec(player->body, RED);
                renderBullets(bullets);
                renderEnemies(enemies);
                DrawText(TextFormat("Score: %d\tFrame: %d", currentScore, frame), screenWidth / 2 - 50, screenHeight - 25, 15, BLUE);
                DrawRectangle(0, 0, screenWidth, screenHeight, (Color) {0, 0, 0, 155});
                break;
            }
            case ENDING:
            {
                DrawTexture(texture, 0, 0, RAYWHITE);
                DrawText(TextFormat("Game Over\n\n\n\nScore: %d", currentScore), screenWidth / 2 - 100, screenHeight / 2, 40, BLACK);
            }
                break;
            default:
                break;
            }
        }

        EndDrawing();
    }


EXIT:
    // CLEAN UP
    cleanupEntities(bullets, enemies);
    MemFree(enemies);
    MemFree(bullets);
    MemFree(player);
    UnloadSound(gunFx);
    UnloadImage(floorBackground);
    UnloadTexture(texture);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

//----------------------------------------------------------------------------------
// Local Function Definitions
//----------------------------------------------------------------------------------

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

    player->speed = 1;

    return player;
}

Entity **initBullets()
{
    Entity **bullets = MemAlloc(sizeof(Entity) * MAX_BULLETS);
    if (bullets == NULL)
    {
        TraceLog(LOG_ERROR, "Error initializing bullets");
        return NULL;
    }
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i] = NULL;
    }

    return bullets;
}

Entity **initEnemies()
{
    Entity **enemies = MemAlloc(sizeof(Entity) * MAX_ENEMIES);
    if (enemies == NULL)
    {
        TraceLog(LOG_ERROR, "Error initializing enemies");
        return NULL;
    }

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i] = NULL;
    }
    return enemies;
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

void createBullet(Entity **bullets, Vector2 playerV, Vector2 mouseV)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    { // create a single bullet in the first non null space
        if (bullets[i] == NULL)
        {
            Entity *bullet = MemAlloc(sizeof(Entity));
            if (!bullet)
            {
                TraceLog(LOG_ERROR, "Error: Unable to create bullet");
                return;
            }

            // Setting the initial stats
            bullet->body.height = bullet->body.width = 10;
            bullet->body.x = playerV.x + 5;
            bullet->body.y = playerV.y + 5;
            bullet->speed = 8.0f;
            bullet->health = 1;
            bullet->direction = Vector2Normalize(Vector2Subtract(mouseV, playerV));

            // put bullet in bullets :)
            bullets[i] = bullet;
            PlaySound(gunFx);
            TraceLog(LOG_INFO, "BULLET CREATED");
            return;
        }
    }
}

/**
 * @brief Generate a new enemy and initialize its stats. Also generate
 * a direction vector towards the player
 *
 * @param enemies
 */
void generateNewEnemy(Entity **enemies, Vector2 playerV)
{
    Entity *newEnemy = MemAlloc(sizeof(Entity));
    bool spaceAvail = false;
    // check for an empty spot in enemies before anything else
    for (int i = 0; i < CURRENT_MAX_ENEMIES; i++)
    {
        if (enemies[i] == NULL)
        {
            enemies[i] = newEnemy;
            spaceAvail = true;
            break;
        }
    }
    if (!spaceAvail)
    {
        TraceLog(LOG_INFO, "No space for enemy");
        MemFree(newEnemy);
        return;
    }

    newEnemy->body.height = newEnemy->body.width = 25;
    newEnemy->speed = 1;

    // Determining which side of the screen the enemy will spawn from
    switch (GetRandomValue(0, 3))
    {
    // UP
    case 0:
        newEnemy->body.x = GetRandomValue(0, screenWidth - 25);
        newEnemy->body.y = screenHeight - 25;
        break;
    // DOWN
    case 1:
        newEnemy->body.x = GetRandomValue(0, screenWidth - 25);
        newEnemy->body.y = 0;
        break;
    case 2:
        newEnemy->body.x = 0;
        newEnemy->body.y = GetRandomValue(0, screenHeight - 25);
        break;
    case 3:
        newEnemy->body.x = screenWidth - 25;
        newEnemy->body.y = GetRandomValue(0, screenHeight - 25);
        break;
    }

    // In this frame, generate a direction vector towards the player
    newEnemy->direction = Vector2Normalize(Vector2Subtract(playerV, createVector2(newEnemy->body.x, newEnemy->body.y)));

    return;
}

void updateEnemies(Entity **enemies, Vector2 playerV)
{ // In one frame, advance the enemies towards the player.
    for (int i = 0; i < CURRENT_MAX_ENEMIES; i++)
    { // go through every enemy
        if (enemies[i] != NULL)
        { //
            enemies[i]->direction = Vector2Normalize(Vector2Subtract(playerV, createVector2(enemies[i]->body.x, enemies[i]->body.y)));
            enemies[i]->body.y += enemies[i]->direction.y * enemies[i]->speed;
            enemies[i]->body.x += enemies[i]->direction.x * enemies[i]->speed;
        }
    }
}

void renderEnemies(Entity **enemies)
{
    for (int i = 0; i < CURRENT_MAX_ENEMIES; i++)
    {
        if (enemies[i] != NULL)
        {
            DrawRectangleRec(enemies[i]->body, GREEN);
        }
    }
}

void updateBullets(Entity **bullets)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i] != NULL)
        {
            TraceLog(LOG_INFO, "BULLET UPDATED");
            bullets[i]->body.x += bullets[i]->direction.x * bullets[i]->speed;
            bullets[i]->body.y += bullets[i]->direction.y * bullets[i]->speed;
        }
    }
}

void renderBullets(Entity **bullets)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i] != NULL)
        {
            TraceLog(LOG_INFO, "BULLET RENDERED");
            DrawRectangleRec(bullets[i]->body, BLUE);
        }
    }
}

void checkBulletCollisions(Entity **bullets)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    { // one of two things happen, either it goes out of bounds , or it collides with an enemy
        if (bullets[i] != NULL)
        {
            if (
                bullets[i]->body.x > screenWidth ||
                bullets[i]->body.x < 0 ||
                bullets[i]->body.y > screenHeight ||
                bullets[i]->body.y < 0)
            {
                TraceLog(LOG_INFO, "BULLET DESTROYED");
                MemFree(bullets[i]);
                bullets[i] = NULL;
            }
        }
    }
}

int checkCollisions(Entity **enemies, Entity **bullets, Entity *player, int *score)
{
    for (int i = 0; i < CURRENT_MAX_ENEMIES; i++)
    {
        for (int j = 0; j < MAX_BULLETS; j++)
        {
            if (enemies[i] != NULL && bullets[j] != NULL)
                if (CheckCollisionRecs(enemies[i]->body, bullets[j]->body))
                {
                    MemFree(bullets[j]);
                    MemFree(enemies[i]);
                    enemies[i] = NULL;
                    bullets[j] = NULL;

                    *score += 1;
                }
        }

        if (enemies[i] != NULL && CheckCollisionRecs(enemies[i]->body, player->body))
        { // The enemy collided with the player. Triggering a game over

            return 1;
        }
    }
}

void cleanupEntities(Entity **bullets, Entity **enemies)
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
}

Vector2 createVector2(int x, int y)
{
    Vector2 v;
    v.x = x;
    v.y = y;

    return v;
}
