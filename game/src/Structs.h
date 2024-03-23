/**
 * @file Structs.h
 * @author your name (you@domain.com)
 * @brief Global Structs and enums 
 * @version 0.1
 * @date 2024-03-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _STRUCTS_H
#define _STRUCTS_H

#include "..\..\raylib\src\raylib.h"
#include "..\..\raylib\src\raymath.h"

typedef enum GameScreen
{
    LOGO = 0,
    TITLE,
    GAMEPLAY,
    PAUSE,
    ENDING,
} GameScreen;

typedef enum Effect
{
    MAXBULLETUP = 0,
    ENEMYWIPE,
    INCREASESPEED,
    PLUS10SCORE,
    PLUS50SCORE,
    HEALTHUP,
} Effect;

/**
 * @brief Everything that moves is considered an entity. This includes our player, enemies, and bullets.
 *
 */
typedef struct Entity
{
    int health;
    float speed;
    
    /// The hitbox of the entity
    struct Rectangle body;

    // Used to determine the trajectory of entities such as bullets and enemies
    Vector2 direction; 
    
    Texture2D sprite;
} Entity;

typedef struct PowerUp
{
    Vector2 position;
    Effect effect;
    Color color;
    bool isActive;
} PowerUp;

#endif