
//****************************************************************************************
//  I hope you find this code helpful. 
//
//  In general this has been a very satisfying endeavor.  Of course, I relied on the 
//  Random Nerd Tutorials, GitHub, online LVGL documentation, and WEB searches for
//  examples and explanations.
//
//  I already have another thermostate working based on and esp8266 that uses the Blynk 
//  IOT interface on my phone to change settings.  My current goal is to use ESP-NOW to
//  interface the CYD to that esp8266 to control the settings.
//
//  Because of the limited IO on the Cheap Yellow Display (CYD) it would be difficult
//  to implement it as a standalone thermostate.  One possiblity is to use a ESP-NOW
//  to communicate to another ESP32 mini that controls intermediate relays that interface
//  to the to the contol board for heat off and cool much like a hardwired thermostat.
//
//                               ****** IMPORTANT *****
//                               ****** IMPORTANT *****
//                               ****** IMPORTANT *****
//
//  Follow one of the Random Nerd Tutorials for the cheap yellow display (CYD).  It will
//  instruct you how to install the initial libraries and setups files.  In addition:
//    (a) Search for credentials in the code and input your network WiFi credentials.
//    (b) Go to the Tools tab to select board ESP32-2432S028R CYD
//    (c) On the Tools tab also select Partition Scheme Huge APPC3MB No OTA/1MB SPIFFS.
//    (d) The programs takes a long time to compile and load; be patient.
//    (e) At some time you will want to update the Time Zone and Weather Station
//        WEB pages for your specific needs; however I suggest doing that after the 
//        initial load.
//
//
//  Screen Notes:  
//    (1) Clicking the button at the top center of the screens will cause the display
//        to switch screens.
//    (2) The increase and decrease buttons disabled when "Off" is selected.
//    (3) When "Off" is selected the temperature setpoint is set to the default value.
//    (4) The decrease [-] and increase [+] button will be blacked out when the 
//        minimum or maximum temperature setpoint is reach.  This shows they are 
//        disabled.
//
//  Regards and Good Luck,
//  JustRoger1956
//  April 27, 2025
//****************************************************************************************


/*  Rui Santos & Sara Santos - Random Nerd Tutorials - 
https://RandomNerdTutorials.com/esp32-cyd-lvgl-digital-clock/  |  
https://RandomNerdTutorials.com/esp32-tft-lvgl-digital-clock/
    THIS EXAMPLE WAS TESTED WITH THE FOLLOWING HARDWARE:
    1) ESP32-2432S028R 2.8 inch 240×320 also known as the Cheap Yellow Display (CYD): 
       https://makeradvisor.com/tools/cyd-cheap-yellow-display-esp32-2432s028r/
       SET UP INSTRUCTIONS: https://RandomNerdTutorials.com/cyd-lvgl/
    2) REGULAR ESP32 Dev Board + 2.8 inch 240x320 TFT 
       Display: https://makeradvisor.com/tools/2-8-inch-ili9341-tft-240x320/ and 
       https://makeradvisor.com/tools/esp32-dev-board-wi-fi-bluetooth/
       SET UP INSTRUCTIONS: https://RandomNerdTutorials.com/esp32-tft-lvgl/lv_obj_t
       Permission is hereby granted, free of charge, to any person obtaining 
       a copy of this software and associated documentation files.
       The above copyright notice and this permission notice shall be included 
       in all copies or substantial portions of the Software.
*/

//*****--------------------------------------------------------------------------------------------*****
//*****  Note: The code of installing the libraries was extracted from a Random Nerd tutorial      *****
//*****  for the CYD. Please follow one of their tutorials for the initial setups.                 *****
//*****--------------------------------------------------------------------------------------------***** 
///////////////////////*****Installing Libraties and Declaring IO*****////////////////////
///////////////////////*****Installing Libraties and Declaring IO*****////////////////////
///////////////////////*****Installing Libraties and Declaring IO*****////////////////////

/*  Install the "lvgl" library version 9.2 by kisvegabor to interface with the 
    TFT Display - https://lvgl.io/
    *** IMPORTANT: lv_conf.h available on the internet will probably NOT work with 
    *** the examples available at Random Nerd Tutorials     
    *** YOU MUST USE THE lv_conf.h FILE PROVIDED IN THE LINK BELOW IN ORDER TO USE 
    *** THE EXAMPLES FROM RANDOM NERD TUTORIALS
    FULL INSTRUCTIONS AVAILABLE ON HOW CONFIGURE THE LIBRARY: 
    https://RandomNerdTutorials.com/cyd-lvgl/ or 
    https://RandomNerdTutorials.com/esp32-tft-lvgl/   
    This file exists only to be compatible with Arduino's library structure*/
#include <lvgl.h>

/*  Install the "TFT_eSPI" library by Bodmer to interface with the TFT Display - 
    https://github.com/Bodmer/TFT_eSPI
    *** IMPORTANT: User_Setup.h available on the internet will probably NOT work with 
    *** the examples available at Random Nerd Tutorials
    *** YOU MUST USE THE User_Setup.h FILE PROVIDED IN THE LINK BELOW IN ORDER TO USE 
    *** THE EXAMPLES FROM RANDOM NERD TUTORIALS
    FULL INSTRUCTIONS AVAILABLE ON HOW CONFIGURE THE LIBRARY:
    https://RandomNerdTutorials.com/cyd-lvgl/ or 
    https://RandomNerdTutorials.com/esp32-tft-lvgl/   */
#include <TFT_eSPI.h>

//****************************************************************************************
//         Web page that provide the hex colors used throughout this program.
//****************************************************************************************
// https://vuetifyjs.com/en/styles/colors/#material-colors

//****************************************************************************************
// AruduinoJSON is a simple and efficient library that supports serialization, 
// deserialization, MessagePack, streams, filtering, when using JSON strings.
//****************************************************************************************
#include <ArduinoJson.h>

//****************************************************************************************
//
// Include the WiFi library that supports connecting the ESP32 to the internet.  It 
// provides functionalities such as creating servers and clients, and managing network 
// connections. 
//
//****************************************************************************************
#include <WiFi.h>

//****************************************************************************************
//
// Include the HTTP libraries that support networking and WEB addressing.  The HTTPClient 
// is specifically designed for the ESP32.  It makes it easier to interact with web 
// servers by facilitating HTTP requests and responses.
//
//****************************************************************************************
#include <HTTPClient.h>
// Store the network credentials.
const char* ssid = "Insert Yours";
const char* password = "Insert Yours";

//****************************************************************************************
//
//                 Setting up the Touchscreen: Random Nerd Tutorial Example
//
//****************************************************************************************
// Install the "XPT2046_Touchscreen" library by Paul Stoffregen to use the Touchscreen -
// https://github.com/PaulStoffregen/XPT2046_Touchscreen 
// - Note: this library doesn't require further configuration
#include <XPT2046_Touchscreen.h>
// Define the touchscreen pins
#define XPT2046_IRQ  36  // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK  25  // T_CLK
#define XPT2046_CS   33  // T_CS
// Define the Light Dependent Resistor pin
#define LDR_PIN      34  // LDR pin
float  ldr_value;
String ldr_value_str;
//Create a touchscreenSPI and touchscreen instances:
SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);
// Declare the touchscreen coordinates: (x, y) and pressure (z)
int x, y, z;
// Define the touchscreen size in pixels
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
//
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

//****************************************************************************************
//
//            Setting up BME280 with the CYD: Random Nerd Tutorial Example
//
//****************************************************************************************
//Install the Adafruit BME library and its dependencies.
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
// Define I2C Communication pins for the BME_280
#define I2C_SDA  27  // Inter-Integrated Circuit (I2C) Serial Data Line
#define I2C_SCL  22  // Inter-Integrated Circuit (I2C) Serial Clock Line
// Temperature Units Celsius "C" or Fahrenheit "F"
#define Units "F"
// Create communication and BME instances.
TwoWire I2CBME = TwoWire(0);
Adafruit_BME280 bme;

////////////////////////*****Done with Libraties and Declaring IO*****////////////////////
////////////////////////*****Done with Libraties and Declaring IO*****////////////////////
////////////////////////*****Done with Libraties and Declaring IO*****////////////////////

//&&&&&&&&&&&&&&&&&&&&&&&&&&&**** Declaring Variables*****&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&**** Declaring Variables*****&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&**** Declaring Variables*****&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

//****************************************************************************************
//          Boolean Variables Used to Control Program Flow and Selectable Criteria
//****************************************************************************************
#define DEBUG 0                // Disable(0)/enable(1) serial output for debugging.
#define DISPLAY_COORD 0        // Disable(0)/enable(1) serial output of touch coordinates.
bool include_am_pm    = true;  // Are the meridiem suffix (am or pm) to be displayed.
bool pm               = false; // Used to display the ante and post merideim (am or pm)
bool display_seconds  = false; // Used to display seconds on the display.
bool date_display_MDY = true;  // true (May 21, 2025); false 2025-05-21
// Although the following booleans are intialized to false, they will be changed
// during the program flow base on upon time.
bool sync_time_date   = false; // Used to sync time to the WorldTimeAPI WEB site
bool sync_weather     = false; // Used to sync weather to the CSU weather station

//****************************************************************************************
//
//                If logging is enabled, it will inform the user about 
//               what is happening in the library: Random Nerd Tutorials
//
//****************************************************************************************
void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}// End of void log_print(lv_log_level_t level, const char * buf) {

//****************************************************************************************
//
//                                Switch Screen Button
//
//  The screen button is used on each of the screens to label the current screen and to 
//  switch to the next screen.  The text display on the button changes to describe the
//  screen currently displayed.
//
//****************************************************************************************
static lv_obj_t * button_switch_screen;              // Button object
static lv_style_t style_button_switch_screen;        // Button style
static lv_obj_t * text_label_button_switch_screen;   // Button text object
// Used to control which screen is loaded to the display. Range 1 - 3
int button_switch_screen_page_num = 1;

//****************************************************************************************
//
//                           Creating an Object for each Screen
//
//  These objects contain the LVGL code for each of the screens.
//
//****************************************************************************************
static lv_obj_t * general_screen;
static lv_obj_t * weather_screen;
static lv_obj_t * thermostat_screen;

//****************************************************************************************
//
//                         General Information Screen Variables
//
//  The information displayed on this screen is downloaded from two (2) web sites that
//  provide JSON docs.  These docs are deserialized first and then stored in variables
//  that will ultimately end up as objects that are displayed on the screen.
//
//****************************************************************************************
// Specify the timezone you want to get the time for: https://worldtimeapi.org/api/timezone
const char* timezone = "America/Denver";
// Days of the week array
char daysOfTheWeek[][10]     = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", 
                                "Friday", "Saturday"};
// Abreviated months of the year array
char abrMonthsOfTheYear[][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", 
                                "Aug", "Sep", "Oct", "Nov", "Dec"};

//  String variables to store the deserialized information from the JSON doc
String current_date;  //ex: 2025-04-16
String current_time;  //ex: 13:55:43
String current_month; //ex: 04
String current_day;   //ex: 16
String current_year;  //ex: 2025

// Variables that store integer conversions of the deserialzed JSON doc strings 
static int32_t day;    //range: 0-6 for Sunday through Saturday
static int32_t hour;   //range: 0-12
static int32_t minute; //range: 0-59
static int32_t second; //range: 0-59
static int32_t date_month;//range: 1-12
static int32_t date_day;  //range: 1-31
static int32_t date_year; //ex:2025

// These string variables are used for displaying the LVGL text; they
// contain units or additional text prefixes and suffices that weren't 
// part of the JSON deserialized strings
String final_date_str;
String final_time_str;

// Create the global LVGL text objects for each line displayed on the screen
static lv_obj_t * text_label_day;  // ex: Monday
static lv_obj_t * text_label_date; // ex. Apr 25, 2025
static lv_obj_t * text_label_time; // ex. 2:21 p.m.
static lv_obj_t * text_label_temp; // ex. 43.3 °F

//****************************************************************************************
//
//                           CSU Weather Station Screen Variables
//
//  The Colorado State University (CSU) campus is close to where I live and it has a 
//  weather station that provides a JSON document on their WEB site.  The information
//  displayed on this screen is downloaded from there.  A similar weather station may
//  be available near you.
//
//  These JSON doc from the CSU site is deserialized first and then stored in variables
//  that will ultimately end up as objects that are displayed on the screen.
//
//****************************************************************************************
// Variable for storing the deserialized JSON doc
float tempF;                   //Temperature in °F 
char tempF_str[7];             //Variable that holds the digital to string conversion

float relative_humidity;       //Relative humidity %
char relative_humidity_str[7]; //Variable that holds the digital to string converstion

float wind_mph;                //Wind speed in miles per hour (MPH)
char wind_mph_str[7];          //Variable that holds the digital to string converstion

// Create the global LVGL text objects for each line displayed on the screen
static lv_obj_t * text_label_outside_temp;
static lv_obj_t * text_label_outside_rhum;
static lv_obj_t * text_label_outside_wind;

//****************************************************************************************
//
//                             Thermostat Screen Variables
//
//****************************************************************************************

//
//------------------------- Inside Temperature, Humidity, Pressure------------------------
//

// Rectangle for the inside readings
static lv_obj_t * rectangle;

