#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "FastLED.h"
#define NUM_LEDS 27
CRGB leds[NUM_LEDS];
#define PIN 3

  // COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120


bool gReverseDirection = false;


#ifndef STASSID
#define STASSID "freebox_CUHGYH"
#define STAPSK  "domenechdomenech"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = LED_BUILTIN;

byte *c;
int i = 0;
int j = 0;
int k = 0;
int dir = 0;
float r = 0.0;
float g = 0.0;
float b = 0.0;
int red = 120;
int green = 0;
int blue = 0;
int Speed = 100;
int EyeWidth = 2;
int EyeSpace = 4;
unsigned long currentMillis;
long prevMillis = 0;
long delayMillis = 0;
long snowMillis = 0;
bool Fade = true;
int fadeSpeed = 200;
bool eyeStat = false;
int Flashes = 0;
int Delay = 0;
int Pause = 0;
int state = 0;
int Steps = random(5,50);
int FadeDelay = random(50,150);
int EndPause = random(1000,10000);
int StartPoint  = random( 0, NUM_LEDS - (2*EyeWidth) - EyeSpace );
int Start2ndEye = StartPoint + EyeWidth + EyeSpace;
int snowDelay = random(100,1000);
String message;
String myIp;
int selectedEffect = 0;

void resetArgs(){
  i = 0;j = 0;k = 0;dir = 0;r = 0.0;g = 0.0;b = 0.0;red = 120;green = 0;blue = 0;Speed = 100;EyeWidth = 2;EyeSpace = 4;prevMillis = 0;delayMillis = 0;snowMillis = 0;Fade = false;fadeSpeed = 200;eyeStat = false;Flashes = 0;Delay = 0;Pause = 0;state = 0;
}

