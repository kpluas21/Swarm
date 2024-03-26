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


#ifdef __unix__
    #include "raylib.h"
    #include "raymath.h"
#endif

#ifdef defined(_WIN32) || defined(WIN32)
    #include "..\..\raylib\src\raylib.h"
    #include "..\..\raylib\src\raymath.h"
#endif

typedef enum GameScreen
{
    LOGO = 0,
    TITLE,
    GAMEPLAY,
    PAUSE,
    ENDING,
} GameScreen;

/**
 * @brief
 *
 */
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
 * @brief
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

/**
 * @brief  PowerUp struct
 *
 */
typedef struct PowerUp
{
    Vector2 position;
    Effect effect;
    Color color;
    bool isActive;
} PowerUp;

#endif