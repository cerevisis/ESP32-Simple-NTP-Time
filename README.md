# ESP32-Simple-NTP-Time

This example is my simple attempt to simplify NTP for my many projects on the EPS32   where I mostly need a clean looking 24 hour clock string for my display based projects.

I have added a time sync variance calculator which I find handy when syncing time on complex projects where there could be timing delays due to functions taking too much time to run.

Thank you to Paul Stoffregen https://github.com/PaulStoffregen/Time (NB required library for this to work) 
   and https://lastminuteengineers.com/esp32-ntp-server-date-time-tutorial/
   and https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
   
which provided the base for this example.

Time values are saved in String variables. See void setCurrentTime() function if you want to get the integer values instead.

NB This example does not take Daylight Savings Time into account, but you can add that quite easily.

Alternate NTP server subzones can be selected from below list (else selected automatically):
Area	         HostName
Worldwide	   pool.ntp.org
Asia	         asia.pool.ntp.org
Europe	      europe.pool.ntp.org
North America	north-america.pool.ntp.org
Oceania	      oceania.pool.ntp.org
South America	south-america.pool.ntp.org
