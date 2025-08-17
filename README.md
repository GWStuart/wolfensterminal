# WolfensTerminal
A 3D terminal-based raycaster with multiplayer support.

<img width="1230" height="909" alt="image" src="https://github.com/user-attachments/assets/aed09fc4-cc5a-4a5e-8717-18c8b029a6c3" />

## Features
- Fully run from within a terminal
- 3D graphics using raycasting
- Multiplayer support through a UDP network server
- Dynamically applied and scaled object textrues & ascii sprites
- Dynamically adjust to termianl screen sizes effectively supporting changing resolution

## Running the program
This project was built entierly using C and is designed to be run on any Linux terminal or terminal emulator.
there are two main components:
### wolfensterminal
the main application. handles all of the rendering, movement and player data recieved from the server
   this is built using 'make' in the root directory. there are a few instructions before use:
- sudo: look, we get that you may not trust us, but this is because the library we make (ncurses) does not accept player input,
         so we need to access a dev file containing the inputs. if you don't trust us, the source code is there. the place to look is fazMain.c and the             components that are included (mainly input.c, this is the file that requires sudo). if you still don't trust us, weeelll...
- IP: the application requires a server to run. no port is required, as the port is fixed. details on how to build the server is below.
### wolfserver
the sever that the game runs on. sends and recieves player data. this is built using 'make' in the serverClient directory. this also builds an excecutable called wolfclient, which is an acting client which reports wolfensterminal coordinates and positions (just don't run it again, you'll get the collatz conjecture).
this server was made in UDP, the goal was to make an FPS deathmatch game in the terminal, so UDP was required. all the protocol code is in protocol.c, uncommented (other than the maker going terminally insane (see what I did there)).
when running the server, it will print the port listened to and the ip of the host. the host will be the only one to have access to this, so distribute it to clients for wolfensterminal. it will then print whenever it connects and disconnect.
   
### secret
try sending sigints to the server and main program. you might get a funny message (why am I acting like I don't know)
## More Screenshots

Image of the game rendered with a terminal that is more zoomed in effectively making the resolution lower.
<img width="1312" height="883" alt="image" src="https://github.com/user-attachments/assets/bfe37164-0d01-45bb-860d-325b6ecb6db9" />

