/**
 * @file Bullet.h
 * @author your name (you@domain.com)
 * @brief Functions pertaining to bullets
 * @version 0.1
 * @date 2024-03-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _BULLET_H
#define _BULLET_H

#include <stdlib.h>

#include "Structs.h"

//----------------------------------------------------------------------------------
// Function Declarations
//----------------------------------------------------------------------------------
Entity **initBullets();

void createBullet(Entity **bullets, Vector2 playerV, Vector2 mouseV);
void updateBullets(Entity **bullets);
void checkBulletCollisions(Entity **bullets);
void renderBullets(Entity **bullets);

//----------------------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------------------
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
void createBullet(Entity **bullets, Vector2 playerV, Vector2 mouseV)
{
    for (int i = 0; i < CURRENT_MAX_BULLETS; i++)
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
            // TraceLog(LOG_INFO, "BULLET CREATED");
            return;
        }
    }
}
void checkBulletCollisions(Entity **bullets)
{
    for (int i = 0; i < CURRENT_MAX_BULLETS; i++)
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

void updateBullets(Entity **bullets)
{
    for (int i = 0; i < CURRENT_MAX_BULLETS; i++)
    {
        if (bullets[i] != NULL)
        {
            // TraceLog(LOG_INFO, "BULLET UPDATED");
            bullets[i]->body.x += bullets[i]->direction.x * bullets[i]->speed;
            bullets[i]->body.y += bullets[i]->direction.y * bullets[i]->speed;
        }
    }
}

void renderBullets(Entity **bullets)
{
    for (int i = 0; i < CURRENT_MAX_BULLETS; i++)
    {
        if (bullets[i] != NULL)
        {
            // TraceLog(LOG_INFO, "BULLET RENDERED");
            DrawRectangleRec(bullets[i]->body, BLUE);
        }
    }
}
#endif