// Variable to store the BME280 sensor's temperature reading.
float  bme_temp = 0;
// String of the temperature reading.
String bme_temp_str;
// Label for displaying the inside temperature string.
static lv_obj_t * text_label_inside_temp;

// Variable to store the BME280 sensor's humidity reading.
float  bme_humd = 0;
// String of the humidity reading.
String bme_humd_str;
// Label for displaying the inside humidity string.
static lv_obj_t * text_label_inside_humd;

// Variable to store the BME280 sensor's barometric pressure reading.
float  bme_pres = 0;
// String of the humidity reading.
String bme_pres_str;
// Label for displaying the inside barometric pressure reading.
static lv_obj_t * text_label_inside_pres;

//
//---------------------------- Temperature Setpoint Variables-----------------------------
//

// Rectangle for the temperature setpoint and the increase and decrease buttons
static lv_obj_t * temp_sp_rectangle;

// Default temperature setpoint to 72.
float temp_setting = 72.0;

// Maximum limit for temperature setpoint when heating.
float max_temp_setting_when_heating = 76.0;
// Minimum limit for temperature setpoint when heating.
float min_temp_setting_when_heating = 55.0;
// Maximum limit for temperature setpoint when cooling.
float max_temp_setting_when_cooling = 80.0;
// Minimum limit for temperature setpoint when cooling.
float min_temp_setting_when_cooling = 70.0;
// State variable for minimum temperatre setpoint reached
bool min_temp_sp = false;
// State variable for maximum temperatre setpoint reached
bool max_temp_sp = false;

// Creat a label to display the current temperature setpoint.
static lv_obj_t * text_label_temp_sp;

// Create a button object for increasing temperature setting.
static lv_obj_t * button_incr_temp;
// Create a text object for increasing temperature button.
static lv_obj_t * text_label_button_incr_temp;
// Create style the increase button.
static lv_style_t style_button_incr_temp;

// Create a button object for decreasing temperature setting.
static lv_obj_t * button_decr_temp;
// Create a text object for decreasing temperature button.
static lv_obj_t * text_label_button_decr_temp;
// Create style the decrease button.
static lv_style_t style_button_decr_temp;

//
//---------------------------------Temperature Mode Select--------------------------------
//

// Rectangle for the temperature control mode buttons.
static lv_obj_t * mode_sel_rectangle;

// Create a button objects for selecting each mode: cool, off, heat.
static lv_obj_t * button_sel_cool;
static lv_obj_t * button_sel_off;
static lv_obj_t * button_sel_heat;

// Create a style for the actual buttons; not the text label.
// This will enable color and border style changes to the button.
static lv_style_t style_button_sel_cool;
static lv_style_t style_button_sel_off;
static lv_style_t style_button_sel_heat;

// Create the text objects for the button labels.
static lv_obj_t * text_label_button_sel_cool;
static lv_obj_t * text_label_button_sel_off;
static lv_obj_t * text_label_button_sel_heat;

// Create the variables for storing the mode selected.
enum Temperature_Control_Mode {cool = 1, off, heat};
Temperature_Control_Mode   temp_mode = heat;

//
//---------------------------------Fan Mode Select--------------------------------
//

// Rectangle for the fan control mode buttons; Fan Auto and Fan On
static lv_obj_t * fan_control_sel_rectangle;

// Create a button objects for selecting each mode: auto, on.
static lv_obj_t * button_sel_fan_auto;
static lv_obj_t * button_sel_fan_on;

// Create a style for the actual buttons; not the text label.
// This will enable color and border style changes to the button
// when selected and deselected.
static lv_style_t style_button_sel_fan_auto;
static lv_style_t style_button_sel_fan_on;

// Create the text objects for the button labels.
static lv_obj_t * text_label_button_sel_fan_auto;
static lv_obj_t * text_label_button_sel_fan_on;

// Create the variables for storing the mode selected.
enum Fan_Control_Mode {fan_auto = 1, fan_on};
// Default fan mode.
Fan_Control_Mode  fan_mode = fan_auto;

//&&&&&&&&&&&&&&&&&&&&&&&&&&**** Done Declaring Variables*****&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&**** Done Declaring Variables*****&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&**** Done Declaring Variables*****&&&&&&&&&&&&&&&&&&&&&&&&&&&&

/////////////////////////////*****Start of Subroutines*****///////////////////////////////
/////////////////////////////*****Start of Subroutines*****///////////////////////////////
/////////////////////////////*****Start of Subroutines*****///////////////////////////////

//******************************************************************************************************
//******************************************************************************************************
// *****                        Get the Touch Screen Data: Random Nerd                             *****
//******************************************************************************************************
//******************************************************************************************************
void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data) {
  // Checks if Touchscreen was touched, and prints X, Y and Pressure (Z)
  if(touchscreen.tirqTouched() && touchscreen.touched()) {
    // Get Touchscreen points
    TS_Point p = touchscreen.getPoint();
    // Calibrate Touchscreen points with map function to the correct width and height
    x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    z = p.z;

    data->state = LV_INDEV_STATE_PRESSED;

    // Set the coordinates
    data->point.x = x;
    data->point.y = y;

    // Print Touchscreen info about X, Y and Pressure (Z) on the Serial Monitor
    #if DISPLAY_COORD
      Serial.print("X = ");
      Serial.print(x);
      Serial.print(" | Y = ");
      Serial.print(y);
      Serial.print(" | Pressure = ");
      Serial.print(z);
      Serial.println();
    #endif
  }
  else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}// End of void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data) {

//******************************************************************************************************
//******************************************************************************************************
//*****                           void format_switch_screen_button(void)                           *****
//*****                                                                                            *****
//*****  This subroutine provides the structure for the common button used to switch screens.      *****
//*****                                                                                            *****
//*****   (a) It is called by each of the subroutines that create the General Information,         *****
//*****       CSU Weather Station, and Thermostate screens.                                        *****
//*****   (b) Any attributes that are specific to any one screen are set up in the code that       *****
//*****       creates the screen.                                                                  *****
//*****                                                                                            *****
//******************************************************************************************************
//******************************************************************************************************
void format_switch_screen_button(void)
{
  //**************************************************************************************
  //                       Format - Button to Switch Between Screens
  //**************************************************************************************
  lv_obj_add_event_cb(button_switch_screen, event_handler_button_switch_screen, LV_EVENT_CLICKED, NULL);
  // Assign its position.
  lv_obj_align(button_switch_screen, LV_ALIGN_CENTER, 0, -100);
  // Size the button to the text.
  lv_obj_set_height(button_switch_screen, LV_SIZE_CONTENT);
  // Adding style to the button
  lv_style_init(&style_button_switch_screen);
  lv_style_set_bg_color(&style_button_switch_screen, lv_color_hex(0x00B0FF));
  lv_style_set_bg_opa(&style_button_switch_screen, LV_OPA_100);
  lv_style_set_border_width(&style_button_switch_screen, 2);
  lv_style_set_border_color(&style_button_switch_screen, lv_color_hex(0x000000));
  lv_obj_add_style(button_switch_screen, &style_button_switch_screen, 0); 
  //--------------------------------------------------------------------------------------
  // Put text in the button
  //--------------------------------------------------------------------------------------
  // Create the text label to be included on the active button 1; not the active screen.
  text_label_button_switch_screen = lv_label_create(button_switch_screen);
  // Create a style for the text label on button_switch_screen.
  static lv_style_t style_text_label_button_switch_screen;
  // Create styles for the text inside the button and add them to the button.
  lv_style_set_text_color(&style_text_label_button_switch_screen, lv_color_hex(0x000000)); // color
  lv_style_set_text_font (&style_text_label_button_switch_screen, &lv_font_montserrat_18); // font
  lv_obj_add_style(text_label_button_switch_screen, &style_text_label_button_switch_screen, 0); 
  // Align the text to the center of button_switch_screen
  lv_obj_center(text_label_button_switch_screen);
}// End of void format_switch_screen_button(void){
 
//******************************************************************************************************
//******************************************************************************************************
//*****                              void create_general_screen(void)                              *****
//*****                                                                                            *****
//*****  This subroutine provides the structure for the General Information interface.             *****
//*****                                                                                            *****
//*****   (a) Subroutine lv_create_main_gui(void) calls this subroutine to create GUI.             *****
//*****   (b) Subroutine void timer_cb(lv_timer_t * timer), loads the screen to the display when   *****
//*****       the variabe button_switch_screen_page_num = 1.                                       *****
//*****                                                                                            *****
//******************************************************************************************************
//******************************************************************************************************
void create_general_screen(void)
{
  // Create the general screen object.
  general_screen = lv_obj_create(NULL);
  // Set the background color.
  lv_obj_set_style_bg_color(general_screen, lv_color_hex(0xFF8F00), LV_PART_MAIN);

  //**************************************************************************************
  //               General Information Screen - Display Day of the Week
  //**************************************************************************************
  // Create a day of the week label and add it to the General screen.
  text_label_day  = lv_label_create(general_screen); 
  // Align the day of the week label with pixel offsets x, y.
  lv_obj_align(text_label_day,  LV_ALIGN_CENTER, 0, -60);
  // To style the text create an object of type lv_style_t, this kind of object is used 
  // to apply styles to LVGL objects.
  static lv_style_t style_text_label_day;
  // Initialize the new style.
  lv_style_init(&style_text_label_day);
  // Set the style font type and size.
  lv_style_set_text_font(&style_text_label_day,  &lv_font_montserrat_44);
  // https://vuetifyjs.com/en/styles/colors/#material-colors
  // Set the style text color.
  lv_style_set_text_color(&style_text_label_day,  lv_color_hex(0x000000));
  // Now add the style to the object.
  lv_obj_add_style(text_label_day,  &style_text_label_day, 0); 

  //**************************************************************************************
  //                     General Information Screen - Display Date
  //**************************************************************************************
  // Create a date label and add it to the General screen.
  text_label_date  = lv_label_create(general_screen); 
  // Align the date label with pixel offsets x, y.
  lv_obj_align(text_label_date,  LV_ALIGN_CENTER, 0, -10);
  // To style the text create an object of type lv_style_t, this kind of object is used 
  // to apply styles to LVGL objects.
  static lv_style_t style_text_label_date;
  // Initialize the new style.
  lv_style_init(&style_text_label_date);
  // Set the style font type and size.
  lv_style_set_text_font(&style_text_label_date,  &lv_font_montserrat_44);
  // https://vuetifyjs.com/en/styles/colors/#material-colors
  // Set the style text color.
  lv_style_set_text_color(&style_text_label_date,  lv_color_hex(0x000000));
  // Now add the style to the object.
  lv_obj_add_style(text_label_date,  &style_text_label_date, 0); 

  //**************************************************************************************
  //                    General Information Screen - Display Time
  //**************************************************************************************
  // Create a time label and add it to the General screen.
  text_label_time  = lv_label_create(general_screen); 
  // Align the time label with pixel offsets x, y.
  lv_obj_align(text_label_time,  LV_ALIGN_CENTER, 0, 40);
  // To style the text create an object of type lv_style_t, this kind of object is used 
  // to apply styles to LVGL objects.
  static lv_style_t style_text_label_time;
  // Initialize the new style.
  lv_style_init(&style_text_label_time);
  // Set the style font type and size.
  lv_style_set_text_font(&style_text_label_time,  &lv_font_montserrat_44);
  // https://vuetifyjs.com/en/styles/colors/#material-colors
  // Set the style text color.
  lv_style_set_text_color(&style_text_label_time,  lv_color_hex(0x000000));
  // Now add the style to the object.
  lv_obj_add_style(text_label_time,  &style_text_label_time, 0); 

  //**************************************************************************************
  //                 General Information Screen - Display Outside Temperature
  //**************************************************************************************
  // Create a outside temperature label and add it to the General screen.
  text_label_temp  = lv_label_create(general_screen); 
  // Align the temperature label with pixel offsets x, y.  See note above.
  lv_obj_align(text_label_temp,  LV_ALIGN_CENTER, 0, 90);
  // To style the text create an object of type lv_style_t, this kind of object is used 
  // to apply styles to LVGL objects.
  static lv_style_t style_text_label_temp;
  // Initialize the new style.
  lv_style_init(&style_text_label_temp);
  // Set the style font type and size.
  lv_style_set_text_font(&style_text_label_temp,  &lv_font_montserrat_44);
  // https://vuetifyjs.com/en/styles/colors/#material-colors
  // Set the style text color.
  lv_style_set_text_color(&style_text_label_temp,  lv_color_hex(0x000000));
  // Now add the style to the object.
  lv_obj_add_style(text_label_temp,  &style_text_label_temp, 0); 

  //**************************************************************************************
  //             General Information Screen - Button to Switch Between Screens
  //**************************************************************************************
  button_switch_screen = lv_button_create(general_screen);
  format_switch_screen_button();
  //Switch Screen Button: Set the button label text.
  lv_label_set_text(text_label_button_switch_screen, "General Information");
}// End of void create_general_screen(void)

