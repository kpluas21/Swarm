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

typedef void (*UpdateFunction)(Entity* entity);
/**
 * @brief Represents an entity in the game.
 * 
 * An entity is a game object that has health, speed, a body, direction, and a sprite.
 */
typedef struct Entity
{
    int health;             /**< The health of the entity. */
    float speed;            /**< The speed of the entity. */
    struct Rectangle body;  /**< The body of the entity. */
    Vector2 direction;      /**< The direction of the entity. */
    Texture2D sprite;       /**< The sprite of the entity. */
    UpdateFunction update;  /**< The update function of the entity. */
    
} Entity;
typedef struct Entity
{
    int health;
    float speed;
    struct Rectangle body;
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
    Texture2D sprite;
} PowerUp;

typedef struct Level
{
    int EnemySpawnInterval;
    int scoreReq;
    int maxEnemies;
    Texture2D floorTexture;
    
}Level;

// Linked List of Entities. Used for bullets and enemies
typedef struct EntityLL
{
    Entity *entity;
    struct EntityLL *next;
} EntityLL;

#endif