#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int lcdKey = btnNONE;
int keyIn = 0;

int diceNum = 1, diceSides = 4, diceType = 1, result = 0;

bool buttonPressed = false, buttonHold = false, roll = false, statsRoll = false, debug = false;



void setup() {
  lcd.begin(16,2);
  
  //Are we debugging?
  if(debug) { Serial.begin(9600); }
  
  //Randomize the seed by reading off of a disconnected pin.
  randomSeed(analogRead(3));
  
  //If the user isn't holding down a button at boot, run the intro.
  lcdKey = readButtons();
  if(lcdKey == btnNONE) {
    runIntro();
  }
  
}



//This function runs the introduction/instruction sequence.
void runIntro() {
  lcd.setCursor(0,0);
  lcdWrite("  Dice Roller!");
  delay(500);
  lcd.setCursor(0,1);
  lcdWrite(" Press a button");
  
  while(lcdKey == btnNONE) { lcdKey = readButtons(); }
  
  if(lcdKey != btnLEFT) {
    lcd.clear();
    lcdWrite("L/R: Dice Type");
    lcd.setCursor(0,1);
    lcdWrite("U/D: Number");
    
    delay(500);
    
    lcd.clear();
    lcdWrite("Press select");
    lcd.setCursor(0,1);
    lcdWrite("to roll dice!");
    
    delay(500);
    
    lcd.clear();
    lcdWrite("Select 0 dice");
    lcd.setCursor(0,1);
    lcdWrite("to roll for");
    
    delay(500);
    
    lcd.clear();
    lcdWrite("stats, 4d6 drop");
    lcd.setCursor(0,1);
    lcdWrite("the lowest die.");
    
    delay(500);
  
    lcd.clear();
    lcd.setCursor(0,0);
    lcdWrite("Dice:   ");
    lcdWrite(String(diceNum));
    lcdWrite("d");
    lcdWrite(String(diceSides));
    
    lcd.setCursor(0,1);
    lcdWrite("Result: ");
    lcdWrite(String(result));
  }
}



//The lcdWrite() function will print characters out on the lcd
//in a pretty fashion.  The argument is the string to be printed.
void lcdWrite(String writeString) {
  for(int i = 0; i < writeString.length(); i++) {
    lcd.write(writeString.charAt(i));
    delay(25);
  }
}

//This function rolls dice, generating appropriate random numbers.
int rollDice(int num, int sides) {
  int total = 0;
  
  //Up to the number of dice we're rolling...
  for(int i = 0; i < num; i++) {
    //... find the value of each die roll.  The random() function returns
    //a random value, based on the seed, from the first argument to the
    //second, including the first as a possibility, but not including
    //the second.  So we account for that.
    total += random(1, sides+1);
  }
  
  return(total);
}



//This function rolls dice to generate character stats.  4d6, drop
//the lowest result.
int statsRollDice() {
  int dice[4], total = 0, lowest;
  
  //Generate four random numbers from 1 to 6.
  for(int i = 0; i < 4; i++) {
    dice[i] = random(1, 7);
    if(debug) {
      Serial.print(dice[i]);
      Serial.print(", ");
    }
  }
  
  //Find the lowest value of these four numbers.
  
  if(dice[0] <= dice[1]) { lowest = 0; }
  else { lowest = 1; }
  if(dice[lowest] > dice[2]) { lowest = 2; }
  if(dice[lowest] > dice[3]) { lowest = 3; }
  
  
  //Alternative method of divining the lowest die.  Unnecessarily complicated.
  /*
  if(dice[0] <= dice[1] && dice[0] <= dice[2] && dice[0] <= dice[3]) { lowest = 0; }
  else if(dice[1] <= dice[0] && dice[1] <= dice[2] && dice[1] <= dice[3]) { lowest = 1; }
  else if(dice[2] <= dice[0] && dice[2] <= dice[1] && dice[2] <= dice[3]) { lowest = 2; }
  else { lowest = 3; }
  */
  
  if(debug) {
    Serial.print("so the lowest is: ");
    Serial.println(dice[lowest]);
  }
  
  //Set the lowest die to 0.
  dice[lowest] = 0;
  
  //Add all dice together and return the value.
  for(int i = 0; i < 4; i++) {
    total += dice[i];
  }
  return(total);
}