void handleRoot() {
  digitalWrite(led, 0);
  message="<html><head><style>input[type='number'] {width:75px;}</style></head><body style='font-family: sans-serif; font-size: 16px'>    The Following functions are available:Turn off leds";
  message+="<form action='http://" + myIp + "/colorwipe'><input type='submit' value='ColorWipe'> Red <input type='number' name='red' min='0' max='255' value='120'> Green <input type='number' name='green' min='0' max='255' value='0'> Blue <input type='number' name='blue' min='0' max='255' value='0'> Speed <input type='number' name='speed' min='1' max='5000' value='100'></form>";
  message+="<form action='http://" + myIp + "/cyclonbounce'><input type='submit' value='CyclonBounce'> Red <input type='number' name='red' min='0' max='255' value='0'> Green <input type='number' name='green' min='0' max='255' value='127'> Blue <input type='number' name='blue' min='0' max='255' value='0'> Speed <input type='number' name='speed' min='1' max='5000' value='100'> EyeWidth <input type='number' name='eyewidth' min='1' max='500' value='2'></form>";
  message+="<form action='http://" + myIp + "/fadeinout'><input type='submit' value='FadeInOut'> Red <input type='number' name='red' min='0' max='255' value='0'> Green <input type='number' name='green' min='0' max='255' value='0'> Blue <input type='number' name='blue' min='0' max='255' value='127'> Speed <input type='number' name='speed' min='1' max='5000' value='10'></form>";
  message+="<form action='http://" + myIp + "/halloweeneyes'><input type='submit' value='HalloweenEyes'> Red <input type='number' name='red' min='0' max='255' value='127'> Green <input type='number' name='green' min='0' max='255' value='127'> Blue <input type='number' name='blue' min='0' max='255' value='127'> EyeWidth <input type='number' name='eyewidth' min='1' max='100' value='1'> EyeSpace <input type='number' name='eyespace' min='1' max='100' value='4'> Fade <input type='checkbox' name='fade' value='true' checked='true'></form>";
  message+="<form action='http://" + myIp + "/meteorrain'><input type='submit' value='MeteorRain'> Red <input type='number' name='red' min='0' max='255' value='20'> Green <input type='number' name='green' min='0' max='255' value='40'> Blue <input type='number' name='blue' min='0' max='255' value='10'> Speed <input type='number' name='speed' min='1' max='5000' value='200'> EyeWidth <input type='number' name='eyewidth' min='1' max='100' value='2'> Fade <input type='checkbox' name='fade' value='true' checked='true'> FadeSpeed <input type='number' name='fadespeed' min='1' max='500' value='150'></form>";
  message+="<form action='http://" + myIp + "/newkitt'><input type='submit' value='NewKITT'> Red <input type='number' name='red' min='0' max='255' value='200'> Green <input type='number' name='green' min='0' max='255' value='0'> Blue <input type='number' name='blue' min='0' max='255' value='0'> Speed <input type='number' name='speed' min='1' max='5000' value='50'> EyeWidth <input type='number' name='eyewidth' min='1' max='100' value='2'></form>";
  message+="<form action='http://" + myIp + "/rainbowcycle'><input type='submit' value='RainbowCycle'> Speed <input type='number' name='speed' min='1' max='5000' value='20'></form>";
  message+="<form action='http://" + myIp + "/rgbloop'><input type='submit' value='RGBLooP'> Speed <input type='number' name='speed' min='1' max='5000' value='10'></form>";
  message+="<form action='http://" + myIp + "/runninglights'><input type='submit' value='RunningLights'> Red <input type='number' name='red' min='0' max='255' value='20'> Green <input type='number' name='green' min='0' max='255' value='40'> Blue <input type='number' name='blue' min='0' max='255' value='10'> Speed <input type='number' name='speed' min='1' max='5000' value='150'></form>";
  message+="<form action='http://" + myIp + "/snowsparkle'><input type='submit' value='SnowSparkle'> Red <input type='number' name='red' min='0' max='255' value='20'> Green <input type='number' name='green' min='0' max='255' value='40'> Blue <input type='number' name='blue' min='0' max='255' value='10'> Speed <input type='number' name='speed' min='1' max='1000' value='100'></form>";
  message+="<form action='http://" + myIp + "/sparkle'><input type='submit' value='Sparkle'> Red <input type='number' name='red' min='0' max='255' value='20'> Green <input type='number' name='green' min='0' max='255' value='40'> Blue <input type='number' name='blue' min='0' max='255' value='10'> Speed <input type='number' name='speed' min='1' max='5000' value='200'></form>";
  message+="<form action='http://" + myIp + "/strobe'><input type='submit' value='Strobe'> Red <input type='number' name='red' min='0' max='255' value='120'> Green <input type='number' name='green' min='0' max='255' value='0'> Blue <input type='number' name='blue' min='0' max='255' value='0'> Speed <input type='number' name='speed' min='1' max='500' value='10'> Flashes <input type='number' name='flashes' min='1' max='100' value='10'> Delay <input type='number' name='delay' min='1' max='500' value='50'> Pause <input type='number' name='pause' min='1' max='5000' value='1000'></form>";
  message+="<form action='http://" + myIp + "/theatrechase'><input type='submit' value='TheatreChase'> Red <input type='number' name='red' min='0' max='255' value='20'> Green <input type='number' name='green' min='0' max='255' value='40'> Blue <input type='number' name='blue' min='0' max='255' value='10'> Speed <input type='number' name='speed' min='1' max='1000' value='100'></form>";
  message+="<form action='http://" + myIp + "/theatrechaserainbow'><input type='submit' value='TheatreChaseRainbow'> Speed <input type='number' name='speed' min='1' max='5000' value='75'></form>";
  message+="<form action='http://" + myIp + "/twinklerandom'><input type='submit' value='TwinkleRandom'> Flashes <input type='number' name='flashes' min='1' max='100' value='10'> Speed <input type='number' name='speed' min='1' max='1000' value='250'> Fade <input type='checkbox' name='fade' value='true' checked='true'></form>";
  message+="<form action='http://" + myIp + "/twinkle'><input type='submit' value='Twinkle'> Red <input type='number' name='red' min='0' max='255' value='20'> Green <input type='number' name='green' min='0' max='255' value='40'> Blue <input type='number' name='blue' min='0' max='255' value='10'> Flashes <input type='number' name='flashes' min='1' max='100' value='10'> Speed <input type='number' name='speed' min='1' max='1000' value='250'> Fade <input type='checkbox' name='fade' value='true' checked='true'></form>";
  message+="<form action='http://" + myIp + "/fire2012'><input type='submit' value='Fire2012'></form>";
  message+="</body></html>";
  server.send(200, "text/html", message);
  digitalWrite(led, 1);
}

