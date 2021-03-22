[bow-and-arrow]: https://classicreload.com/win3x-bow-and-arrow.html
[opengl]: https://en.wikipedia.org/wiki/OpenGL
[demo]: Documentation/demo.webp

# Bow and Arrow
A recreation of the classic [Bow and Arrow][bow-and-arrow] game in [OpenGL][opengl]. All meshes are created programatically. Collision support between arbitrary objects is present, accompanied by a simple physics engine for applying gravity to objects. The objective of the game is to pop the **red** balloons while avoiding popping the **yellow** ones. The player also has to avoid getting hit by the **shurikens** which are constantly being thrown at him, but can also shoot them if he reacts fast enough. The difficulty of the game increases as the player pops more **red** balloons and hits more **shurikens**. Below is a small gameplay snippet:

![demo]

## Controls
| Key | Action |
| - | - |
| `W` | move the bow up |
| `S` | move the bow down |
| `P` | pause the game |
| `Shift` | modifier for `W` and `S` which makes the bow move faster |
| `Alt` | modifier for `W` and `S` which makes the bow move slower |
| `Mouse` | aim with the bow |
| `Left Mouse Button` | fire arrow with velocity based on how much the button was held |
