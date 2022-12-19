/*!
 * @file FusionPhaser_1.0.ino
 * @brief 10 button & joystick lightgun 
 * @n INO file for FusionPhaser Light Gun setup
 * @version  V1.0
 * @date  2022
 */

 /* HOW TO CALIBRATE:
 *  
 *  Step 1: Push Calibration Button
 *  Step 2: Pull Trigger
 *  Step 3: Shoot Center of the Screen (try an do this as acuratley as possible)
 *  Step 4: Mouse should lock to vertiucal axis, use A/B buttons to adjust mouse up/down
 *  Step 5: Pull Trigger
 *  Step 6: Mouse should lock to horizontal axis, use A/B buttons to adjust mouse left/right
 *  Step 7: Pull Trigger to finish
 *  Step 8: Offset are now saved to EEPROM
 *
 *   ATTENTION!!!!!!!!
 *   BUILT FOR PRO MICRO 32U4 5V
 */


#include <HID.h>                // Load libraries
#include <Wire.h>
#include <Keyboard.h>
#include <AbsMouse.h>
#include <DFRobotIRPosition.h>
#include <FusionPhaser.h>
#include <EEPROM.h>

int xCenter = 512;              // Open serial monitor and update these values to save calibration
int yCenter = 450;
float xOffset = 147;             
float yOffset = 82;
       
char _upKey = KEY_UP_ARROW;        // You can update your keyboard keys here        
char _downKey = KEY_DOWN_ARROW;              
char _leftKey = KEY_LEFT_ARROW;             
char _rightKey = KEY_RIGHT_ARROW;                             
char _startKey = KEY_RETURN; 
char _selectKey = KEY_BACKSPACE; 

int finalX;                 // Values after tilt correction
int finalY;

int MoveXAxis;              // Unconstrained mouse postion
int MoveYAxis;               

int conMoveXAxis;           // Constrained mouse postion
int conMoveYAxis;           

int count = 4;                   // Set intial count

int _triggerPin = 4;               // Label Pin to buttons
int _upPin = 6;                
int _downPin = 7;              
int _leftPin = 8;             
int _rightPin = 9;               
int _APin = A0;                
int _BPin = A1;              
int _startPin = A2; 
int _selectPin = A3;               
int _caliPin = 5;
int _pedalPin = 15;                //NOTE: Pedal needs to connected to pin 4 on 3V boards  
#define joystick_X 14    //joystick  X-as
#define joystick_Y 16    //joystick  Y-as
#define button_2 10       // button on d2



int buttonState1 = 0;           
int lastButtonState1 = 0;
int buttonState2 = 0;
int lastButtonState2 = 0;
int buttonState3 = 0;
int lastButtonState3 = 0;
int buttonState4 = 0;         
int lastButtonState4 = 0; 
int buttonState5 = 0;           
int lastButtonState5 = 0;
int buttonState6 = 0;
int lastButtonState6 = 0;
int buttonState7 = 0;
int lastButtonState7 = 0;
int buttonState8 = 0;         
int lastButtonState8 = 0;          
int buttonState9 = 0;           
int lastButtonState9 = 0;
int buttonState10 = 0;
int lastButtonState10 = 0; 
int buttonState11 = 0;
int lastButtonState11 = 0; 

int plus = 0;         
int minus = 0;

DFRobotIRPosition myDFRobotIRPosition;
FusionPhaser myFusionPhaser;

int res_x = 1023;              // UPDATE: These values do not need to change
int res_y = 768;               // UPDATE: These values do not need to change


//*****************Joystick***********************************************//
//****************define the min, max and center you want for your joystick here*******//
long range_X_min = 1000;           //lowest X value
long range_X_max = 10000;          //highest X value
long range_X_center = 0;           //X center calue
unsigned long deadZone_X = 1;      //deadzone X, return center value for center +- deadzone(in steps of 1/1024)

float range_Y_min = 0;             //lowest Y value
float range_Y_max = 1023;          //highest Y value
float range_Y_center = 512;        //Y center value
unsigned long deadZone_Y = 1;      //deadzone Y, return center value for center +- deadzone(in steps of 1/1024)