//******************************************************************************************************
//******************************************************************************************************
//*****                           void create_weather_screen(void)                                 *****
//*****                                                                                            *****
//*****  This subroutine provides the structure for the CSU Weather Station screen.                *****
//*****                                                                                            *****
//*****   (a) Subroutine lv_create_main_gui(void) calls this subroutine to create GUI.             *****
//*****   (b) Subroutine void timer_cb(lv_timer_t * timer), loads the screen to the display when   *****
//*****       the variabe button_switch_screen_page_num = 2.                                       *****
//*****                                                                                            *****
//******************************************************************************************************
//******************************************************************************************************
void create_weather_screen(void)
{ 
  // Create the weather screen obj
  weather_screen = lv_obj_create(NULL);
  // Set the background screen color.
  lv_obj_set_style_bg_color(weather_screen, lv_color_hex(0xFF8F00), LV_PART_MAIN);
  //**************************************************************************************
  //                    Weather Screen - Display Outside Temperature
  //**************************************************************************************
  // Create an outside temperature label and add it to the Weather screen.
  text_label_outside_temp  = lv_label_create(weather_screen); 
  // Align the temperature label with pixel offsets x, y.  See note above.
  lv_obj_align(text_label_outside_temp,  LV_ALIGN_LEFT_MID, 0, -50);
  // To style the text create an object of type lv_style_t, this kind of object is used 
  // to apply styles to LVGL objects.
  static lv_style_t style_text_label_outside_temp;
  // Initialize the new style.
  lv_style_init(&style_text_label_outside_temp);
  // Set the style font type and size.
  lv_style_set_text_font(&style_text_label_outside_temp,  &lv_font_montserrat_48);
  // https://vuetifyjs.com/en/styles/colors/#material-colors
  // Set the style text color.
  lv_style_set_text_color(&style_text_label_outside_temp,  lv_color_hex(0x000000));
  // Now add the style to the object.
  lv_obj_add_style(text_label_outside_temp,  &style_text_label_outside_temp, 0); 

  //**************************************************************************************
  //                      Weather Screen - Display Relative Humidity
  //**************************************************************************************
  // Create an outside humidity label and add it to the Weather screen.
  text_label_outside_rhum  = lv_label_create(weather_screen); 
  // Align the relative humidity label with pixel offsets x, y.
  lv_obj_align(text_label_outside_rhum,  LV_ALIGN_LEFT_MID, 0, 0);
  // To style the text create an object of type lv_style_t, this kind of object is used 
  // to apply styles to LVGL objects.
  static lv_style_t style_text_label_outside_rhum;
  // Initialize the new style.
  lv_style_init(&style_text_label_outside_rhum);
  // Set the style font type and size.
  lv_style_set_text_font(&style_text_label_outside_rhum,  &lv_font_montserrat_48);
  // https://vuetifyjs.com/en/styles/colors/#material-colors
  // Set the style text color.
  lv_style_set_text_color(&style_text_label_outside_rhum,  lv_color_hex(0x000000));
  // Now add the style to the object.
  lv_obj_add_style(text_label_outside_rhum,  &style_text_label_outside_rhum, 0); 

  //**************************************************************************************
  //                        Weather Screen - Display Wind Speed
  //**************************************************************************************
  // Create an outside wind speed label and add it to the Weather screen.
  text_label_outside_wind  = lv_label_create(weather_screen); 
  // Align the wind speed label with pixel offsets x, y.
  lv_obj_align(text_label_outside_wind,  LV_ALIGN_LEFT_MID, 0, 50);
  // To style the text create an object of type lv_style_t, this kind of object is used 
  // to apply styles to LVGL objects.
  static lv_style_t style_text_label_outside_wind;
  // Initialize the new style.
  lv_style_init(&style_text_label_outside_wind);
  // Set the style font type and size.
  lv_style_set_text_font(&style_text_label_outside_wind,  &lv_font_montserrat_48);
  // https://vuetifyjs.com/en/styles/colors/#material-colors
  // Set the style text color.
  lv_style_set_text_color(&style_text_label_outside_wind,  lv_color_hex(0x000000));
  // Now add the style to the object.
  lv_obj_add_style(text_label_outside_wind,  &style_text_label_outside_wind, 0); 

  //**************************************************************************************
  //                   Weather Screen - Button to Switch Between Screens
  //**************************************************************************************
  button_switch_screen = lv_button_create(weather_screen);
  format_switch_screen_button();
  //Switch Screen Button: Set the button label text.
  lv_label_set_text(text_label_button_switch_screen, "CSU Weather Station");
}// End of void create_weather_screen(void)



