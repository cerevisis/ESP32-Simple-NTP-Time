/**

MIT License

Copyright (c) 2022 cerevisis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/


/*
   This example is my simple attempt to simplify NTP for my many projects on the EPS32
   where I mostly need a clean looking 24 hour clock for my display based projects.

   I have added a time sync variance calculator which I find handy when syncing time on complex projects
   where there could be timing delays due to functions taking too much time to run.

   Thank you to Paul Stoffregen https://github.com/PaulStoffregen/Time (NB required library for this to work)
   and https://lastminuteengineers.com/esp32-ntp-server-date-time-tutorial/
   and https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
   which provided the base for this example.

   Time values are saved in String variables. See void setCurrentTime() function if you want to get the integer values instead.
*/

//VERSION NUMBER - UPDATE ON SAVE!//
const char * versionNumber = "V1.1 ESP32 NTP Time Example";

#include <WiFi.h>
#include <TimeLib.h>
#include "time.h"

//WIFI
char ssid[] = "YOUR_SSID";
char pass[] = "YOUR_WIFI_PW";

//NTP Setup
const char* ntpServer = "pool.ntp.org";
unsigned long epochTime;
int epochYear = 1970;
long yourUTC = 2; //Replace with your actual UTC time zone value in hours
long UTCOffset_sec = yourUTC * 60 * 60;
const int daylightOffset_sec = 0;
tmElements_t unixTimeStamp;

//Variables
unsigned long clockTimer = 0;
unsigned long clockSyncTimer = 0;
int weekDayIndex, monthIndex;
bool initialTimeSync = false;

//Date variables
String currMin, currHour, currSec, currTime, currSWeekDay, currLWeekDay, currDay, currMonth, currYear, currSDate, currLDate;
const String Weekday[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
const String shWeekday[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
const String longMonth[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

void setup() {
  Serial.begin(115200);
  Serial.println(versionNumber);

  WiFi.begin( ssid, pass );
  Serial.println("Connecting to wifi");
  unsigned long startAttemptTime = millis();
  while ((WiFi.status() != WL_CONNECTED) && (millis() - startAttemptTime < 10000)) {
    Serial.print(".");
    delay(100);
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi connected!");
  }
  else if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected, restarting!");
    ESP.restart();
  }

  configTime(UTCOffset_sec, daylightOffset_sec, ntpServer);
  epochTime = getTime();
  initialTimeSync = true;
  syncTime(15000); //Synchronise time to NTP server at set millisecond
  initialTimeSync = false;
}


unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

void syncTime(unsigned long syncTimer)
{
  int variance = 0, tempTime = 0;

  if ((millis() - clockSyncTimer >= syncTimer) || (initialTimeSync == true))
  {
    Serial.println("Syncing time from " + String(ntpServer) + " UTC offset: " + String(yourUTC));
    if (getTime() > 0)
    {
      tempTime = getTime();
      variance = epochTime - tempTime;
      epochTime =  tempTime;
      Serial.println("Time Sync [OK] - UNIX EpochTime: " + String(epochTime) + " Variance: " + String(variance));
      initialTimeSync = false;
    }
    else
    {
      Serial.println("Failed to obtain time");
    }
    clockSyncTimer = millis();
    setCurrentTime();
  }
}

void setCurrentTime() {

  if (millis() - clockTimer >= 1000)
  {
    epochTime++;
    breakTime(epochTime + UTCOffset_sec, unixTimeStamp);

    currDay = unixTimeStamp.Day;

    weekDayIndex = unixTimeStamp.Wday - 1;
    currLWeekDay = Weekday[weekDayIndex];
    currSWeekDay = shWeekday[weekDayIndex];

    monthIndex = unixTimeStamp.Month;
    currMonth = longMonth[monthIndex];

    currYear = unixTimeStamp.Year + epochYear;

    /*** If you want to return the integer value instead of using string, use the standard
        functions provided in the TimeLib library (modified to include UTC offset)

        hour(epochTime + UTCOffset_sec);          // returns the hour for the given time t
        minute(epochTime + UTCOffset_sec);        // returns the minute for the given time t
        second(epochTime + UTCOffset_sec);        // returns the second for the given time t
        day(epochTime + UTCOffset_sec);           // the day for the given time t
        weekday(epochTime + UTCOffset_sec);       // day of the week for the given time t
        month(epochTime + UTCOffset_sec);         // the month for the given time t
        year(epochTime + UTCOffset_sec);          // the year for the given time t

     ***/

    if (unixTimeStamp.Minute < 10) {
      currMin = "0" + String(unixTimeStamp.Minute);
    } else {
      currMin = String(unixTimeStamp.Minute);
    }

    if (unixTimeStamp.Hour < 10) {
      currHour = "0" + String(unixTimeStamp.Hour);
    } else {
      currHour = String(unixTimeStamp.Hour);
    }

    if (unixTimeStamp.Second < 10) {
      currSec = "0" + String(unixTimeStamp.Second);
    } else {
      currSec = String(unixTimeStamp.Second);
    }
    currTime = currHour + ":" + currMin + ":" + currSec;

    currSDate = currSWeekDay + "/" + currDay + "/" + monthIndex;

    currLDate = currLWeekDay + " " + currDay + " " + currMonth + " " + currYear;
    Serial.println("Date: " + currSDate + " " + currLDate + " time: " + currTime);

    clockTimer = millis();
  }
}

void loop() {
  syncTime(15000); //Synchronise time to NTP server at set millisecond
  setCurrentTime();
}