void handleArgs(){
//  message = "Arguments: ";
//  message += server.args();
//  message += "\n";
//  for (uint8_t i = 0; i < server.args(); i++) {
//    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
//  }
//  Serial.println(message);
  if (server.hasArg("red")){String colr = server.arg("red");red = colr.toInt();}
  if (server.hasArg("green")){String colg = server.arg("green");green = colg.toInt();}
  if (server.hasArg("blue")){String colb = server.arg("blue");blue = colb.toInt();}
  if (server.hasArg("speed")){String colr = server.arg("speed");Speed = colr.toInt();}
  if (server.hasArg("eyewidth")){String eyew = server.arg("eyewidth");EyeWidth = eyew.toInt();}
  if (server.hasArg("fade")){Fade=true;}else{Fade=false;}
  if (server.hasArg("eyestat")){eyeStat = server.arg("eyestat");}
  if (server.hasArg("eyespace")){String eyes = server.arg("eyespace");EyeSpace = eyes.toInt();}
  if (server.hasArg("fadespeed")){String fspd = server.arg("fadespeed");fadeSpeed = fspd.toInt();}
  if (server.hasArg("flashes")){String flsh = server.arg("flashes");Flashes = flsh.toInt();}
  if (server.hasArg("delay")){String dlay = server.arg("delay");Delay = dlay.toInt();}
  if (server.hasArg("pause")){String paus = server.arg("pause");Pause = paus.toInt();}
}

void handleColorWipe(){resetArgs();handleArgs();selectedEffect=1;handleRoot();}
void handleCyclonBounce(){resetArgs();handleArgs();selectedEffect=2;handleRoot();}
void handleFadeInOut(){resetArgs();handleArgs();selectedEffect=3;handleRoot();}
void handleHalloweenEyes(){resetArgs();handleArgs();setAll(0,0,0);selectedEffect=4;handleRoot();}
void handlemeteorrain(){resetArgs();handleArgs();selectedEffect=5;handleRoot();}
void handleNewKITT(){resetArgs();handleArgs();selectedEffect=6;handleRoot();}
void handleRainbowCycle(){resetArgs();handleArgs();selectedEffect=7;handleRoot();}
void handleRGBLoop(){resetArgs();handleArgs();selectedEffect=8;handleRoot();}
void handleRunningLights(){resetArgs();handleArgs();selectedEffect=9;handleRoot();}
void handleSnowSparkle(){resetArgs();handleArgs();selectedEffect=10;handleRoot();}
void handleSparkle(){resetArgs();handleArgs();selectedEffect=11;handleRoot();}
void handleStrobe(){resetArgs();handleArgs();selectedEffect=12;handleRoot();}
void handleTheaterChase(){resetArgs();handleArgs();selectedEffect=13;handleRoot();}
void handleTheaterChaseRainbow(){resetArgs();handleArgs();selectedEffect=14;handleRoot();}
void handleTwinkleRandom(){resetArgs();handleArgs();selectedEffect=15;handleRoot();}
void handleTwinkle(){resetArgs();handleArgs();selectedEffect=16;handleRoot();}
void handleFire2012(){resetArgs();handleArgs();selectedEffect=17;handleRoot();}

