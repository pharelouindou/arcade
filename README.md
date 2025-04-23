# Arcade Project

## Overview
The Arcade project is a gaming platform that implements multiple display libraries and games using a plugin architecture. The project demonstrates object-oriented design patterns and dynamic library loading in C++.

## Architecture

### Core Components
- **Core Engine**: Manages game states and library switching
- **Dynamic Library Loader**: Handles loading and unloading of game and graphics libraries
- **Event System**: Unified input handling across different display libraries

### Display Libraries
1. **SFML**
   - Modern graphics library with hardware acceleration
   - Full window management and event handling
   - Text rendering with TTF fonts

2. **SDL2**
   - Hardware-accelerated graphics
   - Cross-platform window management
   - Event handling system

3. **NCurses**
   - Terminal-based display
   - Character-based graphics
   - Keyboard input handling

### Games
1. **Snake**
   - Classic snake gameplay
   - Collision detection
   - Score system
   - Level-based difficulty

2. **Pacman**
   - Ghost AI implementation
   - Dot collection mechanics
   - Wall collision system
   - Score tracking

3. **Nibbler**
   - Similar to Snake but with obstacles
   - Level progression
   - Increasing difficulty
   - Wall collision system

## Building and Running

### Prerequisites
- C++17 compatible compiler
- SFML development libraries
- SDL2 development libraries
- NCurses development libraries
- Make build system

### Build Instructions
```bash
make            # Build everything
make core       # Build core only
make graphicals # Build display libraries
make games      # Build game libraries
```

### Running the Program
```bash
./arcade ./lib/arcade_sfml.so    # Start with SFML
./arcade ./lib/arcade_sdl2.so    # Start with SDL2
./arcade ./lib/arcade_ncurses.so # Start with NCurses
```

## Controls
- **Arrow Keys**: Movement
- **Enter**: Select/Start
- **ESC**: Quit
- **P**: Pause
- **M**: Menu
- **N/B**: Next/Previous graphics library
- **[/]**: Next/Previous game

## Project Structure
```
.
├── src/
│   ├── core/           # Core engine components
│   └── interfaces/     # Common interfaces
├── lib/
│   ├── graphicals/     # Display libraries
│   │   ├── SFML/
│   │   ├── SDL2/
│   │   └── NCurses/
│   └── games/         # Game implementations
│       ├── Snake/
│       ├── Pacman/
│       └── Nibbler/
└── assets/
    └── maps/          # Game maps
```

## Implementation Details

### Game Class Hierarchy
```
IGame (Interface)
├── Snake
├── Pacman
└── Nibbler
```

### Graphics Class Hierarchy
```
IGraphical (Interface)
├── SFMLGraphical
├── SDL2Graphical
└── NcursesGraphical
```

## Contributing
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## License
This project is part of the Epitech curriculum and is subject to Epitech's academic rules.