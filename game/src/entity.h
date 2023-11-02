/**
 * @file entity.h
 * @author Kevin Pluas
 * @brief Function and struct declarations for Entity
 * @version 0.1
 * @date 2023-11-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _ENTITY_H
#define _ENTITY_H

#include "raylib.h"

int MAX_ENEMIES = 50;
int CURRENT_MAX_ENEMIES = 5;
int MAX_BULLETS = 1;

typedef enum Effect
{
    MAXBULLETUP = 0,
    ENEMYWIPE,
    INCREASESPEED,
    PLUS10SCORE,
    PLUS50SCORE,
} Effect;

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

/**
 * @brief 
 * 
 */
typedef struct PowerUp
{
    Vector2 position;
    Effect effect;
    Color color;
} PowerUp;
#endif //_ENTITY_H