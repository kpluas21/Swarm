/**
 * @file Enemy.h
 * @author your name (you@domain.com)
 * @brief Enemy function declarations and definitions
 * @version 0.1
 * @date 2024-03-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _ENEMY_H
#define _ENEMY_H

#include <stdlib.h>

#include "Structs.h"
#include "Globals.h"

Entity **initEnemies();

void generateNewEnemy(Entity **enemies, Vector2 playerV);
void updateEnemies(Entity **enemies, Vector2 playerV);
void renderEnemies(Entity **enemies);
void clearEnemies(Entity **enemies);

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

    newEnemy->body.height = newEnemy->body.width = 65.5;
    newEnemy->speed = GetRandomValue(1, 5);
    newEnemy->sprite = zombieSprite;

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

float calculateAngle(Vector2 origin, Vector2 target)
{
    return atan2f(target.y - origin.y, target.x - origin.x) * RAD2DEG;
}

void renderEnemies(Entity **enemies)
{
    Vector2 enemyV;
    Vector2 rotationCenter;
    for (int i = 0; i < CURRENT_MAX_ENEMIES; i++)
    {
        if (enemies[i] != NULL)
        {
            enemyV = createVector2(enemies[i]->body.x, enemies[i]->body.y);
            rotationCenter = (Vector2){enemies[i]->body.x + enemies[i]->body.width, enemies[i]->body.height + enemies[i]->body.y };

            DrawRectangle(enemies[i]->body.x, enemies[i]->body.y, enemies[i]->body.width, enemies[i]->body.width, (Color){155, 0, 0, 155});
            DrawTexturePro(enemies[i]->sprite,
                           (Rectangle){0, 0, zombieSprite.width, zombieSprite.height},
                           (Rectangle){rotationCenter.x - enemies[i]->sprite.width / 2, rotationCenter.y - enemies[i]->sprite.height / 2, zombieSprite.width, zombieSprite.height},
                           (Vector2) {zombieSprite.width / 2, zombieSprite.height / 2},
                           calculateAngle(enemyV, playerV),
                           WHITE);
        }
    }
}
void clearEnemies(Entity **enemies)
{
    for (int i = 0; i < CURRENT_MAX_ENEMIES; i++)
    {
        if (enemies[i] != NULL)
        {
            MemFree(enemies[i]);
            enemies[i] = NULL;
            currentScore++;
        }
    }
}

#endif