//******************************************************************************************************
//******************************************************************************************************
//*****                        void create_thermostat_screen(void)                                 *****
//*****                                                                                            *****
//*****  This subroutine provides the structure for the Thermostat interface.                      *****
//*****                                                                                            *****
//*****   (a) Subroutine lv_create_main_gui(void) calls this subroutine to create GUI.             *****
//*****   (b) Subroutine void timer_cb(lv_timer_t * timer), loads the screen to the display when   *****
//*****       the variabe button_switch_screen_page_num = 1.                                       *****
//*****                                                                                            *****
//******************************************************************************************************
//******************************************************************************************************
void create_thermostat_screen(void)
{
  // Create the general screen object.
  thermostat_screen = lv_obj_create(NULL);
  // Set the background screen color.
  lv_obj_set_style_bg_color(thermostat_screen, lv_color_hex(0xFF8F00), LV_PART_MAIN);

  //**************************************************************************************
  //          Thermostat Screen - Rectangle for Inside Temperature and Humidity
  //**************************************************************************************
  // Add the rectangle to the active display.
  rectangle = lv_button_create(thermostat_screen);
  // Set the rectangle's size
  lv_obj_set_size(rectangle, 105, 190);
  // Align the center of the rectangle at the pixel offset.
  lv_obj_align(rectangle,  LV_ALIGN_CENTER, -106, 20);
  // To style the text create an object of type lv_style_t, this kind of object is used 
  // to apply styles to LVGL objects.
  static lv_style_t style_rectangle;
  // Initialize the new style.
  lv_style_init(&style_rectangle);
  // Add the style to the rectangle object.
  lv_style_set_bg_color(&style_rectangle, lv_color_hex(0x76FF03));
  lv_style_set_bg_opa(&style_rectangle, LV_OPA_50);
  lv_style_set_border_width(&style_rectangle, 3);
  lv_style_set_border_color(&style_rectangle, lv_color_hex(0x000000));
  lv_obj_add_style(rectangle, &style_rectangle, 0);

  //**************************************************************************************
  //                   Thermostat Screen - Display Inside Temperature
  //**************************************************************************************
  // Create an inside temperature label and add it to the rectangle. 
  text_label_inside_temp  = lv_label_create(rectangle); 
  // Align the inside temperature label with pixel offsets x, y.
  lv_obj_align(text_label_inside_temp,  LV_ALIGN_TOP_LEFT, -5, -5);
  // To style the text create an object of type lv_style_t, this kind of object is used 
  // to apply styles to LVGL objects.
  static lv_style_t style_text_label_inside_temp;
  // Initialize the new style.
  lv_style_init(&style_text_label_inside_temp);
  // Set the style font type and size.
  lv_style_set_text_font(&style_text_label_inside_temp,  &lv_font_montserrat_24);
  // https://vuetifyjs.com/en/styles/colors/#material-colors
  // Set the style text color.
  lv_style_set_text_color(&style_text_label_inside_temp,  lv_color_hex(0x000000));
  // Now add the style to the object.
  lv_obj_add_style(text_label_inside_temp,  &style_text_label_inside_temp, 0); 

  //**************************************************************************************
  //                   Thermostat Screen - Display Inside Humidity
  //**************************************************************************************
  // Create an inside humidity label and add it to the rectangle. 
  text_label_inside_humd  = lv_label_create(rectangle); 
  // Align the inside humidity label with pixel offsets x, y.
   lv_obj_align(text_label_inside_humd,  LV_ALIGN_BOTTOM_LEFT, -5, 5);
  // To style the text create an object of type lv_style_t, this kind of object is used 
  // to apply styles to LVGL objects.
  static lv_style_t style_text_label_inside_humd;
  // Initialize the new style.
  lv_style_init(&style_text_label_inside_humd);
  // Set the style font type and size.
  lv_style_set_text_font(&style_text_label_inside_humd,  &lv_font_montserrat_24);
  // https://vuetifyjs.com/en/styles/colors/#material-colors
  // Set the style text color.
  lv_style_set_text_color(&style_text_label_inside_humd,  lv_color_hex(0x000000));
  // Now add the style to the object.
  lv_obj_add_style(text_label_inside_humd,  &style_text_label_inside_humd, 0); 

  //**************************************************************************************
  //                Thermostat Screen - Button to Switch Between Screens
  //**************************************************************************************
  button_switch_screen = lv_button_create(thermostat_screen);
  format_switch_screen_button();
  //Switch Screen Button: Set the button label text.
  lv_label_set_text(text_label_button_switch_screen, "Thermostat");

  //**************************************************************************************
  //      Thermostat Screen - Rectangle for Temperature Setpoint Adjustment (+,-)
  //**************************************************************************************
  // Add the rectangle to the active display.
  temp_sp_rectangle = lv_button_create(thermostat_screen);
  // Set the rectangle's size
  lv_obj_set_size(temp_sp_rectangle, 210, 50);
  // Align the center of the rectangle at the pixel offset.
  lv_obj_align(temp_sp_rectangle,  LV_ALIGN_CENTER, 55, -50);
  // Set the color of the rectangle.
  lv_obj_set_style_bg_color(temp_sp_rectangle , lv_color_hex(0xFFFFFF), 0);
  // To style the text create an object of type lv_style_t, this kind of object is used 
  // to apply styles to LVGL objects.
  static lv_style_t style_temp_sp_rectangle;
  // Initialize the new style.
  lv_style_init(&style_temp_sp_rectangle);
  // Add the style to the rectangle object.
  lv_obj_add_style(temp_sp_rectangle, &style_temp_sp_rectangle, 0);

  //**************************************************************************************
  //                 Thermostat Screen - Display Temperature Setpoint
  //**************************************************************************************
  // Create an inside temperature label and add it to the rectangle. 
  text_label_temp_sp  = lv_label_create(temp_sp_rectangle); 
  // Align the inside temperature label with pixel offsets x, y.
  lv_obj_align(text_label_temp_sp,  LV_ALIGN_CENTER, 0, 0);
  // To style the text create an object of type lv_style_t, this kind of object is used 
  // to apply styles to LVGL objects.
  static lv_style_t style_text_label_temp_sp;
  // Initialize the new style.
  lv_style_init(&style_text_label_temp_sp);
  // Set the style font type and size.
  lv_style_set_text_font(&style_text_label_temp_sp,  &lv_font_montserrat_36);
  // https://vuetifyjs.com/en/styles/colors/#material-colors
  // Set the style text color.
  lv_style_set_text_color(&style_text_label_temp_sp,  lv_color_hex(0x000000));
  // Now add the style to the object.
  lv_obj_add_style(text_label_temp_sp,  &style_text_label_temp_sp, 0); 

  //Set pixel increase for the clickable size of the increase and decrease temperature
  //set point buttons.
  int32_t click_size = 40;
  //**************************************************************************************
  //              Thermostat Screen - Button to Increase Temperature Setpoint
  //**************************************************************************************
  // Add it to the temperature setpoint rectangle.
  button_incr_temp = lv_button_create(temp_sp_rectangle);
  // Assign its event handler.
  lv_obj_add_event_cb(button_incr_temp, event_handler_button_incr_temp, LV_EVENT_CLICKED, NULL);
  // Increase its clickable size
  lv_obj_set_ext_click_area(button_incr_temp, click_size);
  // Assign its position.
  lv_obj_align(button_incr_temp, LV_ALIGN_CENTER, 75, 0);
  // Size the button to the text.
  lv_obj_set_height(button_incr_temp, LV_SIZE_CONTENT);
  // Adding style to the button
  lv_style_init(&style_button_incr_temp);
  lv_style_set_bg_color(&style_button_incr_temp, lv_color_hex(0x757575));
  lv_style_set_bg_opa(&style_button_incr_temp, LV_OPA_50);
  lv_style_set_border_width(&style_button_incr_temp, 6);
  lv_style_set_border_color(&style_button_incr_temp, lv_color_hex(0x000000));
  lv_obj_add_style(button_incr_temp, &style_button_incr_temp, 0); 
  //--------------------------------------------------------------------------------------
  // Put text in the button
  //--------------------------------------------------------------------------------------
  // Create the text label to be included on the active button 1; not the active screen.
  text_label_button_incr_temp = lv_label_create(button_incr_temp);
  // Create a style for the text label on button_incr_temp.
  static lv_style_t style_text_label_button_incr_temp;
  // Create styles for the text inside the button and add them to the button.
  lv_style_set_text_color(&style_text_label_button_incr_temp, lv_color_hex(0x000000)); // color
  lv_style_set_text_font (&style_text_label_button_incr_temp, &lv_font_montserrat_44); // font
  lv_obj_add_style(text_label_button_incr_temp, &style_text_label_button_incr_temp, 0); 
  // Align the text to the center of button_incr_temp
  lv_obj_center(text_label_button_incr_temp);
  //Set the button label to increase temp.
  lv_label_set_text(text_label_button_incr_temp, "+");

  //**************************************************************************************
  //           Thermostat Screen - Button to Decrease Temperature Setpoint
  //**************************************************************************************
  // Add it to the temperature setpoint rectangle.
  button_decr_temp = lv_button_create(temp_sp_rectangle);
  // Assign its event handler.
  lv_obj_add_event_cb(button_decr_temp, event_handler_button_decr_temp, LV_EVENT_CLICKED, NULL);
  // Increase its clickable size
  lv_obj_set_ext_click_area(button_decr_temp, click_size);
  // Assign its position.
  //lv_obj_align(button_decr_temp, LV_ALIGN_LEFT_MID, 0, 0);
  lv_obj_align(button_decr_temp, LV_ALIGN_CENTER, -75, 0);
  // Size the button to the text.
  lv_obj_set_height(button_decr_temp, LV_SIZE_CONTENT);
  // Adding style to the button
  lv_style_init(&style_button_decr_temp);
  lv_style_set_bg_color(&style_button_decr_temp, lv_color_hex(0x757575));
  lv_style_set_bg_opa(&style_button_decr_temp, LV_OPA_50);
  lv_style_set_border_width(&style_button_decr_temp, 6);
  lv_style_set_border_color(&style_button_decr_temp, lv_color_hex(0x000000));
  lv_obj_add_style(button_decr_temp, &style_button_decr_temp, 0); 
  //--------------------------------------------------------------------------------------
  // Put text in the button
  //--------------------------------------------------------------------------------------
  // Create the text label to be included on the active button 1; not the active screen.
  text_label_button_decr_temp = lv_label_create(button_decr_temp);
  // Create a style for the text label on button_incr_temp.
  static lv_style_t style_text_label_button_decr_temp;
  // Create styles for the text inside the button and add them to the button.
  lv_style_set_text_color(&style_text_label_button_decr_temp, lv_color_hex(0x000000)); // color
  lv_style_set_text_font (&style_text_label_button_decr_temp, &lv_font_montserrat_44); // font
  lv_obj_add_style(text_label_button_decr_temp, &style_text_label_button_decr_temp, 0); 
  // Align the text to the center of button_incr_temp
  lv_obj_center(text_label_button_decr_temp);
  lv_obj_set_style_pad_left (button_decr_temp, 18, LV_PART_MAIN);
  lv_obj_set_style_pad_right(button_decr_temp, 18, LV_PART_MAIN);
  //Set the button label to switch to decrease temp.
  lv_label_set_text(text_label_button_decr_temp, "-");

  //**************************************************************************************
  //       Thermostat Screen - Rectangle for Mode Select (Cool, Off, Heat)
  //**************************************************************************************
  // Add the mode select rectangle to the thermostat display.
  mode_sel_rectangle = lv_button_create(thermostat_screen);
  // Set the rectangle's sizes
  lv_obj_set_size(mode_sel_rectangle, 210, 50);
  // Align the center of the rectangle at the pixel offset.
  lv_obj_align(mode_sel_rectangle,  LV_ALIGN_CENTER, 55, 20);
  // Set the color of the rectangle.
  lv_obj_set_style_bg_color(mode_sel_rectangle , lv_color_hex(0xFFFFFF), 0);
  // To style the text create an object of type lv_style_t, this kind of object is used 
  // to apply styles to LVGL objects.
  static lv_style_t style_mode_sel_rectangle;
  // Initialize the new style.
  lv_style_init(&style_mode_sel_rectangle);
  // Add the style to the rectangle object.
  lv_obj_add_style(mode_sel_rectangle, &style_mode_sel_rectangle, 0);

  //**************************************************************************************
  //             Thermostat Screen - Button to Select Air Conditioning (Cool)
  //**************************************************************************************
  // Add it to the mode select rectangle.
  button_sel_cool = lv_button_create(mode_sel_rectangle);
  // Assign its event handler.
  lv_obj_add_event_cb(button_sel_cool, event_handler_button_sel_cool, LV_EVENT_CLICKED, NULL);
  // Assign its position.
  lv_obj_align(button_sel_cool, LV_ALIGN_CENTER, -70, 0);
  // Size the button to the text.
  lv_obj_set_height(button_sel_cool, LV_SIZE_CONTENT);
  // Adding style to the button
  lv_style_init(&style_button_sel_cool);
  lv_style_set_bg_color(&style_button_sel_cool, lv_color_hex(0x757575));
  lv_style_set_bg_opa(&style_button_sel_cool, LV_OPA_50);
  lv_style_set_border_width(&style_button_sel_cool, 6);
  lv_style_set_border_color(&style_button_sel_cool, lv_color_hex(0x000000));
  lv_obj_add_style(button_sel_cool, &style_button_sel_cool, 0); 
  //--------------------------------------------------------------------------------------
  // Put text in the button
  //--------------------------------------------------------------------------------------
  // Create the text label to be included on the active button 1; not the active screen.
  text_label_button_sel_cool = lv_label_create(button_sel_cool);
  // Create a style for the text label on button_incr_temp.
  static lv_style_t style_text_label_button_sel_cool;
  // Create styles for the text inside the button and add them to the button.
  lv_style_set_text_color(&style_text_label_button_sel_cool, lv_color_hex(0x000000)); // color
  lv_style_set_text_font (&style_text_label_button_sel_cool, &lv_font_montserrat_16); // font
  lv_obj_add_style(text_label_button_sel_cool, &style_text_label_button_sel_cool, 0); 
  // Align the text to the center of button_incr_temp
  lv_obj_center(text_label_button_sel_cool);
  //Set the button label to increase temp.
  lv_label_set_text(text_label_button_sel_cool, "Cool");

  //**************************************************************************************
  //              Thermostat Screen - Button to Select Off
  //**************************************************************************************
  // Add it to the mode select rectangle.
  button_sel_off = lv_button_create(mode_sel_rectangle);
  // Assign its event handler.
  lv_obj_add_event_cb(button_sel_off, event_handler_button_sel_off, LV_EVENT_CLICKED, NULL);
  // Assign its position.
  lv_obj_align(button_sel_off, LV_ALIGN_CENTER, -5, 0);
  // Size the button to the text.
  lv_obj_set_height(button_sel_off, LV_SIZE_CONTENT);
  // Adding style to the button
  lv_style_init(&style_button_sel_off);
  lv_style_set_bg_color(&style_button_sel_off, lv_color_hex(0x757575));
  lv_style_set_bg_opa(&style_button_sel_off, LV_OPA_50);
  lv_style_set_border_width(&style_button_sel_off, 6);
  lv_style_set_border_color(&style_button_sel_off, lv_color_hex(0x000000));
  lv_obj_add_style(button_sel_off, &style_button_sel_off, 0); 
  //--------------------------------------------------------------------------------------
  // Put text in the button
  //--------------------------------------------------------------------------------------
  // Create the text label to be included on the active button 1; not the active screen.
  text_label_button_sel_off = lv_label_create(button_sel_off);
  // Create a style for the text label on button_incr_temp.
  static lv_style_t style_text_label_button_sel_off;
  // Create styles for the text inside the button and add them to the button.
  lv_style_set_text_color(&style_text_label_button_sel_off, lv_color_hex(0x000000)); // color
  lv_style_set_text_font (&style_text_label_button_sel_off, &lv_font_montserrat_16); // font
  lv_obj_add_style(text_label_button_sel_off, &style_text_label_button_sel_off, 0); 
  // Align the text to the center of button_incr_temp
  lv_obj_center(text_label_button_sel_off);
  //Set the button label to increase temp.
  lv_label_set_text(text_label_button_sel_off, "Off");

  //**************************************************************************************
  //              Thermostat Screen - Button to Select Heat
  //**************************************************************************************
  // Add it to the mode select rectangle.
  button_sel_heat = lv_button_create(mode_sel_rectangle);
  // Assign its event handler.
  lv_obj_add_event_cb(button_sel_heat, event_handler_button_sel_heat, LV_EVENT_CLICKED, NULL);
  // Assign its position.
  lv_obj_align(button_sel_heat, LV_ALIGN_CENTER, 65, 0);
  // Size the button to the text.
  lv_obj_set_height(button_sel_heat, LV_SIZE_CONTENT);
  // Adding style to the button
  lv_style_init(&style_button_sel_heat);
  lv_style_set_bg_color(&style_button_sel_heat, lv_color_hex(0x757575));
  lv_style_set_bg_opa(&style_button_sel_heat, LV_OPA_50);
  lv_style_set_border_width(&style_button_sel_heat, 6);
  lv_style_set_border_color(&style_button_sel_heat, lv_color_hex(0x000000));
  lv_obj_add_style(button_sel_heat, &style_button_sel_heat, 0); 
  //--------------------------------------------------------------------------------------
  // Put text in the button
  //--------------------------------------------------------------------------------------
  // Create the text label to be included on the active button 1; not the active screen.
  text_label_button_sel_heat = lv_label_create(button_sel_heat);
  // Create a style for the text label on button_incr_temp.
  static lv_style_t style_text_label_button_sel_heat;
  // Create styles for the text inside the button and add them to the button.
  lv_style_set_text_color(&style_text_label_button_sel_heat, lv_color_hex(0x000000)); // color
  lv_style_set_text_font (&style_text_label_button_sel_heat, &lv_font_montserrat_16); // font
  lv_obj_add_style(text_label_button_sel_heat, &style_text_label_button_sel_heat, 0); 
  // Align the text to the center of button_incr_temp
  lv_obj_center(text_label_button_sel_heat);
  //Set the button label to increase temp.
  lv_label_set_text(text_label_button_sel_heat, "Heat");

  //**************************************************************************************
  //       Thermostat Screen - Rectangle for Fan Control (Auto, On)
  //**************************************************************************************
  // Add the fan control rectangle to the active display.
  fan_control_sel_rectangle = lv_button_create(thermostat_screen);
  // Set the rectangle's size
  lv_obj_set_size(fan_control_sel_rectangle, 210, 50);
  // Align the center of the rectangle at the pixel offset.
  lv_obj_align(fan_control_sel_rectangle,  LV_ALIGN_CENTER, 55, 90);
  // Set the color of the rectangle.
  lv_obj_set_style_bg_color(fan_control_sel_rectangle , lv_color_hex(0xFFFFFF), 0);
  // To style the text create an object of type lv_style_t, this kind of object is used 
  // to apply styles to LVGL objects.
  static lv_style_t style_fan_control_sel_rectangle;
  // Initialize the new style.
  lv_style_init(&style_fan_control_sel_rectangle);
  // Add the style to the rectangle object.
  lv_obj_add_style(fan_control_sel_rectangle, &style_fan_control_sel_rectangle, 0);

  //**************************************************************************************
  //             Thermostat Screen - Button to Select Fan Auto
  //**************************************************************************************
  // Add it to the mode select rectangle.
  button_sel_fan_auto = lv_button_create(fan_control_sel_rectangle);
  // Assign its event handler.
  lv_obj_add_event_cb(button_sel_fan_auto, event_handler_button_sel_fan_auto, LV_EVENT_CLICKED, NULL);
  // Assign its position.
  lv_obj_align(button_sel_fan_auto, LV_ALIGN_CENTER, -50, 0);
  // Size the button to the text.
  lv_obj_set_height(button_sel_fan_auto, LV_SIZE_CONTENT);
  // Adding style to the button
  lv_style_init(&style_button_sel_fan_auto);
  lv_style_set_bg_color(&style_button_sel_fan_auto, lv_color_hex(0x757575));
  lv_style_set_bg_opa(&style_button_sel_fan_auto, LV_OPA_50);
  lv_style_set_border_width(&style_button_sel_fan_auto, 6);
  lv_style_set_border_color(&style_button_sel_fan_auto, lv_color_hex(0x000000));
  lv_obj_add_style(button_sel_fan_auto, &style_button_sel_fan_auto, 0); 
  //--------------------------------------------------------------------------------------
  // Put text in the button
  //--------------------------------------------------------------------------------------
  // Create the text label to be included on the active button 1; not the active screen.
  text_label_button_sel_fan_auto = lv_label_create(button_sel_fan_auto);
  // Create a style for the text label on button_incr_temp.
  static lv_style_t style_text_label_button_sel_fan_auto;
  // Create styles for the text inside the button and add them to the button.
  lv_style_set_text_color(&style_text_label_button_sel_fan_auto, lv_color_hex(0x000000)); // color
  lv_style_set_text_font (&style_text_label_button_sel_fan_auto, &lv_font_montserrat_18); // font
  lv_obj_add_style(text_label_button_sel_fan_auto, &style_text_label_button_sel_fan_auto, 0); 
  // Align the text to the center of button_incr_temp
  lv_obj_center(text_label_button_sel_fan_auto);
  //Set the button label to increase temp.
  lv_label_set_text(text_label_button_sel_fan_auto, "Fan Auto");

  //**************************************************************************************
  //             Thermostat Screen - Button to Select Fan On
  //**************************************************************************************
  // Add it to the mode select rectangle.
  button_sel_fan_on = lv_button_create(fan_control_sel_rectangle);
  // Assign its event handler.
  lv_obj_add_event_cb(button_sel_fan_on, event_handler_button_sel_fan_on, LV_EVENT_CLICKED, NULL);
  // Assign its position.
  lv_obj_align(button_sel_fan_on, LV_ALIGN_CENTER, 55, 0);
  // Size the button to the text.
  lv_obj_set_height(button_sel_fan_on, LV_SIZE_CONTENT);
   // Adding style to the button
  lv_style_init(&style_button_sel_fan_on);
  lv_style_set_bg_color(&style_button_sel_fan_on, lv_color_hex(0x757575));
  lv_style_set_bg_opa(&style_button_sel_fan_on, LV_OPA_50);
  lv_style_set_border_width(&style_button_sel_fan_on, 6);
  lv_style_set_border_color(&style_button_sel_fan_on, lv_color_hex(0x000000));
  lv_obj_add_style(button_sel_fan_on, &style_button_sel_fan_on, 0); 
  //--------------------------------------------------------------------------------------
  // Put text in the button
  //--------------------------------------------------------------------------------------
  // Create the text label to be included on the active button 1; not the active screen.
  text_label_button_sel_fan_on = lv_label_create(button_sel_fan_on);
  // Create a style for the text label on button_incr_temp.
  static lv_style_t style_text_label_button_sel_fan_on;
  // Create styles for the text inside the button and add them to the button.
  lv_style_set_text_color(&style_text_label_button_sel_fan_on, lv_color_hex(0x000000)); // color
  lv_style_set_text_font (&style_text_label_button_sel_fan_on, &lv_font_montserrat_18); // font
  lv_obj_add_style(text_label_button_sel_fan_on, &style_text_label_button_sel_fan_on, 0); 
  // Align the text to the center of button_incr_temp
  lv_obj_center(text_label_button_sel_fan_on);
  //Set the button label to increase temp.
  lv_label_set_text(text_label_button_sel_fan_on, "Fan On");
}// End of void create_thermostat_screen(void)

