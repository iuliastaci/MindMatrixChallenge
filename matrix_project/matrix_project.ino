/*
  Matrix Project - MindMatrix Challenge

  This sketch creates a game on an 8x8 LED matrix. It consists of the game itself and a menu for it. 
  It is a memory game. There are 3 levels with progressing difficulty. At each level, a random generated pattern is displayed on the
  matrix for a couple of seconds. After that, the player should reproduce the pattern by turning on the LEDs on the matrix using the joystick. 
  If a LED, that is not in the pattern, is turned on, the game is over. The score for each level is the time spent to complete the level.

  The menu structure:
    - Start game
    - Settings 
      ~ LCD Brightness Control
      ~ Matrix Brightness Control
      ~ Turn sound on/off
      ~ Back to main menu
    - About
    - How to play
  
  For the setup, I have used the following components:
    - an 8x8 LED Matrix
    - MAX7219 Driver (to control the matrix)
    - an LCD
    - a buzzer
    - a potentiometer
    - a joystick
    - capacitors 
    - resistors and wires

  Created 03/12/2023
  By Ana-Iulia Staci

  Last modified 19/12/2023
  By Ana-Iulia Staci

  https://github.com/iuliastaci/IntroductionToRobotics
*/

#include "LedControl.h"
#include <LiquidCrystal.h>
#include "EEPROM.h"

// Define pins for matrix
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  //DIN, CLK, LOAD, No. DRIVER

// Define variables for brightness control
const byte pwmPin = 5;
byte matrixBrightness = 2;
byte lcdBrightness = 8;

// Define pins for LCD
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 13;
const byte d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Define variables for buzzer
const byte buzzerPin = 3;
const int soundDuration = 300;
const int soundDelay = soundDuration + 50;
const int startGameSound = 500;
const int winSound = 1000;
const int gameOverSound = 200;
const int scrollSound = 300;
bool soundON = false;

// Define pins for the joystick
const int pinY = A1;  // Analog pin connected to X output
const int pinX = A0;  // Analog pin connected to Y output
const int pinSW = 2;

// Define game variables
const int patternLengths[] = { 3, 5, 7 };  // Pattern lengths for each difficulty level
const int numLevels = 3;                   // Number of difficulty levels
int currentLevel = 1;                      // Current difficulty level
int patternIndex = 0;                      // Index of the current step in the pattern
int playerIndex = 0;                       // Index of the player's current step
int pattern[7];                            // Array to store the pattern
int playerPattern[7];                      // Array to store the player's input pattern
bool gameOver = false;                     // Flag to indicate if the game is over
unsigned long startTime = 0;               // Start time of the pattern
unsigned long endTime = 0;                 // End time of the pattern
unsigned long score = 0;                   // Player's score
bool gameStarted = false;                  // Flag to indicate if the game has started
const int blinkInterval = 300;
const int rapidBlinkInterval = 100;
unsigned long lastBlink = 0;
byte ledState = LOW;
int noOfTurnedOnLEDs = 0;
const int displayDelay = 1000;
unsigned long lastTimeDisplayed = 0;
const int fiveSDelay = 5000;
const int twoSDelay = 2000;
const int shortDelay = 200;



// Define variables for joystick
int playerX = 0;
int playerY = 0;
int xValue = 0;
int yValue = 0;
byte swState = LOW;
const int minThreshold = 400;
const int maxThreshold = 600;
bool joyMoved = false;
const int up = 0;
const int down = 1;
const int left = 2;
const int right = 3;
const int noOfDirections = 4;

// Define variables for sw button debounce
long lastTimeSWPressed = 0;
const int debounceDelay = 50;
byte currentSWRead = LOW;


// Define variable for button interrupt
volatile bool swPressed = false;

// Define variables for menu
const char* menuItems[] = { "Start Game", "Settings", "About", "How to play" };
const int numMenuItems = sizeof(menuItems) / sizeof(menuItems[0]);
int currentMenuItem = 0;

