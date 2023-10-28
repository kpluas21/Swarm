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

void createBullet(Entity **bullets, Vector2 playerV, Vector2 mouseV);

void updateBullets(Entity **bullets);
void renderBullets(Entity **bullets);

Vector2 createVector2(int x, int y);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    InitWindow(screenWidth, screenHeight, windowTitle);
    SetTargetFPS(60);

    Entity *player = initPlayer();
    Entity **bullets = MemAlloc(sizeof(Entity) * MAX_BULLETS);
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i] = NULL;
    }
    Vector2 mouseV = createVector2(0, 0);
    Vector2 playerV = createVector2(player->body.x, player->body.y);
    bool mousePressedOnFrame = false;

    while (!WindowShouldClose())
    {
        // Input 1 frame
        if (IsKeyDown(KEY_RIGHT))
            player->body.x += player->speed;
        if (IsKeyDown(KEY_LEFT))
            player->body.x -= player->speed;
        if (IsKeyDown(KEY_UP))
            player->body.y -= player->speed;
        if (IsKeyDown(KEY_DOWN))
            player->body.y += player->speed;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            TraceLog(LOG_INFO, TextFormat("X: %d Y: %d", GetMouseX(), GetMouseY()));
            mouseV = createVector2(GetMouseX(), GetMouseY());
            playerV = createVector2(player->body.x, player->body.y);
            createBullet(bullets, playerV, mouseV);
            mousePressedOnFrame = true;
        }
        else
        {
            mousePressedOnFrame = false;
        }

        // Update 1 frame
        updateBullets(bullets);
        // Render 1 frame
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangleRec(player->body, RED);
        renderBullets(bullets);
        if (mousePressedOnFrame == true)
        {
            (DrawLineV(playerV, mouseV, BLUE));
            DrawCircleV(mouseV, 6.5, GREEN);
        }
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

    player->speed = 15;

    return player;
}

void createBullet(Entity **bullets, Vector2 playerV, Vector2 mouseV)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    { // create a single bullet in the first non null space
        if(bullets[i] == NULL)
        {
            Entity *bullet = MemAlloc(sizeof(Entity));
            if (!bullet)
            {
                TraceLog(LOG_ERROR, "Error: Unable to create bullet");
                return;
            }

            bullet->body.height = bullet->body.width = 5;
            bullet->body.x = playerV.x;
            bullet->body.y = playerV.y;
            bullet->speed = 2.0f;
            bullet->health = 1;
            bullet->direction = Vector2Normalize(Vector2Subtract(mouseV, playerV));
            bullets[i] = bullet;
            TraceLog(LOG_INFO, "BULLET CREATED");
            return;
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
            bullets[i]->body.x += bullets[i]->direction.x *bullets[i]->speed;
            bullets[i]->body.y += bullets[i]->direction.y *bullets[i]->speed;
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

Vector2 createVector2(int x, int y)
{
    Vector2 v;
    v.x = x;
    v.y = y;

    return v;
}
