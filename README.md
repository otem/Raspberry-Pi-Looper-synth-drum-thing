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

Nintendo DS Screen: https://www.sparkfun.com/products/retired/13631

Display: https://www.pjrc.com/store/display_ili9341.html

Mux: https://www.sparkfun.com/products/9056

Buttons: https://www.sparkfun.com/products/7835

Button PCB (I drilled out holes where the leds go to use neopixels): https://www.sparkfun.com/products/8033

NeoPixels: https://www.adafruit.com/product/1612

Encoder: https://www.sparkfun.com/products/9117

Pots: https://www.sparkfun.com/products/9940

Slide Pots: https://www.sparkfun.com/products/11621

Sound Card: https://www.sweetwater.com/store/detail/UCA222

3D Model: http://www.thingiverse.com/thing:2318194




[![IMAGE ALT TEXT HERE](https://i.ytimg.com/vi/_nBK8sAl9nw/0.jpg)](http://www.youtube.com/watch?v=_nBK8sAl9nw)


## Pinouts

#### Teensy 3.6
Pin | Description 
--- | ---
0	|	Encoder Button
1	|	Encoder
2	|	Encoder
3	|	Mux control pin s0
4	|	Mux control pin s1
5	|	Mux control pin s2
6	|	Mux control pin s3
7	|	Input #1 Button
8	|	Input #2 Button
9	|	TFT - DC 
10|	TFT - CS 
11|	TFT - SDI(MOSI)
12|	TFT - SDO(MISO)
13|	TFT - SCK
14|	Mux Control Sig pin
15|	Main Volume Pot
16|	Let Input Pot
17|	Right Input Pot
18|	Post Effects Reverb Pot
19|	Post Effects Bitcruncher Pot
20|	Post Effects Volume Slider
21|	NintendoScreen yPin1
22|	NintendoScreen xPin2
23|	LEDs
24|	Clear Button
25|	Play/Stop Button
26|	Instrument Select Button
27|	Robo-drum Button 
28|	Loop 1 Button
29|	Loop 2 Button
30|	Loop 3 Button
31|	NintendoScreen yPin2
32|	NintendoScreen xPin1
33|	Loop 4 Button
34|	Loop 5 Button
35|	Loop 6 Button
36|	Loop 7 Button
37|	Loop 8 Button
38|	Drumpad Column 
39|	Drumpad Column
40|	Drumpad Column
41|	Drumpad Column
42|	Drumpad Row
43|	Drumpad Row
44|	Drumpad Row 
45|	Drumpad Row 

#### Mux
Pin | Description 
--- | ---
0|	Delay - Time Slider
1|	Delay - Feedback Slider
2|	Reverb Slider
3|	Cutoff Slider
4|	Ring Mod Slider
5|	Bit Crusher Slider
6|	Distortion Slider
7|	Retrig Slider
8|	Loop 1 Volume Pot
9|	Loop 2 Volume Pot
10|	Loop 3 Volume Pot
11|	Loop 4 Volume Pot
12|	Loop 5 Volume Pot
13|	Loop 6 Volume Pot
14|	Loop 7 Volume Pot
15|	Loop 8 Volume Pot


