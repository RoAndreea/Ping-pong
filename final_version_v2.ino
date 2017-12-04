#include <LiquidCrystal.h>
#include "LedControl.h"
LedControl lc = LedControl(12, 11, 10, 1);
LiquidCrystal lcd = LiquidCrystal(1, 2, 4, 5, 6, 7);

// Arduino pin numbers
const int swPin = 3; // digital pin connected to switch output
const int xPin = 0; // analog pin connected to X output
const int yPin = 1; // analog pin connected to Y output

const int sw2Pin = 8;
const int x2Pin = 2;
const int y2Pin = 3;

int padYP1, padP1;
int padYP2, padP2;
int ballRow, ballCol;
int player1Score, player2Score;
int player1Position, player2Position;

boolean isGameOn = true;
boolean ballMovingUp = false;
boolean ballMovingRight = true;
int lastSwitchState = HIGH;
int winner;
float difficulty =  250;
int switchState2;
unsigned long moveBallOldTime = 0;
unsigned long movePlayer1OldTime = 0;
unsigned long movePlayer2OldTime = 0;

byte smile[8]   = {B00111100, B01000010, B10100101, B10000001, B10100101, B10011001, B01000010, B00111100};
byte one[8]     = {B00000000, B00010000, B00011000, B00010100, B00010000, B00010000, B00111000, B00000000};
byte two[8]     = {B00000000, B00111100, B00100100, B00010000, B00001000, B00100100, B00111100, B00000000};
byte heart[8]   = {B00000000, B01100110, B10011001, B10000001, B01000010, B00100100, B00011000, B00000000};

void printByte(byte character []) {
    int i = 0;
    for (i = 0; i < 8; i++)
        lc.setRow(0, i, character[i]);
}

void setup() {
    lcd.begin(16, 2);
    analogWrite(9, 100);
    lc.shutdown(0, false); // turn off power saving, enables display
    lc.setIntensity(0, 8); // sets brightness
    lc.clearDisplay(0);    // clear screen
    lcd.clear();           // clear lcd screen
    pinMode(sw2Pin, INPUT_PULLUP);
}

void startGame() {
    player1Score = 0;
    player2Score = 0;
    lc.clearDisplay(0);
    lcd.clear();
    printByte(smile);
    lcd.setCursor(0, 0);
    lcd.print("Hello, press");
    lcd.setCursor(1, 1);
    lcd.print("once to start");
    isGameOn = true;
    ballRow = 4; 
    ballCol = random(1, 6);
    padYP1 = 0;  // initial poisition for Player 1
    padP1 = 3;
    padYP2 = 7;  // initial position for player 2 
    padP2 = 3;
}

//Resets after scoring one point
void  reset() {
    lc.clearDisplay(0);
    ballRow = 4;
    ballCol = random(1, 6);
    padYP1 = 0;
    padP1 = 3;
    padYP2 = 7;
    padP2 = 3;
    delay(400);
}
//Resets the whole game
void restartGame() {
    isGameOn = false;
    lcd.clear();
    lc.clearDisplay(0);
}

//Players/Pads moving conditions
void movePlayers() {
    unsigned long currentTime = millis();
    if (currentTime - movePlayer1OldTime > 100) {
        int joyXValue = analogRead(xPin);
        int joyYValue = analogRead(yPin);

        //   PLAYER 1 PAD
        if (joyYValue < 300) {
            padP1 -= 1;
            movePlayer1OldTime = currentTime;
        }
        else if (joyYValue > 700) {
            padP1 += 1;
            movePlayer1OldTime = currentTime;
        }
        while ( padP1 < 1 )
            padP1 = 1;
        while (padP1 > 6)
            padP1 = 6;
    }

    if (currentTime - movePlayer2OldTime > 100) {
        int joyX2Value = analogRead(x2Pin);
        int joyY2Value = analogRead(y2Pin);

        //   PLAYER 2 PAD
        if (joyY2Value < 300) {
            padP2 -= 1;
            movePlayer2OldTime = currentTime;
        }
        else if (joyY2Value > 700) {
            padP2 += 1;
            movePlayer2OldTime = currentTime;
        }
        while ( padP2 < 1 )
            padP2 = 1;
        while ( padP2 > 6 )
            padP2 = 6;
    }
}

//Ball moving conditions
void moveBall() {
    unsigned long currentTime = millis();
    if (currentTime - moveBallOldTime > difficulty) {
        if (ballMovingUp)
            ballRow--;
        else
            ballRow++;
        if (ballMovingRight)
            ballCol--;
        else
            ballCol++;
        if (ballCol == 7)
            ballMovingRight = true;
        else if (ballCol == 0)
            ballMovingRight = false;

        if (ballRow < 0) {
            player2Score++;
            reset();
        }
        else if (ballRow > 7) {
            player1Score++;
            reset();
        }

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Player1 score");
        lcd.print(" ");
        lcd.print(player1Score);
        lcd.setCursor(0, 1);
        lcd.print("Player2 score");
        lcd.print(" ");
        lcd.print(player2Score);
        moveBallOldTime = currentTime;
        difficulty -= 0.35;
        if ( difficulty <= 120)
        difficulty =  120;
    }
}


int game() {
    if (isGameOn) {
        movePlayers();
        moveBall();

        if (player1Score == 11 && player1Score > player2Score) {
            lcd.print("Player1 wins");
            restartGame();
            winner = 1;
        }
        else if (player2Score == 11 && player2Score > player1Score) {
            lcd.print("Player2 wins");
            restartGame();
            winner = 2;
        }


       if (ballRow == 1 && ballCol >= padP1  - 1 && ballCol  <= padP1 + 1)
            ballMovingUp = false;
       else if (ballRow == 6 && ballCol >= padP2  - 1 && ballCol  <= padP2 + 1)
            ballMovingUp = true;
    
       lc.clearDisplay(0);
       lc.setLed(0, padYP1, padP1, true);
       lc.setLed(0, padYP1, padP1 + 1, true);
       lc.setLed(0, padYP1, padP1 - 1, true);
       lc.setLed(0, padYP2, padP2 + 1, true);
       lc.setLed(0, padYP2, padP2 - 1, true);
       lc.setLed(0, padYP2, padP2, true);
       lc.setLed(0, ballRow, ballCol, true);
       delay(1);
    }
  
    else {
        lc.clearDisplay(0);
        lcd.clear();
        if (winner == 2) {
            lcd.setCursor(1, 0);
            lcd.print("P2 wins, click");
            lcd.setCursor(0, 1);
            lcd.print("to start again");
            printByte(two);
            delay(400);
            printByte(heart);
            delay(200);
        }
        else if (winner == 1) {
            lcd.print("P1 wins, click");
            lcd.setCursor(0, 1);
            lcd.print("to start again");
            printByte(one);
            delay(400);
            printByte(heart);
            delay(200);
        }
        if (wasButtonPressed() == 1) { //if button was pressed now, end the game
            return 1;
        }
    }
    return 0;
}


int wasButtonPressed() {  //Used to read imput from the joystick
    switchState2 = digitalRead(sw2Pin);
    if (lastSwitchState != switchState2 && switchState2 == LOW) {
        lastSwitchState = switchState2;
        return 1;
    }
  
    lastSwitchState = switchState2;
    return 0;
}

void loop() {
    startGame();
    while (wasButtonPressed() == 0); //Continues only when the button is pressed
    while (game() == 0); //Loops the game until it returns !0 (until it finishes, 0 means that it has not finished yet)
}