void handleNotFound() {
  digitalWrite(led, 0);
  message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 1);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  myIp = WiFi.localIP().toString();

  server.on("/", handleRoot);
  server.on("/ledoff", [](){selectedEffect=0;handleRoot();});
  server.on("/colorwipe", handleColorWipe);
  server.on("/cyclonbounce", handleCyclonBounce);
  server.on("/fadeinout", handleFadeInOut);
  server.on("/halloweeneyes", handleHalloweenEyes);
  server.on("/meteorrain", handlemeteorrain);
  server.on("/newkitt", handleNewKITT);
  server.on("/rainbowcycle", handleRainbowCycle);
  server.on("/rgbloop", handleRGBLoop);
  server.on("/runninglights", handleRunningLights);
  server.on("/snowsparkle", handleSnowSparkle);
  server.on("/sparkle", handleSparkle);
  server.on("/strobe", handleStrobe);
  server.on("/theatrechase", handleTheaterChase);
  server.on("/theatrechaserainbow", handleTheaterChaseRainbow);
  server.on("/twinklerandom", handleTwinkleRandom);
  server.on("/twinkle", handleTwinkle);
  server.on("/fire2012", handleFire2012);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  FastLED.addLeds<WS2812, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
}

void loop(void) {
  server.handleClient();
  switch(selectedEffect) {
    case 0  : {setAll(0,0,0);return;}
    case 1  : {colorWipe();break;}
    case 2  : {CyclonBounce();break;}
    case 3  : {FadeInOut();break;}
    case 4  : {HalloweenEyes();break;}
    case 5  : {meteorRain();break;}
    case 6  : {NewKITT();break;}
    case 7  : {rainbowCycle();break;}
    case 8  : {RGBLoop();break;}
    case 9  : {RunningLights();break;}
    case 10 : {SnowSparkle();break;}
    case 11 : {Sparkle();break;}
    case 12 : {Strobe();break;}
    case 13 : {theaterChase();break;}
    case 14 : {theaterChaseRainbow();break;}
    case 15 : {TwinkleRandom();break;}
    case 16 : {Twinkle();break;}
    case 17 : {Fire2012();break;}
  }
}

void colorWipe(){
  currentMillis = millis();
  if(currentMillis - prevMillis > Speed) {
    prevMillis = currentMillis;
    if (dir==0){
      setPixel(i, red, green, blue);
      showStrip();
      if (i==NUM_LEDS-1){dir++;i=0;}else{i++;}
    }else{
      setPixel(i, 0, 0, 0);
      showStrip();
      if (i==NUM_LEDS-1){dir=0;i=0;}else{i++;}
    }
  }
}

void CyclonBounce(){
  currentMillis = millis();
  switch(dir){
    case 0 :{
      if(currentMillis - prevMillis > Speed) {
        prevMillis = currentMillis;
        setAll(0,0,0);
        setPixel(i, red/10, green/10, blue/10);
        for(int j = 1; j <= EyeWidth; j++) {
          setPixel(i+j, red, green, blue); 
        }
        setPixel(i+EyeWidth+1, red/10, green/10, blue/10);
        showStrip();
        if (i==NUM_LEDS-EyeWidth-2){dir=1;}
        i++;
        break;
      }
    }
    case 1 :{
      if(currentMillis - prevMillis > Speed) {
        prevMillis = currentMillis;
        i--;
        setAll(0,0,0);
        setPixel(i, red/10, green/10, blue/10);
        for(int j = 1; j <= EyeWidth; j++) {setPixel(i+j, red, green, blue);}
        setPixel(i+EyeWidth+1, red/10, green/10, blue/10);
        showStrip();
        if (i==0){dir=0;}
        break;
      }
    }
  }
}