//***************calibration variables************************************************//
int val_X, val_Y;

unsigned long X_CENTER, X_MIN, X_MAX;
unsigned long Y_CENTER, Y_MIN, Y_MAX;

unsigned long cal_X;
unsigned long cal_Y;

void setup() {

  myDFRobotIRPosition.begin();               // Start IR Camera
   
  Serial.begin(9600);
  pinMode(joystick_X, INPUT);
  pinMode(joystick_Y, INPUT);
  pinMode(button_2, INPUT_PULLUP);    //we pull this pin high to avoid a floating pin
  calibrate();
}

long joy_X(long X) {
  X += (X_CENTER-512);   //should always be 512
  if (X <= 512) {
    X = mapfloat(X, X_MIN, X_CENTER, range_X_min, range_X_center);   //min,center to range min, range center
    return X;
  }
  else if (X > 512) {
    X = mapfloat(X, 512, 1024 + X_CENTER, range_X_center, range_X_max);
    return X;
  }
}

long joy_Y(long Y) {
  Y += (Y_CENTER-512);
  if (Y <= range_Y_center) {
    Y = mapfloat(Y, Y_CENTER, 512, 0, 512);
    return Y;
  }
  else if (Y > range_Y_center) {
    Y = mapfloat(Y, 512, 1024 + Y_CENTER, 512, 1024);
    return Y;
  }
  Serial.begin(115200);                        // For saving calibration (make sure your serial monitor has the same baud rate)

  loadSettings();
    
  AbsMouse.init(res_x, res_y);            

  pinMode(_triggerPin, INPUT_PULLUP);         // Set pin modes
  pinMode(_upPin, INPUT_PULLUP);
  pinMode(_downPin, INPUT_PULLUP);
  pinMode(_leftPin, INPUT_PULLUP);
  pinMode(_rightPin, INPUT_PULLUP);          // Set pin modes
  pinMode(_APin, INPUT_PULLUP);
  pinMode(_BPin, INPUT_PULLUP);
  pinMode(_startPin, INPUT_PULLUP);  
  pinMode(_selectPin, INPUT_PULLUP);
  pinMode(_caliPin, INPUT_PULLUP);       
  pinMode(_pedalPin, INPUT_PULLUP);
  pinMode(joystick_X, INPUT);
  pinMode(joystick_Y, INPUT);
  pinMode(button_2, INPUT_PULLUP);    //we pull this pin high to avoid a floating pin


  AbsMouse.move((res_x / 2), (res_y / 2));          // Set mouse position to centre of the screen
  
  delay(500);
  
}


