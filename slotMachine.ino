#include <Arduino_SensorKit.h>

//ALL RED LIGHTS 
const int LED_r1c1 = 2; 
const int LED_r1c2= 3;
const int LED_r1c3 = 4;

//ALL GREEN LIGHTS
const int LED_r2c1 = 5; 
const int LED_r2c2= 6;
const int LED_r2c3 = 7;

//ALL YELLOW LIGHTS
const int LED_r3c1= 8; 
const int LED_r3c2= 9;
const int LED_r3c3 = 10;

//switches
const int spinButton = 13;
const int creditButton = 12;

//previous states for switches
int prevSpinButtonState = LOW;
int prevCreditsButtonState = LOW;

//transistor
const int celebrationLEDs = 11;

//Create 3 arrays for different columns
int col_1[] = {LED_r1c1, LED_r2c1, LED_r3c1};
int col_2[] = {LED_r1c2, LED_r2c2, LED_r3c2};
int col_3[] = {LED_r1c3, LED_r2c3, LED_r3c3};

//spin as boolean to change when "spin" button pressed
boolean spin = false;

//variables for OLED & serial command
static int credits = 0;
static int gamesWon = 0;
static int gamesPlayed = 0;

// buzzer
const int buzzer = A5; 

void setup()
{
  //For the serial monitor
  Serial.begin(9600);
   // OLED display configuration
  Oled.begin();
  Oled.setFlipMode(true);
  Oled.setFont(u8x8_font_artosserif8_r);

  //Declaring pin modes for all 
pinMode(spinButton, INPUT);
pinMode(creditButton, INPUT);
pinMode(LED_r1c1, OUTPUT);
pinMode(LED_r1c2, OUTPUT);
pinMode(LED_r1c3, OUTPUT);
pinMode(LED_r2c1, OUTPUT);
pinMode(LED_r2c2, OUTPUT);
pinMode(LED_r2c3, OUTPUT);
pinMode(LED_r3c1, OUTPUT);
pinMode(LED_r3c2, OUTPUT);
pinMode(LED_r3c3, OUTPUT);
pinMode(celebrationLEDs, OUTPUT);
pinMode(buzzer, OUTPUT);

}

//Function that runs until the spin button has been pressed - it is the attraction mode

void attractMode() {
  digitalWrite(celebrationLEDs, HIGH);
  delay(1000);
  digitalWrite(celebrationLEDs, LOW);
  delay(1000);
}

//Function that is called when the credits button is pressed
void checkCredits() {
  int currentCreditsButtonState = digitalRead(creditButton);

  if(prevCreditsButtonState == LOW && currentCreditsButtonState == HIGH) {
    displayEmptyMessage();
    ++credits;
    displayCreditMessage();

  }

  //This is to ensure there is no switch debouncing
  if (currentCreditsButtonState != prevCreditsButtonState) {
    delay(10);
  }
   prevCreditsButtonState = currentCreditsButtonState;
}

//Function that is called when pressing spin button
void checkSpin() {
  int currentSpinButtonState = digitalRead(spinButton);

  if(prevSpinButtonState == LOW && currentSpinButtonState == HIGH) {
    if(credits == 0) {
      displayNoCreditMessage();
    }
    else {
      displayEmptyMessage();
      spin = true;
      gamesPlayed++;
      --credits;
      displayCreditMessage();
    }
  }
    //This is to ensure there is no switch debouncing
  if (currentSpinButtonState != prevSpinButtonState) {
    delay(10);
  }
   prevSpinButtonState = currentSpinButtonState;
}

//Function that actually "spins the wheel" by calling the "spinLoop" function
void spinTheWheel() {
  static int counter = 0;

  if (spin) {
    loopBlink(0, col_1, 3);
    loopBlink(1, col_2, 4);
    loopBlink(2, col_3, 5);

    counter++; // increment the counter variable

    if (counter >= 4000) {
      spin = false; // stop the spinning
      counter = 0; // reset the counter variable

      isItAWin();
    }
  }
}     

