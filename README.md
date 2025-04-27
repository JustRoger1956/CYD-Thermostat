# CYD-Thermostat
CYD Example with Three Pages - General Information, Weather, and Thermostat

<br />//****************************************************************************************
<br />//  I hope you find this code helpful. 
<br />//
<br />//  In general this has been a very satisfying endeavor.  Of course, I relied on the 
<br />//  Random Nerd Tutorials, GitHub, online LVGL documentation, and WEB searches for
<br />//  examples and explanations.
<br />//
<br />//  I already have another thermostate working based on and esp8266 that uses the Blynk 
<br />//  IOT interface on my phone to change settings.  My current goal is to use ESP-NOW to
<br />//  interface the CYD to that esp8266 to control the settings.
<br />//
<br />//  Because of the limited IO on the Cheap Yellow Display (CYD) it would be difficult
<br />//  to implement it as a standalone thermostate.  One possiblity is to use a ESP-NOW
<br />//  to communicate to another ESP32 mini that controls intermediate relays that interface
<br />//  to the to the contol board for heat off and cool much like a hardwired thermostat.
<br />//
<br />//                               ****** IMPORTANT *****
<br />//                               ****** IMPORTANT *****
<br />//                               ****** IMPORTANT *****
<br />//
<br />//  Follow one of the Random Nerd Tutorials for the cheap yellow display (CYD).  It will
<br />//  instruct you how to install the initial libraries and setups files.  In addition:
<br />//    (a) Search for credentials in the code and input your network WiFi credentials.
<br />//    (b) Go to the Tools tab to select board ESP32-2432S028R CYD
<br />//    (c) On the Tools tab also select Partition Scheme Huge APPC3MB No OTA/1MB SPIFFS.
<br />//    (d) The programs takes a long time to compile and load; be patient.
<br />//    (e) At some time you will want to update the Time Zone and Weather Station
<br />//        WEB pages for your specific needs; however I suggest doing that after the 
<br />//        initial load.
<br />//
<br />//
<br />//  Screen Notes:
<br />//    (1) Clicking the button at the top center of the screens will cause the display
<br />//        to switch screens.
<br />//    (2) The increase and decrease buttons disabled when "Off" is selected.
<br />//    (3) When "Off" is selected the temperature setpoint is set to the default value.
<br />//    (4) The decrease [-] and increase [+] button will be blacked out when the 
<br />//        minimum or maximum temperature setpoint is reach.  This shows they are 
<br />//        disabled.
<br />//
<br />//  Regards and Good Luck,
<br />//  JustRoger1956
<br />//  April 27, 2025
<br />//****************************************************************************************
