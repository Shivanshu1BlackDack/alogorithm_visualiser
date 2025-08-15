# 📊 Algorithm Visualizer (C++ & SFML)

An **interactive C++ application** built with the **SFML** library to visualize classic **graph** and **pathfinding algorithms**.  
This tool provides a **hands-on** way to understand how these algorithms operate — watch them execute step-by-step!

---

## ✨ Features

- **Interactive GUI** – Create and manipulate graphs/grids with your mouse & keyboard.
- **Multiple Algorithms** – Visualize multiple fundamental algorithms in one place.
- **Step-by-Step Animation** – Real-time execution with clear visual cues for states and paths.
- **User-Friendly Controls** – On-screen instructions with restart & exit buttons.
- **Cross-Platform** – Runs on Windows, macOS, and Linux.

---

## 📜 Menu Preview
<img width="600" height="400" alt="menu" src="https://github.com/user-attachments/assets/378286ba-aca0-459a-bf6b-d1ebbdec35b7" />

---

## 🧠 Algorithms Visualized

### 1️⃣ Dijkstra's Shortest Path Algorithm
Finds the shortest path between two nodes in a **weighted, undirected graph**.  
The path with the **minimum total weight** is highlighted.

**How to Use**:
1. **Add Nodes** – Click anywhere to place a node.
2. **Add Edges** – Type: `from to weight` (e.g., `0 1 15`) and press Enter.
3. **Find Path** – Click **Find Shortest Path** (from node `0` to the last node).
4. **Animate** – Yellow walker follows the shortest path.

**Example:**
**Adding Node**
<img width="600" height="400" alt="prims" src="https://github.com/user-attachments/assets/5d941000-fae5-4522-8ca6-bd643e6376f8" />

**Output**
<img width="600" height="400" alt="op1" src="https://github.com/user-attachments/assets/03751cd6-cb0f-494b-9221-002e7b763de8" />

---

### 2️⃣ A* Search Algorithm
A popular **pathfinding** algorithm that uses a **heuristic** for efficient searching.

**How to Use**:
1. **Set Start/End** – Left-click a cell for start (green), another for end (red).
2. **Create Obstacles** – Left-click cells to toggle as walls (dark gray).
3. **Clear Cells** – Right-click to remove start/end/wall.
4. **Run Algorithm** – Press **Spacebar** to start.  
   - Blue = visited cells  
   - Yellow = final path

<img width="600" height="400" alt="astar" src="https://github.com/user-attachments/assets/80d323fe-155d-4762-837f-eb4e665c2613" />

---

### 3️⃣ Ford-Fulkerson Algorithm (Max Flow)
Finds **maximum flow** from a source node to a sink node in a **flow network**.

**How to Use**:
1. **Add Nodes** – First node (`0`) is the source, last node is the sink.
2. **Add Edges** – Type: `from to capacity` (e.g., `0 1 10`) and press Enter.
3. **Find Max Flow** – Click **Find Max Flow**.
4. **Animate** – Cyan walker shows augmenting paths, with real-time flow updates.

<img width="600" height="400" alt="fordfulkerson" src="https://github.com/user-attachments/assets/888362d7-8afc-4a05-b139-ce5449378556" />

---

### 4️⃣ Prim's Algorithm (Minimum Spanning Tree)
Builds an **MST** connecting all vertices with the smallest possible total edge weight.

**How to Use**:
1. **Add Nodes** – Click to place nodes.
2. **Add Edges** – Type: `from to weight` (e.g., `0 1 5`) and press Enter.  
   _(Or click two nodes to pre-fill input)_
3. **Run Algorithm** – Click **Run Prim's Algorithm**.
4. **Animate** –  
   - Green = nodes in MST  
   - Yellow = edges being considered  
   - Cyan = edges added to MST

**Example:**
**Adding a node**
<img width="600" height="400" alt="mst1" src="https://github.com/user-attachments/assets/54cefabd-050c-48c3-88f6-ec54458874f9" />

**Result**
<img width="600" height="400" alt="inprocess mstprims" src="https://github.com/user-attachments/assets/e49682c4-3caa-46e1-a5b7-e119aa600608" />

---

## 🛠️ Technologies Used
- **Language:** C++
- **Graphics:** SFML (Simple and Fast Multimedia Library)

---

## 🚀 Getting Started

### Prerequisites
- A **C++ compiler** supporting C++11 or newer (GCC, Clang, MSVC, etc.).
- **SFML 2.5.1+** ([Download here](https://www.sfml-dev.org/download.php)).

---

💡 *This project is a great way to learn algorithms visually, step-by-step!*