//******************************************************************************************************
//******************************************************************************************************
//*****                           Event Handler to Switch Screens                                  *****
//*****           static void event_handler_button_switch_screen(lv_event_t * e)                   *****
//*****                                                                                            *****
//*****  This event handler increments the variable button_switch_screen_page_num from 1 to 3 and  *****
//*****  back to 1 and so forth.  This variable determines which screen is loaded and displayed    *****
//*****  by the static void timer_cb(lv_timer_t * timer){ routine.                                 *****
//******************************************************************************************************
//******************************************************************************************************
static void event_handler_button_switch_screen(lv_event_t * e) 
{
  // Retrieve the current event code.
  lv_event_code_t code = lv_event_get_code(e);
  // Retrieve the object that triggered the event.
  lv_obj_t * obj = (lv_obj_t*) lv_event_get_target(e);
  LV_UNUSED(obj);
  #if DEBUG
    LV_LOG_USER("Clicked %s", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "on" : "off");
  #endif
  // Use the toggle button count to count from 1 to 3 only.
  button_switch_screen_page_num++;
  if(button_switch_screen_page_num > 3) button_switch_screen_page_num = 1;
  #if DEBUG
    Serial.print("Toggle Button Count:  ");
    Serial.println(button_switch_screen_page_num);
  #endif
}// End of static void event_handler_button_switch_screen(lv_event_t * e) {

//******************************************************************************************************
//******************************************************************************************************
//*****                   Event Handler to Increase the Temperature Setpoint                       *****
//*****               static void  event_handler_button_incr_temp(lv_event_t * e)                  *****
//*****                                                                                            *****
//*****  If the increase temperature setpoint button [+] is clicked, this event handler will       *****
//*****  increase the temperature setpoint within the limits defined by the global variables :     *****
//*****                    (1) max_temp_setting_when_heating and                                   *****
//*****                    (2) max_temp_setting_when_cooling                                       *****
//******************************************************************************************************
//******************************************************************************************************
static void  event_handler_button_incr_temp(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = (lv_obj_t*) lv_event_get_target(e);
  LV_UNUSED(obj);
  #if DEBUG
    LV_LOG_USER("Clicked %s", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "on" : "off");
  #endif
  // Increase the temperature setpoint while making sure it is within defined limits.
  // For heat mode
  if (temp_mode == heat && temp_setting <  max_temp_setting_when_heating) temp_setting = temp_setting + 1.0;
  // For cool mode
  if (temp_mode == cool && temp_setting <  max_temp_setting_when_cooling) temp_setting = temp_setting + 1.0;
  #if DEBUG
    Serial.print("Button pressed to increase the temperature.  New setpoint:  ");
    Serial.println(temp_setting);
    Serial.println(code);
    //Serial.println(obj);
  #endif
}// End of static void event_handler_button_incr_temp(lv_event_t * e) {

//******************************************************************************************************
//******************************************************************************************************
//*****                   Event Handler to Decrease the Temperature Setpoint                       *****
//*****               static void  event_handler_button_decr_temp(lv_event_t * e)                  *****
//*****                                                                                            *****
//*****  If the decrease temperature setpoint button [-] is clicked, this event handler will       *****
//*****  decrease the temperature setpoint within the limits defined by the global variables :     *****
//*****                    (1) min_temp_setting_when_heating and                                   *****
//*****                    (2) min_temp_setting_when_cooling                                       *****
//******************************************************************************************************
//******************************************************************************************************
static void  event_handler_button_decr_temp(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = (lv_obj_t*) lv_event_get_target(e);
  LV_UNUSED(obj);
  #if DEBUG
    LV_LOG_USER("Clicked %s", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "on" : "off");
  #endif
  // Decrease the temperature setpoint while making sure it is within defined limits.
  // For heat mode
  if (temp_mode == heat && temp_setting >  min_temp_setting_when_heating) temp_setting = temp_setting - 1.0;
  // For cool mode
  if (temp_mode == cool && temp_setting >  min_temp_setting_when_cooling) temp_setting = temp_setting - 1.0;
  #if DEBUG
    Serial.print("Button pressed to decrease the temperature.  New setpoint:  ");
    Serial.println(temp_setting);
    Serial.println(code);
    Serial.println("Made it through the event_handler_button_decr_temp callback routine.");
  #endif
}// End of static void event_handler_button_incr_temp(lv_event_t * e) {

//******************************************************************************************************
//******************************************************************************************************
//*****                   Event Handler to Enable Air Conditioning (Cool)                          *****
//*****               static void  event_handler_button_sel_cool(lv_event_t * e)                   *****
//*****                                                                                            *****
//*****  If the button [Cool] is clicked, this event handler will change:                          *****
//*****            (1) the temp_mode variable to "cool"                                            *****
//*****            (2) the [Cool] button's background color to blue with a black border            *****
//*****            (3) the [Off] and [Heat] buttons' background color to grey with a white border  *****
//*****            (4) the temperature setting if necessary to insure it is within set limits      *****
//******************************************************************************************************
//******************************************************************************************************
//******************************************************************************************************
static void  event_handler_button_sel_cool(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = (lv_obj_t*) lv_event_get_target(e);
  LV_UNUSED(obj);
  temp_mode = cool;
  // Check the temperature setting to insure it is within the defined limits.
  if (temp_mode == cool && temp_setting < min_temp_setting_when_cooling) temp_setting = min_temp_setting_when_cooling;
  // https://vuetifyjs.com/en/styles/colors/#material-colors
  if (code == LV_EVENT_CLICKED)
  {
    lv_style_init(&style_button_sel_cool);
    lv_style_set_bg_color(&style_button_sel_cool, lv_color_hex(0x29B6F6));
    lv_style_set_bg_opa(&style_button_sel_cool, LV_OPA_80);
    lv_style_set_border_width(&style_button_sel_cool, 3);
    lv_style_set_border_color(&style_button_sel_cool, lv_color_hex(0x000000));
    lv_obj_add_style(button_sel_cool, &style_button_sel_cool, 0); 

    lv_style_init(&style_button_sel_off);
    lv_style_set_bg_color(&style_button_sel_off, lv_color_hex(0x757575));
    lv_style_set_bg_opa(&style_button_sel_off, LV_OPA_80);
    lv_style_set_border_width(&style_button_sel_off, 3);
    lv_style_set_border_color(&style_button_sel_off, lv_color_hex(0xFFFFFF));
    lv_obj_add_style(button_sel_off, &style_button_sel_off, 0); 

    lv_style_init(&style_button_sel_heat);
    lv_style_set_bg_color(&style_button_sel_heat, lv_color_hex(0x757575));
    lv_style_set_bg_opa(&style_button_sel_heat, LV_OPA_80);
    lv_style_set_border_width(&style_button_sel_heat, 3);
    lv_style_set_border_color(&style_button_sel_heat, lv_color_hex(0xFFFFFF));
    lv_obj_add_style(button_sel_heat, &style_button_sel_heat, 0); 
  }
  #if DEBUG
    LV_LOG_USER("Toggled %s", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "on" : "off");
  #endif

  #if DEBUG
    Serial.println(code);
    Serial.print("Current Temperature Control = cool = ");
    Serial.println(temp_mode);
    Serial.println("Made it through the event_handler_button_sel_cool callback routine.");
  #endif
}// End of static void event_handler_button_incr_temp(lv_event_t * e) {

//******************************************************************************************************
//******************************************************************************************************
//*****               Event Handler for Off, Disable both Heat and Cool  (Default)                 *****
//*****               static void  event_handler_button_sel_off(lv_event_t * e) {                  *****
//*****                                                                                            *****
//*****  If the button [Off] is clicked, this event handler will change:                           *****
//*****            (1) the temp_mode variable to "off"                                             *****
//*****            (2) the temperature setpoint temp_setting = 72.0                                *****
//*****            (3) the [Off] button's background color to white with a black border            *****
//*****            (4) the [Cool] and [Heat] buttons' background color to grey with a white border *****
//******************************************************************************************************
//******************************************************************************************************
static void  event_handler_button_sel_off(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = (lv_obj_t*) lv_event_get_target(e);
  LV_UNUSED(obj);
  #if DEBUG
    LV_LOG_USER("Toggled %s", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "on" : "off");
  #endif
  // Set the temperature control mode to off.
  temp_mode = off;
  temp_setting = 72.0;
  // Change the appearance of the control mode buttons.
  // https://vuetifyjs.com/en/styles/colors/#material-colors
  if (code == LV_EVENT_CLICKED)
  {
    lv_style_init(&style_button_sel_cool);
    lv_style_set_bg_color(&style_button_sel_cool, lv_color_hex(0x757575));
    lv_style_set_bg_opa(&style_button_sel_cool, LV_OPA_80);
    lv_style_set_border_width(&style_button_sel_cool, 3);
    lv_style_set_border_color(&style_button_sel_cool, lv_color_hex(0xFFFFFF));
    lv_obj_add_style(button_sel_cool, &style_button_sel_cool, 0); 

    lv_style_init(&style_button_sel_off);
    lv_style_set_bg_color(&style_button_sel_off, lv_color_hex(0xFFFFFF));
    lv_style_set_bg_opa(&style_button_sel_off, LV_OPA_80);
    lv_style_set_border_width(&style_button_sel_off, 3);
    lv_style_set_border_color(&style_button_sel_off, lv_color_hex(0x000000));
    lv_obj_add_style(button_sel_off, &style_button_sel_off, 0); 

    lv_style_init(&style_button_sel_heat);
    lv_style_set_bg_color(&style_button_sel_heat, lv_color_hex(0x757575));
    lv_style_set_bg_opa(&style_button_sel_heat, LV_OPA_80);
    lv_style_set_border_width(&style_button_sel_heat, 3);
    lv_style_set_border_color(&style_button_sel_heat, lv_color_hex(0xFFFFFF));
    lv_obj_add_style(button_sel_heat, &style_button_sel_heat, 0); 
  }
  #if DEBUG
    Serial.println(code);
    Serial.print("Current Temperature Control = off = ");
    Serial.println(temp_mode);
    Serial.println("Made it through the event_handler_button_sel_off callback routine.");
  #endif
}// End of static void event_handler_button_incr_temp(lv_event_t * e) {

//******************************************************************************************************
//******************************************************************************************************
//*****                            Event Handler to Enable Heating                                 *****
//*****               static void  event_handler_button_sel_heat(lv_event_t * e) {                 *****
//*****                                                                                            *****
//*****  If the button [Heat] is clicked, this event handler will change:                          *****
//*****            (1) the temp_mode variable to "heat"                                            *****
//*****            (2) the [Heatl] button's background color to yellow with a black border         *****
//*****            (3) the [Cool] and [Off] buttons' background color to grey with a white border  *****
//*****            (4) the temperature setting if necessary to insure it is within set limits      *****
//******************************************************************************************************
//******************************************************************************************************
static void  event_handler_button_sel_heat(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = (lv_obj_t*) lv_event_get_target(e);
  LV_UNUSED(obj);
  #if DEBUG
    LV_LOG_USER("Toggled %s", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "on" : "off");
  #endif
  // Set the temperature control mode to heat.
  temp_mode = heat;
  // Check the temperature setting to insure it is within the defined limits.
  if (temp_mode == heat && temp_setting > max_temp_setting_when_heating) temp_setting = max_temp_setting_when_heating;

  // Change the appearance of the control mode buttons.
  // https://vuetifyjs.com/en/styles/colors/#material-colors
  if (code == LV_EVENT_CLICKED)
  {
    lv_style_init(&style_button_sel_cool);
    lv_style_set_bg_color(&style_button_sel_cool, lv_color_hex(0x757575));
    lv_style_set_bg_opa(&style_button_sel_cool, LV_OPA_80);
    lv_style_set_border_width(&style_button_sel_cool, 3);
    lv_style_set_border_color(&style_button_sel_cool, lv_color_hex(0xFFFFFF));
    lv_obj_add_style(button_sel_cool, &style_button_sel_cool, 0);

    lv_style_init(&style_button_sel_off);
    lv_style_set_bg_color(&style_button_sel_off, lv_color_hex(0x757575));
    lv_style_set_bg_opa(&style_button_sel_off, LV_OPA_80);
    lv_style_set_border_width(&style_button_sel_off, 3);
    lv_style_set_border_color(&style_button_sel_off, lv_color_hex(0xFFFFFF));
    lv_obj_add_style(button_sel_off, &style_button_sel_off, 0); 

    lv_style_init(&style_button_sel_heat);
    lv_style_set_bg_color(&style_button_sel_heat, lv_color_hex(0xFFFF00));
    lv_style_set_bg_opa(&style_button_sel_heat, LV_OPA_80);
    lv_style_set_border_width(&style_button_sel_heat, 3);
    lv_style_set_border_color(&style_button_sel_heat, lv_color_hex(0x000000));
    lv_obj_add_style(button_sel_heat, &style_button_sel_heat, 0); 
  }
  #if DEBUG
    Serial.println(code);
    Serial.print("Current Temperature Control = heat = ");
    Serial.println(temp_mode);
    Serial.println("Made it through the event_handler_button_sel_heat callback routine.");
  #endif
}// End of static void event_handler_button_incr_temp(lv_event_t * e) {