// Define variables for About submenu
int currentLine = 0;
const char* aboutLines[] = {
  "MindMatrix",
  "Ana Iulia Staci",
  "Git user:",
  "iuliastaci",
  "Click to go back"

};

// Define custom characters

byte hourglass[8] = {
  0b00000,
  0b11111,
  0b10001,
  0b01010,
  0b00100,
  0b01010,
  0b10001,
  0b11111
};


bool currentMatrix[matrixSize][matrixSize] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

// Define custom symbols to print on the matrix
const int noOfSymbols = 5;
byte symbols[noOfSymbols][matrixSize] = {
  { B00110011, B00110011, B00111111, B11111100, B11111100, B00111111, B00001100, B00001100 },  // puzzle symbol for game intro
  { B01100000, B10100000, B11100000, B00010000, B00001100, B00001110, B00000111, B00000010 },  // symbol for settings
  { B00010000, B00110001, B01110010, B11110100, B11110111, B01110100, B00110010, B00010001 },  // symbol for sound settings
  { B00011000, B00011000, B00000000, B00011000, B00011000, B00011000, B00011000, B00011000 },  // I symbol for About
  { B00000000, B00111100, B01100110, B00000110, B00011100, B00011000, B00000000, B00011000 }   // ? symbol for how to play
};
const int introSymbol = 0;
const int settingsSymbol = 1;
const int soundSymbol = 2;
const int aboutSymbol = 3;
const int howToPlaySymbol = 4;

// Function for button interrupt
void buttonPressed() {
  swPressed = true;
}

void setup() {

  soundON = EEPROM.read(2);
  matrixBrightness = EEPROM.read(0);
  //lcdBrightness = EEPROM.read(1);
  lc.shutdown(0, false);                 // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness);  // sets brightness (0~15 possible values)
  lc.clearDisplay(0);
  analogWrite(pwmPin, lcdBrightness);

  lcd.begin(16, 2);
  lcd.createChar(0, hourglass);

  // Initialize joystick pins
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(pinSW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinSW), buttonPressed, FALLING);

  // Initialize buzzer and potentiometer pins
  pinMode(buzzerPin, OUTPUT);
  pinMode(potentiometerPin, INPUT);

  Serial.begin(9600);

  //Display an intro message on lcd via a function call
  displayIntro();

  resetGame();  // Reset the game variables
  displayMenu();
}


void loop() {
  if (gameStarted) {
    playGame();
  } else {
    readJoystick();
    updateMenu();
  }
}

// Define functions for the game
void playGame() {
  // Check if the game is over
  if (gameOver) {
    displayGameOver();
    delay(fiveSDelay);
    resetGame();
    displayPattern();
    gameStarted = false;
  } else {
    // Check if the player has completed the pattern
    if (noOfTurnedOnLEDs == patternLengths[currentLevel - 1]) {
      displayWin();
      delay(fiveSDelay);
      noOfTurnedOnLEDs = 0;
      if (currentLevel < numLevels) {
        increaseLevel();
        resetGame();
        generatePattern();
      } else {
        // The player has completed all levels
        gameStarted = false;
      }
    } else {
      // The player is still playing the current level
      moveJoystick(joyMoved);
      buttonPress();
      checkGameStatus();

      for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
          if (i == playerX && j == playerY) {
            if (currentMatrix[playerX][playerY] == 1) {
              blinkPlayerLED(rapidBlinkInterval);
              // blinked the player LED more quickly when that LED is already on
            } else {
              blinkPlayerLED(blinkInterval);
              // blink the player LED normally when that LED is off
            }
          } else if (currentMatrix[i][j] == 1) {
            lc.setLed(0, i, j, true);
          } else {
            lc.setLed(0, i, j, false);
          }
        }
      }
      if (millis() - lastTimeDisplayed >= displayDelay) {
        score = (millis() - startTime) / 1000;
        displayScore();
        lastTimeDisplayed = millis();
      }
    }
  }
}

