#include <Bounce.h>
#include <font_Arial.h>
#include "SPI.h"
#include "ILI9341_t3.h"
#include <Adafruit_NeoPixel.h>

#define PIN            23
#define NUMPIXELS      86
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//drumPads
#define NUM_BTN_COLUMNS (4)
#define NUM_BTN_ROWS (4)
#define MAX_DEBOUNCE (1)
static const uint8_t btncolumnpins[NUM_BTN_COLUMNS] = {38, 39, 40, 41};
static const uint8_t btnrowpins[NUM_BTN_ROWS]       = {42, 43, 44, 45};
static int8_t debounce_count[NUM_BTN_COLUMNS][NUM_BTN_ROWS];

//Encoder
Bounce encBtn = Bounce(0,5);
int encBtnStatus;
int encPrevBtnStatus;
unsigned long encHold;
int encoderPin1 = 1;
int encoderPin2 = 2;
volatile int lastEncoded = 0;
volatile long encoderValue = 0;
long lastencoderValue = 0;
int scaledEncVal;
int prevScaledEncVal;

//InputTog
Bounce inputOneBtn = Bounce(7,5);
Bounce inputTwoBtn = Bounce(8,5);
bool inputOneTog = false;
bool inputTwoTog = false;
//transport
Bounce clearBtn = Bounce(24,5);
Bounce playStopBtn = Bounce(25,5);
//bool playStop = false;
//Inst Select
Bounce instSelBtn = Bounce(26,5);
int instNumber;
bool instSelectMode = false;
String instName[16] = {
  "Kit 1", "Kit 2","Kit 3","Kit 4",
  "Lead 1", "Pad 1", "Bass 1", "Keys 1",
  "Lead 2", "Pad 2", "Bass 2", "Keys 2",
  "Lead 3", "Pad 3", "Bass 3", "Keys 3",
};
//roboDrums
Bounce roboDrumBtn = Bounce(27,5);

//LP Buttons
int lpBtnPins[8] = {28,29,30,33,34,35,36,37};
int lpBtnState[8];
int lpPrevBtnState[8] = {HIGH};
Bounce lpBounce[] = {
  Bounce(28,10),
  Bounce(29,10),
  Bounce(30,10),
  Bounce(33,10),
  Bounce(34,10),
  Bounce(35,10),
  Bounce(36,10),
  Bounce(37,10),
};
unsigned long clearTimstamp;

String digitalStrings[8] = {
  "D1: ","D2: ","D3: ", "D4: ","D5: ","D6: ","D7: ","D8: "
};
String drumStrings[16] = {
  "Dr1: ","Dr2: ","Dr3: ", "Dr4: ","Dr5: ","Dr6: ","Dr7: ","Dr8: ",
  "Dr9: ","Dr10: ","Dr11: ", "Dr12: ","Dr13: ","Dr14: ","Dr15: ","Dr16: "
};
uint32_t neoColorArray[24] = {
  pixels.Color(0, 255, 255),
  pixels.Color(0, 255, 0),
  pixels.Color(255, 255, 0),
  pixels.Color(255,0,50),
  pixels.Color(0, 255, 255),
  pixels.Color(0, 255, 0),
  pixels.Color(255, 255, 0),
  pixels.Color(255,0,50),

  pixels.Color(0, 255, 255),
  pixels.Color(0, 255, 0),
  pixels.Color(255, 255, 0),
  pixels.Color(255,0,50),
  pixels.Color(0, 255, 255),
  pixels.Color(0, 255, 0),
  pixels.Color(255, 255, 0),
  pixels.Color(255,0,50),

  pixels.Color(0, 255, 255),
  pixels.Color(0, 255, 0),
  pixels.Color(255, 255, 0),
  pixels.Color(255,0,50),
  pixels.Color(0, 255, 255),
  pixels.Color(0, 255, 0),
  pixels.Color(255, 255, 0),
  pixels.Color(255,0,50),
};
uint32_t neoColorArrayDim[8] = {
  pixels.Color(0, 15.9375, 15.9375),
  pixels.Color(0, 15.9375, 0),
  pixels.Color(15.9375, 15.9375, 0),
  pixels.Color(15.9375,0,50),
  pixels.Color(0, 15.9375, 15.9375),
  pixels.Color(0, 15.9375, 0),
  pixels.Color(15.9375, 15.9375, 0),
  pixels.Color(15.9375,0,3.125),
};
uint32_t neoColorOff = pixels.Color(0,0,0);


// LCD STUFF
#define TFT_DC  9
#define TFT_CS 10
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);

//Mux control pins
int s0 = 3;
int s1 = 4;
int s2 = 5;
int s3 = 6;
//Mux in "SIG" pin
int SIG_pin = A0;

int analogValues[22];
int analogValuesLag[22];
String analogStrings[22] = {
  "A1: ","A2: ","A3: ", "A4: ","A5: ","A6: ","A7: ","A8: ","A9: ","A10: ","A11: ",
  "A12: ","A13: ","A14: ","A15: ","A16: ","A17: ","A18: ","A19: ","A20: ","A21: ","A22: "
};