//******************************************************************************************************
//******************************************************************************************************
//*****                     Event Handler to Enable the Fan Auto Mode  (Default)                   *****
//*****              static void  event_handler_button_sel_fan_auto(lv_event_t * e) {              *****
//*****                                                                                            *****
//*****  If the button [Fan Auto] is clicked, this event handler will change:                      *****
//*****          (1) the fan_mode variable to "fan_auto"                                           *****
//*****          (2) the [Fan Autol] button's background color to bright green with a black border *****
//*****          (3) the [Fan On] button's background color to grey with a white border            *****
//******************************************************************************************************
//******************************************************************************************************
static void  event_handler_button_sel_fan_auto(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = (lv_obj_t*) lv_event_get_target(e);
  LV_UNUSED(obj);
  #if DEBUG
    LV_LOG_USER("Toggled %s", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "on" : "off");
  #endif
  // Set the fan mode to auto.
  fan_mode = fan_auto;
  // Change the appearance of the fan control buttons.
  // https://vuetifyjs.com/en/styles/colors/#material-colors
  if (code == LV_EVENT_CLICKED)
  {
    lv_style_init(&style_button_sel_fan_auto);
    lv_style_set_bg_color(&style_button_sel_fan_auto, lv_color_hex(0x76FF03));
    lv_style_set_bg_opa(&style_button_sel_fan_auto, LV_OPA_80);
    lv_style_set_border_width(&style_button_sel_fan_auto, 3);
    lv_style_set_border_color(&style_button_sel_fan_auto, lv_color_hex(0x000000));
    lv_obj_add_style(button_sel_fan_auto, &style_button_sel_fan_auto, 0); 

    lv_style_init(&style_button_sel_fan_on);
    lv_style_set_bg_color(&style_button_sel_fan_on, lv_color_hex(0x757575));
    lv_style_set_bg_opa(&style_button_sel_fan_on, LV_OPA_80);
    lv_style_set_border_width(&style_button_sel_fan_on, 3);
    lv_style_set_border_color(&style_button_sel_fan_on, lv_color_hex(0xFFFFFF));
    lv_obj_add_style(button_sel_fan_on, &style_button_sel_fan_on, 0); 

  }//End of if (code == LV_EVENT_CLICKED)
  #if DEBUG
    Serial.println(code);
    Serial.print("Current Fan Control = auto = ");
    Serial.println(fan_mode);
    Serial.println("Made it through the event_handler_button_sel_fan_auto callback routine.");
  #endif
}// End of static void event_handler_button_sel_fan_auto(lv_event_t * e) {

//******************************************************************************************************
//******************************************************************************************************
//*****                          Event Handler to Enable the Fan On Mode                           *****
//*****               static void  event_handler_button_sel_fan_on(lv_event_t * e) {               *****
//*****                                                                                            *****
//*****  If the button [Fan On] is clicked, this event handler will change:                        *****
//*****          (1) the fan_mode variable to "fan_on"                                             *****
//*****          (2) the [Fan Onl] button's background color to bright green with a black border   *****
//*****          (3) the [Fan Auto] button's background color to grey with a white border          *****
//******************************************************************************************************
//******************************************************************************************************
static void  event_handler_button_sel_fan_on(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = (lv_obj_t*) lv_event_get_target(e);
  LV_UNUSED(obj);
  #if DEBUG
    LV_LOG_USER("Toggled %s", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "on" : "off");
  #endif
  //Set the fan mode to on.
  fan_mode = fan_on;
  // Change the appearance of the fan control buttons.
  // https://vuetifyjs.com/en/styles/colors/#material-colors
  if (code == LV_EVENT_CLICKED)
  {
    lv_style_init(&style_button_sel_fan_auto);
    lv_style_set_bg_color(&style_button_sel_fan_auto, lv_color_hex(0x757575));
    lv_style_set_bg_opa(&style_button_sel_fan_auto, LV_OPA_80);
    lv_style_set_border_width(&style_button_sel_fan_auto, 3);
    lv_style_set_border_color(&style_button_sel_fan_auto, lv_color_hex(0xFFFFFF));
    lv_obj_add_style(button_sel_fan_auto, &style_button_sel_fan_auto, 0); 

    lv_style_init(&style_button_sel_fan_on);
    lv_style_set_bg_color(&style_button_sel_fan_on, lv_color_hex(0x76FF03));
    lv_style_set_bg_opa(&style_button_sel_fan_on, LV_OPA_80);
    lv_style_set_border_width(&style_button_sel_fan_on, 3);
    lv_style_set_border_color(&style_button_sel_fan_on, lv_color_hex(0x000000));
    lv_obj_add_style(button_sel_fan_on, &style_button_sel_fan_on, 0); 
  }//End of if (code == LV_EVENT_CLICKED)
  Serial.println(" fan on fan on");
  #if DEBUG
    Serial.println(code);
    Serial.print("Current Fan Control = on = ");
    Serial.println(fan_mode);
    Serial.println("Made it through the event_handler_button_sel_fan_on callback routine.");
  #endif
}// End of static void event_handler_button_sel_fan_on(lv_event_t * e) {

//******************************************************************************************************
//******************************************************************************************************
//*****                                  Timer Call Back Function                                  *****
//*****                          static void timer_cb(lv_timer_t * timer){                         *****
//*****                                                                                            *****
//*****  This timer call back function updates the text labels that are used to display text       *****
//*****  on the displays screen.  This is done by creating a LVGL timer that runs a specific       *****
//*****  function periodically.                                                                    *****
//*****                                                                                            *****
//*****  The timer_cb function runs every one second and each time it runs it increments the time  *****
//*****  by 1 second. This inturn increments the minutes after 60 seconds, which inturn increments *****
//*****  the the hour every 60 minutes.                                                            *****
//*****                                                                                            *****
//*****  Depending on the button_switch_screen_page_num, this routine will update the screen every *****
//*****  second.                                                                                   *****
//*****                                                                                            *****
//*****  In addition the sync_weather and sync_time_date booleans are set "true" every 15 minutes  *****
//*****  and every 30 minutes respectively; this causes the weather and time to re-sync to the CSU *****
//*****  weather stations and the WorldTimeAPI WEB sites at their respective intervals.            *****
//******************************************************************************************************
//******************************************************************************************************
//*****--------------------------------------------------------------------------------------------*****
//*****  Note: Beginning of Random Nerd code for the timer_cb code function.                       *****
//*****--------------------------------------------------------------------------------------------***** 
static void timer_cb(lv_timer_t * timer){
  LV_UNUSED(timer);
  second++;
  if(second > 59) {
    second = 0;
    minute++;
    // Get the inside temp, humd, and pres
    get_inside_THP();
    if (minute % 15 == 0) sync_weather = true;
    if (minute % 30 == 0) sync_time_date = true;
    if(minute > 59) {
      minute = 0;
      hour++;
      //sync_time_date = true;
      if(hour > 23) {
        hour = 0;
      }// End of if(hour > 23) {
      #if DEBUG
        Serial.print("Re-sync time date requested: ");
        Serial.println(sync_time_date);
        Serial.print("Re-sync weather requested: ");
        Serial.println(sync_weather);
      #endif
    }// End of if(minute > 59) {
  }// End of if(second > 59) {

  //**************************************************************************
  // Update the auxiliary string variables that are used to display the time.
  //**************************************************************************
  // Here the strings are corrected for display purposes.
  String hour_time_f = String(hour);
  if (hour < 10) hour_time_f = " " + String(hour);
  String minute_time_f = String(minute);
  if (minute < 10) minute_time_f = "0" + String(minute);
  String second_time_f = String(second);
  if (second < 10) second_time_f = "0" + String(second);

  //********************************************************************************
  // Here the final time string is created by concatenating the hours, minutes, and 
  // the optional seconds and meridiem together into the final_time_str.
  //********************************************************************************
  // Concatenate the hour, minute, and optional seconds.
  if(display_seconds)
  {
    // Include hour, minute, and seconds in the display text.
    final_time_str = String(hour_time_f) + ":" + String(minute_time_f) + ":"  + String(second_time_f);
  }// End of if(display_seconds)
  else
  {
    // Only include hours and minutes; exclude seconds.
    final_time_str = String(hour_time_f) + ":" + String(minute_time_f);
  }// End of else

  // Add post and ante meridiem (am and pm) if desired.
  if(include_am_pm)
  {
   if (pm  == true)   final_time_str = final_time_str + " p.m.";
   if (pm  == false)  final_time_str = final_time_str + " a.m.";
  }// End of if(include_am_pm)
  //This completes the creation of the final time string.

  //*************************************************************************
  // Update the auxiliary string variables that are used to display the date.
  // Here the strings are corrected for display purposes.
  //************************************************************************
  final_date_str = current_month + " " + current_day + ", " + current_year;
  #if DEBUG
    if(second == 0)//Display once per minute.
    {
      Serial.print("final_time_string = ");
      Serial.println(final_time_str);
      Serial.print("final_date_string = ");
      Serial.println(final_date_str);
      Serial.print("current_date = ");
      Serial.println(current_date);
      Serial.print("\n\n\n");
    }// End of if(second == 0)
  #endif
//*****--------------------------------------------------------------------------------------------*****
//*****  Note: End of Random Nerd code for the timer_cb code function.                             *****
//*****--------------------------------------------------------------------------------------------***** 


  //**************************************************************************************
  // The c_str() function in C++ is used to convert a std::string object to a 
  // C-style string (a null-terminated character array). This is particularly 
  // useful when you need to pass a string to a function that expects a C-style string.
  //
  // You can set the text on a label at runtime with lv_label_set_text(label, "New text"). 
  // It will allocate a buffer dynamically, and the provided string will be copied into 
  // that buffer. Therefore, you don't need to keep the text you pass to lv_label_set_text 
  // in scope after that function returns.
  //
  // A label is the basic object type that is used to display text.
  //
  // Convert the final time, date, and day strings to a C-style string and pass them to the 
  // their respective labels that will be used to display the text.
  //**************************************************************************************

  //*************************************************
  //***** Load the General screen that displays *****
  //*****     day of week,                      *****
  //*****     date,                             *****
  //*****     time, and                         *****
  //*****     outside temperature               *****
  //*************************************************
  if(button_switch_screen_page_num == 1)
  {
    // Load the general screen.
    lv_disp_load_scr(general_screen);

    // Set the text displayed by the label for day.
    lv_label_set_text(text_label_day, daysOfTheWeek[day]);
  
    // Based on the boolean set at the top of the program select which
    // text is displayed by the label for date.
    if(date_display_MDY)
    { lv_label_set_text(text_label_date, final_date_str.c_str());} //ex. Apr 15, 2025
    else
    { lv_label_set_text(text_label_date, current_date.c_str());  } //ex. 2025-04-15

    // Set the text displayed by the label for time.
    lv_label_set_text(text_label_time, final_time_str.c_str());

    // Outside Temperature: Convert the float to a string with 1 digit after the decimal point.
    dtostrf(tempF,5,1,tempF_str);
    // Set the text displayed by the label for temperature.
    lv_label_set_text(text_label_temp,  (String(tempF_str) + " °F").c_str());   
  }// End of if(button_switch_screen_page_num == 1)

  //*************************************************
  //***** Load the Weather screen that displays *****
  //***** Colorado State University's weather   *****
  //***** station information.                  *****
  //*****     temperature,                      *****
  //*****     humidity, and                     *****
  //*****     wind speed.                       *****
  //*************************************************
  if(button_switch_screen_page_num == 2)// Display the weather
  {
    // Load the weather screen.
    lv_disp_load_scr(weather_screen);

    // Outside Temperature: Convert the float to a string with 1 digit after the decimal point.
    dtostrf(tempF,5,1,tempF_str);
    // Set the text displayed by the label for temperature.
    lv_label_set_text(text_label_outside_temp,  ("T:    " + String(tempF_str) + " °F").c_str());

    // Relative Humidity: Convert the float to a string with 1 digit after the decimal point.
    //char relative_humidity_str[7];
    dtostrf(relative_humidity,5,1,relative_humidity_str);
    // Set the text displayed by the label for relative humidity.
    lv_label_set_text(text_label_outside_rhum, ("H:   " + String(relative_humidity_str) + " %").c_str());

    // Wind Speed: Convert the float to a string with 1 digit after the decimal point.
    dtostrf(wind_mph,5,1,wind_mph_str);
    // Set the text displayed by the label for wind speed.
    lv_label_set_text(text_label_outside_wind, ("W: " + String(wind_mph_str) + " mph").c_str());    
  }//End of if(button_switch_screen_page_num == 2)// Display the weather

  //*************************************************
  //***** Load the Thermostat screen that       *****
  //***** displays:                             *****
  //*****     inside temperature,               *****
  //*****     inside humidity, and              *****
  //*****     accepts user inputs.              *****
  //*************************************************
  if(button_switch_screen_page_num == 3) //Display Thermostat
  {
    // Load the thermostat screen.
    lv_disp_load_scr(thermostat_screen);
    
    // Inside Temperature: Convert the float to a string with 1 digit after the decimal point.
    char bme_temp_str[7];
    dtostrf(bme_temp,5,1,bme_temp_str);
    // Set the text displayed by the label for temperature.
    lv_label_set_text(text_label_inside_temp,  ("Inside\n\nTemp\n" + String(bme_temp_str) + " °F").c_str());
  
    // Inside Humidity: Convert the float to a string with 1 digit after the decimal point.
    char bme_humd_str[7];
    dtostrf(bme_humd,5,1,bme_humd_str);
    // Set the text displayed by the label for relative humidity.
    lv_label_set_text(text_label_inside_humd, ("Humd\n" + String(bme_humd_str) + " %").c_str());

    //Temperature Setpoint: Convert the float to a string with 1 digit after the decimal point.
    char temp_sp_str[7];
    dtostrf(temp_setting,5,1,temp_sp_str);
    // Set the text displayed by the label for relative humidity.
    //lv_label_set_text(text_label_temp_sp, (String(temp_sp_str) + " °F").c_str());
    lv_label_set_text(text_label_temp_sp, (String(temp_sp_str)).c_str());

    // The following code changes the background color of decrease [-] button to black
    // showing that it is disabled when miniumum temperature setpoint is reached.
    static bool last_min_temp_state;
    if (temp_mode == cool && temp_setting == min_temp_setting_when_cooling  ||
        temp_mode == heat && temp_setting == min_temp_setting_when_heating) 
    { min_temp_sp = true; }
    else // Set it to the default color showing it is enabled.
    { min_temp_sp = false; }//End of else
    if ((min_temp_sp != last_min_temp_state) && min_temp_sp == true)
    { 
      last_min_temp_state = true;
      lv_style_init(&style_button_decr_temp);
      lv_style_set_bg_color(&style_button_decr_temp, lv_color_hex(0x000000));
      lv_style_set_bg_opa(&style_button_decr_temp, LV_OPA_100);
      lv_style_set_border_width(&style_button_decr_temp, 6);
      lv_style_set_border_color(&style_button_decr_temp, lv_color_hex(0x000000));
      lv_obj_add_style(button_decr_temp, &style_button_decr_temp, 0); 
    }
    //Note: If you replace the following statement with and else statement the button will flash.
    if ((min_temp_sp != last_min_temp_state) && min_temp_sp == false)
    {
      last_min_temp_state = false;
      lv_style_init(&style_button_decr_temp);
      lv_style_set_bg_color(&style_button_decr_temp, lv_color_hex(0x757575));
      lv_style_set_bg_opa(&style_button_decr_temp, LV_OPA_50);
      lv_style_set_border_width(&style_button_decr_temp, 6);
      lv_style_set_border_color(&style_button_decr_temp, lv_color_hex(0x000000));
      lv_obj_add_style(button_decr_temp, &style_button_decr_temp, 0); 
    }//End if ((min_temp_sp != last_min_temp_state) && min_temp_sp == false)

    // The following code changes the background color of increase [+] button to black
    // showing that it is disabled when maxiumum temperature setpoint is reached.
    static bool last_max_temp_state;
    // If the maximum temperature setpoint has been reached make the increase [+]
    // button black showing it is disabled
    if (temp_mode == cool && temp_setting == max_temp_setting_when_cooling  ||
        temp_mode == heat && temp_setting == max_temp_setting_when_heating) 
    { max_temp_sp = true;  }
    else // Set it to the default color showing it is enabled.
    { max_temp_sp = false; }//End of else
    if ((max_temp_sp != last_max_temp_state) && max_temp_sp == true)
    { 
      last_max_temp_state = true;
      lv_style_init(&style_button_incr_temp);
      lv_style_set_bg_color(&style_button_incr_temp, lv_color_hex(0x000000));
      lv_style_set_bg_opa(&style_button_incr_temp, LV_OPA_100);
      lv_style_set_border_width(&style_button_incr_temp, 6);
      lv_style_set_border_color(&style_button_incr_temp, lv_color_hex(0x000000));
      lv_obj_add_style(button_incr_temp, &style_button_incr_temp, 0); 
    }
    //Note: If you replace the following statement with and else statement the button will flash.
    if ((max_temp_sp != last_max_temp_state) && max_temp_sp == false)
    {
      last_max_temp_state = false;
      lv_style_init(&style_button_incr_temp);
      lv_style_set_bg_color(&style_button_incr_temp, lv_color_hex(0x757575));
      lv_style_set_bg_opa(&style_button_incr_temp, LV_OPA_50);
      lv_style_set_border_width(&style_button_incr_temp, 6);
      lv_style_set_border_color(&style_button_incr_temp, lv_color_hex(0x000000));
      lv_obj_add_style(button_incr_temp, &style_button_incr_temp, 0); 
    }//if ((max_temp_sp != last_max_temp_state) && max_temp_sp == false)

  }// End of if(button_switch_screen_page_num == 3) //Display Thermostat
}// End of static void timer_cb(lv_timer_t * timer){

