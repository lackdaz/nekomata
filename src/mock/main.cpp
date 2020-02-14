

//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 2. prints anything it receives to Serial.print
//
//
//************************************************************
#include <Arduino.h>
#include <painlessMesh.h>
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include <FastLED.h>
#include <U8g2lib.h>
#include "pin_definitions.h"

#define   MESH_PREFIX     "snf"
#define   MESH_PASSWORD   "chewbacca"
#define   MESH_PORT       5555

#define NUM_LEDS 1

#ifdef MOCK
  #define LED_TYPE APA106
#else
  #define LED_TYPE WS2812B
#endif

CRGB leds[NUM_LEDS];

Scheduler ts;
painlessMesh  mesh;

int threshold = 40;
bool touch_detected = false;
bool inhabited = false;

// User stub
void touch();
void flight();
void inhabit();

Task task_touch( TASK_MILLISECOND * 100 , TASK_FOREVER, &touch );
Task task_flight( TASK_IMMEDIATE , TASK_ONCE, &flight, &ts, false);
Task task_inhabit( TASK_IMMEDIATE , TASK_ONCE, &inhabit, &ts, false );

void touch() {
  if(touch_detected && inhabited){
    touch_detected = false;
    task_flight.restart();
  }
}

void flight() {
  FastLED.clear();
  FastLED.show();
  uint32_t nodes[100];
  byte random_index;
  size_t i = 0;
  SimpleList<uint32_t> myList = mesh.getNodeList();
  SimpleList<uint32_t>::iterator itr=myList.begin();
  random_index = random(0, myList.size());

  while (itr != myList.end()) {
    nodes[i]=(*itr);
    // Serial.println(*itr);
    ++itr;
    ++i;
  }
  inhabited = false;
  digitalWrite(LED_BUILTIN, HIGH);
  mesh.sendSingle(nodes[random_index], "Boo!");
}

void inhabit() {

}

/* 
  ISR
*/

void got_touch(){
 touch_detected = true;
}

// Needed for painless library
void receivedCallback( uint32_t from, String msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());

  if (!inhabited) {
    inhabited = true;
    digitalWrite(LED_BUILTIN, LOW);
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.show();
  }
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

void setup() {
  Serial.begin(115200);
  delay(100);
  #ifdef MOCK
    LEDS.addLeds<LED_TYPE, DATA_PIN, RGB>(leds, NUM_LEDS);
  #else
    LEDS.addLeds<LED_TYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
  #endif
  LEDS.setCorrection( SodiumVapor );
  // LEDS.setMaxPowerInVoltsAndMilliamps(5, 25200); 
  // FastLED.setTemperature(0xC4FFB0); // 0xC4FFB0
  FastLED.setTemperature(SodiumVapor); // 0xC4FFB0
  FastLED.setBrightness(128);
  // FastLED.setDither( 0 );

  randomSeed(analogRead(0));

  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
  delay(1000);
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  FastLED.show();
  delay(1000);
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
  FastLED.show();
  delay(1000);
  FastLED.clear();
  FastLED.show();

  pinMode(LED_BUILTIN, OUTPUT);
  touchAttachInterrupt(TOUCH_PIN, got_touch, threshold);

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &ts, MESH_PORT );
  mesh.setContainsRoot(true);

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  ts.addTask( task_touch );
  ts.addTask( task_flight );
  ts.addTask( task_inhabit );
  task_touch.enable();
  Serial.println("================");
  Serial.println("I AM MAO. I AM MANY!");
  Serial.println("================");
}

void loop() {
  mesh.update();
}