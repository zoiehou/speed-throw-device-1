// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

// ========= DOT MATRIX SETUP ==========
#include "MatrixCascade.h" //incl. library file

//Define matrix pins
#define clk 10
#define cs 11
#define din 12

const int maxCols = 75;
uint8_t cols[maxCols]; //cols is the array for text that you want to display
uint8_t colsCount = sizeof(cols); //set length to 64
// ========= DOT MATRIX SETUP ==========

// ========= WIRED BUTTON SETUP ==========
#define buttonA A1   // USB white (data 2) - red button; wired button
#define buttonB A2  // USB green (data 1) - green button
#define remoteA 5
#define remoteB 4

// ========= WIRED BUTTON SETUP ==========

// ========= SPEAKER SETUP ==========
//Define the speaker pins
#define spkr 3
#define spkr 9 
// ========= SPEAKER SETUP ==========

// ========= ACCELEROMETER SETUP ==========
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 sensor1(0x68); //ADD to GND

//initialize variables
int16_t ax1, ay1, az1;
int16_t gx1, gy1, gz1;
long ax1avg, ay1avg, az1avg;

int16_t ax2, ay2, az2;
int16_t gx2, gy2, gz2;
long ax2avg, ay2avg, az2avg;
bool started = false;
bool menu = true;
int mode = 0;
int button_state = 0;
bool strike = false;

//set sensitivity of accelerometer
const int sensitivity = 5000;

const uint8_t matrixSize = 8;

uint8_t offset = 0;

const uint8_t cascadeSize = 4; //# of connected matrices
MatrixCascade<cascadeSize> cascade(din, clk, cs); //initialize matrix

// For remaining pins, check Google Drive!

// ========= ACCELEROMETER SETUP ==========


// ================================================================
// ===                          TEXT                            ===
// ================================================================

// ========== MODES ========== //
//  1. start timing when the first knife hits the board (mode A)
//  2. start timing when a beep (mode B)
// ========== MODE A ==========
// start at first hit of board; mode == 0
uint8_t modeA [29] {
  B11111111, B01000000, B00100000, B01000000, B11111111, B00000000, //M
  B00001110, B00010001, B00010001, B00001110, B00000000, //o
  B00001110, B00010001, B00010001, B11111110, B00000000, //d
  B00001110, B00010101, B00010101, B00001100, B00000000, //e
  
  B00000000, B00000000, B00000000, //space
  
  B00111111, B01010000, B10010000, B01010000, B00111111, //A
};

// ========== MODE B ========== //
// start at first beep of speaker; mode == 1
uint8_t modeB [29] {
  B11111111, B01000000, B00100000, B01000000, B11111111, B00000000, //M
  B00001110, B00010001, B00010001, B00001110, B00000000, //o
  B00001110, B00010001, B00010001, B11111110, B00000000, //d
  B00001110, B00010101, B00010101, B00001100, B00000000, //e
  
  B00000000, B00000000, B00000000, //space
  
  B11111111, B10010001, B10010001, B10010001, B01101110, //B
};

// ========== ASURE SOLUTIONS ========== //

uint8_t asure[75] {
  B01111111,B10010000,B10010000,B01111111,B00000000, // A
  B01100010,B10010001,B10010001,B01001110,B00000000, // S
  B11111110,B00000001,B00000001,B11111110,B00000000, // U
  B11111111,B10011000,B10010100,B01100011,B00000000, // R
  B11111111,B10010001,B10010001,B10010001,B00000000, // E
  
  B00000000,B00000000,B00000000, // space
  
  B01100010,B10010001,B10010001,B01001110,B00000000, // S
  B00001110,B00010001,B00010001,B00001110,B00000000, // o
  B11111111,B00000000,                               // l
  B00011110,B00000001,B00000001,B00011110,B00000000, // u
  B00100000,B11111110,B00100001,B00000010,B00000000, // t
  B00101111,B00000000,                               // i
  B00001110,B00010001,B00010001,B00001110,B00000000, // o
  B00011111,B00001000,B00010000,B00001111,B00000000, // n
  B00001001,B00010101,B00010101,B00010010,           // s

  // thicc space
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000
};

