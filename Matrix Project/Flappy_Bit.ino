#include <LiquidCrystal.h>
#include "LedControl.h"
#include <string.h>
#include "letters.h"
#include <EEPROM.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

LedControl lc = LedControl(12, 11, 10, 1);

// menu variables

const int pinSW = 8;
const int pinX = A0;
const int pinY = A1;
int switchValue = 0;
int xValue = 0;
int yValue = 0;
int select = 0;
int swJ = 1;
int swB = 1;
int pressed = 0;
int level = 0;
int prevLevel = 0;
int score = 0;
int scoreShow = 0;
int lives = 3;
bool endGame = 0;
String player = "X", playerName = "Anonymus";
String a;
char inChar;
byte index = 0;
bool name_introduced = 0;
bool showOnce = 0;

// matrix variables

bool sw = 0;
int swName = 1;
int jump = 0;
int wallsSpeed = 250;
long timer = 0;
long timer2 = 0;
long timer3 = 0;
long timer4 = 0;
long timer5 = 0;
long timer6 = 0;
long timer7 = 0;
long game_start = 0;
long game_start2 = 0;

bool game_over = 0;
int flappyX = 6;
int flappyY = 3;

int wallsX = 0;
int wallsX2 = 0;
int hole = rand() % 6;
int hole2 = rand() % 6;

int i = 0;
int ind = 0;
int nameLength;

byte text[9][5]= {G, A, M, E, O, V, E, R, EX};
int len = sizeof(text) / 5;
int pos[18]= {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

//*********************************************************************************

void setup()
{
    lc.shutdown(0, false);
    lc.setIntensity(0, 2);
    lc.clearDisplay(0);
    lc.setLed(false, flappyX, flappyY, true);

    lcd.begin(16, 2);
    pinMode(pinSW, INPUT_PULLUP);
    Serial.begin(9600);
}

void show_menu()
{
    lcd.setCursor(2, 0);
    lcd.print("Start");
    lcd.setCursor(9, 0);
    lcd.print("Setup");
    lcd.setCursor(3, 1);
    lcd.print("HighScore");
}

int joyStickX(int i, int maxNr)
{
    if(xValue < 50 && swJ == 1)
    {
        i++;
        swJ = 0;
        i = i % maxNr;
    }

    if(xValue >950 && swJ == 1)
    {
        if(i == 0)
            i = maxNr - 1;
        else
            i--;
        swJ = 0;
        i = i % maxNr;
    }

    if(xValue > 500 && xValue < 600)
        swJ = 1;
    return i;
}

//**************************************************** MATRIX FUNCTIONS


void gameOverText()
{
    for (int letter = 0; letter < len; letter++)
        for (int shift = 0; shift <= 13; shift++)
        {
            for (int i = 0; i < 5; i++)
                lc.setRow(0, pos[i + shift], text[letter][i]);

            delay(10); // speed of animation
            lc.clearDisplay(0);
        }


    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            lc.setLed(false, i, j, true);
            delay(2);
        }

}

void hiScoreName()
{
    for(int i = 0; i <= 16; i++)
        EEPROM.write(i, 0);

    i = 0;

    Serial.println(playerName);
    Serial.println(nameLength);

    while (nameLength != 0)
    {
        EEPROM.write(i, playerName[i]);
        i++;
        nameLength--;
    }
}

void flappyMove()
{
    switchValue = !digitalRead(pinSW);

    if (switchValue == 1 && sw == 1)
    {
        jump = 1;
        sw = 0;
    }
    else
        jump = 0;
    if (switchValue == 0)
        sw = 1;

    lc.setLed(false, flappyX, flappyY, false);

    if (jump == 1 && flappyY < 1)//asta e egal cu
        flappyY = 0;
    else if (jump == 1)
    {
        flappyY -= 1;//asta
        timer = millis();
    }
    else if (flappyY != 7 && timer + 250 < millis())
    {
        flappyY++;
        timer = millis();
    }
    if (millis() - timer7 < 2000)
    {
        if (millis() % 250 > 125)
            lc.setLed(false, flappyX, flappyY, true);
        else
            lc.setLed(false, flappyX, flappyY, false);
    }
    else
        lc.setLed(false, flappyX, flappyY, true);
}