void checkGameStatus() {
  noOfTurnedOnLEDs = 0;
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      // check no of turned on LEDs corresponding to the pattern
      if (i < patternLengths[currentLevel - 1] && j == pattern[i] && currentMatrix[i][j] == 1) {
        noOfTurnedOnLEDs++;
      }

      // Check if exists a turned on LED that is not in the pattern
      if (i >= patternLengths[currentLevel - 1] && currentMatrix[i][j] == 1) {
        gameOver = true;
        return;
      }
      if (i < patternLengths[currentLevel - 1] && j != pattern[i] && currentMatrix[i][j] == 1) {
        gameOver = true;
        return;
      }
    }
  }
}

void moveJoystick(bool& joyMoved) {
  // Read the joystick values
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  if (xValue > maxThreshold && joyMoved == false) {
    joyMoved = true;
    movePlayer(up);
  }
  // Check if the joystick was moved down
  if (xValue < minThreshold && joyMoved == false) {
    joyMoved = true;
    movePlayer(down);
  }
  // Check if the joystick was moved left
  if (yValue < minThreshold && joyMoved == false) {
    joyMoved = true;
    movePlayer(left);
  }
  // Check if the joystick was moved right
  if (yValue > maxThreshold && joyMoved == false) {
    joyMoved = true;
    movePlayer(right);
  }

  if (xValue > minThreshold && xValue < maxThreshold && yValue > minThreshold && yValue < maxThreshold) {
    joyMoved = false;
  }
}


void movePlayer(int direction) {
  // Clear the previous player position
  lc.setLed(0, playerX, playerY, false);

  // Move the player in the specified direction
  switch (direction) {
    case up:
      playerY--;
      break;
    case down:
      playerY++;
      break;
    case left:
      playerX--;
      break;
    case right:
      playerX++;
      break;
  }

  // Check if the player has moved off the matrix
  if (playerX < 0) {
    playerX = matrixSize - 1;
  }
  if (playerX >= matrixSize) {
    playerX = 0;
  }
  if (playerY < 0) {
    playerY = matrixSize - 1;
  }
  if (playerY >= matrixSize) {
    playerY = 0;
  }
  lc.setLed(0, playerX, playerY, true);
}

void buttonPress() {
  if (swPressed) {
    swPressed = false;
    lastTimeSWPressed = millis();
  }
  if (millis() - lastTimeSWPressed >= debounceDelay) {
    currentSWRead = digitalRead(pinSW);
    if (currentSWRead != swState) {
      swState = currentSWRead;
      if (swState == LOW) {
        if (currentMatrix[playerX][playerY] == 1) {
          currentMatrix[playerX][playerY] = 0;  // turn off the LED at player position
        } else {
          currentMatrix[playerX][playerY] = 1;  // turn on the LED at player position
        }
      }
    }
  }
}

void generatePattern() {
  // Generate a random pattern for the current level
  for (int i = 0; i < patternLengths[currentLevel - 1]; i++) {
    pattern[i] = random(8);  // Generate a random number between 0 and 7
  }
  // Display the pattern on the matrix
  displayPattern();
  // Start the timer
  startTime = millis();
  // Display player at start position
  blinkPlayerLED(blinkInterval);
}

void displayPattern() {
  // Display the pattern on the matrix for 5s
  lc.clearDisplay(0);
  for (int i = 0; i < patternLengths[currentLevel - 1]; i++) {
    lc.setLed(0, i, pattern[i], true);
  }
  delay(fiveSDelay);
}

void displayIntro() {
  displaySymbol(introSymbol);
  // Display an intro message on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to Mind-");
  lcd.setCursor(0, 1);
  lcd.print("Matrix Challenge");
  delay(twoSDelay);
}

void displayGameOver() {
  playSound(gameOverSound);
  // Display "Game Over" on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
}

void displayWin() {
  playSound(winSound);
  // Display "You Win!" on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You Win!");
}

void displayScore() {
  // Display the player's score on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.print(score);
  lcd.print(" ");
  lcd.write(byte(0));  // Display hourglass custom character
}