//Function that checks whether it is a winning pattern or not
void isItAWin() {
 
  //red LEDs
  if (digitalRead(LED_r1c1) == HIGH && digitalRead(LED_r1c2) == HIGH &&
        digitalRead(LED_r1c3) == HIGH) {
    buzzerSound();
    credits += 5;
    gamesWon++;
   invokeCelebration();
   
  }

  // green LEDs
  else if (digitalRead(LED_r2c1) == HIGH && digitalRead(LED_r2c2) == HIGH &&
        digitalRead(LED_r2c3) == HIGH) {
    buzzerSound();
    credits += 10;
    gamesWon++;
    invokeCelebration();
  }
  // yellow LEDs
  else if (digitalRead(LED_r3c1) == HIGH && digitalRead(LED_r3c2) == HIGH &&
        digitalRead(LED_r3c3) == HIGH) {
      buzzerSound();
      credits += 15; 
      gamesWon++; 
      invokeCelebration();
      }
    }

//How LEDs within each column are being lit up
int ticks[] = { 0, 0, 0 };
void loopBlink(int which_column, int column[], int led_period) {

  if (ticks[which_column] < led_period) {
    digitalWrite(column[0], HIGH);
    digitalWrite(column[1], LOW);
    digitalWrite(column[2], LOW);
  } 
  else if(ticks[which_column] < 2*led_period) {
    digitalWrite(column[0], LOW);
    digitalWrite(column[1], HIGH);
    digitalWrite(column[2], LOW);
  } 
  else {
    digitalWrite(column[0], LOW);
    digitalWrite(column[1], LOW);
    digitalWrite(column[2], HIGH);
  }

  ++ticks[which_column];
  if (ticks[which_column] == 3*led_period) {
    ticks[which_column] = 0;
  }

}

//The function that sets off the celebratory lights
void invokeCelebration() {

  // digitalWrite(celebrationLEDs, HIGH);
  // delay(500);
  // digitalWrite(celebrationLEDs, LOW);
  // delay(500);
  // digitalWrite(celebrationLEDs, HIGH);
  // delay(500);
  // digitalWrite(celebrationLEDs, LOW);
  // delay(500);

//Decided to go with a sequence of fades instead
   int level;
    for (level = 0; level != 256; level++) {
    analogWrite(celebrationLEDs, level);
    delay(1);
  }

  for (level = 255; level >= 0; level--) {
    analogWrite(celebrationLEDs, level);
    delay(1);
  }
}

//Function to get stats from serial monitor 
void loopSerialControl() {
  if (Serial.available() > 0) {
    String inputLine = Serial.readStringUntil('\n');
          //"s/S" for stats
    if (inputLine == "s" || inputLine == "S") {
        Serial.print("\nGames played: ");
        Serial.print(gamesPlayed);
        Serial.print("\nGames won: ");
        Serial.print(gamesWon);
    } 
  }
}

void displayNoCreditMessage() {
      Oled.setCursor(0, 0); 
      Oled.print("No credits!");
}

void displayEmptyMessage() {
      Oled.setCursor(0, 0); 
      Oled.print("               ");
}

void displayCreditMessage() {
    Oled.setCursor(0, 0); // reposition the cursor
    Oled.print("Credits: "); 
      if(credits < 10) {
        Oled.print("  "); // remove the last 0
        Oled.setCursor(9, 0); // reposition the cursor
      }
      Oled.print(credits); 
}

//Function to invoke buzzer sound - will only be called when a winning pattern is detected
void buzzerSound() {
  tone(buzzer, 440);
  delay(1000);
  noTone(buzzer);
  delay(500);
}

void loop()
{
  checkCredits();
  checkSpin();
  loopSerialControl();
  spinTheWheel();

  // if(!spin) {
  //   attractMode();
  // }

}
