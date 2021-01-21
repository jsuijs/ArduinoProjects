@echo off
rem This script sets the prefences.txt of arduino for this sketch
rem Suggested name: ArduinoSettings.bat

rem See python script (below) for details.

rem ******************************************
rem This is a generated script, do not modify.
rem ******************************************

set sketchdir=C:\MyRobot\Arduino\SketchbookSTM32\MaqueenPlus_i2c
set sketchname=MaqueenPlus_i2c.ino
set fqbn="STM32:stm32:GenF1:pnum=Generic_F103C8,upload_method=swdMethod,xserial=none,usb=none,xusb=FS,opt=osstd,rtlib=nano"

C:\RobotLib\Tools\Arduino\ArduinoSettings.py %fqbn%

if %ERRORLEVEL%==0 goto done
C:\MyRobot\Arduino\arduino-1.8.13\arduino_debug --board %fqbn% --save-prefs 2>out.txt
if %ERRORLEVEL%==0 goto bijgewerkt

type out.txt
echo *****************************
echo ERROR - er is iets misgegaan.
echo *****************************
pause
goto done

:bijgewerkt
echo Instellingen IDE bijgewerkt.
pause
:done