// ========== BLANK MENU ========== //
uint8_t blank[4] {
  B00000000, B00000000, B00000000, B00000000,
};

// ========== NUMBERS ========== //
uint8_t const dot[4] = {
  B00000000, B00000011, B00000011, B00000000
};

uint8_t const numbers[][5] = {
  {B01111110, B10000001, B10000001, B10000001, B01111110}, // 1
  {B00000000, B00100001, B01000001, B11111111, B00000001}, // 2
  {B01000011, B10000101, B10001001, B10010001, B01100001}, // 3
  {B01000010, B10000001, B10010001, B10010001, B01101110}, // 4
  {B00011000, B00101000, B01001000, B11111111, B00001000}, // 5
  {B11110010, B10010001, B10010001, B10010001, B10001110}, // 6
  {B01111110, B10010001, B10010001, B10010001, B01001110}, // 7
  {B10000000, B10000000, B10001111, B10010000, B11100000}, // 8
  {B01101110, B10010001, B10010001, B10010001, B01101110}, // 9
  {B01110010, B10001001, B10001001, B10001001, B01111110}  // 0
};

uint8_t number_20[32] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00100001, B01000001, B11111111, B00000001, // 2
  B01110010, B10001001, B10001001, B10001001, B01111110, // 0
  B00000000, B00000011, B00000011, B00000000, //dot
  B01110010, B10001001, B10001001, B10001001, B01111110, // 0
  B01110010, B10001001, B10001001, B10001001, B01111110, // 0
  B00000000, B00000000, B00000000, B00000000,
};

// Declare functions
int setText(uint8_t *text, int len, bool scroll);
int checkButton();
int system_mode();
int printTime(int remtime);


// ================================================================
// ===                     OTHER VARIABLES                      ===
// ================================================================

int remtime = 2000;
int starttime, endtime;


