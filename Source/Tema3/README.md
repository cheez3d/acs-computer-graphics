[opengl]: https://en.wikipedia.org/wiki/OpenGL
[demo]: Documentation/demo.webp

# Stylized Runner
An endless runner created from scratch in [OpenGL][opengl]. The objective of the game is to travel as much as possible while trying to keep the top bar filled. The player can collect various **pickups** as he moves. The speed at which the player moves increases as the he travels farther. **Red** cubes start falling as soon as the player steps on them. Below is a small gameplay snippet:

![demo]

## Pickups
| Color |  Effect |
| - | - |
| **Green** | fills top bar |
| **Orange** | depletes top bar |
| **Cyan** | flips the camera to a random orientation |
| **Yellow** | allows player to jump a lot higher while also locking him at full speed for a short amount of time |

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