//NintendoScreen
int yPin1 = A7;
int xPin2 = A8;
int yPin2 = A12;
int xPin1 = A13;
int touchX;
int touchY;
int fxTogOn = 0;
int fxTogOff = 1; 
unsigned long readTime;
unsigned long prevReadTime;
unsigned long readTimeTwo;
unsigned long prevReadTimeTwo;

int readX(){
  pinMode(yPin1, INPUT);
  pinMode(xPin2, OUTPUT);
  pinMode(yPin2, INPUT);
  pinMode(xPin1, OUTPUT);
  digitalWrite(xPin2, LOW);
  digitalWrite(xPin1, HIGH);
  return getSmooth(yPin1);
}

int readY(){
  pinMode(yPin1, OUTPUT);
  pinMode(xPin2, INPUT);
  pinMode(yPin2, OUTPUT);
  pinMode(xPin1, INPUT);
  digitalWrite(yPin1, LOW);
  digitalWrite(yPin2, HIGH);
  return getSmooth(xPin2);
}

int getSmooth(int pin){
  int vals[10]; //array that stores 10 readings.
  for(int i = 0; i < 10; i++){
    vals[i] = analogRead(pin); //takes 10 readings.
  }
  float smooth = (vals[0] + vals[1] + vals[2] + vals[3] + vals[4] + vals[5] + vals[6] + vals[7] + vals[8] + vals[9]) / 10;
  return smooth;
}

int readMux(int channel){
  int controlPin[] = {s0, s1, s2, s3};

  int muxChannel[16][4]={
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8
    {1,0,0,1}, //channel 9
    {0,1,0,1}, //channel 10
    {1,1,0,1}, //channel 11
    {0,0,1,1}, //channel 12
    {1,0,1,1}, //channel 13
    {0,1,1,1}, //channel 14
    {1,1,1,1}  //channel 15
  };

  //loop through the 4 sig
  for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  int val;
  val = analogRead(SIG_pin);

  return val;
}

void setup() {

  //Btns
  pinMode(0, INPUT_PULLUP);
  //LPBTNS
  pinMode(28,INPUT_PULLUP);
  pinMode(29,INPUT_PULLUP);
  pinMode(30,INPUT_PULLUP);
  pinMode(33,INPUT_PULLUP);
  pinMode(34,INPUT_PULLUP);
  pinMode(35,INPUT_PULLUP);
  pinMode(36,INPUT_PULLUP);
  pinMode(37,INPUT_PULLUP);

  //LCD Stuff
  tft.begin();
  tft.setRotation(3);
  Serial.begin(9600);
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_14);
  //Pixels
  pixels.begin();
  //Waiting for Serial Animation
  while (!Serial){
    rainbowCycle(2);
  }
  //Initialize stuff
  tft.fillScreen(ILI9341_BLACK);
  drawLoopBtns();
  drawInputToggle("left","0");
  drawInputToggle("right","0");
  drawPosition(0);
  drawBpm(00.00);

  //ENCODER STUFF
  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);
  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on
  //call updateEncoder() when any high/low changed seen
  //on interrupt 5 (pin 5), or interrupt 6 (pin 6) 
  attachInterrupt(1, updateEncoder, CHANGE); 
  attachInterrupt(2, updateEncoder, CHANGE);

  //MUX
  pinMode(s0, OUTPUT); 
  pinMode(s1, OUTPUT); 
  pinMode(s2, OUTPUT); 
  pinMode(s3, OUTPUT); 
  pinMode(SIG_pin, INPUT);

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);

  //digitalBtns
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(24, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);

  //drumPads
  setuppins();
}

//Serial Read Vars
bool serialStarted = false;
bool serialEnded = false;
char inData[80];
int serIndex;


//Serial Write Vars
const int num_of_digital_pins = 1;
int digital_values[num_of_digital_pins];


///SCREEN VARS for recall
int playStatus;
float bpmVal;
int loopPosVal;
int inputLeftActive;
int inputRightActive;
String Recstatus;
int leftInputAmp;
int rightInputAmp;
int lpNumber;
int lpStatus;
int loopRec[8];
int loopMute[8];
bool ledBlinking[8] = {false};
int blinkBrightness;
bool lpCurrentlyRec[8] = {false};