void FadeInOut(){
  currentMillis = millis();
  if(currentMillis - prevMillis > Speed) {
    prevMillis = currentMillis;
    if (k==255){dir++;}
    if (k==0){dir=0;}
    if (dir==0){k++;}else{k--;}
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
  }
}

void HalloweenEyes(){
  currentMillis = millis();
  if(currentMillis - delayMillis > EndPause) {
    if (eyeStat==false){
      for(i = 0; i < EyeWidth; i++) {setPixel(StartPoint + i, red, green, blue);setPixel(Start2ndEye + i, red, green, blue);}
      k=255;
      showStrip();
      eyeStat=true;
    }else{
      if (currentMillis - prevMillis > Steps){
        if (Fade == true){
          prevMillis=currentMillis;
          k--;
          r = (k/256.0)*red;g = (k/256.0)*green;b = (k/256.0)*blue;
          for(i = 0; i < EyeWidth; i++) {setPixel(StartPoint + i, r, g, b);setPixel(Start2ndEye + i, r, g, b);}
          showStrip();
          if (k==0){
            eyeStat=false;Steps = random(5,50);FadeDelay = random(50,150);EndPause = random(1000,5000);
            delayMillis = currentMillis;StartPoint  = random( 0, NUM_LEDS - (2*EyeWidth) - EyeSpace );
            Start2ndEye = StartPoint + EyeWidth + EyeSpace;
          }
        }else{
          setAll(0,0,0);showStrip();
          eyeStat=false;Steps = random(5,50);FadeDelay = random(50,150);EndPause = random(1000,5000);
          delayMillis = currentMillis;StartPoint  = random( 0, NUM_LEDS - (2*EyeWidth) - EyeSpace );
          Start2ndEye = StartPoint + EyeWidth + EyeSpace;
        }
      }
    }
  }
}

void meteorRain(){
  currentMillis = millis();
  if(currentMillis - prevMillis > Speed) {
    prevMillis = currentMillis;
    for(int j=0; j<NUM_LEDS; j++) {if( (!Fade) || (random(10)>5) ) {fadeToBlack(j, fadeSpeed );}}
    for(int j = 0; j < EyeWidth; j++) {if( ( i-j <NUM_LEDS) && (i-j>=0) ) {setPixel(i-j, red, green, blue);}}
    showStrip();
    if (i==NUM_LEDS-1){i=0;}else{i++;}
  }
}

void fadeToBlack(int ledNo, byte fadeValue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
    // NeoPixel
    uint32_t oldColor;uint8_t r, g, b;int value;
    oldColor = strip.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;g = (oldColor & 0x0000ff00UL) >> 8;b = (oldColor & 0x000000ffUL);
    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);g=(g<=10)? 0 : (int) g-(g*fadeValue/256);b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
    strip.setPixelColor(ledNo, r,g,b);
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[ledNo].fadeToBlackBy( fadeValue );
 #endif  
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
  if(WheelPos < 85) {
    c[0]=WheelPos * 3;
    c[1]=255 - WheelPos * 3;
    c[2]=0;
  } else if(WheelPos < 170) {
    WheelPos -= 85;
    c[0]=255 - WheelPos * 3;
    c[1]=0;
    c[2]=WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0]=0;
    c[1]=WheelPos * 3;
    c[2]=255 - WheelPos * 3;
  }
  return c;
}

