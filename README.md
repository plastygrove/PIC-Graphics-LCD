Running a 128x64 JHD12864E Graphics LCD with a PIC18F4550 microcontroller. This code is written completely from scratch and doesn't use any of the earlier code written in the repo AVR-Graphics-LCD

Current Features
================
1. Print a string of characters to the screen in different fonts
1. Take a 128x64 1-bit image and display it to the screen
1. Display the time from a DS1307 display
1. Output is multiplexed using a 74HC595 shift register
1. Working pong clock (needs a little bit of improvements, but works pretty darned well :))
1. Ability to adjust time by long press of a button

Here is a quick video:
https://www.youtube.com/watch?v=VhGNs9608oI

Components Included
===================
1. PIC18F4550 - Using this microcontroller because it has 40 Pins and I had a few lying around
1. DS1307 Realtime Clock (RTC) - The clock chip for keeping the time
1. 74HC595 Shift register - Using this for the data bus to the LCD. Instead of 8 pins for the data bus, I just need 3 now (data, clock and latch)
1. 20/24 MHz crystal - Oscillator for the microcontroller
1. 32.768 KHz crystal - Oscillator for the DS1307
1. CR2032 battery - For keeping the clock powered
1. 10K Trimpot - To adjust the LCD contrast
1. Resistors and capacitors - 1K, 10K resistors and 1uF capacitors for use with hardware debounced switches

See circuit_pic.jpg for a quick photo of the circuit