String songNames[52];
String songName;
int lpOffset;
int songIttr;
int songSelIndex;;
bool selectEvent = false;
bool menu = false;
int menuIndex;
int menuSelPos[4] = {118,148,178,208};
bool songMode = false;
bool newSongNameFlag = false;
int charPosition;
int charIndex;
char charArray[52] = {
  'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
  'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'
};
char newSongCharArray[18];
int lpOffsetArray[8] = {20,80,140,200,20,80,140,200};
int lpOffsetVertArray[8] = {120,120,120,120,180,180,180,180};
uint32_t lcdColorArray[16] = {
  0x033f,0x07f0,0xffa4,0xf8ea,0x033f,0x07f0,0xffa4,0xf8ea,
  0x033f,0x07f0,0xffa4,0xf8ea,0x033f,0x07f0,0xffa4,0xf8ea
};
//InstPads
static void setuppins()
{
  uint8_t i;
  // button columns
  for (i = 0; i < NUM_BTN_COLUMNS; i++)
  {
    pinMode(btncolumnpins[i], OUTPUT);
    // with nothing selected by default
    digitalWrite(btncolumnpins[i], HIGH);
  }
  // button row input lines
  for (i = 0; i < NUM_BTN_ROWS; i++)
  {
    pinMode(btnrowpins[i], INPUT_PULLUP);
  }
  // Initialize the debounce counter array
  for (uint8_t i = 0; i < NUM_BTN_COLUMNS; i++)
  {
    for (uint8_t j = 0; j < NUM_BTN_ROWS; j++)
    {
      debounce_count[i][j] = 0;
    }
  }
}
//InstPads
static void scan()
{
  static uint8_t current = 0;
  uint8_t val;
  uint8_t i, j;

  // Select current columns
  digitalWrite(btncolumnpins[current], LOW);

  // Read the button inputs
  for ( j = 0; j < NUM_BTN_ROWS; j++)
  {
    val = digitalRead(btnrowpins[j]);

    if (val == LOW)
    {
      // active low: val is low when btn is pressed
      if ( debounce_count[current][j] < MAX_DEBOUNCE)
      {
        debounce_count[current][j]++;
        if ( debounce_count[current][j] == MAX_DEBOUNCE )
        {
          if(instSelectMode){
            Serial.print("Inst: ");
            Serial.print((current * NUM_BTN_ROWS) + j);
            Serial.print(" ");
            Serial.println(0);
            instNumber = (current * NUM_BTN_ROWS) + j;
            drawInstrument(instNumber);
          }else{
            Serial.print(drumStrings[(current * NUM_BTN_ROWS) + j]);
            Serial.print(1);
            Serial.print(" ");
            Serial.println("");
          }
          // int btnNum = (13 - (current * NUM_BTN_ROWS) + j);
          // pixels.setPixelColor(btnNum+9,pixels.Color(255,0,50));
        }
      }
    }
    else
    {
      // otherwise, button is released
      if ( debounce_count[current][j] > 0)
      {
        debounce_count[current][j]--;
        if ( debounce_count[current][j] == 0 )
        {
          Serial.print(drumStrings[(current * NUM_BTN_ROWS) + j]);
          Serial.print(0);
          Serial.print(" ");
          Serial.println("");
          // int btnNum = (13 - (current * NUM_BTN_ROWS) + j);
          // pixels.setPixelColor(btnNum+9,pixels.Color(0,0,0));
        }
      }
    }
  }// for j = 0 to 3;

  digitalWrite(btncolumnpins[current], HIGH);

  current++;
  if (current >= NUM_BTN_COLUMNS)
  {
    current = 0;
  }

}
// ====================================================================================================================================// 
// ! Start Loop                                                                  
// ====================================================================================================================================// 