void setup()
{
  //initialize
  Serial.begin(9600);

  // Set the brightness. (0 lowest..15 highest)
  cascade.setIntensity(15);

  //rotate matrices to correct orientation  
  for (uint8_t i = 0; i < cascadeSize; i++){ 
    cascade[i].setRotation(2);
  }

  starttime = millis();
  Serial.println(starttime);

  // ASURE SOLUTIONS scrolling
  while (
  setText(asure, 75, true);

  // ========== Accelerometer Setup ===========
  Serial.println("  Initializing Accelerometer Setup...");
  Wire.begin(); //join I2C bus
  Serial.println("    Initializing I2C devices...");
  sensor1.initialize();
  Serial.println("    Testing device connections...");
  Serial.println(sensor1.testConnection() ? "    MPU6050 #1 connection successful" : "    MPU6050 #1 connection failed");
  Serial.println("  Accelerometer Setup Complete");
  // ========== Accelerometer Setup ===========
  
  // ========== Wired Button Setup ===========
  Serial.println("  Initializing Wired Button Setup...");
  pinMode(buttonA, INPUT);
  pinMode(buttonB, INPUT);
  Serial.println("  Wired Button Setup Complete");
  // ========== Wired Button Setup ===========
  
  // ========== Wireless Button Setup ===========
  Serial.println("  Initializing Wireless Button Setup...");
  pinMode(remoteA, INPUT);
  pinMode(remoteB, INPUT);
  Serial.println("  Wireless Button Setup Complete");
  // ========== Wireless Button Setup ===========
}

//Calibrate accelerometers
void calibrate() {
  //reset value
  int cnt = 0;
  ax1avg = 0;
  ay1avg = 0;
  az1avg = 0;
  ax2avg = 0;
  ay2avg = 0;
  az2avg = 0;

   while (cnt < 484) {
    sensor1.getAcceleration(&ax1, &ay1, &az1);
    ax1avg = ax1avg + ax1;
    ay1avg = ay1avg + ay1;
    az1avg = az1avg + az1;
    ax2avg = ax2avg + ax2;
    ay2avg = ay2avg + ay2;
    az2avg = az2avg + az2;
    cnt++;
  }
  ax1avg = ax1avg / 484;
  ay1avg = ay1avg / 484;
  az1avg = az1avg / 484;
  ax2avg = ax2avg / 484;
  ay2avg = ay2avg / 484;
  az2avg = az2avg / 484;
}
//Calibrate accelerometers  

void loop()
//the overall rundown from scrolling asure, check for button, select the mode, and to actually start the mode
{ 
  if (menu == true){
    setText(blank,4,false);
    
    switch(mode) {
      case 0: setText(modeA, 29, false); break;
      case 1: setText(modeB, 29, false); break;
    }
//    setText(blank,4,false);
    
    //CHECK FOR BUTTON PRESS
    button_state = checkButton();
//   if first_button_pressed == return 1; second_button_pressed == return 2; nothing pressed return 0
  
    switch(button_state){
      case 0: break;
      case 1: mode == 0 ? mode = 1: mode = 0; break; //red/A button is pressed, red button is switching between the menus
      case 2: menu = false; break; //green/B button is pressed, green button is confirmation
    }
    
    button_state = 0; //reset button
  }
  
  else if (menu == false && started == false){
    setText(blank,4,false);
    
    //check if button is pressed again
    button_state = checkButton();
    
    switch(button_state){
      case 0: break; //not pressed, nothing happens
      case 1: break; //red/A button, nothing happens
      case 2: started = true; break; //confirmation button, mode starts
    }
    
    button_state = 0; //reset button
  }
  
  else if (menu == false && started == true) {
    //start at first hit mode
    if (mode == 0) {
      setText(number_20, 32, false); //display number 20 on dot matrix
      sensor1.getAcceleration(&ax1, &ay1, &az1); // get accelerometer information
  
      // Calculate difference between current accelerometer reading and average value calculated during setup
      int ax1Diff = ax1 - ax1avg;
      int ay1Diff = ay1 - ay1avg;
      int az1Diff = az1 - az1avg;
      
      if (abs(ax1Diff) > sensitivity || abs(ay1Diff) > sensitivity || abs(az1Diff) > sensitivity) {
        system_mode(); // use mode A/start with hit function
    }
  }
    // start at beep mode
    if (mode == 1) {
      setText(number_20, 32, false); //display number 20 on dot matrix
      tone(3, 2000, 500); //beep sound with speaker
      system_mode(); //use mode B/start with beep function
    }
    
    menu = true;
    started = false;
}
}

// ================================================================
// ===                         FUNCTIONS                        ===
// ================================================================

// PRINT TEXT
//  - returns -1 if error
//  - returns 1 or 2 for red or green button press respectively
//  - returns 0 for no button text

int setText(uint8_t *text, int len, bool scroll){  //text: text array; len: length of text; scroll=true for scrolling
  //ensure max length of text is maxCols
  if (len > maxCols){
    return -1;
  }
  //copy text to cols
  colsCount = len;
//  state = uint8_t *text;
  uint8_t state = *text;
        
  for (int i = 0; i < len; i++){ 
    cols[i] = text[i];
  }
  
  //fill in remaining columns with 0s
  if (len < maxCols) {
     for (int j = len; j < maxCols; j++){ 
    cols[j] = B00000000;
  } 
  }

  //****DISPLAY TEXT****//
  uint8_t colInd = 0;
  for (auto &matrix: cascade)
  {
      for (auto &col: matrix.cols())
      {
          matrix.set(col, cols[(offset + colInd)%colsCount]);
          colInd++;
      }
  }
  //****END DISPLAY TEXT****//
  
  //if scrolling needed, increase offset
  //for (int offset = 0; offset < text.length(); offset++)
  if (scroll){    
    ++offset;
    offset %= colsCount;
  }
  
}

// RUNNING SYSTEMS 
//  1. start timing when the first knife hits the board (mode A)
//  2. start timing when a beep (mode B)
int system_mode() {
  endtime = 10000000; // initiatializing endtime
  starttime = millis(); //getting start time
  //loop until time exceeds 20 seconds
  while (endtime - starttime <= 20){
    endtime = millis(); //reset endtime
    printTime(remtime); //print the current time
    delay(1000); //stop for 1 second
    remtime = remtime - 1000; // subtract 1 second from the reminder time
  }
  tone(3, 2000, 2000); // beep when 20 seconds is up
  delay(5000); // delay for 5 seconds, then user can pick a mode again
  return 0;
}

//int system_mode_B ()


//PRINT TIME
//  - creates text array for time, then runs setText
//  - takes integer "remtime", which is the remaining time in MILLISECONDS
int printTime (int remtime){
  uint8_t timeText[32]; //length of text is 32
  int i, j; //set up integers for indexing
  int tens = remtime/1000;
  remtime = remtime - tens*1000;
  int ones = remtime/100;                   
  remtime = remtime - ones*100;
  int tenths = remtime/10;
  
  //fill beginning for spacers
  for (i = 0; i < 4; i++){
    timeText[i] = B00000000;
  }

  //tens digit
  if (tens == 0) { //if time left < 10s
    for (i = 4; i < 9; i++){
      timeText[i] = B00000000;
    }
  }
  else { //if time left >=10s Xx.x
    j = 0;
    for (i = 4; i < 9; i++){
      timeText[i] = numbers[tens][j];
      j++;
    }
    timeText[9] = B00000000;
  }
  
  //ones digit xX.x
  j = 0;
  for (i = 10; i < 15; i++){
    timeText[i] = numbers[ones][j];
    j++;
  }
  
  //decimal place
  j = 0;
  for (i = 15; i < 19; i++){
    timeText[i] = dot[j];
    j++;
  }

  //tenths digit xx.X 
  j = 0;
  for (i = 19; i < 24; i++){
    timeText[i] = numbers[tenths][j];
    j++;
  }
  
  //fill remaining with spacers
  for (i = 24; i < 32; i++){
    timeText[i] = B00000000;
  }

  return setText(timeText, 32, false);
}
  
// Testing for delay in printTime
/*printTime(remtime);
remtime = remtime - 10;
delay(1); //why delay(1) here, should it be delay(10)
if (remtime < 0){
  endtime = millis(); //getting current time
  Serial.println(endtime); //printing current time
  Serial.println(starttime-endtime); //
  Serial.println(""); //print a blank line
  delay(2000);
  remtime = 2000;
  starttime = millis();
  Serial.println(starttime);*/ 


//CHECK BUTTONS
//  - does not take in any parameters
//  - returns 1 if red pressed AND/OR remoteA pressed, 2 if green pressed AND/OR remoteB pressed
//  - return 0 if none pressed
//  ** RED BUTTON IS buttonA = first button, GREEN BUTTON IS buttonB = second button**
int checkButton(){
//  red_button_state = false
//  remote_A_state = false
  bool first_button_pressed = false;
  if ((digitalRead(buttonA) || digitalRead(remoteA)) ==  HIGH){
    first_button_pressed = true;
  }
//  green_button_state == false
//  remote_B_state = false
  bool second_button_pressed = false;
  if ((digitalRead(buttonB) || digitalRead(remoteB)) == HIGH) {
    second_button_pressed = true;
  }
  if (first_button_pressed){
    return 1;
  }
  if (second_button_pressed){
    return 2;
  }
  else{
    return 0;
  }
}