//******************************************************************************************************
//******************************************************************************************************
//*****                         Create the Main GUI: Random Nerd Tutorials                         *****
//*****                               void lv_create_main_gui(void) {                              *****
//*****                                                                                            *****
//*****  The LVGL library works asynchronously. You must call the function to draw on the screen   *****
//*****  on the displays in the setup() routine.  This is done by creating a LVGL timer that runs  *****
//*****  a specific function periodically.  Then everything works with events and callbacks. The   *****
//*****  code will always be listening for events in the background.  When something happens, it   *****
//*****  will run the callback function associated with the event so there is no need to check for *****
//*****  any events in the loop.                                                                   *****
//*****                                                                                            *****
//*****  Most of the Random Nerd examples will use the lv_create_main_gui() function to draw the   *****
//*****  screen.   Instructions are added inside of this function to build the GUI.                *****
//******************************************************************************************************
//******************************************************************************************************
//*****--------------------------------------------------------------------------------------------*****
//*****  Note: Although I made some contributions to the following code, a Random Nerd tutorial    *****
//*****  for the CYD provided the foundation.                                                      *****
//*****--------------------------------------------------------------------------------------------***** 
void lv_create_main_gui(void) {

  // Prototype of a LVGL time that will run a specified call back function periodically.:
  // lv_timer_t * timer = lv_timer_create(timer_cb, period_ms, user_data) will return an lv_timer_t *
  //----------------------------------------------------------------------------------------------------
  // Here a timer called timer is create and it will run the timer_cb function every second. 
  // In this case timer_cb function will update the screens every second and re-sync as specified
  // to WEB pages.  
  lv_timer_t * timer = lv_timer_create(timer_cb, 1000, NULL);// Run the timer_cb function every second.
  lv_timer_ready(timer);  //Make the timer run on the next call of the lv_timer_handler()

//*****--------------------------------------------------------------------------------------------*****
//*****  Note: Additions                                                                           *****
//*****--------------------------------------------------------------------------------------------***** 
  create_general_screen();
  create_weather_screen();
  create_thermostat_screen();
}// End of void lv_create_main_gui(void) {

//******************************************************************************************************
//******************************************************************************************************
//*****                             Get the Date and Time from the WEB                             *****
//*****                                void get_day_date_and_time()                                *****
//*****                                                                                            *****
//*****  This function will deserialize the latest day of the week, date, and time from the        *****
//*****  WorldTimeAPI WEB site and store the information into integer numbers.  Theses integers    *****
//*****  will be used to update screen text labels.                                                *****
//******************************************************************************************************
//******************************************************************************************************
//*****--------------------------------------------------------------------------------------------*****
//*****  Note: Although I made some contributions to the following code, a Random Nerd tutorial    *****
//*****  for the CYD provided the foundation.                                                      *****
//*****--------------------------------------------------------------------------------------------***** 
void get_day_date_and_time() {
  int splitIndex;
  if (WiFi.status() == WL_CONNECTED) 
  {
    #if DEBUG
      Serial.println("Connected to Wi-Fi");
    #endif
    HTTPClient http;

    // Construct the API endpoint
    String url = String("http://worldtimeapi.org/api/timezone/") + timezone;
    http.begin(url);
    int httpCode = http.GET(); // Make the GET request

    if (httpCode > 0) 
    {
      // Check for the response
      if (httpCode == HTTP_CODE_OK) 
      {
        String payload = http.getString();
        #if DEBUG
          Serial.println("Time information payload:");
          Serial.println(payload);
        #endif
        JsonDocument doc;
        // Call deserializeJson(), passing the JsonDocument as the first argument and 
        // the JSON input as the second.  If successful, doc will contain the deserialized
        // information retrieved from the WEB.
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) // Deserialization was successful.
        {
          #if DEBUG
            //Was the day_of_week key found?
            if(doc.containsKey("day_of_week")) 
            {Serial.println("The day_of_week key is present.");}
            else
            {Serial.println("The day_of_week key is missing.");}

            // Was the datetime key found?
            if(doc.containsKey("datetime")) 
            {Serial.println("The datetime key is present.");}
            else
            {Serial.println("The datetime key is missing.");}
          #endif

          //***********************************************
          // Using day_of_week key get the day number (int).
          //      key      int
          // "day_of_week":3,
          // Number 0-6 for Sunday to Saturday
          //***********************************************
          day = doc["day_of_week"];

          #if DEBUG
            Serial.print("day of week: ");
            Serial.print(day);
            Serial.print("   ");
            Serial.println(daysOfTheWeek[day]);
          #endif

          //***********************************************
          // Using the datetime key get the datetime string.
          //     key               string
          // "datetime":"2025-04-09T16:01:17.151801-06:00"
          // Note the 'T' between the date and the time.
          //***********************************************
          const char* datetime = doc["datetime"];          
          String datetime_str = String(datetime);
          // Split the string into date and time
          splitIndex = datetime_str.indexOf('T');
          // Split out the date.
          current_date = datetime_str.substring(0, splitIndex);

          #if DEBUG
            Serial.print("*");
            Serial.print(datetime_str);
            Serial.println("*");
            Serial.println(datetime_str.substring(0,4));
            Serial.println(datetime_str.substring(5,7));
            Serial.println(datetime_str.substring(8,10));
          #endif

          //***********************************************
          // Split out the year month and day, convert them
          // to integers and store them in auxilliary 
          // variables.
          //***********************************************
          // Auxillary integer variables
          date_month    = datetime_str.substring(5,7).toInt();
          date_year     = datetime_str.substring(0,4).toInt();
          date_day      = datetime_str.substring(8,10).toInt();
          // Auxilliary string variables
          current_month = abrMonthsOfTheYear[date_month-1];
          current_day   = datetime_str.substring(8,10);
          current_year  = datetime_str.substring(0,4);


          #if DEBUG
            Serial.print("Month ");
            Serial.print(date_month);
            Serial.print("    ");
            Serial.println(abrMonthsOfTheYear[date_month-1]);
            Serial.print("Day ");
            Serial.println(date_day);
            Serial.print("Year ");
            Serial.println(date_year);
          #endif

          //***********************************************
          // Using the same splitIndex, extract the time 
          // portion of the string.
          //     key               string
          // "datetime":"2025-04-09T16:01:17.151801-06:00"
          // Note the 'T' between the date and the time.
          //***********************************************
          current_time = datetime_str.substring(splitIndex + 1, splitIndex + 9);
          hour   = current_time.substring(0, 2).toInt();
          minute = current_time.substring(3, 5).toInt();
          second = current_time.substring(6, 8).toInt();

          //***********************************************
          // Adjust the hours for the 12 hour clock and 
          // set the pm boolean flag.
          //***********************************************
          pm = false;
          if(hour > 12) 
          {
            hour = hour - 12;
            pm = true;
          }// End of if(hour > 12) 
        } // End of if (!error) {
        else 
        { 
          // An error occurred.
          #if DEBUG
           Serial.print("deserializeJson() failed: ");
           Serial.println(error.c_str());
          #endif
        }//End of else
      }// End of if (httpCode == HTTP_CODE_OK)
    } // End of if (httpCode > 0) 
    else 
    {
      //Get request failed
     #if DEBUG
       Serial.printf("GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
      #endif

      // Still need to sync to WorldTimeAPI
      sync_time_date = true;
    }// End of else
    http.end(); // Close connection
  }// End of if (WiFi.status() == WL_CONNECTED)
  else 
  {
    // Failed to connect to Wi-Fi
    #if DEBUG
     Serial.println("Not connected to Wi-Fi");
    #endif
  }//End of else
}// End of void get_day_date_and_time() {

