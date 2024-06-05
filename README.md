# CHOMP 
CHOMP Multiband Clipper is a high-quality, dynamically resizeable, and ergonomic soft clipper for either creative distortion or mastering-grade precision clipping.



https://github.com/yaboypax/PaxMBClip/assets/79543704/0970d540-f3d8-456c-830c-3e5496104033



***

You have a clip function for each of the three bands (as well as one for master bus) to select from three clipping algorithms based on algebraic curves:

<img width="1050" alt="curves" src="https://github.com/yaboypax/PaxMBClip/assets/79543704/20327211-7aa9-4bd1-bfe2-921a7269cc74">


When selecting a band, use the mouse to drag around the crossover points and the ScrollWheel on your mouse to control the clip cieling. Ctrl/Cmd + ScrollWheel applies a band gain. Extra controls are also to the right to be precise with the band crossover points, band gain and clip cieling, next to a master input and output control.

***
## Dependencies / Acknowledgements
This project was written in C++ using the JUCE framework. 
  - Chomp began as a multiband implementation of Free Clip by Jonathan Hyde of Venn Audio (https://gitlab.com/JHVenn/Free-Clip) and uses a modified version of his transfer function algorithms.
  - For additional DSP, modules from Vincent Falco's DSP filter suite (http://code.google.com/p/dspfilterscpp/)
  - as well as Jatin Chowdhry's chowdsp_utils (https://github.com/Chowdhury-DSP/chowdsp_utils) were ported.
  - Also, ff_meters from Daniel Walz / Foleys Finest Audio UG (https://github.com/ffAudio/ff_meters). 

## License
CHOMP is licensed under the GPLv3.
