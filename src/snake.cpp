/*-----------------------------------------------------------------------------
 **
 ** - Snake on Heltect WifiKit8, based on Proteus -
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

#include "snake.h"

U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 5, /* data=*/ 4, /* reset=*/ 16);
// There are some Heltec 8 Boards, which need the config below.
// I haven't found one yet, but I heard stories of.
// U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 14, /* data=*/ 2, /* reset=*/ 4);

void setup()
{
    // start serial port
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    delay(200);

    // set framerate
    eachFrameMillis = 1000 / 60;

    Serial.println("+ Initialising u8g2");

    // initialise the display connected to the heltec 8 wifi kit and set a default font, just in case
    u8g2.begin();
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_logisoso32_tf);
    u8g2.setFontMode(0);    // enable transparent mode, which is faster

    Serial.println("+ Initialisation done");

    Serial.println("+ Init game");

    resetSnake();
    resetApple();
    currentSpeed = slowDelay;
    addUnit();

    Serial.println("+ Done");
}

void loop()
{
    // enforce framerate
    if(!nextFrame())
    {
        delay(19);
        return;
    }

    handleEvents();

    // Paint the next frame now
    paintFrame();

    Serial.println("- Loop");
}

bool nextFrame()
{
    long now = millis();

    // if it's not time for the next frame yet
    if (now < nextFrameStart) {
        // remaining = nextFrameStart - now;
        // if we have more than 1ms to spare, lets sleep
        // we should be woken up by timer0 every 1ms, so this should be ok
        //if (remaining > 1)
        //    idle();
        return false;
    }

    // define when next render happens
    nextFrameStart = now + eachFrameMillis;
    return true;
}

void paintFrame()
{
    u8g2.firstPage();
    do {
        // check if not gameover. if gameover show "game over" until next click
        if(gameover)
        {
            // score
            u8g2.setFont(u8g2_font_6x10_tf);
            u8g2.drawUTF8(40,10, "Score:");
            u8g2.drawUTF8(80,10, String(score).c_str());

            u8g2.setFont(u8g2_font_logisoso16_tf);
            u8g2.drawUTF8(20,30,"GAME OVER");
        }
        else
        {
            // score
            u8g2.setFont(u8g2_font_6x10_tf);
            u8g2.drawUTF8(40,10, "Score:");
            u8g2.drawUTF8(80,10, String(score).c_str());

            // snake head
            u8g2.drawBox(x, y, UNIT_SIZE, UNIT_SIZE);

            // snake body
            if (unitCount > 0)
            {
                for (byte i = 0; i < unitCount - 1; i++)
                {
                    u8g2.drawBox(body[i].x, body[i].y, UNIT_SIZE, UNIT_SIZE);
                }
            }

            // apple
            u8g2.drawBox(apple.x, apple.y, UNIT_SIZE, UNIT_SIZE);
            u8g2.drawPixel(apple.x + 1, apple.y - 1);
            u8g2.drawPixel(apple.x, apple.y - 2);
        }

    } while ( u8g2.nextPage() );
}

void handleEvents()
{
    lastBitmask = currentBitmask;

    // get current button state
    currentBitmask = ((~PIN_IN) & (bit(0) | bit(12) | bit(13)));

    Serial.print("Bitmask loaded: ");
    Serial.println(currentBitmask);

    if(!gameover)
    {
        updateHead();
        checkCollision();
    }
    else
    {
        // any button restarts the game when the game is over
        if(isRightJustPressed() || isLeftJustPressed())
        {
            resetSnake();
            resetApple();
            currentSpeed = slowDelay;
            addUnit();
            gameover = false;
        }
    }
}

void updateUnits()
{
    int oldPosX, oldPosY, newPosX, newPosY;

    newPosX = x;
    newPosY = y;

    if (unitCount > 0)
    {
        for (byte i = 0; i < MAX_UNITS; i++)
        {
            oldPosX = body[i].x;
            oldPosY = body[i].y;

            body[i].x = newPosX;
            body[i].y = newPosY;
            newPosX = oldPosX;
            newPosY = oldPosY;
        }
    }
}