void increaseLevel() {
  // Increase the difficulty level and reset the player's position
  currentLevel++;
  playerX = 0;
  playerY = 0;
  score = 0;
}

void resetGame() {
  // Reset the game variables
  gameOver = false;
  patternIndex = 0;
  playerIndex = 0;
  score = 0;
  // Clear the matrix display
  lc.clearDisplay(0);
  for (int i = 0; i < matrixSize; i++)
    for (int j = 0; j < matrixSize; j++) {
      currentMatrix[i][j] = 0;
    }
}

void blinkPlayerLED(int delayTime) {
  // Blink the LED for the player on the matrix
  if (millis() - lastBlink >= delayTime) {
    lastBlink = millis();
    ledState = !ledState;
    lc.setLed(0, playerX, playerY, ledState);
  }
}

//Functions used for main menu and submenus
void readJoystick() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
}

void updateMenu() {
  if (yValue < minThreshold) {
    playSound(scrollSound);
    // Joystick moved up
    moveUp();
  } else if (yValue > maxThreshold) {
    playSound(scrollSound);
    // Joystick moved down
    moveDown();
  }

  if (swPressed) {
    swPressed = false;
    lastTimeSWPressed = millis();
  }
  if (millis() - lastTimeSWPressed >= debounceDelay) {
    currentSWRead = digitalRead(pinSW);
    if (currentSWRead != swState) {
      swState = currentSWRead;
      if (swState == LOW) {
        selectMenuItem();
      }
    }
  }
}

void moveUp() {
  if (currentMenuItem > 0) {
    currentMenuItem--;
    displayMenu();
    delay(shortDelay);  // Delay for visual feedback
  }
}

void moveDown() {
  if (currentMenuItem < numMenuItems - 1) {
    currentMenuItem++;
    displayMenu();
    delay(shortDelay);  // Delay for visual feedback
  }
}
// Displaying main menu
void displayMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(">");
  lcd.setCursor(2, 0);
  lcd.print(menuItems[currentMenuItem]);

  lcd.setCursor(0, 1);
  lcd.print("                ");  // Clear the second row

  lcd.setCursor(0, 1);
  lcd.print(menuItems[(currentMenuItem + 1) % numMenuItems]);

  delay(100);  // A short delay to avoid flickering
}

void selectMenuItem() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Selected: ");
  lcd.setCursor(0, 1);
  lcd.print(menuItems[currentMenuItem]);

  // Perform action based on selected menu item
  switch (currentMenuItem) {
    case 0:
      // Start game
      playSound(startGameSound);
      lcd.setCursor(0, 1);
      lcd.print("Starting game...");
      delay(twoSDelay);  // Simulate starting the game
      generatePattern();
      gameStarted = true;
      break;
    case 1:
      // Settings submenu
      currentMenuItem = 0;  // Reset submenu to the first item
      settingsMenuRuntime(menuItems[currentMenuItem]);
      break;
    case 2:
      // About submenu
      currentMenuItem = 0;  // Reset submenu to the first item
      displayAbout();
      break;
    case 3:
      // How to play submenu
      currentMenuItem = 0;
      displayHowToPlay();
      break;
  }

  delay(twoSDelay);  // Display selected menu item for 2 seconds
  displayMenu();     // Return to the main menu
}

const char* submenuItems[] = { "LCD Brightness", "Matrix Brightness", "Sound ON/OFF", "Back" };
const int numSubMenuItems = sizeof(submenuItems) / sizeof(submenuItems[0]);

