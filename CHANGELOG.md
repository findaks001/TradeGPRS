Code for TraDe ADAM Device

## v 4.1.1 (2016-09-19)
### Bug-fix
1. Previously dataread was struck after 2 or 3 reads due to garbage in buffer. Now buffer is cleared everytime.

## v 4.1.0 (2016-09-19)
### Added
1. 4-20 mA Error Added (It will send the negative of the 4-20 mA value if the value received is less than 4 or more than 20)

## v 4.0.0 (2016-09-19)
### Added
1. RS-485 read error added.

## v 2.5.0 (2016-09-19)
### Merged
1. This single code will now be applied to both SIM800 and SIM900 with the change of the library "PROCESS.h" only.
### Modified 
1. DataRead() - while(1) is replaced by millis() based time-out.

## v 2.4.1 (2016-09-10)
### Bug Fix
1. Random number problem if SPCB_ID % 101 is less than 0 then random number is not generated. This is resolved here.
### Removed
1. EEPROM SMS to start and stop is removed.

## v 2.4.0 (2016-08-17)
### Added
1. If nothing is received in RS-485 then send random noise to tackle SPCB "0 non-detection problem".
### Removed
1. RS-485 health-status (because it would send 0 to SPCB server which is unacceptable)

## v 2.3.0 (2016-07-08)
### Added
1. SMS control (Password and mobile number which is stored inside the EEPROM is encrypted by AES-128 Encryption).

## v 2.2.0 (2016-07-07)
### Added( Major)
1. SMS control (Password recovery or reset by ADMIN only)
### Bug Fix
1. DataRead() function - while(1) is replaced. with millis() to wait for data read only 10 seconds.
2. If any of the parameter string is empty or "", then send "error" in place of data.

## v 2.1.0 (2016-07-06)
### Added( Major - control)
1. SMS control (Now data send can be stopped and resumed with the use of SMS and master Password)

## v 2.0.0 (2016-07-01)
### Added
1. Trade SPCB version (SPCB server always connected and our server connection open and close)
2. Same data is going to both servers.

## v 0.6.0 (2016-05-10)
### Added
1. RESET SIM 900 added. If network is gone for a while RESET the modem.

## v 0.5.0(2016-05-09)
### Added
1. '*' and '#' before sending data to spcb and if a parameter is coming -ve (i.e. less than 4 mA or greater than 20 mA then send 0 )

## v 0.4.1(2016-04-14)
### Bug Fix
1. Serial.flush() included otherwise TraDe was unable to send completely. line no 81

## v 0.4.0 (2016-04-13)
### Features
1. Re-organization of code for easy modification.
2. Automation of Configuration.


## v 0.1.2 (2015-12-10)
### Features

 1.This code is currently implemented in most of industry version
 2.In next version major upgrade will be there   
 3.No change with respect tp  v 0.1.1  All the irrelevant functions are deleted

## v 0.1.1 (2015-12-10)
### Features
1.compatible with "PSEXTRACT.h" compatible with v 0.1.1 if "PSEXTRACT.h" only
2.In  php when date is received if yy is 2 digit format thenreceived year is exchanged with 3date (ie dd becomes yy and yy becomes dd)
3.this bug is fixed in this version.
4.Date format is mm/dd/yy or mm/dd/yyyy

## v 0.1.0 (2015-12-09)
### Features
1. Created GIT repository for the project - Initial commit
2. sends AES 128 encrypted data in RAW TCP to CPCB server and normal data in HTTP format to Phoenix Robotix Server.
3. User sets the device no, device ID, device key manually.
4. User sets the delimiter,start delimiter, separater by himself
5. Data extraction using "PSEXTRACT.h" compatible with v 0.1.0 if "PSEXTRACT.h" only
6. Compaible with v 0.1.0 of "ISAGSM.h"
6. Autometic SIM detection
7. Basic standard version
8. Data transmission occurs fine (tested 4000 datapoint over 12 hours)
9. Reliability issue as code haas to be modified by installer each time.