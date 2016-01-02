//-------------------------------------------------------------------------
//  
//  Solar Tracker Using Stepper Motors
//  
//  By : Evan Richinson, Aka MrRetupmoc42
//  Date : December 23, 2015
//  
//-------------------------------------------------------------------------

//Pin Setup
int YdirectionPin = 2;
  int YstepPin = 3;
int XdirectionPin = 4;
  int XstepPin = 5;
int sensorPin = A0;
  int sensorValue = 0;
  

int LightValue[4]; //lightX, positionX, lightY, positionY
  int x,y = 0;
  int ymore, yless, xmore, xless = 0;

//Timings
int Ystepdelay = 13; //Y Movement Delay
int Xstepdelay = 15; //X Movement Delay
int steppause = 250; //Time Pause Between Movements
int trackingupdatepause = 3*1000;

//Angles / Counts for Moves
int YInitScanCount = 85; //Count = Degree ---> 70 = 170? 80 = 160?
int XInitScanCount = 120; //Count = Degree ---> 75 = 120? 100 = 160? 120 = 180? 
int sampleAngleCount = 5;

//debug output
boolean debug = false;

//-------------------------------------------------------------------------

void setup() {
  pinMode(XdirectionPin, OUTPUT);
  pinMode(XstepPin, OUTPUT);
  pinMode(YdirectionPin, OUTPUT);
  pinMode(YstepPin, OUTPUT);
  
  Serial.begin(9600);
    Serial.println(".");
    Serial.println("Hello Sunny World.");
    Serial.println("Now Finding Light...");
  
  //Get Panel Up and Somewhat Pointed
  InitScanYAxis();
  GotoBestYPos();
  
  //Get the Rough X Direction with best value
  //If Y Axis is not Pointing Straight Up (Halfway)
  if (LightValue[3] != (YInitScanCount/2)) {
    InitScanXAxis();
    GotoBestXPos();
  }
}

void loop() {
  //Tune in X Axis and Y Axis within every 60 Seconds ( 10 Degree? Window )
  ScanYAxis();
  ScanXAxis();
  
  delay(trackingupdatepause);
}

//-------------------------------------------------------------------------

void InitScanYAxis() {
  digitalWrite(YdirectionPin, LOW);
  MoveY(YInitScanCount, true);
  /*
  for(int i = 0; i < YInitScanCount; i++) {
    digitalWrite(YstepPin, LOW);
    delay(Ystepdelay);
    digitalWrite(YstepPin, HIGH);
    delay(Ystepdelay);
    
    y = checkLightLevels();
    if (y > LightValue[2]) {
      LightValue[2] = y;
      LightValue[3] = i;

      Serial.print("Light Value : ");
      Serial.print(LightValue[2]);
      Serial.print(" @ X = ");
      Serial.println(LightValue[3]);
    }
  }
  */
  printvalues();
}

void GotoBestYPos() {
  printvalues();
  
  digitalWrite(YdirectionPin, HIGH);
  MoveY(YInitScanCount - LightValue[3], false);
}

void InitScanXAxis() {
  //Move Half the Rotation
  digitalWrite(XdirectionPin, LOW);
  MoveX(XInitScanCount / 2, false);
  
  //Scan the X-Axis
  digitalWrite(XdirectionPin, HIGH);
  MoveY(XInitScanCount, true);
  /*
  for(int i = 0; i < XInitScanCount; i++) {
    digitalWrite(XstepPin, LOW);
    delay(Xstepdelay);
    digitalWrite(XstepPin, HIGH);
    delay(Xstepdelay);
    
    x = checkLightLevels();
    if (x > LightValue[0]) {
      LightValue[0] = x;
      LightValue[1] = i;
      
      Serial.print("Light Value : ");
      Serial.print(LightValue[0]);
      Serial.print(" @ X = ");
      Serial.println(LightValue[1]);
    }
  }
  */
  printvalues();
}

