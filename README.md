# Afterburn - No Escape

## Game Description

**Afterburn - No Escape** is a 2D top-down vehicular combat and arcade action game created using the **iGraphics** library in C/C++. Set on an infinitely scrolling highway, players control an armed vehicle, fending off waves of enemy cars, heavily-armed special units, and challenging bosses. The game features an endless loop system where difficulty scales up after completing all stages, challenging players to achieve the highest score possible for the leaderboard. This project demonstrates basic graphics programming concepts like drawing shapes, handling user input, concepts of file-management, code modularity using header files and simple animations.

## Features
- **360-Degree Aiming:** Smooth rotational aiming using the mouse to shoot enemies dynamically.
- **Progressive Stages & Loops:** Three distinct stages with increasing difficulty loops and seamless cloud transitions.
- **Dynamic Boss Fights:** Three unique multi-phase boss encounters (Police Boss Car, Tank, and Helicopter), each with varied attack patterns like spread missiles and bomb drops.
- **Keyboard and Mouse Support**
- **Dynamic Enemy and NPC AI:** Three distinct special enemy types with their own set of behaviors. NPC characters are also added with their own set of behaviors.



## Project Details
IDE: Visual studio 2013

Language: C,C++.

Platform : Windows PC.

Genre : 2D Top-Down Vehicular Combat


## How to Run the Project

Make sure you have the following installed:
- **Visual Studio 2013**
- **MinGW Compiler** (if needed)
- **iGraphics Library** (included in this repository)


Open the project in Visual Studio 2013
- Open Visual Studio 2013.
- Go to File → Open → Project/Solution.
- Locate and select the .sln file from the cloned repository.
- Click Build → Build Solution
- Run the program by clicking Debug → Start Without Debugging


## How to Play

### **Controls**
| Action | Input / Key |
|--------|------------|
| **Move Up** | `W` |
| **Move Down** | `S` |
| **Move Left** | `A` |
| **Move Right** | `D` |
| **Aim Weapon** | Mouse Cursor |
| **Fire Cannon** | `Left Mouse Click` / Hold `Space` |
| **Fire Heavy Missile** | `Right Mouse Click` (requires ammo) |
| **Pause Game** | `ESC` |
| **Restart (Game Over)** | `R` |


### **Game Rules**

- **Survive and Score:** The player starts with 100 health points. Points are earned over time by driving and by destroying enemy vehicles.
- **Avoid Collisions & Hazards:** Getting rammed by enemy cars or hit by enemy projectiles reduces health. Driving over oil puddles will cause a temporary loss of vehicle control.
- **Avoid Civilians:** Ramming or shooting civilians will penalize both the score and health of the player.
- **Boss Fights:** At specific score thresholds, a warning will appear, and a boss will engage. Score accumulation pauses during boss fights.
- **Collection of Supplies:** During boss fights, Supply Trucks will drop missiles which are essential for defeating the bosses. Also after a boss fight has concluded a Supply Truck will drop a health orb which will get the player back to full HP before moving to the next stage.
- **Endless Loop:** Defeating the final boss (Helicopter) loops the game back to Stage 1 with scaled-up enemy speeds, damage, and health. The game ends when player health reaches 0.


## Project Contributors

1. **Samin**
2. **Mushfiq**
3. **Adib**
4. **Zamil**


## Screenshots

### **Menu**
<img src="https://github.com/user-attachments/assets/75dc26d6-5bea-4c17-84fa-ca22af69e349" width="1920" height="1080">

### **Character**
<img src="https://github.com/user-attachments/assets/bc7a1111-64f0-4822-8d71-e144bb2e7ca2" width="1800" height="1800">

## Youtube Link
[CSE 1200 Project: Afterburn - No Escape](https://youtu.be/yCqrMYrr-1M)

## Project Report
[Project Report: Afterburn - No Escape](https://drive.google.com/file/d/1QklqQxyZ0LnkwYwFfpz1u5FobKDMBdo_/view?usp=sharing)
