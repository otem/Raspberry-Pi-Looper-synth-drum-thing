# Raspberry-Pi-Looper-synth-drum-thing
My first venture into raspberry pi stuff. Running a custom pure data patch I've been working on for a couple years on a Raspberry Pi 3. This project took a couple months and I'm still tweaking stuff here and there but it's pretty much complete, it even survived it's first live show!

I've left out the drum samples and Sessions directoroy. To add your own samples just add .wav files to the piLooper directory with names like:

kick_01.wav - kick_24.wav
hh_01.wav - hh_12.wav
snare_01.wav - snare_24.wav
crash_01.wav - crash_04.wav

Or change the sample names to whatever you want in the pd patch. 
The Sessions directory is pointed to where it lives on my pi, you will need to change that to wherever you are storing your patch on your pi.

You will need comport and shell externals in pd.

Here is a list of the main hardware components used (left out some usb male/female connections, push buttons and audio output):
Pi3: https://www.sparkfun.com/products/13825

Teensy 3.6: https://www.sparkfun.com/products/14057

Display: https://www.pjrc.com/store/display_ili9341.html

Mux: https://www.sparkfun.com/products/9056

Buttons: https://www.sparkfun.com/products/7835

Button PCB (I drilled out holes where the leds go to use neopixels): https://www.sparkfun.com/products/8033

NeoPixels: https://www.adafruit.com/product/1612

Encoder: https://www.sparkfun.com/products/9117

Pots: https://www.sparkfun.com/products/9940

Slide Pots: https://www.sparkfun.com/products/11621

Sound Card: https://www.sweetwater.com/store/detail/UCA222




[![IMAGE ALT TEXT HERE](https://i.ytimg.com/vi/_nBK8sAl9nw/0.jpg)](http://www.youtube.com/watch?v=_nBK8sAl9nw)
