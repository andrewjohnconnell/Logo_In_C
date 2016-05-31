
#Turtles
Back in the late 80s, when I started primary school, I was first introduced to computers by learning Logo.  

I've decided to come back and recreate one of those childhood loves in C, using SDL.  This is basically just a recursive descent parser for the Logo grammar (I've made a couple of little amusing tweaks, which are documented in the source code -> avid Logo lovers might like to check out Camouflage [which goes much better with a bottle of Rioja or Syrah! ;-)]).

I've built this out as an interpreter and placed some example files in the directory as well.


## How to run
For Linux users, I've included a makefile (it SHOULD work on OSX as well -> but don't quote me on that).
Please make sure you've got SDL2 installed on your system before you try running this.

To execute on the command line, simply use: ./turtles <filename> or ./turtles <filename> <extension tags> as required.