void settingsMenuRuntime(const char* submenuTitle) {
  displaySymbol(settingsSymbol);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(submenuTitle);

  // Define submenu items

  int submenuSelection = 0;
  displaySettingsMenu(submenuSelection);
  while (true) {

    readJoystick();

    if (yValue < minThreshold) {
      playSound(scrollSound);
      // Joystick moved up in submenu
      if (submenuSelection > 0) {
        submenuSelection--;
        displaySettingsMenu(submenuSelection);
      }
    } else if (yValue > maxThreshold) {
      playSound(scrollSound);
      // Joystick moved down in submenu
      if (submenuSelection < numSubMenuItems - 1) {
        submenuSelection++;
        displaySettingsMenu(submenuSelection);
      }
    }

    if (swPressed) {
      swPressed = false;
      lastTimeSWPressed = millis();
    }
    if (millis() - lastTimeSWPressed >= debounceDelay) {
      currentSWRead = digitalRead(pinSW);
      if (currentSWRead != swState) {
        swState = currentSWRead;
        if (swState == LOW) {
          if (submenuSelection == numSubMenuItems - 1) {
            return;  // Back to main menu
          }
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Selected: ");
          lcd.setCursor(0, 1);
          lcd.print(submenuItems[submenuSelection]);

          switch (submenuSelection) {
            case 0:
              // Change LCD Brightness
              submenuSelection = 0;
              changeLCDBrightness();
              break;
            case 1:
              // Change Matrix Brightness
              lcd.clear();
              changeMatrixBrightness();
              submenuSelection = 0;
              break;
            case 2:
              // Turn sound on or off
              changeSoundSetting();
              submenuSelection = 0;
              break;
          }

          // Back to settings menu
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(submenuTitle);
          displaySettingsMenu(submenuSelection);
        }
      }
    }


    delay(shortDelay);  
  }
}

void displaySettingsMenu(int submenuSelection) {
  lcd.setCursor(0, 1);
  lcd.print("                ");  

  lcd.setCursor(0, 1);
  lcd.print(submenuItems[submenuSelection]);
}

int brightnessValue = 0;
const int minAnalogValue = 0;
const int maxAnalogValue = 1023;
const int minDigitalValue = 0;
const int maxDigitalValue = 255;
void changeLCDBrightness() {
  while (true) {
      readJoystick();

      // Control the brightness value on Ox axis of the joystick
      if (xValue > maxThreshold) {
          brightnessValue++;
      }
      else if (xValue < minThreshold) {
          brightnessValue--;
      }

      lcdBrightness = map(brightnessValue, minAnalogValue, maxAnalogValue, minDigitalValue, maxDigitalValue);
      analogWrite(pwmPin, lcdBrightness);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Brightness: ");
      lcd.print(lcdBrightness);
      lcd.print(" ");

      if (swPressed) {
        swPressed = false;
        lastTimeSWPressed = millis();
      }
      if (millis() - lastTimeSWPressed >= debounceDelay) {
        currentSWRead = digitalRead(pinSW);
        if (currentSWRead != swState) {
          swState = currentSWRead;
          if (swState == LOW) {
            EEPROM.update(1, lcdBrightness);
            return;  // Back
          }
        }
      }

      delay(shortDelay);

  }
}

const int minByteValue = 0;
const int maxByteValue = 15;
void changeMatrixBrightness() {
  brightnessValue = 0;
  while (true) {
    readJoystick();

    // Control the brightness value on Ox axis of the joystick
    if (xValue > maxThreshold) {
      brightnessValue++;
    } else if (xValue < minThreshold) {
      brightnessValue--;
    }

    matrixBrightness = map(brightnessValue, minAnalogValue, maxAnalogValue, minByteValue, maxByteValue);
    lc.setIntensity(0, matrixBrightness);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Brightness: ");
    lcd.print(matrixBrightness); // Print the updated value of the brightness
    lcd.print(" ");

    lc.clearDisplay(0);
    for (int row = 0; row < 8; row++) {
      lc.setRow(0, row, B11111111);
    }

    if (swPressed) {
        swPressed = false;
        lastTimeSWPressed = millis();
    }
    if (millis() - lastTimeSWPressed >= debounceDelay) {
      currentSWRead = digitalRead(pinSW);
      if (currentSWRead != swState) {
        swState = currentSWRead;
        if (swState == LOW) {  
          EEPROM.update(0, matrixBrightness);
          return;  // Back 
        }
      }
    }
    delay(shortDelay);
  }
}

