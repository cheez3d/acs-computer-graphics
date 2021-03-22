[skyroads]: https://classicreload.com/skyroads.html
[opengl]: https://en.wikipedia.org/wiki/OpenGL
[demo]: Documentation/demo.webp

# Skyroads
A recreation of the game [SkyRoads][skyroads] in [OpenGL][opengl]. The objective of the game is to travel as much as possible while trying not to run out of fuel. The speed at which the player moves increases as the he travels farther. Below is a small gameplay snippet:

![demo]

## Platforms
| Color |  Effect |
| - | - |
| **Blue** | no special effect |
| **Green** | increases player's fuel |
| **Orange** | decreases player's fuel |
| **Lime** | causes the player to be locked at full speed for a short amount of time |
| **Cyan** | flips the camera to a random orientation |
| **Red** | starts falling as soon as the player steps on it |

## Controls
| Key | Action |
| - | - |
| `W` | move faster |
| `S` | move slower |
| `A` | move to the column to the left |
| `D` | move to the column to the right |
| `P` | pause the game |
| `Mouse` | move the camera around |
| `Mouse Scroll Wheel` | zoom in and out, enter/exit first person mode |