void walls(int &wallsX, long &timer2, int &hole, int Size)
{
    if (wallsX != 10 && timer2 + wallsSpeed < millis())
    {
        for (int i = 0; i < 8; i++)
        {
            if (Size == 3)
            {
                if (i != hole && i != hole + 1 && i != hole + 2)
                {
                    lc.setLed(false, wallsX, i, true);
                    lc.setLed(false, wallsX - 1, i, true);
                    lc.setLed(false, wallsX - 2, i, false);
                    lc.setLed(false, wallsX - 3, i, false);
                }
            }
            else if (i != hole && i != hole + 1)
            {
                lc.setLed(false, wallsX, i, true);
                lc.setLed(false, wallsX - 1, i, true);
                lc.setLed(false, wallsX - 2, i, false);
                lc.setLed(false, wallsX - 3, i, false);
            }

            if (collision(wallsX, hole, Size) && millis() - timer7 > 2000)
            {
                timer7 = millis();
                lives--;
            }

            if (lives == 0)
                game_over = 1;
        }

        timer2 = millis();
        wallsX++;
    }
    if (Size == 3)
    {
        if(wallsX == 10)
        {
            wallsX = 0;
            hole = rand() % 6;
            score++;
            scoreShow++;
        }
    }
    else if(wallsX == 10)
    {
        wallsX = 0;
        hole = rand() % 7;
        score++;
        scoreShow++;
    }
}

int collision(int wallsX, int hole, int Size)
{
    //collision occurred
    if (Size == 3)
    {
        if (flappyX == wallsX || flappyX == wallsX - 1 || flappyX == wallsX - 2)
            if (flappyY != hole && flappyY != hole + 1 && flappyY != hole + 2)
                return 1;
    }
    else if (flappyX == wallsX || flappyX == wallsX - 1 || flappyX == wallsX - 2)
        if (flappyY != hole && flappyY != hole + 1)
            return 1;

    return 0;
}

bool sw2 = 1, sw3 = 1, sw4 = 1, sw5 = 1, sw6 = 1, sw7 = 1, sw8 = 1, sw9 = 1, sw10 = 0;

void difficultyChange()
{

    ///////////////////////////////////// LEVEL 1   SPEED 250 SPACE 3 WALLS 1

    if(score < 5)
        walls(wallsX, timer2, hole, 3);

    ///////////////////////////////////// LEVEL 2   SPEED 250 SPACE 2 WALLS 1

    if (score >= 5 && score < 10)
        walls(wallsX, timer2, hole, 2);

    ///////////////////////////////////// LEVEL 3   SPEED 200 SPACE 3 WALLS 1

    if (score >= 10)
        walls(wallsX, timer2, hole, 3);

    if(score == 10 && sw2 == 1)
    {
        wallsSpeed = 200;
        sw2 = 0;
    }

    ///////////////////////////////////// LEVEL 4   SPEED 150 SPACE 3 WALLS 1

    if(score >= 15 && score <= 25 && sw3 == 1)
    {
        wallsSpeed = 150;
        sw3 = 0;
    }

    if(score == 35)
        wallsSpeed = 250;

    ///////////////////////////////////// LEVEL 5   SPEED 300 SPACE 3 WALLS 2

    if(score == 25 && sw6 == 1)
    {
        timer4 = game_start2 + 3;
        sw6 = 0;
    }

    if (game_start2 - timer4 > 960 && timer4 != 0)
        walls(wallsX2, timer3, hole2, 3);

    if(game_start2 - timer4 > 960 && timer4 != 0 && sw4 == 1)
    {
        wallsSpeed = 300;
        sw4 = 0;
    }

    ///////////////////////////////////// LEVEL 6   SPEED 250 SPACE 3 WALLS 2

    if(score == 35 && sw8 == 1)
    {
        timer5 = game_start2 + 3;
        sw8 = 0;
    }

    if (game_start2 - timer5 > 1260 && timer5 != 0)
        walls(wallsX2, timer3, hole2, 3);

    ///////////////////////////////////// LEVEL 7   SPEED 250 SPACE 3/2 WALLS 2

    if(score == 45 && sw9 == 1)
    {
        timer6 = game_start2 + 3;
        sw9 = 0;
    }

    if (game_start2 - timer6 > 1260 && timer6 != 0)
    {
        walls(wallsX2, timer3, hole2, 2);
        timer5 = 0;
        timer4 = 0;
    }
}