//******************************************************************************************************
//******************************************************************************************************
//*****                                Get the Weather from the WEB                                *****
//*****                                    void get_weather() {                                    *****
//*****                                                                                            *****
//*****  This function will deserialize the latest weather information from the                    *****
//*****  Colorado State University (CSU) weather station and store the information into integer    *****
//*****  numbers.  Theses integers will be used to update screen text labels.                      *****
//******************************************************************************************************
//******************************************************************************************************
//*****--------------------------------------------------------------------------------------------*****
//*****  Note: Although I made some contributions to the following code, a Random Nerd tutorial    *****
//*****  for the CYD provided the foundation.                                                      *****
//*****--------------------------------------------------------------------------------------------***** 
void get_weather() {
  if (WiFi.status() == WL_CONNECTED) 
  {
    #if DEBUG
      Serial.println("Connected to Wi-Fi");
    #endif
    HTTPClient http;

    //Boulder - https://api.weather.gov/gridpoints/BOU/58,94/forecast
    //Boulder - https://api.weather.gov/gridpoints/BOU/58,94/forecast/hourly
    //CSU     - https://ccc.atmos.colostate.edu/~autowx/fclwx_livefeed.json

    // Construct the API endpoint
    String url = String("https://ccc.atmos.colostate.edu/~autowx/fclwx_livefeed.json");
    http.begin(url);
    int httpCode = http.GET(); // Make the GET request

    if (httpCode > 0) 
    {
      // Check for the response
      if (httpCode == HTTP_CODE_OK) 
      {
        String payload = http.getString();
        #if DEBUG
          Serial.println("Weather information payload:");
          Serial.println(payload);
        #endif
        // Declare a variable to stored the deserialized documnet.
        JsonDocument doc;
        // Call deserializeJson(), passing the JsonDocument as the first argument and 
        // the JSON input as the second.  If successful, doc will contain the deserialized
        // information retrieved from the WEB.
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) // Deserialization was successful.
        {
          #if DEBUG
            //Was the tempF key found?
            if(doc.containsKey("tempF")) 
            {Serial.println("The tempF key is present.");}
            else
            {Serial.println("The tempF key is missing.");}

            // Was the dewpointF key found?
            if(doc.containsKey("dewpointF")) 
            {Serial.println("The dewpointF key is present.");}
            else
            {Serial.println("The dewpointF key is missing.");}

            // Was the relative_humidity key found?
            if(doc.containsKey("relative_humidity")) 
            {Serial.println("The relative_humidity key is present.");}
            else
            {Serial.println("The relative_humidity key is missing.");}

            // Was the wind_mph key found?
            if(doc.containsKey("wind_mph")) 
            {Serial.println("The wind_mph key is present.");}
            else
            {Serial.println("The wind_mph key is missing.");}

          #endif

          //***********************************************
          // Using tempF key get the temperature.
          //      key      float
          // "tempF":84.4,
          // Number 0-6 for Sunday to Saturday
          //***********************************************
          tempF = doc["tempF"];

          #if DEBUG
            Serial.print("temperature: ");
            Serial.println(tempF);
          #endif

          //***********************************************
          // Using the relative_humidity key get humidity.
          //     key               float
          // "relative_humidity":6.4
          //***********************************************
          relative_humidity = doc["relative_humidity"];          

          #if DEBUG
            Serial.print("relative humidity: ");
            Serial.println(relative_humidity);
          #endif

          //***********************************************
          // Using the wind_mph key get wind speed.
          //     key               float
          // "wind_mph":6.4
          //***********************************************
          wind_mph = doc["wind_mph"];          

          #if DEBUG
            Serial.print("wind mph: ");
            Serial.println(wind_mph);
          #endif
        } // End of if (!error) {
        else 
        { 
          // An error occurred.
          #if DEBUG
           Serial.print("deserializeJson() failed: ");
           Serial.println(error.c_str());
          #endif
        }//End of else
      }// End of if (httpCode == HTTP_CODE_OK)
    } // End of if (httpCode > 0) 
    else 
    {
      //Get request failed
      #if DEBUG
        Serial.printf("GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
      #endif

      // Still need to sync to CSU weather station
      sync_weather = true;
    }// End of else
    http.end(); // Close connection

  }// End of if (WiFi.status() == WL_CONNECTED)
  else 
  {
    // Failed to connect to Wi-Fi
    #if DEBUG
     Serial.println("Not connected to Wi-Fi");
    #endif
  }//End of else
}// End of void void get_weather() 

//******************************************************************************************************
//******************************************************************************************************
//*****                     Get the Inside Temperature, Humidity, and Pressure                     *****
//*****                                  void get_inside_THP() {                                   *****
//*****                                                                                            *****
//*****  This function gets the BME280's sensor readings and stores them into strings with the     *****
//*****  appropriate units.  These strings are used for the text labels supporting the Thermostat  *****
//*****                                                                                            *****
//*****  The function also reads and stores the light dependent resistor values.                   *****
//******************************************************************************************************
//******************************************************************************************************
void get_inside_THP() {
  // Temperature
  bme_temp = bme.readTemperature();
  if(strcmp (Units, "F") == 0) // Are the units in F
  {
    bme_temp = 1.8 * bme.readTemperature() + 32;
    bme_temp_str = "Inside Temperature: " + String(bme_temp) + " °F";
  }
  else
  {
    bme_temp_str = "Inside Temperature: " + String(bme_temp) + " °C";
  }
  // Humidity
  bme_humd      = bme.readHumidity();
  bme_humd_str  = "Inside Humidity: " + String(bme_humd) + "%";
  // Pressure
  bme_pres      = bme.readPressure();
  bme_pres_str  = "Inside Pressure: " + String(bme_pres / 100.0F) + " hPa";
  // Light Dependent Resistor (LDR) value
  ldr_value     = analogRead(LDR_PIN);
  ldr_value_str = "LDR value : " + String (ldr_value);
  #if DEBUG
    Serial.println(bme_temp_str);
    Serial.println(bme_humd_str);
    Serial.println(bme_pres_str);
    Serial.println(ldr_value_str);
  #endif
}// End of get_inside_THP()


//******************************************************************************************************
//******************************************************************************************************
//*****                                       Setup Routine                                        *****
//*****                                       void setup() {                                       *****
//*****--------------------------------------------------------------------------------------------*****
//*****   Note: This setup() routine code was extracted from a Random Nerd tutorial for the CYD.   *****
//*****--------------------------------------------------------------------------------------------***** 
//*****                                                                                            *****
//*****  This routine only runs once after power up or reset.   It will:                           *****
//*****   (1) Start the serial communications library.                                             *****
//*****   (2) Start the WiFi connection.                                                           *****
//*****   (3) Start the I2C (Inter-Integrated Circuit) communications.                             *****
//*****   (4) Start the LVGL library.                                                              *****
//*****   (5) Register the custom print function used for builtin LVGL log module.                 *****
//*****   (6) Register the custom print function (log_print) for the built in LVGL log module.     *****
//*****   (7) Start the touchscreen library and initialize the touchscreen.                        *****
//*****   (8) Startup and intialize the Serial Peripheral Interface (SPI) for the touchscreen.     *****
//*****   (9) Create the disp object and initialize the TFT display using the TFT_eSPI library.    *****
//*****  (10) Initialize an LVGL input device object(Touchscreen) and the callback function to     *****
//*****       read the Touchscreen                                                                 *****
//*****  (11) Call the main GUI function that will create the sreens.                              *****
//*****  (12) Initialize the readings for day, date, time, weather, and inside BME readings.       *****
//*****  (13) Send manual a button press to initialize temperature control to off.                 *****
//*****  (14) Send manual a button press to initialize fan control to auto.                        *****
//******************************************************************************************************
//******************************************************************************************************
//*****--------------------------------------------------------------------------------------------*****
//*****                 Note: Beginning of Random Nerd setup code for the CYD.                     *****
//*****--------------------------------------------------------------------------------------------***** 
void setup() {

  // The following lines are for debugging. These will print the version of LVGL that you’re using. 
  // You must be using version 9.
  String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.begin(115200);
  #if DEBUG
    Serial.print("At the top of the setup routine.");
   Serial.println(LVGL_Arduino);
  #endif
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  #if DEBUG
    Serial.print("Connecting");
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #if DEBUG
      Serial.print(".");
    #endif
  }//End of while (WiFi.status() != WL_CONNECTED) {

  #if DEBUG
    Serial.print("\nConnected to Wi-Fi network with IP Address: ");
    Serial.println(WiFi.localIP());
  #endif

  //***********************************************
  //***** Start I2C communications.           *****
  //***********************************************
  I2CBME.begin(I2C_SDA, I2C_SCL, 100000);
  bool status;
  // Passing a &Wire2 to set custom I2C ports
  status = bme.begin(0x76, &I2CBME);
  if (!status) 
  {
    #if DEBUG
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
    #endif
    while (1);
  }// End of if (!status) 

  //***********************************************
  //***** Initialize LVGL library.            *****
  //***********************************************
  // Initialize the LVGL Library.
  lv_init();

  //***********************************************
  //***** Register the custom print function  *****
  //***********************************************
  // Register the lfunction used for printing the 
  // LVGL built in logging.
  lv_log_register_print_cb(log_print);

  //***********************************************
  //***** Startup and intialize the SPI       *****
  //***** interface for the touchscreen.      *****
  //***********************************************
  // Startup the Serial Peripheral Interface (SPI) for the touchscreen.
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  // Initialize the touchscreen.
  touchscreen.begin(touchscreenSPI);
  // Set the Touchscreen rotation in landscape mode
  // Set the touchscreen rotation: valid values are 0-3
  touchscreen.setRotation(2);

  //***********************************************
  //***** Create the disp object and          *****
  //***** initialize the TFT display using    *****
  //***** the TFT_eSPI library.               *****
  //***********************************************
  // Create a display object
  lv_display_t * disp;
  // Initialize the TFT display using the TFT_eSPI library
  disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);

  //***********************************************
  //***** Initialize an LVGL input device     *****
  //***** object(Touchscreen) and the callback*****
  //***** function to read the Touchscreen    *****
  //***** input.                              *****
  //***********************************************
  // Initialize an LVGL input device object (Touchscreen)
  lv_indev_t * indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  // Set the callback function to read Touchscreen input
  lv_indev_set_read_cb(indev, touchscreen_read);

  //***********************************************
  //***** Call the main GUI function that     *****
  //***** that will create the screens.       *****
  //***********************************************
  lv_create_main_gui();

  //***********************************************
  //***** Get the initial readings that are   *****
  //***** required for the display labels.    *****
  //***********************************************
  // Get the initial time and date from WorldTimeAPI
  while(hour==0 && minute==0 && second==0) 
  {
    get_day_date_and_time();
  }// End of while(hour==0 && minute==0 && second==0) 

//*****--------------------------------------------------------------------------------------------*****
//*****                     Note: End of Random Nerd setup code for the CYD.                       *****
//*****--------------------------------------------------------------------------------------------***** 

  // Get the initial weather readings from the CSU weather station
  while(tempF==0.0 && relative_humidity==0.0 && wind_mph==0.0) 
  {
    get_weather();
  }// End of while(tempF==0.0 && relative_humidity==0.0 && wind_mph==0.0)

  // Get the initial inside temperature, humidity, and pressure readings from the BME280 sensor.
  get_inside_THP();

  //***********************************************
  //***** Manually send a button click to set *****
  //***** the default temperature control mode*****
  //***** to off.                             *****
  //***********************************************
  // Set the default temperature control mode to off.
  lv_obj_send_event(button_sel_off, LV_EVENT_CLICKED, NULL);

  //***********************************************
  //***** Manually send a button click to set *****
  //***** the default fan control mode to auto*****
  //***********************************************
  lv_obj_send_event(button_sel_fan_auto, LV_EVENT_CLICKED, NULL);
}//End of void setup() {

//******************************************************************************************************
//******************************************************************************************************
//*****                                        Loop Routine                                        *****
//*****                                        void loop() {                                       *****
//*****                                                                                            *****
//*****   Note: This loop() routine code was extracted from a Random Nerd tutorial for the CYD.    *****
//*****         The only change was to add the code to sync the weather.                           ***** 
//*****                                                                                            *****
//******************************************************************************************************
//******************************************************************************************************
// If the sync_time_date boolean is true then sync the date and time with the WorldTimeAPI.
void loop() {
  // Get the time and date from WorldTimeAPI at specific intervals
  if(sync_time_date) {
    sync_time_date = false;
    get_day_date_and_time();
    while(hour==0 && minute==0 && second==0) 
    {
      get_day_date_and_time();
    }// End of while
  }// End of if(sync_time_date) {

  // Get the weather information from the CSU weather station at specific intervals
  if(sync_weather) {
    sync_weather = false;
    get_weather();
    while(tempF==0.0 && relative_humidity==0.0 && wind_mph==0.0)
    {
      get_weather();
    }// End of while
  }// if(sync_weather) {
  
  lv_task_handler();  // let the GUI do its work
  lv_tick_inc(5);     // tell LVGL how much time has passed
  delay(5);           // let this time pass
}




