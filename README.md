# CSCI3161 FinalProject - Flight Simulator
Maria Jessen B00743170

__HEY__ - Are you a current CSCI3161 student? Were you assigned this _exact_ project too?  
__STOP!__ Plagiarism will get you nowhere! It's not worth it! Please take the time to develop your own solution. Do not copy mine! They check GitHub!

Allows the user to "fly" through a scene, controlling the camera/plane with mouse motions and keyboard entries. 

![Demo of gameplay](demo.gif)

## Basic Functionality
### Controls Overview
```
Scene Controls
------------------
f: toggle fullscreen
g : toggle fog
m : toggle mountains
t : toggle mountain texture
s : toggle sea & sky
w : toggle wire frame
x : toggle snow mode
r : toggle rain mode
q : quit

Camera Controls
---------------- -
Page Up : faster
Page Down : slower
Up    Arrow : move up
Down  Arrow : move down
Mouse Right : move right
Mouse Left : move left
```
 
### Plane Motion
The plane and propellers are loaded from text files. Their vertexes, normals and faces are stored using array and LinkedLists. The propellers are positioned by finding the center
point of each hub (the part of the plane where the propellers attach) and translating the propeller objects to those points.

The plane remains in constant forward motion. Users control the yaw of the plane (turning) by moving the mouse in the direction they want to turn.
Plane direction is determined trigonometrically using a function that treats places the plane's current and future positions at opposite ends of the hypotenuse of a right triangle and 
determines the X- and Z-offsets of the camera position using an angle produced from the mouse's x-position on the screen. The plane's position in relation to the camera and the camera's 
focal point are determined using the new camera position and a similar triangle to that used to determine the camera position.
  
Plane speed can be increased by hitting the `PAGE-UP` key or decreased by hitting the `PAGE-DOWN` key. The plane cannot stop or move backwards.
  
Plane height can be increased by hitting the `ARROW-UP` key and decreased by hitting the `ARROW-DOWN` key. 

### Basic Scene
The basic scene is composed of a grid of squares and an axis and origin marker. This and the "Sea, Sky & Land" scene can be shown in solid or wireframe mode by hitting the `w` key.
  
### Sea, Sky & Land Scene
#### Sea and Sky
The advanced scene can be activated by hitting the `s` key (the user can return to the basic scene by hitting the same key again). 
This scene is composed of a textured disk for the sea and a textured cylinder for the sky. Both textures are loaded from files provided with the project instructions.
  
The sky texture is tiled several times to wrap around the cylinder. The sea texture is simply cut by the disk to fill the disk.
  
#### Mountains
The mountains are generated using a recursive function that produces an array of points in which each has had a random amount of height added to it. Edge points are pulled down to reduce open
gaps in the side of mountains. 
  
The mountains can be shown textured by hitting the `t` key or simply coloured. The texture is loaded in the same way that the sky and sea textures are. 
When in simple coloring mode, the mountains are colored using a function that applies white to the top of the mountain, a layer of grey below the white, then randomly-generated shades of green
for the lower bulk of the mountain. The coloring function looks at each point in the mountain point array and attaches a color to that point based on its height. 
  
Mountains are scaled and translated randomly when drawn on the scene. 
  
## Special Functionality
### Information Text
The information text displays an arbitrary speed and altitude. The speed is determined using amount the plane moves forward in each frame and a factor to make the speed appear more "realistic".
The altitude is determined in similar way but using the height of the camera instead of the forward travel amount. 
  
The information text also shows the current weather conditions, either "CLEAR", "CLOUD", "RAIN" or "SNOW".
  
The information text could not be placed simply: like the camera, plane and focal point, the text position has to be determined trigonometrically. It follows the same triangle scenario used to 
position the plane, but uses the focal point as the initial position and adds offsets determined trigonometrically for each letter drawn. 
 
### Weather Modes
Three weather modes exist in addition to the normal sunny scene: cloud, rain and snow.
  
When any of these modes is activated, the sky changes from bright to cloudy and grey. This effect is acheived by drawing another sky cylinder behind the original one and fading the original cylinder
out of view once the rain/snow mode is activated, thus making it look like the dark clouds have formed suddenly but not sharp or abruptly.
  
The sea and sea fog colors also change to grey to look more stormy simulate low cloud cover.
 
### Cloud Mode 
The cloud mode just causes the sky to cloud over and the sea and sea fog to turn grey. There is no precipitation in cloud mode.

#### Rain Mode
The rain mode causes "raindrops" in the form of tiny, translucent line segments to be drawn and move down from the top of the scene. The raindrop values are generated and stored before the animation 
loop starts, and are updated at each frame of the animation to make the raindrops fall.
  
When a raindrop reaches the water, it produces a small, circular ripple. The ripples are drawn using a circle function and increasing radius value.
The raindrop's height is then reset to its original height so that it can fall again. 

#### Snow Mode
The snow mode is very similar to the rain mode (snowflakes fall in the same way, reset in the same way and cause ripples on the water in the same way) but are drawn differently.
When first generated, the snowflakes are stored as a center point, which the then used to draw the arms of the snowflake. 
When drawn, the snowflakes rotate so that the appear to wobble and dance as they drift down.

In addition, when snowflakes begin reaching the water, fog will begin to obscure the plane and mountain to simulate the accumulation of snow. The fog fades in slowly to make it look
like the snow builds up gradually.

## Sources
Alternate Sky Image: http://www.benkyoustudio.com/Textures/TexturesSnowAndIce/