void NewKITT(){
  currentMillis = millis();
  switch(dir){
    case 0 :{
      if(currentMillis - prevMillis > Speed) {
        prevMillis = currentMillis;
        setAll(0,0,0);
        setPixel(i, red/10, green/10, blue/10);
        for(int j = 1; j <= EyeWidth; j++) {
          setPixel(i+j, red, green, blue); 
        }
        setPixel(i+EyeWidth+1, red/10, green/10, blue/10);
        showStrip();
        if (i==NUM_LEDS-EyeWidth-2){dir++;break;}
        i++;
      }
      break;
    }
    case 1 :{
      if(currentMillis - prevMillis > Speed) {
        prevMillis = currentMillis;
        setAll(0,0,0);
        setPixel(i, red/10, green/10, blue/10);
        for(int j = 1; j <= EyeWidth; j++) {
          setPixel(i+j, red, green, blue); 
        }
        setPixel(i+EyeWidth+1, red/10, green/10, blue/10);
        showStrip();
        if (i==0){dir++;break;}
        i--;
      }
      break;
    }
    case 2 :{
      if(currentMillis - prevMillis > Speed*2) {
        prevMillis = currentMillis;
        int inverse_value = map(i, 0, NUM_LEDS-EyeWidth-2, NUM_LEDS-EyeWidth-2, 0);
        setAll(0,0,0);
        setPixel(i, red/10, green/10, blue/10);
        setPixel(inverse_value, red/10, green/10, blue/10);
        setPixel(i+EyeWidth+1, red/10, green/10, blue/10);
        setPixel(inverse_value+EyeWidth+1, red/10, green/10, blue/10);
        for(int j = 1; j <= EyeWidth; j++) {setPixel(i+j, red, green, blue);}
        for(int j = 1; j <= EyeWidth; j++) {setPixel(inverse_value+j, red, green, blue);}
        showStrip();
        if (i==NUM_LEDS-EyeWidth-2){dir++;i=NUM_LEDS-EyeWidth-2;break;}
        i++;
      }
      break;
    }
    case 3:{
      if(currentMillis - prevMillis > Speed) {
        prevMillis = currentMillis;
        setAll(0,0,0);
        setPixel(i, red/10, green/10, blue/10);
        for(int j = 1; j <= EyeWidth; j++) {setPixel(i+j, red, green, blue);}
        setPixel(i+EyeWidth+1, red/10, green/10, blue/10);
        showStrip();
        if (i==0){dir++;break;}
        i--;
      }
      break;
    }
    case 4:{
      if(currentMillis - prevMillis > Speed) {
        prevMillis = currentMillis;
        setAll(0,0,0);
        setPixel(i, red/10, green/10, blue/10);
        for(int j = 1; j <= EyeWidth; j++) {setPixel(i+j, red, green, blue);}
        setPixel(i+EyeWidth+1, red/10, green/10, blue/10);
        showStrip();
        if (i==NUM_LEDS-EyeWidth-2){dir++;i=0;break;}
        i++;
      }
      break;
    }
    case 5 :{
      if(currentMillis - prevMillis > Speed*2) {
        prevMillis = currentMillis;
        int inverse_value = map(i, 0, NUM_LEDS-EyeWidth-2, NUM_LEDS-EyeWidth-2, 0);
        setAll(0,0,0);
        setPixel(i, red/10, green/10, blue/10);
        setPixel(inverse_value, red/10, green/10, blue/10);
        setPixel(i+EyeWidth+1, red/10, green/10, blue/10);
        setPixel(inverse_value+EyeWidth+1, red/10, green/10, blue/10);
        for(int j = 1; j <= EyeWidth; j++) {setPixel(i+j, red, green, blue);}
        for(int j = 1; j <= EyeWidth; j++) {setPixel(inverse_value+j, red, green, blue);}
        showStrip();
        if (i==NUM_LEDS-EyeWidth-2){dir=0;i=0;break;}
        i++;
      }
      break;
    }
  }
}

void rainbowCycle(){
  currentMillis = millis();
  if(currentMillis - prevMillis > Speed) {
    prevMillis = currentMillis;
    if (j==256){j=0;}else{j++;}
    for(i=0; i< NUM_LEDS; i++) {c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);setPixel(i, *c, *(c+1), *(c+2));}
    showStrip();
  }
}

