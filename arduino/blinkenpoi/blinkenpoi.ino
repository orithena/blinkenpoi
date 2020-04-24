#ifdef ESP32
#include <WiFi.h>
#include <WebServer.h>
#endif
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#endif

// Install the libraries with arduino library manager in Tools/Manage libraries


#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
                                   // NEEDS v5.x !! >6.x will fail to compile
#ifdef ESP32
#include <ESPmDNS.h>
#endif
#ifdef ESP8266
#include <ESP8266mDNS.h>
#endif
#include <FS.h>
#ifdef ESP32
#include <SPIFFS.h>         
#endif
#include <Adafruit_DotStar.h>
#include <SPI.h>         

#include "OneButton.h"
#include "lib8tion.h"




/*
 * 
 * 
 * 
 * 
 *   What is going on in here?
 * 
 *  This is the firmware running on the Blinkenpoi 
 *  Documentation: 
 *   https://metalab.at/wiki/Blinkenpoi
 * 
 * 
 *   PLEASE READ THIS!
 *   This is a rather complex setup and you need to install various libraries, some at a specific version.
 *   Please read the information provided here;
 *   https://github.com/overflo23/blinkenpoi/wiki/Building-the-Firmware
 *   
 * 
 *  First time turn on:
 * 
 *  When you turn on the blinkenpoi checks its internally ROM if it is already configured for a specific wlan.
 *  If it is configured it will try to connect to it, if it fails it opens its own acces point for configuration
 *  Once connected to the accesspoint you should be welcomed by a captive portal.
 *  If this fails the poi is accessable at 192.168.4.1
 * 
 *  After configuration it saves the stick name and network credentials if it was successfully connecting.
 *  Success is indicted by a green led.
 * 
 *  --
 *  
 *  Normal operation:
 *  Turn on the stick, it shines a pink led for ~ 2 seconds.
 *  This can be interrupted by a short press of the button.
 *  If you interrupt now the network is NOT configured and the stick is available offline and you can cycle trough the animations on the stick with the button.
 *  
 *  If you press the button during the pink phase and keep it pressed or ~1.5 seconds you activate a configuration RESET.
 *  The first led will blink red 3 times and the saved wifi information is deleted and an access point is opened for configuration.
 *  
 *  Once the stick operates it waits for a trigger on the webinterface.
 *  Animation playback is triggered by an HTTP request.
 *  for example: http://10.0.0.42/run/rgb1.tek 
 *  Alternatively you may press Button 2 shortly to select the next animation,
 *  long press to activate automatic playback.
 * 
 *  Of course you can override the animation playback with the button at any time.
 * 
 *   My code is CC0 Public domain, (ab)use at will. 
 *   But some libraries use GPL or other licenses which apply.
 *   
 */





// These are the internal pin numbers used to drive the leds.
// if C and D are swapped on your ledstrip just swap these two numbers
#ifdef ESP32
#define DATAPIN    23
#define CLOCKPIN   18
#endif
#ifdef ESP8266
#define DATAPIN    14
#define CLOCKPIN   13
#endif


// button1 is borken because the wemos D1 has a strong pulldown resistor that can not be overridden with internal_pullup on D8 / GPIO15
// well. fuck.
// so it is a one-button interface than :(
// kann man so machen ist dann halt scheisse.

// valid pinout for PCB rev. 3.0
#ifdef ESP32
const int button2_pin=16;
#endif
#ifdef ESP8266
const int button2_pin=2; // D4
#endif
// valid pinout for PCB rev. 1.0
//const int button1_pin=12; // D6







String SW_VERSION="0.9";


#ifdef ESP32
WebServer server(80);       // Create a webserver object that listens for HTTP request on port 80
#endif
#ifdef ESP8266
ESP8266WebServer server(80);       // Create a webserver object that listens for HTTP request on port 80
#endif

File fsUploadFile;                 // a File variable to temporarily store the received file



const char *ssid = "Blinkenpoi"; // The name of the Wi-Fi network that will be created

String STICK_NAME="No. 1";


const char* mdnsName = "blinkenpoi"; // Domain name for the mDNS responder



//  TODO: neue button belegung evaluieren / too late for CCCamp 2019 :(

OneButton button2(button2_pin, true);
//OneButton button1(button1_pin, false);

typedef struct Pixel {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} Pixel;

struct AnimationState;
typedef int (*animationfunction)(struct AnimationState*);
typedef struct AnimationState {
  uint32_t frame;               // Counts the frame number. One frame is the state of all LEDs for roughly 4 milliseconds, which _very_ roughly amounts to a "square" pixel.
                                // Even if the animation function is called more often, the frame counter is increasing every 4ms.
  File file;                    // the file to read from if loaded == 1
  int loaded;                   // 0: nothing is loaded. 1: a file is loaded. 2: a generative animation is selected, no need to load anything
  int running;                  // internal index number of animation
  animationfunction callback;   // reference to the actual animation function, which fills one row with data
  struct Pixel pixels[25];      // array of RGB pixel data, to be filled by callback().
} AnimationState;

AnimationState state = {
  frame: 0,
  file: File(),
  loaded: 0, 
  running: 0,
  callback: NULL,
  pixels: {}
};

typedef struct AnimationInfo {
  String name;
  animationfunction func;
} AnimationInfo;

#include "generative_animations.h"

/// global vars, no touchy
/// but I did touch them...
//int animation_running=0;
int total_animations=0;
bool online_mode=true;
bool looping = false; // a.k.a. "wifi setup is done", used by button functions
bool autoanimation = false;

// set true if button is pressed during pink phase
boolean reset_config = false;


// for wifimanager 
boolean shouldSaveConfig = false;
char custom_stick_name_str[20] = "";


void setup() {

  Serial.begin(115200);        // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println("\r\n");

  

  startPixel();

  startButtons();
  
  startSPIFFS();               // Start the SPIFFS and list all contents

  checkforButtonInterrupt(); // see if button is pressed.
  
  online_mode = startWiFi();
  if( online_mode ) {          // Start a Wi-Fi access point, and try to connect to some given access points. Then wait for either an AP or STA connection
  
    startMDNS();                 // Start the mDNS responder

    startServer();               // Start a HTTP server with a file read handler and an upload handler
  }
}


void loop() {
  looping = true;
  checkButtons();
  EVERY_N_SECONDS(20) {
    if( autoanimation ) {
        state.running = (state.running + 1) % total_animations;
        load_animation(state.running);
    }
  }
  #ifndef ESP32 //not needed on ESP32, ESPmDNS.cpp registers a sys event handler instead
  MDNS.update();
  #endif
  if( online_mode ) {
    server.handleClient();                      // run the http server
  }
  showAnimation();
}