void loop() {
      val_X = analogRead(joystick_X);
  Serial.print(joy_X(val_X)); Serial.print(" : ");
  delay(50);
  val_Y = analogRead(joystick_Y);
  Serial.println(joy_Y(val_Y));
  delay(50);
  if (digitalRead(button_2) == LOW) {
    calibrate();    //you can call any function you want here, this is just to show the possibilities
  }

/* ------------------ START/PAUSE MOUSE ---------------------- */


  if (count > 3 ) {


    skip();
    mouseCount();
    PrintResults();


  }


  /* ---------------------- CENTRE --------------------------- */


  else if (count > 2 ) {

    AbsMouse.move((res_x / 2), (res_y / 2));

    mouseCount();
    getPosition();

    xCenter = finalX;
    yCenter = finalY;

    PrintResults();

  }


  /* -------------------- OFFSET ------------------------- */


  else if (count > 1 ) {

    mouseCount();
    AbsMouse.move(conMoveXAxis, conMoveYAxis);
    getPosition();

    MoveYAxis = map (finalY, (yCenter + ((myFusionPhaser.H() * (yOffset / 100)) / 2)), (yCenter - ((myFusionPhaser.H() * (yOffset / 100)) / 2)), 0, res_y);
    conMoveXAxis = res_x/2;
    conMoveYAxis = constrain (MoveYAxis, 0, res_y);
    
    if (plus == 1){
    yOffset = yOffset + 1;
    delay(10);
    } else {
      }

    if (minus == 1){
    yOffset = yOffset - 1;
    delay(10);
    } else {
      }
      
    PrintResults();

  }

  
  else if (count > 0 ) {

    mouseCount();
    AbsMouse.move(conMoveXAxis, conMoveYAxis);
    getPosition();

    MoveXAxis = map (finalX, (xCenter + ((myFusionPhaser.H() * (xOffset / 100)) / 2)), (xCenter - ((myFusionPhaser.H() * (xOffset / 100)) / 2)), 0, res_x);
    conMoveXAxis = constrain (MoveXAxis, 0, res_x);
    conMoveYAxis = res_y/2;
    
    if (plus == 1){
    xOffset = xOffset + 1;
    delay(10);
    } else {
      }

    if (minus == 1){
    xOffset = xOffset - 1;
    delay(10);
    } else {
      }
      
    PrintResults();

  }

  else if (count > -1) {
    
    count = count - 1;
    
    EEPROM.write(0, xCenter - 256);
    EEPROM.write(1, yCenter - 256);
    EEPROM.write(2, xOffset);
    EEPROM.write(3, yOffset);
  }


  /* ---------------------- LET'S GO --------------------------- */


  else {

    AbsMouse.move(conMoveXAxis, conMoveYAxis);

    mouseButtons();
    getPosition();

    MoveXAxis = map (finalX, (xCenter + ((myFusionPhaser.H() * (xOffset / 100)) / 2)), (xCenter - ((myFusionPhaser.H() * (xOffset / 100)) / 2)), 0, res_x);
    MoveYAxis = map (finalY, (yCenter + ((myFusionPhaser.H() * (yOffset / 100)) / 2)), (yCenter - ((myFusionPhaser.H() * (yOffset / 100)) / 2)), 0, res_y);
    conMoveXAxis = constrain (MoveXAxis, 0, res_x);
    conMoveYAxis = constrain (MoveYAxis, 0, res_y);
    
    PrintResults();
    reset();

  }

}


/*        -----------------------------------------------        */
/* --------------------------- METHODS ------------------------- */
/*        -----------------------------------------------        */


void getPosition() {    // Get tilt adjusted position from IR postioning camera

myDFRobotIRPosition.requestPosition();
    if (myDFRobotIRPosition.available()) {
    myFusionPhaser.begin(myDFRobotIRPosition.readX(0), myDFRobotIRPosition.readY(0), myDFRobotIRPosition.readX(1), myDFRobotIRPosition.readY(1),myDFRobotIRPosition.readX(2), myDFRobotIRPosition.readY(2),myDFRobotIRPosition.readX(3), myDFRobotIRPosition.readY(3), xCenter, yCenter);
    finalX = myFusionPhaser.X();
    finalY = myFusionPhaser.Y();
    }
    else {
    Serial.println("Device not available!");
    }
}


void go() {    // Setup Start Calibration Button

  buttonState1 = digitalRead(_caliPin);

  if (buttonState1 != lastButtonState1) {
    if (buttonState1 == LOW) {
      count--;
    }
    else { // do nothing
    }
    delay(50);
  }
  lastButtonState1 = buttonState1;
}


