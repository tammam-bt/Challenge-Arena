<h1 align = "center">🎮 Challenge Arena</h1> 

<h2 align = "center">A competitive Programming Platform</h2>

A fun and interactive console-based platform for students to solve algorithmic challenges, learn programming concepts, and compete with peers through a global leaderboard system.

## 🎯 Overview

This project is designed to help students:
- **Solve stimulating algorithmic problems**
- **Compare performance** with other solutions
- **Learn through practice** with graduated challenges
- **Develop computational thinking** skills

The platform provides 5 unique coding challenges with varying difficulty levels, a persistent scoring system, and a competitive leaderboard.

## ✨ Features

### Core Features
- 🎨 **Attractive ASCII art interface** with colored menus and visual feedback
- 👤 **User management system** with personalized welcome messages
- 💾 **Persistent data storage** using text files (`users.txt`, `scores.txt`)
- 🏆 **Global leaderboard** to track top performers
- 📊 **Individual statistics** tracking for each player
- 🎯 **Dynamic scoring** based on challenge difficulty and performance efficiency

### User Experience
- New users receive a welcome message and start fresh
- Returning users see "Welcome back" with their current statistics
- Real-time feedback on challenge performance
- Personal best tracking for each challenge

## 🎲 Challenges

### 1. 🎯 Le Compte est Bon (30 points)
**Difficulty:** ⭐⭐⭐

Find a combination of mathematical operations to reach a target number using given numbers.

- **Input:** 6 numbers and a target
- **Output:** Sequence of operations (e.g., `25*5=125, 125+10=135, 135-2=133`)
- **Scoring:** Based on time taken and solution correctness

### 2. 🧩 Mastermind Algorithmique (25 points)
**Difficulty:** ⭐⭐⭐

Guess a secret 4-digit sequence with feedback on correct/misplaced digits.

- **Mechanic:** System provides hints (✓ for correct position, ? for wrong position)
- **Goal:** Minimize the number of attempts (max 10 attempts)
- **Scoring:** Fewer attempts = higher score

### 3. 🤖 Course de Robots (50 points)
**Difficulty:** ⭐⭐⭐⭐⭐

Find the shortest path through a maze from start (S) to end (E).

- **Input:** 10x10 grid with walls, start, and finish
- **Output:** Sequence of movements (N, S, E, O for North/South/East/West)
- **Visualization:** Path is displayed on the maze
- **Scoring:** Shorter paths earn more points

### 4. 📊 Tri Ultime (20 points)
**Difficulty:** ⭐⭐

Sort an array using swap operations with minimal swaps.

- **Goal:** Sort an 8-element array
- **Constraint:** Minimize the number of swap operations
- **Scoring:** Fewer swaps = higher score

### 5. 🗼 Tour de Hanoi (40 points)
**Difficulty:** ⭐⭐⭐⭐

Solve the classic Tower of Hanoi puzzle with 4 disks.

- **Rules:** 
  - Move all disks from tower A to tower C
  - Only one disk can be moved at a time
  - Larger disks cannot be placed on smaller disks
- **Minimum moves:** 15 (2^4 - 1)
- **Scoring:** Closer to optimal = higher score

## 🚀 Installation

### Prerequisites
- GCC compiler or any C compiler
- Terminal/Console access

### Compilation

```bash
# Clone the repository
git clone <your-github-repo-url>
cd programming-challenges-platform

# Compile the program
gcc main.c -o challenges

# Run the program
./challenges
```

### For Windows:
```cmd
gcc main.c -o challenges.exe
challenges.exe
```

## 📖 Usage

1. **Start the program** and enter your name
2. **Choose a challenge** from the main menu (1-5)
3. **Complete the challenge** by following the on-screen instructions
4. **View your score** after completing each challenge
5. **Check the leaderboard** (option 6) to see how you rank
6. **View your statistics** (option 7) to track your progress

### First-Time Users
```
Enter your name: Alice
Welcome, Alice! Let's start your coding journey! 🚀
```

### Returning Users
```
Enter your name: Alice
Welcome back, Alice!
Your Statistics:
  • Total Score: 85 points
  • Challenges Completed: 3/5
```

## 🎯 Scoring System

Each challenge awards points based on:
1. **Base difficulty points** (fixed per challenge)
2. **Performance efficiency** (varies by challenge type):
   - Time taken (Le Compte est Bon)
   - Number of attempts (Mastermind)
   - Path length (Course de Robots)
   - Number of swaps (Tri Ultime)
   - Number of moves (Tour de Hanoi)

### Point Distribution
| Challenge | Max Points | Difficulty |
|-----------|------------|------------|
| Le Compte est Bon | 30 | ⭐⭐⭐ |
| Mastermind | 25 | ⭐⭐⭐ |
| Course de Robots | 50 | ⭐⭐⭐⭐⭐ |
| Tri Ultime | 20 | ⭐⭐ |
| Tour de Hanoi | 40 | ⭐⭐⭐⭐ |
| **Total Possible** | **165** | - |

### Personal Best System
- Each player's **best score per challenge** is saved
- Only improvements update the total score
- Encourages replay to optimize solutions

## 📁 File Structure

```
programming-challenges-platform/
├── main.c                 # Main source code
├── users.txt             # Stores registered usernames
├── scores.txt            # Stores player scores (auto-generated)
└── README.md             # This file
```

### Data Files Format

**users.txt:**
```
Alice
Bob
Charlie
```

**scores.txt:**
```
Alice,30,25,50,20,40,165,5
Bob,28,20,45,18,35,146,5
Charlie,25,23,0,20,0,68,3
```
Format: `name,compte_bon,mastermind,robot,tri,hanoi,total,completed`

## 🛠️ Technologies

- **Language:** C (Standard C99)
- **Libraries Used:**
  - `stdio.h` - Input/output operations
  - `stdlib.h` - Memory allocation and system functions
  - `string.h` - String manipulation
  - `time.h` - Time tracking for scoring
  - `ctype.h` - Character type checking

## 🎓 Learning Outcomes

By using this platform, students will practice:
- **Algorithm design** and optimization
- **Problem decomposition** and logical thinking
- **Data structures** (arrays, sequences)
- **File I/O operations** and data persistence
- **Pathfinding algorithms** (BFS/DFS concepts)
- **Recursive thinking** (Tower of Hanoi)
- **Sorting algorithms** and efficiency analysis

## 🏆 Leaderboard

The global leaderboard displays:
- Top 10 players ranked by total score
- Player name
- Total accumulated points
- Number of challenges completed

Players are sorted by total score in descending order, encouraging friendly competition!

## 📝 Notes

- All data is stored locally in text files
- Scores persist across sessions
- The program uses console-based UI with ASCII art
- Cross-platform compatible (Windows, Linux, macOS)

## 🤝 Contributing

This project is part of an academic assignment. Suggestions for improvements are welcome!

## 📧 Contact

For questions or feedback about this project, please contact through the course platform.

---

**Made with ❤️ for learning and fun coding challenges!**

🚀 Happy Coding! 🚀