void checkCollision()
{
    if ((x < 0) | (x > SCREEN_WIDTH - UNIT_SIZE) | (y > SCREEN_HEIGHT - UNIT_SIZE) | (y < 0))
    {
        Serial.println("Left playfield");

        delay(500);
        gameover = true;
    }

    if (unitCount > 0)
    {
        for (byte i = 0; i < unitCount - 1; i++)
        {

            if (collidePointRect(x, y, body[i].x, body[i].y, UNIT_SIZE, UNIT_SIZE))
            {
                Serial.println("Collision");

                delay(500);
                gameover = true;

            }

        }
    }

    if (collideRectRect(apple.x, apple.y, UNIT_SIZE, UNIT_SIZE, x, y, UNIT_SIZE, UNIT_SIZE))
    {
        Serial.println("Apple hit, adding points, resetting apple");

        switch(currentSpeed)
        {
            case slowDelay:
                score += 5;
                break;
            case medDelay:
                score += 10;
                break;
            case fastDelay:
                score += 20;
                break;
            default:
                score += 5;
                break;
        }

        resetApple();
        addUnit();
    }
}

void addUnit()
{
    unitCount++;

    if (unitCount > MAX_UNITS)
    {
        unitCount = MAX_UNITS;
    }
    else if(unitCount == 6)
    {
        currentSpeed = medDelay;
    }
    else if(unitCount == 12)
    {
        currentSpeed = fastDelay;
    }

    Serial.println("Unit added");
}

void resetApple()
{
    apple.x = random(UNIT_SIZE + 1, (SCREEN_WIDTH / UNIT_SIZE) - UNIT_SIZE);
    apple.x *= UNIT_SIZE;
    apple.y = random(UNIT_SIZE + 1, (SCREEN_HEIGHT / UNIT_SIZE) - UNIT_SIZE);
    apple.y *= UNIT_SIZE;

    Serial.println("Apple reset");
}

void updateHead()
{
    if(isRightJustPressed())
    {
        //one right
        direction++;
        Serial.println("To the left");
    }

    if(isLeftJustPressed())
    {
        //one left
        direction--;
        Serial.println("To the right");
    }

    if(direction < 1)
    {
        direction = 4;
    }
    else if(direction > 4)
    {
        direction = 1;
    }

    // TODO:
    // this is wrong, fix me
    moveDelay--;

    if (moveDelay <= 0)
    {
        moveDelay = currentSpeed;

        updateUnits();

        switch (direction) {
            case RIGHT:
                x += moveAmount;
                break;
            case LEFT:
                x -= moveAmount;
                break;
            case UP:
                y -= moveAmount;
                break;
            case DOWN:
                y += moveAmount;
                break;
        }

        /*
        Serial.print("Player x: ");
        Serial.print(x);
        Serial.print(" y: ");
        Serial.println(y);

        Serial.print("Apple x: ");
        Serial.print(apple.x);
        Serial.print(" y: ");
        Serial.println(apple.y);

        Serial.print("Direction: ");
        Serial.print(direction);
        Serial.print(" moveDelay: ");
        Serial.print(moveDelay);
        Serial.print(" currentSpeed: ");
        Serial.print(currentSpeed);
        Serial.print(" moveAmount: ");
        Serial.println(moveAmount);
         */
    }
}

void resetSnake()
{
    Serial.println("Snake reset");

    unitCount = 0;
    score = 0;

    x = START_X;
    y = START_Y;

    direction = RIGHT;
}

bool collidePointRect(uint8_t x1, uint8_t y1 ,uint8_t x2 ,uint8_t y2, uint8_t w, uint8_t h)
{
    if((x1 >= x2) && (x1 < x2 + w))
    {
        if((y1 >= y2) && (y1 < y2 + h))
        {
            /*
            Serial.print("Collision at: ");
            Serial.print(x1);
            Serial.print(" ");
            Serial.println(x2);
             */

            return true;
        }
    }

    return false;
}

bool collideRectRect(uint8_t x1, uint8_t y1, uint8_t w1, uint8_t h1, uint8_t x2, uint8_t y2, uint8_t w2, uint8_t h2)
{
    return !( x2 >= x1 + w1 ||
              x2 + w2 <= x1 ||
              y2 >= y1 + h1 ||
              y2 + h2 <= y1 );
}

boolean pressed(uint16_t buttons)
{
    return (currentBitmask & buttons) == buttons;
}

boolean isRightPressed()
{
    return pressed(RIGHT_BUTTON_BIT);
}
boolean isLeftPressed()
{
    return pressed(LEFT_BUTTON_BIT);
}

boolean justPressed(uint16_t button)
{
    return (!(lastBitmask & button) && (currentBitmask & button));
}
boolean isRightJustPressed()
{
    return justPressed(RIGHT_BUTTON_BIT);
}
boolean isLeftJustPressed()
{
    return justPressed(LEFT_BUTTON_BIT);
}