void loop()
{
    show_menu();

    switchValue = !digitalRead(pinSW);
    xValue = analogRead(pinX);

    select = joyStickX(select, 3);

    if (select == 0)
    {
        lcd.setCursor(8, 0);
        lcd.print(" ");
        lcd.setCursor(2, 1);
        lcd.print(" ");
        lcd.setCursor(1, 0);
        if(millis() % 750 > 325)
            lcd.print(">");
        else
            lcd.print(" ");
    }
    else if (select == 1)
    {
        lcd.setCursor(1, 0);
        lcd.print(" ");
        lcd.setCursor(2, 1);
        lcd.print(" ");
        lcd.setCursor(8, 0);
        if(millis() % 750 > 325)
            lcd.print(">");
        else
            lcd.print(" ");
    }
    else
    {
        lcd.setCursor(8, 0);
        lcd.print(" ");
        lcd.setCursor(1, 0);
        lcd.print(" ");
        lcd.setCursor(2, 1);
        if(millis() % 750 > 325)
            lcd.print(">");
        else
            lcd.print(" ");
    }

    if (switchValue == 1 && swB == 1)
    {
        if (pressed == 1)
            pressed = 0;
        else
            pressed = 1;
        swB = 0;
    }
    if (switchValue == 0)
        swB = 1;

    while(Serial.available() && name_introduced == 0)
    {
      player = Serial.readString();
    }

    if (player != "X" && swName == 1)
    {
      playerName = "";
        name_introduced = 1;
        for (int i = 0; i < player.length() - 1; i++)
            playerName = playerName + player[i];
        swName = 0;
    }

    nameLength = playerName.length();
    long timer = millis();

//************************************* START *************************************\\

    while (pressed == 1 && select == 0)
    {
        if(millis() > timer && millis() < timer + 50)
            lcd.clear();

        if (game_start == 0)
            game_start = millis();

        game_start2 = millis() - game_start;

        difficultyChange();
        flappyMove();

        if (score == 0)
            level = 1;
        else if (score == 5)
            level = 2;
        else if (score == 10)
            level = 3;
        else if (score == 15)
            level = 4;
        else if (score == 25)
            level = 5;
        else if (score == 35)
            level = 6;
        else if (score == 45)
            level = 7;

        lcd.setCursor(0, 0);
        lcd.print("Lives: ");
        lcd.print(lives);

        lcd.setCursor(9, 0);
        lcd.print("Lvl: ");
        lcd.print(level);

        lcd.setCursor(0, 1);
        lcd.print("Score: ");
        lcd.print(scoreShow);

        if (game_over == 1)
        {
            while (1)
            {
                yValue = analogRead(pinY);
                lcd.setCursor(0, 0);
                lcd.print("Lives: ");
                lcd.print(lives);

                lcd.setCursor(9, 0);
                lcd.print("Lvl: ");
                lcd.print(level);

                lcd.setCursor(0, 1);
                lcd.print("Score: ");
                lcd.print(scoreShow);

                if (showOnce == 0)
                {
                    gameOverText();
                    showOnce = 1;
                }
                if (yValue > 800)
                    break;
            }

            pressed = 0;
            game_over = 0;

            if (scoreShow > EEPROM.read(50))
            {
                EEPROM.write(50, scoreShow);
                hiScoreName();
            }

            score = 0;
            level = 0;
            lcd.clear();
            lc.clearDisplay(0);
            lc.setLed(false, flappyX, flappyY, false);
            flappyY = 3;
            lc.setLed(false, flappyX, flappyY, true);
            wallsX = 0;
            wallsX2 = 0;
            lives = 3;
            hole = rand() % 6;
            hole2 = rand() % 6;
            game_start = 0;
            showOnce = 0;
            scoreShow = 0;
            timer = 0;
            timer2 = 0;
            timer3 = 0;
            timer4 = 0;
            timer5 = 0;
            timer6 = 0;
            sw2 = 1;
            sw3 = 1;
            sw4 = 1;
            sw5 = 1;
            sw6 = 1;
            sw7 = 1;
            sw8 = 1;
            sw9 = 1;
            sw10 = 0;
        }
    }

//************************************* SETUP *************************************\\

    timer = millis();

    while (pressed == 1 && select == 1)
    {
        xValue = analogRead(pinX);
        yValue = analogRead(pinY);

        if(millis() > timer && millis() < timer + 50)
            lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("Player: ");
        if(player == "X")
            lcd.print("Anonymus");
        else
            lcd.print(playerName);

        lcd.setCursor(0, 1);
        lcd.print("Starting lvl: ");

        level = joyStickX(level, 7);

        if (level == 0)
            score = 0;
        else if (level == 1)
            score = 5;
        else if (level == 2)
            score = 10;
        else if (level == 3)
            score = 15;
        else if (level == 4)
            score = 25;
        else if (level == 5)
            score = 35;
        else if (level == 6)
            score = 45;

        lcd.print(level + 1);

        if (yValue > 800)
        {
            lcd.clear();
            pressed = 0;
        }
    }

//************************************* HIGHSCORE *************************************\\

    timer = millis();

    while (pressed == 1 && select == 2)
    {
        yValue = analogRead(pinY);

        if(millis() > timer && millis() < timer + 50)
            lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("Player: ");

        while(EEPROM.read(ind) != 0)
        {
            Serial.println(char(EEPROM.read(ind)));
            lcd.setCursor(10, 0);
            a = a + char(EEPROM.read(ind));
            ind++;
        }

        lcd.print(a);
        lcd.setCursor(0, 1);
        lcd.print("HighScore: ");
        lcd.print(EEPROM.read(50));

        if (yValue > 800)
        {
            lcd.clear();
            pressed = 0;
        }
    }
}
