# Algorithm Visualizer project using Cpp and SFMl for visualizing these algorithms
Algorithm Visualizer 📊
An interactive C++ application built with the SFML library to visualize classic graph and pathfinding algorithms. This tool provides a hands-on way to understand how these algorithms operate by watching them execute step-by-step.

✨ Features
Interactive GUI: Easily create and manipulate graphs and grids using your mouse and keyboard.

Multiple Algorithms: Visualize a selection of fundamental algorithms in one application.

Step-by-Step Animation: Watch the algorithms work in real-time, with clear visual cues for node states, edge considerations, and path discovery.

User-Friendly Controls: Simple on-screen instructions, along with restart and exit buttons for easy navigation.

Cross-Platform: Built with SFML, making it compatible with Windows, macOS, and Linux.

#MENU 
<img width="400" height="250" alt="menu" src="https://github.com/user-attachments/assets/378286ba-aca0-459a-bf6b-d1ebbdec35b7" />


🧠 Algorithms Visualized
The application currently supports the visualization of four key algorithms:

1. Dijkstra's Shortest Path Algorithm
Finds the shortest path between two nodes in a weighted, undirected graph. The path with the minimum total weight is highlighted.

How to Use:

Add Nodes: Click anywhere on the screen to place a node.

Add Edges: Type the edge details in the format from to weight (e.g., 0 1 15) into the input box and press Enter.

Find Path: Click the "Find Shortest Path" button to run the algorithm from the first node (0) to the last node.

Animate: A yellow walker will traverse the calculated shortest path.

#Adding Node
<img width="1392" height="927" alt="prims" src="https://github.com/user-attachments/assets/5d941000-fae5-4522-8ca6-bd643e6376f8" />
#Output
<img width="1181" height="741" alt="op1" src="https://github.com/user-attachments/assets/03751cd6-cb0f-494b-9221-002e7b763de8" />

2. A* Search Algorithm
A popular pathfinding algorithm used for finding the shortest path on a grid. It uses a heuristic to efficiently guide its search.

How to Use:

Set Start/End: Left-click on a cell to set the start point (green), and then left-click on another cell to set the end point (red).

Create Obstacles: Left-click on any other cells to toggle them as walls (dark gray).

Clear Cells: Right-click on a cell to remove a start/end point or a wall.

Run Algorithm: Press the Spacebar to find the path. Visited cells are shown in blue, and the final path is highlighted in yellow.

<img width="1256" height="832" alt="astar" src="https://github.com/user-attachments/assets/80d323fe-155d-4762-837f-eb4e665c2613" />


3. Ford-Fulkerson Algorithm (Max Flow)
Calculates the maximum flow from a source node to a sink node in a flow network. The animation shows augmenting paths being found and the flow being updated.

How to Use:

Add Nodes: Click to place nodes. The first node (0) is the source, and the last node is the sink.

Add Edges: Type the edge details in the format from to capacity (e.g., 0 1 10) and press Enter. This creates a directed edge.

Find Max Flow: Click the "Find Max Flow" button to start the visualization.

Animate: A cyan walker will show each augmenting path found by the algorithm. The edge labels update in real-time to show flow/capacity.

<img width="1545" height="882" alt="fordfulkerson" src="https://github.com/user-attachments/assets/888362d7-8afc-4a05-b139-ce5449378556" />



4. Prim's Algorithm (Minimum Spanning Tree)
Finds the Minimum Spanning Tree (MST) for a weighted, undirected graph. The MST is a subset of the edges that connects all vertices together with the minimum possible total edge weight.

How to Use:

Add Nodes: Click to place nodes.

Add Edges: Type from to weight (e.g., 0 1 5) and press Enter. You can also click two nodes to pre-fill the input box.

Run Algorithm: Click "Run Prim's Algorithm" to start the animation.

Animate: The algorithm starts from node 0. Nodes in the MST turn green, and edges being considered are highlighted in yellow before being added to the MST (cyan).

##Adding a node
<img width="1512" height="797" alt="mst1" src="https://github.com/user-attachments/assets/54cefabd-050c-48c3-88f6-ec54458874f9" />
##Result
<img width="1511" height="816" alt="inprocess mstprims" src="https://github.com/user-attachments/assets/e49682c4-3caa-46e1-a5b7-e119aa600608" />

🛠️ Technologies Used
Language: C++

Graphics & Windowing: SFML (Simple and Fast Multimedia Library)

🚀 Getting Started
Follow these instructions to compile and run the project on your local machine.

Prerequisites
A C++ compiler that supports C++11 or newer (e.g., GCC, Clang, MSVC).

SFML 2.5.1 or newer. You can download it from the official SFML website.
