# WolfensTerminal
A 3D terminal-based raycaster with multiplayer support.

Submission to the University of Queensland Computing Society's 2025 Hackathon winning a $1000 prize for Best Overall Runner Up.

<img width="410" height="303" alt="image" src="https://github.com/user-attachments/assets/aed09fc4-cc5a-4a5e-8717-18c8b029a6c3" />

As per the rules of the hackathon, this entire project was completed in 48 hours. Some further work has been done on the project since the completion of the hackathon however to keep it seperate from the hackathon submission the development was moved to [this](https://github.com/Zell-Doost/Wolfensterminal) GitHub repo.

## Features
- Fully run from within a terminal
- 3D graphics using raycasting techniques
- Multiplayer support through a UDP network server
- Dynamically applied and scaled object textrues & ASCII sprites
- Dynamically adjusts to terminal screen sizes effectively supporting varied resolutions
- Basic map editor

## Demo
Want to checkout the program without actually running it yourself? I recorded a basic demo that shows off the main functionality. Check it out [here](https://www.youtube.com/watch?v=S64v6coJ--w).

## Running the program
This project was built entierly using C and is designed to be run on any Linux terminal or terminal emulator. There are two main components,

### wolfensterminal
The main application. Handles all of the rendering, movement and player data recieved from the server. This is built using `make` in the root directory. There are a few instructions before use:
- This program must be run with super user permissions. We tried to avoid this at all costs however unfortunately due to the time pressure environment of the hackathon no better solution was found. Essentially super user permissions are required to allow us to detect key presses effectively in the game. Detecting key presses through a terminal environment is notoriously cumbersome and often requires creative workarounds. In our case the only method that we found that worked reliably was to directly accees the `/dev/inputs` file which understandbly is only an action that can be performed by authorised users. For details on exactly how this is implemented see the `fazMain.c` file.
- IP: the application requires a server to run. No port number is required as the port is fixed in the code. Details on how to set up the server can be found below.

### wolfserver
- The sever that the game runs on.
- Sends and recieves player data.
- Built using `make` in the serverClient directory.
- Also builds an excecutable called `wolfclient`, which is an acting client that reports wolfensterminal coordinates and positions (just don't run it again, you'll get the collatz conjecture).
- Server was made using UDP, the goal was to make an FPS deathmatch game in the terminal, so UDP was required. All the protocol code is in `protocol.c`.
- When running the server it will print the port listened to and the IP of the host. The host will be the only one to have access to this so distribute it to clients for wolfensterminal. It will then print whenever it connects and disconnect.

## More Screenshots

Image of the game rendered with a terminal that is more zoomed in effectively making the resolution lower.
<img width="437" height="294" alt="image" src="https://github.com/user-attachments/assets/bfe37164-0d01-45bb-860d-325b6ecb6db9" />