void mouseButtons() {    // Setup Left, Right & Middle Mouse buttons

  buttonState2 = digitalRead(_triggerPin);
  buttonState3 = digitalRead(_upPin);  
  buttonState4 = digitalRead(_downPin);
  buttonState5 = digitalRead(_leftPin);
  buttonState6 = digitalRead(_rightPin);   
  buttonState7 = digitalRead(_APin);
  buttonState8 = digitalRead(_BPin);
  buttonState9 = digitalRead(_startPin);      
  buttonState10 = digitalRead(_selectPin); 
  buttonState11 = digitalRead(_pedalPin); 
  
  if (buttonState2 != lastButtonState2) {
    if (buttonState2 == LOW) {
      AbsMouse.press(MOUSE_LEFT);
    }
    else {
      AbsMouse.release(MOUSE_LEFT);
    }
    delay(10);
  }

  if (buttonState3 != lastButtonState3) {
    if (buttonState3 == LOW) {
    Keyboard.press(_upKey);
    }
    else {
    Keyboard.release(_upKey);
    }
    delay(10);
  }

  if (buttonState4 != lastButtonState4) {     
    if (buttonState4 == LOW) {
    Keyboard.press(_downKey);
    }
    else {
    Keyboard.release(_downKey);
    }
    delay(10);
  }

  if (buttonState5 != lastButtonState5) {
    if (buttonState5 == LOW) {
    Keyboard.press(_leftKey);
    }
    else {
    Keyboard.release(_leftKey);
    }
    delay(10);
  }
  
  if (buttonState6 != lastButtonState6) {
    if (buttonState6 == LOW) {
    Keyboard.press(_rightKey);
    }
    else {
    Keyboard.release(_rightKey);
    }
    delay(10);
  }

  if (buttonState7 != lastButtonState7) {
    if (buttonState7 == LOW) {
      AbsMouse.press(MOUSE_RIGHT);
    }
    else {
      AbsMouse.release(MOUSE_RIGHT);
    }
    delay(10);
  }

  if (buttonState8 != lastButtonState8) {     
    if (buttonState8 == LOW) {
      AbsMouse.press(MOUSE_MIDDLE);
    }
    else {
      AbsMouse.release(MOUSE_MIDDLE);
    }
    delay(10);
  }
  if (buttonState9 != lastButtonState9) {
    if (buttonState9 == LOW) {
    Keyboard.press(_startKey);
    }
    else {
    Keyboard.release(_startKey);
    }
    delay(10);
  }
  
  if (buttonState10 != lastButtonState10) {
    if (buttonState10 == LOW) {
    Keyboard.press(_selectKey);
    }
    else {
    Keyboard.release(_selectKey);
    }
    delay(10);
  }

  if (buttonState11 != lastButtonState11) {
    if (buttonState11 == LOW) {
      AbsMouse.press(MOUSE_RIGHT);
    }
    else {
      AbsMouse.release(MOUSE_RIGHT);
    }
    delay(10);
  }

  lastButtonState2 = buttonState2;
  lastButtonState3 = buttonState3;
  lastButtonState4 = buttonState4;      
  lastButtonState5 = buttonState5;
  lastButtonState6 = buttonState6;
  lastButtonState7 = buttonState7;
  lastButtonState8 = buttonState8;
  lastButtonState9 = buttonState9;
  lastButtonState10 = buttonState10; 
  lastButtonState11 = buttonState11;     
}


void mouseCount() {    // Set count down on trigger

  buttonState2 = digitalRead(_triggerPin);
  buttonState3 = digitalRead(_BPin);
  buttonState4 = digitalRead(_APin);   

  if (buttonState2 != lastButtonState2) {
    if (buttonState2 == LOW) {
      count--;
    }
    else {
    }
    delay(10);
  }

  if (buttonState3 != lastButtonState3) {
    if (buttonState3 == LOW) {
      plus = 1;
    }
    else {
      plus = 0;
    }
    delay(10);
  }

  if (buttonState4 != lastButtonState4) {     
    if (buttonState4 == LOW) {
      minus = 1;
    }
    else {
      minus = 0;
    }
    delay(10);
  }

  lastButtonState2 = buttonState2;
  lastButtonState3 = buttonState3;
  lastButtonState4 = buttonState4;            
}


void reset() {    // Pause/Re-calibrate button

  buttonState1 = digitalRead(_caliPin);

  if (buttonState1 != lastButtonState1) {
    if (buttonState1 == LOW) {
      count = 4;
      delay(50);
    }
    else { // do nothing
    }
    delay(50);
  }
  lastButtonState1 = buttonState1;
}


