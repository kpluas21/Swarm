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

Sound gunFx;
Sound impactFx;
Image floorBackground;
Image healthTic;

Texture2D healthTexture;


#endif