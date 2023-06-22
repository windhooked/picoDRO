# Simple DRO with bluetooth for TouchDro

The pico has some interesting hardware modules like PIO and interpolator, with CORDIC math functions in ROM
Fractional dividers? Not sure if this could be used to generate accurate step outputs?

This would allow for efficient implementation of electronic lead screw ELS, or gear hobbing controller

Concept seems simple capture and count Quadrature pulses divide/multiply by ratio and genearate stepper counts

Just learning as I go along, starting with the simple task of driving TouchDRO with counts for X,Y,Z

Next would be to implement an efficient processing pipeline for ELS

who knows I might even attempt adding the gcode G76 Scre Thread Cycle to grblHAL  

QDEC -> DMA - CALC -> DMA PIO

https://github.com/grblHAL/RP2040
https://en.wikipedia.org/wiki/CORDIC 
https://github.com/raspberrypi/pico-bootrom/blob/master/bootrom/mufplib.S
"50 Years of CORDIC: Algorithms, Architectures, and Applications" by Meher et al., @ IEEE Transactions on Circuits and Systems Part I, Volume 56 Issue 9.

https://media.unpythonic.net/emergent-files/projects/01149271333/mkstate.py

https://github.com/raspberrypi/pico-examples

https://gregchadwick.co.uk/blog/playing-with-the-pico-pt4/

https://gregchadwick.co.uk/blog/playing-with-the-pico-pt2/

https://vanhunteradams.com/Pico/TimerIRQ/SPI_DDS.html

https://people.ece.cornell.edu/land/courses/ece4760/RP2040/C_SDK_interpolator/index_interpolator.html

https://vanhunteradams.com/FixedPoint/FixedPoint.html

https://github.com/rotorvator/quadrature-decoder

https://www.hackster.io/ahmsville/raspberry-pi-pico-pio-stepper-motor-control-1ec3e9

https://forums.raspberrypi.com/viewtopic.php?t=337719

https://www.patreon.com/posts/60027249

https://github.com/atelierdbp/Digital-Hobbing-Code

https://github.com/ahmsville/Pi-Pico-PIO-Stepper-Controller/

https://github.com/clough42/electronic-leadscrew

http://rysium.com/images/stories/projects/story_arduino-dro-q/download/ArduinoDRO_Tach_V6_0.ino

https://github.com/stephenhouser/Arduino-DRO/blob/master/src/Arduino-DRO.ino
