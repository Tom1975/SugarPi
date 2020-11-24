# SugarPi

SugarPi is an implementation of Sugarbox for bare-metal raspberry Pi.
As a first version, it emulatse an Amstrad 6128+ without disk drive nor keyboard.

It uses the great [Circle++ library](https://github.com/rsta2/circle) as a pseudo-os basis.

# How to use

## Start
- Unzip the zip file to an empty SD card.
- Fill the CART folder with you favorite CPR files
- Insert the SD card in your Raspberry Pi3 or 4
- Power it : The Amstrad will be immediately displayed.

## Commands
You should have an USB joypad plugged. I tested it with my XBox pad successfully.
A and X button act as button 1 and 2 from the Amstrad pad.
Directional pad is used for controler.
Start is pause, and Select as a way to go into SugarPi settings.

## Changing settings
From anytime, push Select button : it will bring the settings menu on the screen.
From here, you can : 
- Select a cartridge to load : Go into "select cartridge" menu. It will display the CPR files from your CART folder. Select one, then press A : It will load the cartridge, and reset the computer. Note : On the next SugarPi power on, this cartridge will be the default one.
- Select the synchronisation type : 
  - Frame sync will synchronise with the frame. It means that the synchronisation will be smooth, but the timings (and the sound) will be (almost) correct, as long as you are plugged on a 50hz monitor or TV.
  - If frame sync is unchecked, synchronisation will be done on sound : Sound should be more precise, but you can experience some tearing on the display.