void loop(void) { 
  while (!Serial){
    rainbowCycle(2);
  }
  pixels.show();
  //DrumBtns
  scan();
  //Loop Btns
  for(int i=0; i<8; i++){
    lpBtnState[i] = digitalRead(lpBtnPins[i]);
    if(lpBounce[i].update()){
      if (lpBtnState[i] == LOW && lpPrevBtnState[i] == HIGH){
        clearTimstamp = millis();
        Serial.print(digitalStrings[i]);
        Serial.print(1);
        Serial.print(" ");
        Serial.println(0);
      }
    }
    if (lpBtnState[i] == LOW && lpPrevBtnState[i] == LOW){
      if((millis() - clearTimstamp) > 1000 && (millis() - clearTimstamp) < 1200){
        Serial.print("clearSingle: ");
        Serial.print(i+1);
        Serial.print(" ");
        Serial.println("");
        clearTimstamp = millis();
      }
    }
    lpPrevBtnState[i] = lpBtnState[i];
  }
  readTimeTwo = millis();
  //FX and Loop Volume 
  for(int i = 0; i < 22; i ++){
    if(i<16){
      analogValues[i] = readMux(i);
    }else{
      analogValues[i] = analogRead(i-15);
    }
    if (abs(analogValues[i] - analogValuesLag[i]) > 8){
      if(readTimeTwo - prevReadTimeTwo > 5){
        Serial.print(analogStrings[i]);
        Serial.print(analogValues[i]);
        Serial.print(" ");
        Serial.println(0);
        analogValuesLag[i] = analogValues[i];
        prevReadTimeTwo = readTimeTwo;
      }
    }
  }

  //Btns
  if(inputOneBtn.update()){
    if(inputOneBtn.fallingEdge()){
      inputOneTog = !inputOneTog;
      Serial.print("InputOne: ");
      Serial.print(inputOneTog);
      Serial.print(" ");
      Serial.println("");
    }
  }
  if(inputTwoBtn.update()){
    if(inputTwoBtn.fallingEdge()){
      inputTwoTog = !inputTwoTog;
      Serial.print("InputTwo: ");
      Serial.print(inputTwoTog);
      Serial.print(" ");
      Serial.println("");
    }
  }
  if(clearBtn.update()){
    if(clearBtn.fallingEdge()){
      Serial.print("Clear: ");
      Serial.print(1);
      Serial.print(" ");
      Serial.println("");
    }
  }
  if(playStopBtn.update()){
    if(playStopBtn.fallingEdge()){
      //playStop = !playStop;
      Serial.print("PlayStop: ");
      if(playStatus == 1){
        Serial.print(0);
      }else{
        Serial.print(1);
      }
      Serial.print(" ");
      Serial.println("");
    }
  }
  //Instrument Selection
  if(instSelBtn.update()){
    if(instSelBtn.fallingEdge()){
      for(int i=0; i<16; i++){
        pixels.setPixelColor(i+10, pixels.Color(0, 0, 0));
      }
      drawInstrument(instNumber);
      instSelectMode = true;
    }else{
      instSelectMode = false;
      returnToGui(songName);
    }
  }

  if(roboDrumBtn.update()){
    if(roboDrumBtn.fallingEdge()){
      Serial.print("RoboDrums: ");
      Serial.print(1);
      Serial.print(" ");
      Serial.println("");
    }else{
      Serial.print("RoboDrums: ");
      Serial.print(0);
      Serial.print(" ");
      Serial.println("");
    }
  }

  //NDS_TOUCH
  //limit reads to 5ms
  readTime = millis();
  touchX = readX();
  touchY = readY();

  if ((touchX < 900) && (touchY < 900)){
    if(readTime - prevReadTime > 16){
      Serial.print("Xval: ");
      Serial.print(touchX);
      Serial.print(" ");
      Serial.println(0);
      Serial.print("Yval: ");
      Serial.print(touchY);
      Serial.print(" ");
      Serial.println(0);
      fxTogOn = 1;
      fxTogOff = 0; 
      prevReadTime = readTime;
    }
  }else{
    if (abs(fxTogOn - fxTogOff) > 0){
      Serial.print("postTog: ");
      Serial.print(0);
      Serial.print(" ");
      Serial.println(0);
      fxTogOff = fxTogOn;  
    }
  }
  //NEOPIXELS
  //Drum Pad Pixels
  if(!instSelectMode){

    if(instNumber>3){
      if (instNumber == 4 || instNumber == 8 || instNumber == 12){
        for(int i=0; i<16; i++){
          pixels.setPixelColor(i+10, neoColorArray[0]);
        }
      }else if (instNumber == 5 || instNumber == 9 || instNumber == 13){
        for(int i=0; i<16; i++){
          pixels.setPixelColor(i+10, neoColorArray[1]);
        }
      }else if (instNumber == 6 || instNumber == 10 || instNumber == 14){
        for(int i=0; i<16; i++){
          pixels.setPixelColor(i+10, neoColorArray[2]);
        }
      }else{
        for(int i=0; i<16; i++){
          pixels.setPixelColor(i+10, neoColorArray[3]);
        }
      }
    }else{
      pixels.setPixelColor(10, pixels.Color(0, 255, 0));
      pixels.setPixelColor(11, pixels.Color(0, 255, 0));
      pixels.setPixelColor(12, pixels.Color(0, 255, 0));
      pixels.setPixelColor(18, pixels.Color(0, 255, 0));
      pixels.setPixelColor(19, pixels.Color(0, 255, 0));
      pixels.setPixelColor(20, pixels.Color(0, 255, 0));

      pixels.setPixelColor(14, pixels.Color(0, 255, 255));
      pixels.setPixelColor(15, pixels.Color(0, 255, 255));
      pixels.setPixelColor(16, pixels.Color(0, 255, 255));
      pixels.setPixelColor(22, pixels.Color(0, 255, 255));
      pixels.setPixelColor(23, pixels.Color(0, 255, 255));
      pixels.setPixelColor(24, pixels.Color(0, 255, 255));

      pixels.setPixelColor(13, pixels.Color(255,0,50));
      pixels.setPixelColor(17, pixels.Color(255,0,50));
      pixels.setPixelColor(21, pixels.Color(255,0,50));
      pixels.setPixelColor(25, pixels.Color(255,0,50));

      //pixels.Color(255,0,50),

    }
  }else{
    for(int i=0; i<16; i++){
      pixels.setPixelColor(i+10, neoColorArray[i]);
    }
    if (instNumber < 4){
      pixels.setPixelColor(instNumber+22, pixels.Color(blinkBrightness, blinkBrightness, blinkBrightness));
    }else if(instNumber > 3 && instNumber < 8){
      pixels.setPixelColor(instNumber+14, pixels.Color(blinkBrightness, blinkBrightness, blinkBrightness));
    }else if (instNumber > 7 && instNumber < 12){
      pixels.setPixelColor(instNumber+6, pixels.Color(blinkBrightness, blinkBrightness, blinkBrightness));
    }else if(instNumber > 11){
      pixels.setPixelColor(instNumber-2, pixels.Color(blinkBrightness, blinkBrightness, blinkBrightness));
    }
  }
  //Loop Pad Pixes
  blinkBrightness = 128+127*cos(2*PI/500*readTime);

  for(int i=0; i<8; i++){
    if(loopMute[i] == 1){
      if(!ledBlinking[i]){
        pixels.setPixelColor(i+2, neoColorArray[i]);
      }else{
        if(lpCurrentlyRec[i]){
          pixels.setPixelColor(i+2, pixels.Color(blinkBrightness, 0, 0));
        }else{
          pixels.setPixelColor(i+2, pixels.Color(blinkBrightness, blinkBrightness, 0));
        }
      }
    }else{
      if(!ledBlinking[i]){
        if(loopRec[i]){
          pixels.setPixelColor(i+2, neoColorArrayDim[i]);
        }else{
          pixels.setPixelColor(i+2, neoColorOff);
        }
      }else{
        if(lpCurrentlyRec[i]){
          pixels.setPixelColor(i+2, pixels.Color(blinkBrightness, 0, 0));
        }else{
          pixels.setPixelColor(i+2, pixels.Color(blinkBrightness, blinkBrightness, 0));
        }
      }
    }
  }
  //Encode Menu
  encBtnStatus = digitalRead(0);
  if(encBtn.update()){
    if (encBtnStatus == LOW && encPrevBtnStatus == HIGH){
      encHold = millis();
      songMode = true;

      if(songMode){
        if(!selectEvent && !menu && !newSongNameFlag){
          //Query folders for song names (Shell)
          tft.fillScreen(ILI9341_BLACK);
          Serial.print("Select_Song: ");
          Serial.print(1);
          Serial.print(" ");
          Serial.println(0);
          drawSongSelection(songSelIndex);
          selectEvent = true;
        }else if(!menu && !newSongNameFlag){
          //Load Menu
          menu = true;
          tft.fillRect(10,menuSelPos[menuIndex],200,20,0x31a6);
          drawMenu();
        }else if(menuIndex == 0 && !newSongNameFlag){
          //Cancel
          returnToGui(songName);
        }else if (menuIndex == 1 && !newSongNameFlag){
          //Load Song
          Serial.print("Clear_Song: ");
          Serial.print(1);
          Serial.print(" ");
          Serial.println(0);
          
          Serial.print("Load_Song: ");
          Serial.print(1);
          Serial.print(" ");
          Serial.println(0);
          returnToGui(songNames[songSelIndex]);
        }else if (menuIndex == 2 && !newSongNameFlag){
          if(songNames[songSelIndex].length() == 0){
            //create new song name
            newSongNameFlag = true;
            drawCharacter(charIndex);
          }else{
            //overwrite
            //delete directory first
            Serial.print("Delete_Song: ");
            Serial.print(songNames[songSelIndex]);
            Serial.print(" ");
            Serial.println(0);
            //recreate directory
            Serial.print("New_Song: ");
            Serial.print(songNames[songSelIndex]);
            Serial.print(" ");
            Serial.println(0);
            //then save
            Serial.print("Save_Song: ");
            Serial.print(1);
            Serial.print(" ");
            Serial.println(0);
            returnToGui(songNames[songSelIndex]);
          }
        }else if (menuIndex == 3 && !newSongNameFlag){
          //delete song
          Serial.print("Delete_Song: ");
          Serial.print(songNames[songSelIndex]);
          Serial.print(" ");
          Serial.println(0);
          returnToGui(songName);
        }else if (newSongNameFlag){
          //new Song Name Character Skip
          charPosition++;
          if(charPosition > 17){
            charPosition = 0;
          }
          tft.fillRect(0,70,320,4,ILI9341_BLACK);
          tft.fillRect(50+(charPosition*12),70,10,4,0xffa4);
        }
      }
    }
  }
  //Hold save for new song
  if (encBtnStatus == LOW && encPrevBtnStatus == LOW){
    if((millis() - encHold) > 2000 && newSongNameFlag){
      String newSongNameString = String(newSongCharArray);
      //create new directory
      Serial.print("New_Song: ");
      Serial.print(newSongNameString);
      Serial.print(" ");
      Serial.println(0);
      //then save
      Serial.print("Save_Song: ");
      Serial.print(1);
      Serial.print(" ");
      Serial.println(0);
      returnToGui(newSongNameString);
      encHold = millis();
    }
  }

  encPrevBtnStatus = encBtnStatus;
  
  //Select Song/Character with Encoder
  if(songMode){
    scaledEncVal = encoderValue;
    if (abs(scaledEncVal - prevScaledEncVal) > 3){

      if(menu && !newSongNameFlag){
        menuIndex = scaledEncVal/4;
        if(menuIndex > 3){
          encoderValue = 0;
        }
        if(menuIndex<4){
          tft.fillRect(0,118,320,120,ILI9341_BLACK);
          tft.fillRect(10,menuSelPos[menuIndex],200,20,0x31a6);
          drawMenu();
        }
      }else if(!newSongNameFlag){
        songSelIndex = scaledEncVal/4;
        if(songSelIndex == 52){
            songSelIndex = 0;
        }
        drawSongSelection(songSelIndex);
        if(songNames[songSelIndex].length() > 0){
          Serial.print("Song_Name: ");
          Serial.print(songNames[songSelIndex]);
          Serial.print(" ");
          Serial.println(0);
        }
      }else if(newSongNameFlag){
        charIndex = scaledEncVal/4;
        if(charIndex == 52){
          charIndex = 0;
        }
        drawCharacter(charIndex);
      }
      prevScaledEncVal = scaledEncVal;
    }  
  }
  //Read Serial
  while(Serial.available() > 0)
  {
    char c = Serial.read();
    if(c=='-'){
      serIndex = 0;
      inData[serIndex] = '\0';
      serialStarted = true;
    }else if(c=='/'){
      serialEnded = true;
      break;
    }else{
      if(serIndex < 79){
        inData[serIndex] = c;
        serIndex++;
        inData[serIndex] = '\0';
      }
    }
  }
  //Process Serial Package
  if(serialStarted && serialEnded){

    String PrintString = String(inData);

    int commaIndex = PrintString.indexOf('_');
    int secondCommaIndex = PrintString.indexOf('_', commaIndex+1);
    int thirdCommaIndex = PrintString.indexOf('_', secondCommaIndex+1);


    String part1 = PrintString.substring(0, commaIndex);
    String part2 = PrintString.substring(commaIndex+1, secondCommaIndex);
    String part3 = PrintString.substring(secondCommaIndex+1, thirdCommaIndex);

    //testTEXT
    //debugMessage(part1, part2, part3);
   
    if (part1.equals("bpm")) {
      bpmVal = part2.toFloat();
      if(!songMode && !instSelectMode) drawBpm(bpmVal);
    }else if (part1.equals("pos")){
      loopPosVal = part2.toInt();
      if(!songMode && !instSelectMode) drawPosition(loopPosVal);
    }else if (part1.equals("inp")){
      if(part2.equals("left")){
        inputLeftActive = part3.toInt();
      }else{
        inputRightActive = part3.toInt();
      }
      if(!songMode && !instSelectMode) drawInputToggle(part2, part3);
    }else if (part1.equals("sta")){
      Recstatus = part2;
      if(Recstatus.equals("r")){
        ledBlinking[part3.toInt()-1] = true;
        lpCurrentlyRec[part3.toInt()-1] = true;
      }else if(Recstatus.equals("p")){
        ledBlinking[part3.toInt()-1] = true;
        lpCurrentlyRec[part3.toInt()-1] = false;
      }else{
        ledBlinking[part3.toInt()-1] = false;
      }
      if(!songMode && !instSelectMode) drawStatus(Recstatus);
    }else if (part1.equals("mut")){
      lpNumber = part2.toInt();
      lpStatus = part3.toInt();
      loopMute[lpNumber-1] = lpStatus;
      if(!songMode && !instSelectMode) drawMute(lpNumber, lpStatus);
    }else if(part1.equals("wav")){
      lpNumber = part2.toInt();
      lpStatus = part3.toInt();
      loopRec[lpNumber-1] = lpStatus;
      if(!songMode && !instSelectMode) drawWav(lpNumber, lpStatus);
    }else if (part1.equals("amp")){
      if(part2.equals("left")){
        leftInputAmp = part3.toInt()*1.6;
        if(!songMode && !instSelectMode) drawAmp("left",leftInputAmp);
      }else if (part2.equals("right")){
        rightInputAmp = part3.toInt()*1.6;
        if(!songMode && !instSelectMode) drawAmp("right",rightInputAmp);
      }else{
        extraNeoPixels(part3.toInt());
      }
    }else if (part1.equals("sng")){
      if(songIttr==0){
         for(int i=0; i<52; i++){
          songNames[i] = "";
        } 
      }
      songNames[songIttr] = part2;
      songIttr++;
    }else if(part1.equals("sgn")){
      songName = part2;
      drawSongName(part2);
    }else if(part1.equals("ply")){
      playStatus = part2.toInt();
    }

    serialStarted = false;
    serialEnded = false;
    serIndex = 0;
    for(int i=0; i<80; i++){
       inData[i] = '\0'; 
    }  
  }
}

