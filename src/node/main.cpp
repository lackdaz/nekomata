

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

#define   MESH_PREFIX     "snf"
#define   MESH_PASSWORD   "chewbacca"
#define   MESH_PORT       5555

#define NUM_LEDS 64*3
#define NUM_SEG 64

#define DATA_PIN D3
#define CLOCK_PIN -1

#ifdef TESTER
  #define LED_TYPE APA106
#else
  #define LED_TYPE WS2812B
#endif

// #define BREATHE_DURATION    7000 // milliseconds until cycle repeat

CRGB leds[NUM_LEDS];

Scheduler userScheduler;
painlessMesh  mesh;

// User stub
void breathe();

Task task_breathe( TASK_MILLISECOND * 100 , TASK_FOREVER, &breathe );

void breathe() {

  FastLED.setBrightness(128);
  // fill_solid(leds, NUM_LEDS, CHSV(hue, sat, val));


  // fill_solid(&leds[0], NUM_SEG, CRGB::Red);
  // fill_solid(&leds[64], NUM_SEG, CRGB::Green);
  // fill_solid(&leds[128], NUM_SEG, CRGB::Blue);

  fill_solid(&leds[0], NUM_SEG, CRGB::Red);
  fill_solid(&leds[64], NUM_SEG, CRGB::Blue);
  fill_solid(&leds[128], NUM_SEG, CRGB::DarkOrchid);
  FastLED.show();
  Serial.print("FPS: "); Serial.println(LEDS.getFPS());   // Display FPS in serial monitor
}

// Needed for painless library
void receivedCallback( uint32_t from, String msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  digitalWrite(LED_BUILTIN, LOW);
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

void setup() {
  Serial.begin(115200);
  LEDS.addLeds<LED_TYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
  LEDS.setCorrection( SodiumVapor );
  // LEDS.setMaxPowerInVoltsAndMilliamps(5, 25200); 
  // FastLED.setTemperature(0xC4FFB0); // 0xC4FFB0
  FastLED.setTemperature(SodiumVapor); // 0xC4FFB0
  FastLED.setBrightness(128);
  // FastLED.setDither( 0 );

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

  pinMode(LED_BUILTIN, OUTPUT);

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.setContainsRoot(true);

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( task_breathe );
  
  task_breathe.enable();
  Serial.println("================");
  Serial.println("I AM NODE. I AM MANY!");
  Serial.println("================");
}

void loop() {
  mesh.update();
}