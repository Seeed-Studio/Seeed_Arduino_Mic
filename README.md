# Seeed Arduino Mic

<p align="center">
<img alt="logo" src="resources/mic2.jpg" width="30%">
</p>

Seeed Arduino Mic is an extensible C++ library for interfacing Arduino-compatible hardware devices with microphone sensors for sound recording and further processing, e.g. calculating FFT, MFCC.


## Installation

The library is in development stage, once it is ready for release it will be made available in Arduino libraries index. Currently you can install latest development version by downloading ZIP archive of repository or cloning it directly to your Arduino libraries folder.

```bash
cd [Arduino_sketch_folder/libraries]
git clone https://github.com/Seeed-Studio/Seeed_Arduino_Mic.git
```

## Usage

For using the library, have a look at provided examples (currently tested and working with Seeed Studio Wio Terminal internal microphone):

### Sound plotter

<p align="center">
<img alt="plotter" src="resources/plotter.gif" width="100%">
</p>

Press on Button C to start audio acquisition for 3 seconds. Sound data will be output using Serial connection and can be viewed as raw numbers in Serial monitor or as graph in Serial plotter.

### Sound recorder

Install pyserial then upload the example code to your device. Run 
```python3 recording.py```
and follow the interactive prompt instruction. You'll find sound files with recordings in your current working direcotry.

### FFT visualizer

<p align="center">
<img alt="fft" src="resources/fft.gif" width="100%">
</p>

Upload the example script to see FFT calculated in real time and visualized on your device's screen. Pretty!


## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License
[Apache](https://github.com/Seeed-Studio/Seeed_Arduino_Mic/blob/master/LICENSE)