//This function will tell us what button the user has pressed.  It
//returns the button in question.
int readButtons()
{
  //Read the key in analog pin, then wait 5 milliseconds and read it
  //again.  If they differ by too much, assume it's noise and return
  //no button pressed.
  keyIn = analogRead(0);
  delay(3);
  int k = (analogRead(0) - keyIn);
  if (5 < abs(k)) return btnNONE; 
  
  //If we pass the debounce, return the correct button being pressed.
  if (keyIn > 1000) return btnNONE; 
  if (keyIn < 50)   return btnRIGHT;  
  if (keyIn < 195)  return btnUP; 
  if (keyIn < 380)  return btnDOWN; 
  if (keyIn < 555)  return btnLEFT; 
  if (keyIn < 790)  return btnSELECT;
  //If we made it this far, just return no button pressed.
  return btnNONE;
}



void loop() {
  //Start by reading the button being pressed.
  lcdKey = readButtons();
  
  //Determine what to do based on the key being pressed.
  switch (lcdKey) {
    case btnRIGHT:
      {
        //If the user hits right, move to a higher dice type.
        if(!buttonPressed) {
          buttonPressed = true;
          diceType++;
          result = 0;
        }
        break;
      }
    case btnLEFT:
      {
        //If the user hits left, move to a lower dice type.
        if(!buttonPressed) {
          buttonPressed = true;
          diceType--;
          result = 0;
        }
        break;
      }
    case btnUP:
      {
        //If the user hits up, increase the number of dice to roll.
        if(!buttonPressed) {
          buttonPressed = true;
          diceNum++;
          result = 0;
        }
        break;
      }
    case btnDOWN:
      {
        //If the user hits down, roll fewer dice.  Or go into one of
        //our other modes, rolling stats or numenera results.
        if(!buttonPressed) {
          buttonPressed = true;
          diceNum--;
          result = 0;
        }
        break;
      }
    case btnSELECT:
      {
        //If the user hits select, actually roll some dice.
        if(!buttonPressed) {
          buttonPressed = true;
          roll = true;
        }
        break;
      }
    case btnNONE:
      {
        //If the user didn't hit a button, make sure that we know it.
        buttonPressed = false;
        buttonHold = false;
        break;
      }
        
  }
  
  //If the user pressed a button, and it's the first time
  //through the loop, do this.
  if(buttonPressed && !buttonHold) { 
    lcd.clear();
    
    //Make sure our selection makes sense.
    if(diceNum < 0) { diceNum = 0; }
    if(diceNum > 100) { diceNum = 100; }
    if(diceType < 1) { diceType = 1; }
    if(diceType > 7) { diceType = 7; }
    
    if(diceNum == 0) { statsRoll = true; }
    else if(diceNum > 0) { statsRoll = false; }
    
    //Figure out what type of dice we're rolling!
    switch(diceType) {
      case 1: { diceSides = 4; break; }
      case 2: { diceSides = 6; break; }
      case 3: { diceSides = 8; break; }
      case 4: { diceSides = 10; break; }
      case 5: { diceSides = 12; break; }
      case 6: { diceSides = 20; break; }
      case 7: { diceSides = 100; break; }
    }
    
    //If the user hit the roll button, and it's not a stat roll,
    //roll the dice.  If it is a stat roll, call that function instead.
    if(roll) {
      if(!statsRoll) { result = rollDice(diceNum, diceSides); }
      else if(statsRoll) { result = statsRollDice(); }
      roll = false;
    }
    
    //Initialize a boolean for numenera mode.
    bool numenera = false;
    
    //If the user hit down when on the d20 selection, go into 
    //numenera mode!
    if(diceNum == 0 && diceSides == 20) { numenera = true; }
    
    //Print some shit.
    lcd.setCursor(0,0);
    if(!statsRoll) {
      lcd.print("Dice:   ");
      lcd.print(diceNum);
      lcd.print("d");
      lcd.print(diceSides);
    }
    //If we're in numenera mode, print some different shit.
    else if(numenera && statsRoll) {
      lcd.print("Numenera Mode");
    }
    //Or if we're in statroll mode, print some even differenter shit.
    else if(statsRoll) {
      lcd.print("Dice: Stat Roll");
    }
    
    //If we're not in numenera mode, print normal results
    lcd.setCursor(0,1);
    if(!numenera) {
      lcd.print("Result: ");
      if(result > 0) { lcd.print(result); }
    }
    //Otherwise, print numenera results.  Tell the user the die result,
    //the level of obstacle beaten, and the bonus damage on this roll if
    //it's high enough.
    else if(numenera) {
      int lvlNum = 0;
      for(int i = 0; i < result/3; i++) { lvlNum++; }
      lcd.print("Die: ");
      lcd.print(result);    
      if(result < 10) { lcd.print(" "); }
      lcd.print(" Lvl: ");
      lcd.print(lvlNum);
      if(result > 16) {
        lcd.print("+");
        lcd.print(result - 16);
      }
    }
    
    //Because we did something due to a button press, make sure we know
    //it next time around.
    buttonHold = true;
  }
}
