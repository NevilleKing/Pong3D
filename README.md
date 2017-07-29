# Pong 3D
![pong3d](https://user-images.githubusercontent.com/9254173/28749451-bbdbf1fe-74bf-11e7-9e2b-ea032f7c6aa9.png)

A 3D version of Pong created using OpenGL and SDL written in C++. This was submitted for the Graphics assignment at the University
of Lincoln. Also see the [2D version of Pong](https://github.com/NevilleKing/Pong2D) submitted for another assignment for this module.

## Playing the Game
The easiest way to download the game is via the [releases page](https://github.com/NevilleKing/Pong3D/releases/latest).

If you have visual studio, you can also generate an sln file by using premake from the command line:

    premake5.bat vs2015
    
Controls are:

###### Player 1
`A` - Left

`S` - Right
###### Player 2
`Left Arrow` - Left

`Right Arrow` - Right

You can also change the camera angle using `C`, between 3 angles: behind player 1, behind player 2 and a long shot angle.

## Dependencies
The game uses the following dependencies:
- glew
- premake
- SDL2
- glm
