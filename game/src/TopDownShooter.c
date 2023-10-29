/*******************************************************************************************
 *
 *   raylib game template
 *
 *   TopDownShooter
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
static const int screenWidth = 800;
static const int screenHeight = 450;
static const char *windowTitle = "TopDownShooter";

static const int MAX_ENEMIES = 20;
static const int MAX_BULLETS = 3;

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

Entity *initPlayer();
Entity **initBullets();
Entity **initEnemies();

void playerMovementInput(Entity *player);

void createBullet(Entity **bullets, Vector2 playerV, Vector2 mouseV);
void generateNewEnemy(Entity** enemies);


void updateBullets(Entity **bullets);
void renderBullets(Entity **bullets);
void checkBulletCollisions(Entity **bullets);

void cleanupEntities(Entity **bullets, Entity **enemies);

Vector2 createVector2(int x, int y);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    InitWindow(screenWidth, screenHeight, windowTitle);
    SetTargetFPS(60);

    Entity *player = initPlayer();
    Entity **bullets = initBullets();
    Entity **enemies = initEnemies();

    Vector2 mouseV = createVector2(0, 0);
    Vector2 playerV = createVector2(player->body.x, player->body.y);

    while (!WindowShouldClose())
    {
        // Input 1 frame
        playerMovementInput(player);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            TraceLog(LOG_INFO, TextFormat("X: %d Y: %d", GetMouseX(), GetMouseY()));
            mouseV = createVector2(GetMouseX(), GetMouseY());
            playerV = createVector2(player->body.x, player->body.y);
            createBullet(bullets, playerV, mouseV);
        }
        // Update 1 frame
        updateBullets(bullets);

        // Check collisions 1 frame
        checkBulletCollisions(bullets);

        // Render 1 frame
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangleRec(player->body, RED);
        renderBullets(bullets);

        EndDrawing();
    }
    CloseWindow();
    MemFree(player);
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

    player->speed = 10;

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

            //Setting the initial stats
            bullet->body.height = bullet->body.width = 10;
            bullet->body.x = playerV.x;
            bullet->body.y = playerV.y;
            bullet->speed = 8.0f;
            bullet->health = 1;
            bullet->direction = Vector2Normalize(Vector2Subtract(mouseV, playerV));
            
            //put bullet in bullets :)
            bullets[i] = bullet;
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
void generateNewEnemy(Entity **enemies)
{
    Entity *newEnemy = MemAlloc(sizeof(Entity));
    bool spaceAvail = false;
    //check for an empty spot in enemies before anything else
    for(int i = 0; i < MAX_ENEMIES; i++)
    {
        if(enemies[i] == NULL) 
        {
            enemies[i] = newEnemy;
            spaceAvail = true;
            break;
        }
    }
    if(!spaceAvail)
    {
        TraceLog(LOG_INFO, "No space for enemy");
        MemFree(newEnemy);
        return;
    }


    newEnemy->body.height = newEnemy->body.width = 25;
    newEnemy->speed = (float)GetRandomValue(5,25);

    //Determining which side of the screen the enemy will spawn from
    switch (GetRandomValue(0, 3))
    {
    //UP
    case 0:
        newEnemy->body.x = GetRandomValue(0, screenWidth - 25);
        newEnemy->body.y = screenHeight - 25;
        break;
    //DOWN
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
        newEnemy->body.y = GetRandomValue(0 , screenHeight - 25);
        break;
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
    {//one of two things happen, either it goes out of bounds , or it collides with an enemy
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
