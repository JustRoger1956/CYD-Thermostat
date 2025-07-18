<pre>
# CYD-Thermostat
//****************************************************************************************
//  I hope you find this code helpful. 
//
//  The Random Nerd Tutorials provided the initiative and examples that assisted so much.
//
//  In general this has been a very satisfying endeavor.  Of course I relied on the 
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
//    (a) Search for credentials and input your network WiFi credentials.
//    (b) Go to the Tools tab to select board ESP32-2432S028R CYD; its almost at the end
//        of the list.
//    (c) Go to tools -> board -> boards manager and use ESP32 verions 3.0.7 version by 
//        Epressif Systems.  Later versions may not work.
//    (d) On the Tools tab also select Partition Scheme Huge APPC3MB No OTA/1MB SPIFFS.
//    (e) The program takes a long time to compile and load; be patient.
//    (f) At some time you will want to update the Time Zone and Weather Station
//        WEB pages for your specific needs; however I suggest doing that after the 
//        initial load.
//
//
//  Screen Notes:  
//    (1) Clicking the button at the top center of the screens will cause the display
//        to switch screens.
//    (2) The increase and decrease buttons are disabled when "Off" is selected.
//    (3) When "Off" is selected the temperature setpoint is set to the default value.
//    (4) The decrease [-] and increase [+] button will be blacked out when the 
//        minimum or maximum temperature setpoint is reach.  This shows they are 
//        disabled.
//
//  Regards and Good Luck,
//  Roger Yacobucci (JustRoger1956)
//  May 29, 2025
//****************************************************************************************
//-------------------------------------Revision History-----------------------------------
//
// 1.0.0 - 04/27/2025 Initial Release
//
// 1.0.1 - 04/30/2025 Some code clean up: no functional changes.
//
// 1.0.2 - 05/07/2025 Bug fixes to the loop routine for getting time and weather.
//                    Added ability to display either a 12 hour or 24 hour clock.
//
// 1.0.3 - 05/29/2025 Added a screen saver.
//
// 1.0.4 - 06-06-2025 - Compiling with ESP32 version 3.2.0 instead of version 3.0.7 now 
//                      appears to work.  Not sure why.
//
// 1.0.5 - 07-08-2025 - Updated screen saver code.
//----------------------------------------------------------------------------------------
</pre>
