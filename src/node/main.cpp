

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

  #define DEBUG

  #define INHABIT_DELAY (TASK_MILLISECOND * 2000)

  // TODO: DO POLL
  // TODO: CODE OUT PREASSIGNMENT

  CRGB leds[NUM_LEDS];

  Scheduler ts;
  painlessMesh  mesh;

  int threshold = 40;
  bool touch_detected = false;
  bool search_in_progress = false;

  #if BUILD_TYPE == BUILD_TYPE_ROOT
    bool inhabited = true;
  #else
    bool inhabited = false;
  #endif

  // User stub
  void touch();
  void flight();
  void inhabit();
  void knock_knock();
  void neko_drain();
  void neko_imbue();

  Task task_touch( TASK_MILLISECOND * 500 , TASK_FOREVER, &touch );
  Task task_flight( TASK_IMMEDIATE , TASK_ONCE, &flight, &ts, false);
  Task task_inhabit( TASK_IMMEDIATE , TASK_ONCE, &inhabit, &ts, false );
  Task task_knock_knock( TASK_IMMEDIATE , TASK_ONCE, &knock_knock, &ts, false );

  /* Light Effects */
  Task task_drain( TASK_IMMEDIATE , TASK_ONCE, &neko_drain, &ts, false );
  Task task_imbue( TASK_IMMEDIATE , TASK_ONCE, &neko_imbue, &ts, false );

  void touch() {
    Serial.print(touch_detected);
    Serial.print("|");
    Serial.print(inhabited);
    Serial.print("|");
    Serial.println(search_in_progress);


    if (touch_detected && inhabited && !search_in_progress){
      touch_detected = false; // reset touch flag
      task_knock_knock.restart();
    } else {
      touch_detected = false; // debounce;
    }
  }

  void flight() {
    #ifdef DEBUG
      Serial.println("flight");
    #endif

    task_drain.restart();

    search_in_progress = false;
    inhabited = false;
  }

  void inhabit() {
    inhabited = true;
    #ifdef DEBUG
      Serial.println(String(mesh.getNodeId()) + " is now inhabited");
    #endif

    task_imbue.restart();
  }

  void knock_knock() {
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
    mesh.sendSingle(nodes[random_index], "?Meow anyone at home?"); // poll
    search_in_progress = true;
  }

  void neko_drain() {
    FastLED.clear();
    FastLED.show();
  }

  void neko_imbue() {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.show();
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

    char callsign = msg.charAt(0);
    switch (callsign){
      case '?':
        if (!inhabited) {
          mesh.sendSingle(from, "<Come...");
          task_inhabit.restartDelayed(INHABIT_DELAY); // TODO: Must be exact else it would be desynced
        } else {
          mesh.sendSingle(from, "!Go Away");
        }
        break;
      case '<': // time to flight
        task_flight.restart();
        break;
      case '!': // search again
        task_knock_knock.restart();
        break;
      default:
        Serial.println("Forbidden 304");
        break;
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

    #if BUILD_TYPE == BUILD_TYPE_ROOT
      fill_solid(leds, NUM_LEDS, CRGB::Red);
      FastLED.show();
    #endif

    pinMode(LED_BUILTIN, OUTPUT);
    touchAttachInterrupt(TOUCH_PIN, got_touch, threshold);

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
    mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

    mesh.init( MESH_PREFIX, MESH_PASSWORD, &ts, MESH_PORT );
    mesh.setContainsRoot(true);
    #if BUILD_TYPE == BUILD_TYPE_ROOT
      mesh.setRoot(true);
    #endif

    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    ts.addTask( task_touch );
    task_touch.enable();
    #ifdef DEBUG
      Serial.println("================");
      #if BUILD_TYPE == BUILD_TYPE_ROOT
        Serial.println("I AM SOURCE NEKO. I AM ONE!");
      #else
        Serial.println("I AM A NEKOSHRINE. I AM MANY!");
      #endif
      Serial.println("================");
    #endif    
  }

  void loop() {
    mesh.update();
  }