void changeSoundSetting() {
  while (true) {
    displaySymbol(soundSymbol);
    lcd.clear();
    lcd.setCursor(0, 0);
    if (soundON == false) {
      lcd.print("Sound off");
    } else {
      lcd.print("Sound on");
    }
    lcd.setCursor(0, 1);
    lcd.print("Click to change");
    if (swPressed) {
      swPressed = false;
      lastTimeSWPressed = millis();
    }
    if (millis() - lastTimeSWPressed >= debounceDelay) {
      currentSWRead = digitalRead(pinSW);
      if (currentSWRead != swState) {
        swState = currentSWRead;
        if (swState == LOW) {
          soundON = !soundON;
          EEPROM.update(2, soundON);
          return;
        }
      }
    }
  }
}

void scrollAboutMenu(int direction) {
  currentLine += direction;
  if (currentLine < 0) {
    currentLine = 4;  // Wrap around to the last line
  } else if (currentLine > 4) {
    currentLine = 0;  // Wrap around to the first line
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(aboutLines[currentLine]);
  lcd.setCursor(0, 1);
  lcd.print("                ");  
}

void displayAbout() {

  displaySymbol(aboutSymbol);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(aboutLines[currentLine]);
  lcd.setCursor(0, 1);
  lcd.print("                ");  

  while (true) {
    readJoystick();
    if (yValue < minThreshold) {
      scrollAboutMenu(-1);
      delay(shortDelay);
    } else if (yValue > maxThreshold) {
      scrollAboutMenu(1);
      delay(shortDelay);
    }

    if (swPressed) {
      swPressed = false;
      lastTimeSWPressed = millis();
    }
    if (millis() - lastTimeSWPressed >= debounceDelay) {
      currentSWRead = digitalRead(pinSW);
      if (currentSWRead != swState) {
        swState = currentSWRead;
        if (swState == LOW) {
          return;  // Back to main menu
        }
      }
    }
    delay(shortDelay);
  }
}

unsigned long lastScroll = 0;
const char* howToPlayMessages[] = {
  "A random pattern will light up.",
  "Memorize it. After it disappears,",
  "try to reproduce it moving the",
  "joystick and click to turn on the",
  "LED. If successful, you level up.",
  "If you turn on a LED that's not in",
  "the pattern, the game is over."
};
const int howToPlayMessagesSize = sizeof(howToPlayMessages) / sizeof(char *);

void displayHowToPlay() {
  displaySymbol(howToPlaySymbol);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("How to play");
  delay(twoSDelay);

  for (int i = 0; i < howToPlayMessagesSize; i++) {
    bool finished = displayScrollingText(howToPlayMessages[i]);
    if (!finished) {
      return;
    }
  }
}

// Returns false if the user cancelled the scrolling text
bool displayScrollingText(const char* message) {
  const unsigned int scrollingDelay = 1000;
  const int noOfCharacters = strlen(message);
  int position = 0;

  // print message
  lcd.clear();
  lcd.home();
  lcd.print(message);

  lastScroll = millis();
  while (true) {
    // button press
    if (swPressed) {
      swPressed = false;
      lastTimeSWPressed = millis();
    }

    if (millis() - lastTimeSWPressed >= debounceDelay) {
      currentSWRead = digitalRead(pinSW);
      if (currentSWRead != swState) {
        swState = currentSWRead;
        if (swState == LOW) {
          return false; // user cancelled 
        }
      }
    }

    // scroll message
    if (millis() - lastScroll >= scrollingDelay) {
      lastScroll = millis();
      lcd.scrollDisplayLeft();
      position++;
    }

    if (position >= noOfCharacters - 16) {
      delay(scrollingDelay);
      return true;
    }
  }
}

void displaySymbol(int symbolNumber) {
  lc.clearDisplay(0);
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, symbols[symbolNumber][row]);
  }
}

void playSound(int frequency) {
  if (soundON) {
    tone(buzzerPin, frequency, soundDuration);
    delay(soundDelay);
    noTone(buzzerPin);
  }
}
