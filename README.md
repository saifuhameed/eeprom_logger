# eeprom_logger

As a Primary logging system
-------------------------------------------
Since eeprom size is usually small in range of 1kbits to 128kbits, we  can't log a large number of data per day in this logger.
But if you have a very few data (say 1 to 24 samples per day) you are good to go
eg: If you use commonly available 24C32 which comes along with RTC, you can log upto 256 days for 1spd (sample per day) or 10 days for 24spd. This can be more than 2.75 years or 42 days respectively if you use 24C128

If you log data every minute then this system is not suitable to use as a primary logging system, but check the next section secondary/fall back logging system.

As a Secondary logging system (fall back logging)
-------------------------------
This datalogger could be used as a fall back logging system working along with your existing remote data logging system when server or internet is down. eeprom data can be uploaded to server easily when internet / server is available.
This is usefull as we can avoid loss of data when server is down even for few minutes or loss of of internet.
