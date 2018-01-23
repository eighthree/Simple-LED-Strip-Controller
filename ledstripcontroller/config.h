/*
  LED Strip Controller Config
*/

/*
  Enabled by default to free up some ram.
  Can be safely left enabled, turn off if you are having issues.
*/
#define HEAPCHECKER 1               // set to 1 to test HEAP loss fix
#ifdef HEAPCHECKER
#include "lwip/tcp_impl.h"        // losing bytes work around
#endif

/*
  LED Strip Configuration
  https://learn.adafruit.com/adafruit-neopixel-uberguide/best-practices

  DEFAULT_BRIGHTNESS must be set appropriately.
*/
#define LED_PIN D4
#define LED_COUNT 120
#define DEFAULT_COLOR 0xFF5900
#define DEFAULT_BRIGHTNESS 200        // Set this conservatively based on your power supply. 
#define DEFAULT_SPEED 100
#define DEFAULT_MODE FX_MODE_STATIC
#define BRIGHTNESS_STEP 15            // in/decrease brightness by this amount per click              
#define SPEED_STEP 10                 // in/decrease brightness by this amount per click                   

/*
    Portal AP & Device ID Setup
    PORTAL_AP_NAME is used as the mdns name, change this to something
    sensible if you plan to run more than one installation.
*/
#define PORTAL_AP_NAME "ledstrip"   // Set Portal Name
#define PORTAL_AP_PW "password"     // Set Portal Password
#define PORTAL_FW "simple LED strip controller 2.0"

/*
    Double Reset
*/
#define DRD_TIMEOUT 10  // Number of seconds after reset during which a subseqent reset will be considered a double reset.         
#define DRD_ADDRESS 0   // RTC Memory Address for the DoubleResetDetector to use 

/*
   LED Default State/Mode
*/
uint8_t LED_STATE = 0;

/*
   LED Default Values
   #define DEFAULT_COLOR 0xFF5900
*/
uint8_t R_LED = 255;
uint8_t G_LED = 89;
uint8_t B_LED = 0;
uint8_t CURRENT_MODE = 0;

String modes = "";
String ipaddress = "";


/*
    TCPCleanup
    Runs every minute by default, Update Interval in milliseconds
    #define HEAPCHECKER 1 set to 0
*/
long previousMillis = 0;
long interval = 60000;
uint32_t originalram;

/*
   Timezone Settings
   Used for device time
*/
int timezone = 3;
int dst = 0;


