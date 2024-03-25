/**
 * @file Globals.h
 * @author your name (you@domain.com)
 * @brief Global variables
 * @version 0.1
 * @date 2024-03-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _GLOBALS_H
#define _GLOBALS_H

const int screenWidth = 1280;
const int screenHeight = 720;
const char *windowTitle = "Swarm";
const char *logoString = "A GAME BY\n\nKEVIN PLUAS";

// Player Globals
int PLAYER_SPEED = 1;

const int MAX_ENEMIES = 50; // The upper capacity on the number of enemies on screen at once. Should only really be used when MemAllocing the array
const int MAX_BULLETS = 10;
int CURRENT_MAX_BULLETS = 1;
int CURRENT_MAX_ENEMIES = 1; // The current capacity. Used for all the other loops.
int currentScore = 0;

Vector2 mousePos;
Vector2 playerV;

Sound gunFx;
Sound impactFx;

Music backgroundSong;
Music introSong;

Image floorBackground;
Image healthTic;
Image crosshairImage;

Texture2D floorTexture;
Texture2D healthTexture;
Texture2D crosshairTexture;
Texture2D zombieSprite;

Vector2 createVector2(int x, int y)
{
    Vector2 v;
    v.x = x;
    v.y = y;

    return v;
}


#endif