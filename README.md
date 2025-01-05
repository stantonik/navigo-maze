# Navigo-Maze
> Navigo-Maze is a puzzle adventure game set in Paris, where you must navigate the city's maze-like streets and Métro system without a Navigo pass.

## Table of Contents
- [Synopsis](#synopsis)
- [Features](#features)
- [How to Play](#how-to-play)
- [Installation](#installation)
- [Dependencies](#dependencies)
- [License](#license)

## Synopsis
After a wild Parisian soirée, you find yourself in the heart of the City of Light, disoriented and with no Navigo pass. The city's maze-like streets and Métro lines stretch before you like an impenetrable labyrinth. You must gather clues scattered across iconic Parisian neighborhoods, solve puzzles, and outwit the Métro controllers who patrol the city.

You must piece together cryptic messages, forgotten notes, and whispers from locals to get to your rendezvous on time. Be careful: one wrong turn could send you deeper into the labyrinth, and the clock is ticking!

<!-- **DEMO** -->
<!-- ![demo](./assets/demo.gif) -->

## Features
- **Iconic Parisian neighborhoods:** Explore famous locations such as Montmartre, Châtelet, and more!
- **Challenging puzzles:** Solve intricate puzzles that require logic and exploration to advance.
- **Time pressure:** The clock is ticking, and you need to make it to your meeting on time.
- **Stealth elements:** Avoid Métro controllers who patrol the city looking for fare evaders.
- **Mystery and exploration:** Discover hidden messages and cryptic notes to guide you on your journey.

## How to Play
- Use the arrow keys or WASD to move around the Parisian streets.
- Interact with NPCs and objects to gather clues and information.
- Solve puzzles by following the clues found in your surroundings.
- Watch out for the Métro controllers—they will catch you if you're without a valid pass!
- Keep an eye on the timer as it counts down toward your rendezvous.

## Installation
> You can download the precompiled binaries for Windows and MacOS in the release section. However, if your platform is not listed, you can choose to build the project.

### Prerequisites
Please install GLEW and GLFW manually and edit the Makefile.

### Building the game
1. Clone and build the repository
```bash
git clone --recurse-submodules https://github.com/stantonik/navigo-maze.git
cd navigo-maze
make
```
2. Launch the game

You will find the binary in `./build/$YOUR_PLATFORM`. Good game !

## Dependencies
The game relies on the following third-party libraries:

- **GLEW:** OpenGL Extension Wrangler for handling OpenGL extensions.
- **GLFW:** Window and input management library.
- **cglm:** Lightweight math library for 3D applications.
- **jsmn:** A lightweight JSON parser.
- **stb:** A collection of single-file libraries for various purposes (e.g., image loading).

## License
This project is licensed under the Apache-2.0 License - see the [LICENSE](./LICENSE) file for details.