void skip() {    // Unpause button

  buttonState1 = digitalRead(_caliPin);

  if (buttonState1 != lastButtonState1) {
    if (buttonState1 == LOW) {
      count = 0;
      delay(50);
    }
    else { // do nothing
    }
    delay(50);
  }
  lastButtonState1 = buttonState1;
}


void loadSettings() {
  if (EEPROM.read(1023) == 'T') {
    //settings have been initialized, read them
    xCenter = EEPROM.read(0) + 256;
    yCenter = EEPROM.read(1) + 256;
    xOffset = EEPROM.read(2);
    yOffset = EEPROM.read(3);
  } else {
    //first time run, settings were never set
    EEPROM.write(0, xCenter - 256);
    EEPROM.write(1, yCenter - 256);
    EEPROM.write(2, xOffset);
    EEPROM.write(3, yOffset);
    EEPROM.write(1023, 'T');    
  }
}

void set_range_X(float min, float max, float center) {
  range_X_min = min;
  range_X_max = max;
  range_X_center = center;
}

void set_range_Y(float min, float max, float center) {
  range_Y_min = min;
  range_Y_max = max;
  range_Y_center = center;
}

void calibrate() {
  Serial.println("\n---calibrating joystick---\n");
  Serial.println("place the joystick in the center position");
  cal_X = 0;
  cal_Y = 0;
  delay(2500);
  Serial.print("calibrating center");
  for (int i = 0; i < 100; i++) {
    Serial.print(".");
    cal_X += analogRead(joystick_X);
    delay(5);
    cal_Y += analogRead(joystick_Y);
    delay(5);
  }
  X_CENTER = (cal_X/100);
  Y_CENTER = (cal_Y/100);
  Serial.print("\nCorrection X: ");Serial.print(X_CENTER);
  Serial.print("\nCorrection Y: ");Serial.println(Y_CENTER);
  
  Serial.println("\nplace the joystick in the bottom-left corner");
  X_MIN = 0;    //reset the values
  Y_MIN = 0;
  delay(2500);
  Serial.print("calibrating position");
   for (int i = 0; i < 100; i++) {    //take 100 readings
    Serial.print(".");
    X_MIN += analogRead(joystick_X);
    delay(5);
    Y_MIN += analogRead(joystick_Y);
    delay(5);
  }
  X_MIN /= 100;
  Y_MIN /= 100;
  Serial.println();
  Serial.print("X: "); Serial.println(X_MIN);
  Serial.print("Y: "); Serial.println(Y_MIN);

Serial.println("\nplace the joystick in the top-right corner");
  X_MAX = 0;    //reset the values
  Y_MAX = 0;
  delay(2500);
  Serial.print("calibrating position");
   for (int i = 0; i < 100; i++) {    //take 100 readings
    Serial.print(".");
    X_MAX += analogRead(joystick_X);
    delay(5);
    Y_MAX += analogRead(joystick_Y);
    delay(5);
  }
  X_MAX /=  100;
  Y_MAX /=  100;
  Serial.println();
  Serial.print("X: "); Serial.println(X_MAX);
  Serial.print("Y: "); Serial.println(Y_MAX);

  if(X_MAX < X_MIN){
    unsigned long val = X_MAX;
    X_MAX = X_MIN;
    X_MIN = val;
  }
  if(Y_MAX < Y_MIN){
    unsigned long val = Y_MAX;
    Y_MAX = Y_MIN;
    Y_MIN = val;
  }

  Serial.print("\nrange X: ");Serial.print(X_MIN);Serial.print(" - ");Serial.println(X_MAX);
  Serial.print("range X: ");Serial.print(Y_MIN);Serial.print(" - ");Serial.println(Y_MAX);
    
  Serial.println("\n---calibration done---\n");
}

void PrintResults() {    // Print results for saving calibration

  Serial.print("CALIBRATION:");
  Serial.print("     Cam Center x/y: ");
  Serial.print(xCenter);
  Serial.print(", ");
  Serial.print(yCenter);
  Serial.print("     Offsets x/y: ");
  Serial.print(xOffset);
  Serial.print(", ");
  Serial.println(yOffset);

}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