void RGBLoop(){
  currentMillis = millis();
  if(currentMillis - prevMillis > Speed) {
    prevMillis = currentMillis;
    if (k==255){dir++;}
    if (k==0){if (dir!=0){j++;}dir=0;}
    if (j>2){j=0;}
    if (dir==0){k++;}else{k--;}
    switch(j) { 
      case 0: setAll(k,0,0); break;
      case 1: setAll(0,k,0); break;
      case 2: setAll(0,0,k); break;
    }
  }
}

void RunningLights(){
  currentMillis = millis();
  if(currentMillis - prevMillis > Speed) {
    prevMillis = currentMillis;
    if (j==0){j++;}
    if (i==NUM_LEDS*2-1){i=0;j=1;prevMillis=currentMillis-Speed;return;}
    for(k=0;k<NUM_LEDS;k++){setPixel(k,((sin(k+j) * 127 + 128)/255)*red,((sin(k+j) * 127 + 128)/255)*green,((sin(k+j) * 127 + 128)/255)*blue);}
    showStrip();
    i++;
    j++;
  }
}

void SnowSparkle(){
  currentMillis = millis();
  if(currentMillis - prevMillis > Speed) {
    prevMillis = currentMillis;
    setAll(red,green,blue);
    if (currentMillis - snowMillis > snowDelay){
      snowMillis=currentMillis;
      setPixel(random(NUM_LEDS),250, 250, 250);
      snowDelay=random(100,1000);
      showStrip();
    }
  }
}

void Sparkle(){
  currentMillis = millis();
  if(currentMillis - prevMillis > Speed) {prevMillis = currentMillis;setAll(0,0,0);setPixel(random(NUM_LEDS),red,green,blue);showStrip();}
}

void Strobe(){
  currentMillis = millis();
  if (i==Flashes && state==0){if(currentMillis - prevMillis > Pause) {i=0;}else{setAll(0,0,0);return;}}
  if(currentMillis - prevMillis > Delay) {prevMillis = currentMillis;if (state==0){setAll(red,green,blue);state=1;i++;}else{setAll(0,0,0);state=0;}}
}

void theaterChase(){
  currentMillis = millis();
  if(currentMillis - prevMillis > Speed) {prevMillis = currentMillis;setAll(0,0,0);for (i=0; i < NUM_LEDS; i=i+3) {if (i+j<NUM_LEDS){setPixel(i+j, red, green, blue);}}showStrip();if (j==2){j=0;}else{j++;}}
}

void theaterChaseRainbow(){
  currentMillis = millis();
  if(currentMillis - prevMillis > Speed) {prevMillis = currentMillis;setAll(0,0,0);for (i=0; i < NUM_LEDS; i=i+3) {if (i+j<NUM_LEDS){c = Wheel( (i+k) % 255);setPixel(i+j, *c, *(c+1), *(c+2));}}showStrip();if (j==2){j=0;}else{j++;}if (k==256){k=0;}else{k++;}}
}

void TwinkleRandom(){
  currentMillis = millis();
  if(currentMillis - prevMillis > Speed) {
    prevMillis = currentMillis;
    if (Fade){setAll(0,0,0);}
    if (i==Flashes){setAll(0,0,0);i=0;setPixel(random(NUM_LEDS),random(0,255),random(0,255),random(0,255));showStrip();return;}
    setPixel(random(NUM_LEDS),random(0,255),random(0,255),random(0,255));
    showStrip();
    i++;
  }
}

void Twinkle(){
  currentMillis = millis();
  if(currentMillis - prevMillis > Speed) {
    prevMillis = currentMillis;
    if (Fade){setAll(0,0,0);}
    if (i==Flashes){setAll(0,0,0);i=0;setPixel(random(NUM_LEDS),red,green,blue);showStrip();return;}
    setPixel(random(NUM_LEDS),red,green,blue);
    showStrip();
    i++;
  }
}

void Fire2012()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
     showStrip();
    i++;
}


// Apply LED color changes
void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

// Set a LED color (not yet visible)
void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H 
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

// Set all LEDs to a given color and apply it (visible)
void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}
