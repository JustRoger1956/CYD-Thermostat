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
//    (b) Go to the Tools tab to select board ESP32-2432S028R CYD
//    (c) On the Tools tab also select Partition Scheme Huge APPC3MB No OTA/1MB SPIFFS.
//    (d) The program takes a long time to compile and load; be patient.
//    (e) At some time you will want to update the Time Zone and Weather Station
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
//  Roger Yacobucci (JustRoger)
//  April 30, 2025
//****************************************************************************************
//-------------------------------------Revision History-----------------------------------
//
// 1.0.0 - 04/27/2025 Initial Release
//
// 1.0.1 - 04/30/2025 Some code clean up: no functional changes.
//
//----------------------------------------------------------------------------------------
</pre>