void drawBpm(String newBpm){
  tft.fillRect(20, 30, 140, 30, ILI9341_BLACK);
  tft.setTextColor(0x033f);
  tft.setCursor(20, 30);
  tft.print("BPM:");  
  tft.setCursor(70, 30);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(newBpm);  
}
void drawPosition(int position){
  if(position == 0){
    tft.fillRect(0, 1, 320, 20, ILI9341_BLACK);
  }
  tft.fillRect(20*position+1, 1, 18, 20, ILI9341_BLACK);
  if(Recstatus.equals("r")){
    tft.fillRect(20*position+1, 1, 18, 20, 0xf8ea);
  }else if(Recstatus.equals("p")){
    tft.fillRect(20*position+1, 1, 18, 20, 0xffa4);
  }else{
    tft.fillRect(20*position+1, 1, 18, 20, 0x07f0);
  }
  tft.setCursor(160,30);
  tft.setTextColor(ILI9341_WHITE);
  
  if(position==0){
    tft.fillRect(160, 30, 20, 30, ILI9341_BLACK);
    tft.print("1.");
  }else if(position == 4){
    tft.fillRect(160, 30, 20, 30, ILI9341_BLACK);
    tft.print("2."); 
  }else if(position == 8){
    tft.fillRect(160, 30, 20, 30, ILI9341_BLACK);
    tft.print("3."); 
  }else if(position == 12){
    tft.fillRect(160, 30, 20, 30, ILI9341_BLACK);
    tft.print("4."); 
  }
  tft.fillRect(180, 30, 20, 30, ILI9341_BLACK);
  tft.setCursor(180,30);
  tft.setTextColor(0x07f0);
  tft.print((position%4)+1);
  pixels.setPixelColor(0, neoColorArray[position%4]);
  pixels.setPixelColor(1, neoColorArray[position%4]);
}
void drawInputToggle(String input, String onOff){
  if(input.equals("left")){
    if(onOff.equals('1')){
      tft.fillCircle(275, 40, 10, ILI9341_WHITE);
    }else{
      tft.fillCircle(275, 40, 10, ILI9341_BLACK);
      tft.drawCircle(275, 40, 10, ILI9341_WHITE);
    }
  }else{
    if(onOff.equals('1')){
      tft.fillCircle(305, 40, 10, 0xf8ea);
    }else{
      tft.fillCircle(305, 40, 10, ILI9341_BLACK);
      tft.drawCircle(305, 40, 10, 0xf8ea);
    }
  }
}
void drawStatus(String status){
  tft.fillRect(20, 90, 100, 30, ILI9341_BLACK);
  tft.setCursor(20, 90);
  if(status.equals("r")){
    tft.setTextColor(0xf8ea);
    tft.print("Recording");
  }else if(status.equals("p")){
    tft.setTextColor(0xffa4);
    tft.print("Pending");
  }
}
// void ledBlink(String status, int lpNumber){
  

