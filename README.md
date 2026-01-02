# SimulationGame
Attempt at creating a 3D simulation game.

## Roadmap

- [] Core Engine
    - [] Basic UI Elements
    - [x] 2D Rendering (sprites/text)
    - [] 3D Rendering (meshes/materials)
    - [] Audio
    - [] Asset Loading (textures/images/meshes/materials/fonts/models/sounds)
- [] Main Menu
    - [] Settings Menu
    - [] Character Creation Menu
    - [] World Creation Menu
    - [] Start Game!
- [] Gameplay
    - [] pause menu
    - [] chunk loading/saving
    - [] player controls/interaction
    - [] game time
    - [] game mechanics
- [] ???


## Setup Development on Windows

In power shell, use winget to obtain dependencies:

```powershell
# git on windows
winget install --id Git.Git
winget install ssh

# VS Code
winget install code

# Build tools
winget install cmake
winget install GnuWin32.make
winget install GnuWin32.grep
winget install GnuWin32.File
winget install python3
winget install ninja
```

After installing tools with winget, add the GNU tools, `C://Program Files(x86)/GnuWin32/bin` to system Path variable.

Once tools are added to Path, install required compilers.

todo: Might be worthwhile to create a build container to streamline development for Linux/Windows.