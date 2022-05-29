/*-----------------------------------------------------------------------------
 **
 ** - Hello World on Heltect WifiKit8, based on Proteus -
 **
 ** Copyright 2018-22 by Krogoth and the contributing authors
 **
 ** This program is free software; you can redistribute it and/or modify it
 ** under the terms of the GNU Affero General Public License as published by the
 ** Free Software Foundation, either version 3 of the License, or (at your option)
 ** any later version.
 **
 ** This program is distributed in the hope that it will be useful, but WITHOUT
 ** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 ** FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License
 ** for more details.
 **
 ** You should have received a copy of the GNU Affero General Public License
 ** along with this program. If not, see <http://www.gnu.org/licenses/>.
 **
 ** -----------------------------------------------------------------------------*/

#ifndef MC_SNAKE_SNAKE_H
#define MC_SNAKE_SNAKE_H

#include <Arduino.h>
#include <U8g2lib.h>

/*
 * Search for a file called "*eagle.app.v6.*.ld"
 * It usually is at:
 * /home/user/.platformio/packages/framework-arduinoespressif8266/tools/sdk/ld/eagle.app.v6.common.ld
 * Then add this line:
 * PROVIDE(PIN_IN = 0x60000318);
 * accessing PIN_IN will give you access to the register
 * with all the pins current states
 * */
extern volatile uint32_t PIN_IN;

#define PRG_BUTTON 0
#define RIGHT_BUTTON 12
#define LEFT_BUTTON 13

#define PRG_BUTTON_BIT bit(PRG_BUTTON)
#define RIGHT_BUTTON_BIT bit(RIGHT_BUTTON)
#define LEFT_BUTTON_BIT bit(LEFT_BUTTON)

uint16_t currentBitmask = 0;
uint16_t lastBitmask = 0;

uint8_t eachFrameMillis;
long nextFrameStart;
bool nextFrame();
void paintFrame();

static const byte RIGHT = 1;
static const byte DOWN = 2;
static const byte LEFT = 3;
static const byte UP = 4;
byte direction = RIGHT;             // the direction into which the snake is moving

// screen size
static const uint8_t SCREEN_WIDTH = 128;
static const uint8_t SCREEN_HEIGHT = 32;

// indicator if the game is over
bool gameover = false;

// can't be uint, since we could leave the board...
int8_t x = 0;
int8_t y = 0;

uint16_t score = 0;      // user score
byte unitCount = 0;     // active number of elements behind the snakes head
byte moveAmount = 3;    // pixel number to move the head per step

static const uint8_t UNIT_SIZE = 3;

static const uint8_t slowDelay = 4;
static const uint8_t medDelay = 2;
static const uint8_t fastDelay = 1;

uint8_t currentSpeed = slowDelay;   // the amount to set moveDelay when resetting
uint8_t moveDelay = slowDelay;      // the sleep steps currently used in the game until we move the snake again

static const byte MAX_UNITS = 200  ; //the maximum body segments you can have
static const byte START_X = 4 * UNIT_SIZE;
static const byte START_Y = 8 * UNIT_SIZE;

// unit object (lightweight c++ object, look up struct)
class Unit {
public:
    uint8_t x, y;
};

// the body elements of the snake
Unit body[MAX_UNITS]; //makes maxUnits number of body segments.

// the apple the snake needs to get
Unit apple;

void updateUnits();
void checkCollision();
void addUnit();
void resetApple();
void updateHead();
void resetSnake();

bool collidePointRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t w, uint8_t h);
bool collideRectRect(uint8_t x1, uint8_t y1, uint8_t w1, uint8_t h1, uint8_t x2, uint8_t y2, uint8_t w2, uint8_t h2);

void handleEvents();

boolean pressed();
boolean isRightPressed();
boolean isLeftPressed();

boolean justPressed(uint16_t button);
boolean isRightJustPressed();
boolean isLeftJustPressed();


#endif //MC_SNAKE_SNAKE_H