//   if(status.equals("r")){
//     pixels.setPixelColor(lpNumber+1, pixels.Color(value, 0, 0));
//   }else if(status.equals("p")){
//     pixels.setPixelColor(lpNumber+1, pixels.Color(255, value, 0));
//   }
// }

void drawMute(int loopNum, int loopStatus){
  tft.fillRoundRect(lpOffsetArray[loopNum-1], lpOffsetVertArray[loopNum-1], 50, 50, 3, ILI9341_BLACK);
  if(loopStatus == 1){
    tft.fillRoundRect(lpOffsetArray[loopNum-1], lpOffsetVertArray[loopNum-1], 50, 50, 3, lcdColorArray[loopNum-1]);
  }else{
    tft.drawRoundRect(lpOffsetArray[loopNum-1], lpOffsetVertArray[loopNum-1], 50, 50, 3, lcdColorArray[loopNum-1]);
  }   
  drawWav(loopNum, loopRec[loopNum-1]);
}
void drawWav(int loopNum, int loopStatus){
  if(loopNum< 5){
    if(loopNum == 1){
      lpOffset = 0;
    }else{
      lpOffset = 16*(loopNum-1);
    }    
    tft.fillCircle((44*loopNum)+lpOffset, 144, 14, ILI9341_BLACK);
    if(loopStatus == 1){
      tft.fillCircle((44*loopNum)+lpOffset, 144, 14, 0x31a6);
      tft.fillCircle((44*loopNum)+lpOffset, 144, 10, ILI9341_WHITE);
    }
  }else{
    loopNum = loopNum-4;
    if(loopNum == 1){
      lpOffset = 0;
    }else{
      lpOffset = 16*(loopNum-1);
    }
    tft.fillCircle((44*loopNum)+lpOffset, 204, 14, ILI9341_BLACK);
    if(loopStatus == 1){
      tft.fillCircle((44*loopNum)+lpOffset, 204, 14, 0x31a6);
      tft.fillCircle((44*loopNum)+lpOffset, 204, 10, ILI9341_WHITE);
    }
  }
}
void drawAmp(String input, int ampVal){
  if(input.equals("left")){
    tft.fillRect(265, 230 - ampVal, 20, ampVal, ILI9341_WHITE);
    tft.fillRect(265, 230 - 160, 20, 160 - ampVal, ILI9341_BLACK);   
  }else{
    tft.fillRect(295, 230 - ampVal, 20, ampVal, 0xf8ea);
    tft.fillRect(295, 230 - 160, 20, 160 - ampVal, ILI9341_BLACK);       
  }
}
void drawLoopBtns(){
  tft.drawRoundRect(20, 120, 50, 50, 3, 0x033f);
  tft.drawRoundRect(80, 120, 50, 50, 3, 0x07f0);
  tft.drawRoundRect(140, 120, 50, 50, 3, 0xffa4);
  tft.drawRoundRect(200, 120, 50, 50, 3, 0xf8ea);
  tft.drawRoundRect(20, 180, 50, 50, 3, 0x033f);
  tft.drawRoundRect(80, 180, 50, 50, 3, 0x07f0);
  tft.drawRoundRect(140, 180, 50, 50, 3, 0xffa4);
  tft.drawRoundRect(200, 180, 50, 50, 3, 0xf8ea);
}
void drawSongName(String songName){
  tft.fillRect(20, 60, 240, 30, ILI9341_BLACK);
  tft.setCursor(20,60);
  if(songName.equals("Saving...")){
    tft.setTextColor(0xf8ea);
  }else{
    tft.setTextColor(0x07f0);
  }
  tft.print(songName);
}
void returnToGui(String songName){
  tft.fillScreen(ILI9341_BLACK);
  drawSongName(songName);
  drawLoopBtns();
  for(int i=0; i<8; i++){
    drawWav(i+1,loopRec[i]);
    drawMute(i+1,loopMute[i]);
  }
  drawInputToggle("left",inputLeftActive);
  drawInputToggle("right",inputRightActive);
  drawPosition(0);
  drawPosition(loopPosVal);
  drawBpm(bpmVal);
  for(int i=0; i<18; i++){
    newSongCharArray[i] = (char)0;
  }
  selectEvent = false;
  songIttr = 0;
  charPosition = 0;
  songMode = false;
  newSongNameFlag = false;
  menu = false;
}
void drawSongSelection(int songIndex){
  tft.fillRect(0,20,320,30,ILI9341_BLACK);
  tft.setCursor(20,20);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("Save // Load Song");  
  tft.fillRect(20,50,320,30,ILI9341_BLACK);
  tft.setCursor(20, 50);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(songIndex+1);
  tft.setCursor(50, 50);
  tft.setTextColor(0x07f0);
  tft.print(songNames[songIndex]);
}

