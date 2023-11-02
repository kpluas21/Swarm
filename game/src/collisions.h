#ifndef _COLLISIONS_H
#define _COLLISIONS_H

/**
 * @file collisions.h
 * @author Kevin Pluas
 * @brief Collision function declarations
 * @version 0.1
 * @date 2023-11-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "entity.h"
#include<stdlib.h>

typedef struct Entity Entity;
typedef struct PowerUp PowerUp;

//********************************
//
// Function Declarations
//
//********************************

int checkPlayerEnemyCollision(Entity* player, Entity** enemies);
int checkPlayerPowerupCollision(Entity* player, PowerUp* powerup);

int checkBulletEnemyCollision(Entity **bullets, Entity **enemies);
int checkBulletOutOfBounds(Entity** bullets);



int checkPlayerEnemyCollision(Entity *player, Entity **enemies)
{
    for(int i = 0; i < CURRENT_MAX_ENEMIES; i++)
    {
        if(enemies[i] != NULL &&
        CheckCollisionRecs(enemies[i]->body, player->body))
        {
            return 1; //Player was hit by an enemy
        }
    }
    return 0; //Player was not hit by an enemy in this frame
}

int checkPlayerPowerupCollision(Entity *player, PowerUp *powerup)
{
    return 0;
}

int checkBulletEnemyCollision(Entity **bullets, Entity **enemies)
{
    for(int i = 0; i < CURRENT_MAX_ENEMIES; i++)
    {
        for(int j = 0; j < MAX_BULLETS; j++)
        {
            if((enemies[i] != NULL && bullets[j] != NULL) &&
            CheckCollisionRecs(enemies[i]->body, bullets[j]->body))
            {
                
            }
        }
    }
    return 0;
}

int checkBulletOutOfBounds(Entity **bullets)
{
    return 0;
}

#endif //_COLLISIONS_H