void GotoBestXPos() {
  printvalues();
  
  digitalWrite(XdirectionPin, LOW);
  MoveX(XInitScanCount - LightValue[1], false);
}

void ScanYAxis() {
  //if (LightValue[3] >  < 0) { // Not Higher or less than the bounds
    //Take Current Reading at Position
    LightValue[3] = checkLightLevels();
    
    //Take Reading on a Greater Angle / Posistion
    digitalWrite(YdirectionPin, HIGH);
    MoveY(sampleAngleCount,false);
    ymore = checkLightLevels();
    
    //Take Reading on a Lesser Angle / Posistion
    digitalWrite(XdirectionPin, LOW);
    MoveY(sampleAngleCount*2,false);
    yless = checkLightLevels();
    
    //Compare and make a move
    //if(ymore > LightValue[1]) { //Stay at Current Pos
    if (yless > LightValue[1]) { // Go Back to the Other Pos
      digitalWrite(XdirectionPin, LOW);
      MoveY(sampleAngleCount*2,false);
    }
    else { //Init Position was Better
      digitalWrite(XdirectionPin, LOW);
      MoveY(sampleAngleCount,false);
    }
  //}
}

void ScanXAxis() {
  //if (LightValue[1] >  < 0) { // Not Higher or less than the bounds
    
    LightValue[1] = checkLightLevels();
    
    digitalWrite(XdirectionPin, HIGH);
    MoveX(sampleAngleCount,false);
    xmore = checkLightLevels();
    
    digitalWrite(XdirectionPin, LOW);
    MoveX(sampleAngleCount*2,false);
    xless = checkLightLevels();
    
    //Compare and make a move
    if(xmore > LightValue[1]) { //Stay at Highest Pos
    }
    else if (xless > LightValue[1]) { // Go Back to Lowest Pos
      digitalWrite(XdirectionPin, LOW);
      MoveX(sampleAngleCount*2,false);
    }
    else { //Init Position was Better
      digitalWrite(XdirectionPin, LOW);
      MoveX(sampleAngleCount,false);
    }
  //}
}

void MoveY(int count, boolean saveValues) {
  for(int i = 0; i < count; i++) {
    digitalWrite(YstepPin, LOW);
    delay(Ystepdelay);
    digitalWrite(YstepPin, HIGH);
    delay(Ystepdelay);
    
    if(saveValues) {
      y = checkLightLevels();
      if (y > LightValue[2]) {
        LightValue[2] = y;
        LightValue[3] = i;

        Serial.print("Light Value : ");
        Serial.print(LightValue[2]);
        Serial.print(" @ Y = ");
        Serial.println(LightValue[3]);
      }
    }
  }
  delay(steppause);
}

void MoveX(int count, boolean saveValues) {
  for(int i = 0; i < count; i++) {
    digitalWrite(XstepPin, LOW);
    delay(Xstepdelay);
    digitalWrite(XstepPin, HIGH);
    delay(Xstepdelay);
    
    if(saveValues) {
      x = checkLightLevels();
      if (x > LightValue[0]) {
        LightValue[0] = x;
        LightValue[1] = i;
        
        Serial.print("Light Value : ");
        Serial.print(LightValue[0]);
        Serial.print(" @ X = ");
        Serial.println(LightValue[1]);
      }
    }
  }
  delay(steppause);
}

void printvalues() {
    Serial.print(" When Scanning X : ");
    Serial.print(LightValue[0]);
    Serial.print(" @ X = ");
    Serial.print(LightValue[1]);
    Serial.print(" When Scanning Y : ");
    Serial.print(LightValue[2]);
    Serial.print(" @ Y = ");
    Serial.println(LightValue[3]);
}

int checkLightLevels() {
  sensorValue = analogRead(sensorPin);
    if (debug) {
      Serial.print("Light Value : ");
      Serial.println(sensorValue);
    }
  return sensorValue;
}