void drawCharacter(int index){
  tft.fillRect(0,70,320,4,ILI9341_BLACK);
  tft.fillRect(50+(charPosition*12),70,10,4,0xffa4);
  tft.drawChar(50+(charPosition*12), 50, charArray[index], ILI9341_WHITE,ILI9341_BLACK,2);
  //store character into what will be new song name string
  newSongCharArray[charPosition] = charArray[index];
}
void drawMenu(){
  tft.setTextColor(0xffa4);
  tft.setCursor(20,120);
  tft.print("Cancel");
  tft.setTextColor(0x07f0);
  tft.setCursor(20,150);
  tft.print("Load");
  tft.setTextColor(0xf8ea);
  tft.setCursor(20,180);
  tft.print("Save / Overwrite");
  tft.setTextColor(0x033f);
  tft.setCursor(20,210);
  tft.print("Delete");
}
void drawInstrument(int instNum){
  tft.fillScreen(ILI9341_BLACK);
  if(instNum > 3){
    //Draw Keys
    tft.fillRect(50,70,220,160,lcdColorArray[instNum]);
    for(int i=0; i<7; i++){
      tft.fillRect((i*30)+60,120,20,100,ILI9341_WHITE);
    }
    for(int i=0; i<6; i++){
      tft.fillRect((i*30)+75,120,20,50,ILI9341_BLACK);
      if(i<4){
        tft.fillCircle((i*30)+70,90,10,ILI9341_WHITE);
      }
    }
  }else if(instNum <4){
    //Draw Drums
    tft.fillRect(50,70,220,160,lcdColorArray[instNum]);
    for(int i=0; i<4; i++){
      tft.fillRect((i*50)+60,130,40,40,ILI9341_WHITE);
    }
    for(int i=0; i<4; i++){
      tft.fillRect((i*50)+60,180,40,40,ILI9341_WHITE);
    }
    for(int i=0; i<4; i++){
      tft.fillCircle((i*30)+70,90,10,ILI9341_WHITE);
    }
    tft.fillRect(180,80,80,20,ILI9341_WHITE);
  }
  tft.setCursor(50,20);
  if(instNum>7){
    tft.setTextColor(lcdColorArray[instNum]);
    tft.print(instName[instNum]);
  }else{
    tft.setTextColor(lcdColorArray[instNum]);
    tft.print(instName[instNum]);
  }
}
void debugMessage(String pt1, String pt2, String pt3){
  tft.fillRect(0, 100, 320, 240, 0x033f);
  tft.setCursor(40, 120);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(pt1);
  tft.setCursor(40, 140);
  tft.print(pt2);
  tft.setCursor(40, 160);
  tft.print(pt3);  
}
void updateEncoder(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;
  if(encoderValue > 208){
    encoderValue = 208;
  }else if (encoderValue < 0){
    encoderValue = 0;
  }

  lastEncoded = encoded; //store this value for next time
}
void extraNeoPixels(int amp){
  int ampOne = map(amp, 0, 100, 0, 30);
  for (int i = 0; i < 30; ++i){
    if (i<ampOne){
      pixels.setPixelColor(i+26, Wheel((i * 256 / pixels.numPixels()-26*ampOne)));
    }else{
      pixels.setPixelColor(i+26, pixels.Color(0,0,0));
    }
  }
  int ampTwo = map(amp, 0, 100, 30, 40);
  for (int i = 30; i < 40; ++i){
    if (i<ampTwo){
      pixels.setPixelColor(i+26, Wheel((i * 256 / pixels.numPixels()-26*ampTwo)));
    }else{
      pixels.setPixelColor(i+26, pixels.Color(0,0,0));
    }
  }  
  int ampThree = map(amp, 0, 100, 40, 60);
  for (int i = 40; i < 60; ++i){
    if (i<ampThree){
      pixels.setPixelColor(i+26, Wheel((i * 256 / pixels.numPixels()-26*ampThree)));
    }else{
      pixels.setPixelColor(i+26, pixels.Color(0,0,0));
    }
  }  
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
