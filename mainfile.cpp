// Include all headers at the top
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <thread>
#include<functional>
#include <chrono>
using namespace std;
using namespace sf;

// -- ENUM FOR APP STATE --
enum AppState { MENU, VIEW1, VIEW2, VIEW3, VIEW4 };

// -- STRUCTURES & CLASSES FROM YOUR DIJKSTRA VISUALIZER --
struct Edge {
    int from, to;
    float weight;
    float capacity = 0;  // Added for Ford-Fulkerson
    float flow = 0;      // Added for Ford-Fulkerson
};

class InputBox {
public:
    RectangleShape box;
    Text inputText;
    Font font;
    string content;

    InputBox() {
        font.loadFromFile("arial.ttf");
        box.setSize(Vector2f(300, 30));
        box.setFillColor(Color(200, 200, 200));
        box.setPosition(20, 20);

        inputText.setFont(font);
        inputText.setCharacterSize(18);
        inputText.setFillColor(Color::Black);
        inputText.setPosition(25, 25);
    }

    void draw(RenderWindow& window) {
        window.draw(box);
        inputText.setString(content);
        window.draw(inputText);
    }

    void addChar(char c) {
        if (c == '\b' && !content.empty())
            content.pop_back();
        else if (c != '\b')
            content += c;
    }

    void clear() {
        content.clear();
    }
};

class Graph {
    RectangleShape exitButton;
    Text exitButtonText;
    RectangleShape restartButton;
    Text restartButtonText;
private:
    struct Node {
        Vector2f position;
        CircleShape shape;
        Text label;
    };

    vector<Node> nodes;
    vector<Edge> edges;
    vector<Text> edgeLabels;
    vector<Vertex> pathLines;
    vector<int> shortestPath;
    size_t animationIndex = 0;
    bool animatePath = false;

   CircleShape walker;
   Font font;
   Text instruction;
   RectangleShape button;
   Text buttonText;

public:
    Graph() {
        font.loadFromFile("arial.ttf");

        instruction.setFont(font);
        instruction.setCharacterSize(16);
        instruction.setFillColor(Color::White);
        instruction.setPosition(20, 60);
        instruction.setString("Click to add nodes. Type edges as: from to weight");

        button.setSize({ 200, 40 });
        button.setPosition(20, 520);
        button.setFillColor(Color::White);

        buttonText.setFont(font);
        buttonText.setCharacterSize(18);
        buttonText.setFillColor(Color::Black);
        buttonText.setString("Find Shortest Path");
        buttonText.setPosition(30, 528);

        exitButton.setSize({ 150, 35 });
        exitButton.setFillColor(Color(150, 0, 0));
        exitButton.setPosition(650, 20);

        exitButtonText.setFont(font);
        exitButtonText.setCharacterSize(18);
        exitButtonText.setFillColor(Color::White);
        exitButtonText.setString("Exit");
        exitButtonText.setPosition(680, 25);


        walker.setRadius(8.f);
        walker.setFillColor(Color::Yellow);
        walker.setOrigin(8.f, 8.f);
        //new code added
        restartButton.setSize({ 150, 35 });
        restartButton.setFillColor(Color(0, 150, 0));
        restartButton.setPosition(470, 20);

        restartButtonText.setFont(font);
        restartButtonText.setCharacterSize(18);
        restartButtonText.setFillColor(Color::White);
        restartButtonText.setString("Restart");
        restartButtonText.setPosition(500, 25);
    }
    void reset() {
        nodes.clear();
        edges.clear();
        edgeLabels.clear();
        pathLines.clear();
        shortestPath.clear();
        animationIndex = 0;
        animatePath = false;
    }
    bool isRestartButtonClicked(Vector2f pos) {
        return restartButton.getGlobalBounds().contains(pos);
    }
    void draw(RenderWindow& window) {
        for (size_t i = 0; i < edges.size(); i++) {
            Vertex line[] = {
                Vertex(nodes[edges[i].from].position, Color::White),
                Vertex(nodes[edges[i].to].position,Color::White)
            };
            window.draw(line, 2, Lines);
            window.draw(edgeLabels[i]);
        }

        if (!pathLines.empty())
            window.draw(&pathLines[0], pathLines.size(),Lines);

        for (auto& node : nodes) {
            window.draw(node.shape);
            window.draw(node.label);
        }

        if (animatePath)
            window.draw(walker);

        window.draw(instruction);
        window.draw(button);
        window.draw(buttonText);
        window.draw(exitButton);
        window.draw(exitButtonText);
        window.draw(restartButton);
        window.draw(restartButtonText);

    }

    void addNode(Vector2f pos) {
        Node node;
        node.position = pos;
        node.shape.setRadius(10);
        node.shape.setFillColor(Color::Blue);
        node.shape.setPosition(pos - Vector2f(10, 10));

        node.label.setFont(font);
        node.label.setCharacterSize(15);
        node.label.setFillColor(Color::White);
        node.label.setString(to_string(nodes.size()));
        node.label.setPosition(pos.x - 5, pos.y - 20);

        nodes.push_back(node);
    }

    void addEdgeFromText(const string& str) {
        istringstream iss(str);
        int u, v;
        float w;
        if (iss >> u >> v >> w) {
            edges.push_back({ u, v, w });
            edges.push_back({ v, u, w });

            Text label;
            label.setFont(font);
            label.setCharacterSize(15);
            label.setFillColor(Color::Red);
            Vector2f mid = (nodes[u].position + nodes[v].position) / 2.f;
            label.setPosition(mid);
            label.setString(to_string(static_cast<int>(w)));
            edgeLabels.push_back(label);
            edgeLabels.push_back(label);
        }
    }

    void handleClick(Vector2f pos) {
        if (button.getGlobalBounds().contains(pos)) {
            findShortestPath(0, nodes.size() - 1);
            animatePath = true;
            animationIndex = 0;
            if (!shortestPath.empty())
                walker.setPosition(nodes[shortestPath[0]].position);
        }
        else {
            addNode(pos);
        }
    }

    void update() {
        if (animatePath && animationIndex < shortestPath.size() - 1) {
            Vector2f current = walker.getPosition();
            Vector2f target = nodes[shortestPath[animationIndex + 1]].position;
            Vector2f dir = target - current;
            float dist = sqrt(dir.x * dir.x + dir.y * dir.y);
            if (dist > 1.5f) {
                dir /= dist;
                walker.move(dir * 2.f);
            }
            else {
                walker.setPosition(target);
                animationIndex++;
            }
        }
    }

    void findShortestPath(int start, int end) {
        int n = nodes.size();
        vector<float> dist(n, numeric_limits<float>::max());
        vector<int> parent(n, -1);
        dist[start] = 0;
        using P = pair<float, int>;
        priority_queue<P, vector<P>, greater<P>> pq;
        pq.push({ 0, start });

        while (!pq.empty()) {
            int u = pq.top().second;
            float d = pq.top().first;
            pq.pop();
            if (d > dist[u]) continue;
            for (const auto& edge : edges) {
                if (edge.from == u) {
                    int v = edge.to;
                    float w = edge.weight;
                    if (dist[u] + w < dist[v]) {
                        dist[v] = dist[u] + w;
                        parent[v] = u;
                        pq.push({ dist[v], v });
                    }
                }
            }
        }

        shortestPath.clear();
        for (int v = end; v != -1; v = parent[v])
            shortestPath.push_back(v);
        reverse(shortestPath.begin(), shortestPath.end());

        pathLines.clear();
        for (size_t i = 1; i < shortestPath.size(); ++i) {
            pathLines.push_back(Vertex(nodes[shortestPath[i - 1]].position, Color::Cyan));
            pathLines.push_back(Vertex(nodes[shortestPath[i]].position, Color::Cyan));
        }
    }
    bool isExitButtonClicked(Vector2f pos) {
        return exitButton.getGlobalBounds().contains(pos);
    }
};

//A* algorithm 
struct Cell {
    int row, col;
    bool isWall = false;
    bool isVisited = false;
    bool isPath = false;
    float g = INFINITY, h = 0;
    Cell* parent = nullptr;
};

class AStarVisualizer {
private:
    const int rows = 50;
    const int cols = 80;
    const int cellSize = 30;
    vector<vector<Cell>> grid;
    Cell* start = nullptr;
    Cell* end = nullptr;
    bool running = false;
    bool pathFound = false;
    //new member variable for reset buuton
    RectangleShape restartButton;
    Text restartButtonText;
    Font font;
public:
    AStarVisualizer() {
        grid.resize(rows, vector<Cell>(cols));
        for (int r = 0; r < rows; ++r)
            for (int c = 0; c < cols; ++c)
                grid[r][c] = { r, c };

        font.loadFromFile("arial.ttf");
        restartButton.setSize({ 150, 35 });
        restartButton.setFillColor(Color::Blue);
        restartButton.setPosition(700, 20);

        restartButtonText.setFont(font);
        restartButtonText.setCharacterSize(18);
        restartButtonText.setFillColor(Color::White);
        restartButtonText.setString("Restart");
        restartButtonText.setPosition(730, 25);
    }

    void reset() {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                grid[r][c] = { r, c }; 
            }
        }
        start = nullptr;
        end = nullptr;
        running = false;
        pathFound = false;
    }

    bool isRestartButtonClicked(Vector2f pos) {
        return restartButton.getGlobalBounds().contains(pos);
    }

    void draw(RenderWindow& window) {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                Cell& cell = grid[r][c];
                RectangleShape rect(Vector2f(cellSize - 1, cellSize - 1));
                rect.setPosition(c * cellSize, r * cellSize);

                if (&cell == start) rect.setFillColor(Color::Green);
                else if (&cell == end) rect.setFillColor(Color::Red);
                else if (cell.isWall) rect.setFillColor(Color(50, 50, 50));
                else if (cell.isPath) rect.setFillColor(Color::Yellow);
                else if (cell.isVisited) rect.setFillColor(Color(100, 100, 255));
                else rect.setFillColor(Color::White);

                window.draw(rect);
            }
        }
        window.draw(restartButton);
        window.draw(restartButtonText);
    }

    void handleClick(Vector2f pos) {
        int row = pos.y / cellSize;
        int col = pos.x / cellSize;
        if (row >= 0 && row < rows && col >= 0 && col < cols) {
            Cell* clicked = &grid[row][col];
            if (Mouse::isButtonPressed(Mouse::Left)) {
                if (!start) start = clicked;
                else if (!end && clicked != start) end = clicked;
                else if (clicked != start && clicked != end) clicked->isWall = !clicked->isWall;
            }
            else if (Mouse::isButtonPressed(Mouse::Right)) {
                if (clicked == start) start = nullptr;
                if (clicked == end) end = nullptr;
                clicked->isWall = false;
            }
        }
    }

    void runAStar() {
        struct Node {
            Cell* cell;
            float f;
            bool operator>(const Node& other) const { return f > other.f; }
        };

        priority_queue<Node, vector<Node>, greater<Node>> openSet;
        start->g = 0;
        start->h = heuristic(start, end);
        openSet.push({ start, start->h });

        while (!openSet.empty()) {
            Cell* current = openSet.top().cell;
            openSet.pop();

            if (current == end) {
                Cell* p = end;
                while (p) {
                    p->isPath = true;
                    p = p->parent;
                }
                pathFound = true;
                return;
            }

            current->isVisited = true;

            for (auto neighbor : neighbors(current)) {
                float tentativeGScore = current->g + 1; // Distance between neighbors is 1
                if (tentativeGScore < neighbor->g) {
                    neighbor->parent = current;
                    neighbor->g = tentativeGScore;
                    neighbor->h = heuristic(neighbor, end);
                    openSet.push({ neighbor, neighbor->g + neighbor->h });
                }
            }
        }
    }

private:
    float heuristic(Cell* a, Cell* b) {
        return abs(a->row - b->row) + abs(a->col - b->col); // Manhattan distance
    }

    vector<Cell*> neighbors(Cell* cell) {
        vector<Cell*> result;

        int dr[] = { -1, 1, 0, 0 };
        int dc[] = { 0, 0, -1, 1 };

        for (int i = 0; i < 4; ++i) {
            int nr = cell->row + dr[i];
            int nc = cell->col + dc[i];
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && !grid[nr][nc].isWall)
                result.push_back(&grid[nr][nc]);
        }

        return result;
    }
};

// -- FORD FULKERSON IMPLEMENTATION --
class FordFulkersonVisualizer {
    RectangleShape restartButton;
    Text restartButtonText;

private:
    struct Node {
        Vector2f position;
        CircleShape shape;
        Text label;
    };

    vector<Node> nodes;
    vector<Edge> edges;
    Font font;

    RectangleShape button;
    Text buttonText;
    Text instructionText;
    Text maxFlowText;
    Text inputPrompt;
    Text inputText;
    RectangleShape exitButton;
    Text exitButtonText;

    string userInput;

    bool animating = false;
    vector<int> currentPath;
    size_t animationIndex = 0;
    CircleShape walker;

    float totalFlow = 0;

    RenderWindow* renderWindow = nullptr; // For animation

public:
    FordFulkersonVisualizer() {
        font.loadFromFile("arial.ttf");

        button.setSize(Vector2f(180, 35));
        button.setFillColor(Color::Green);
        button.setPosition(600, 35);

        buttonText.setFont(font);
        buttonText.setString("Find Max Flow");
        buttonText.setCharacterSize(18);
        buttonText.setFillColor(Color::White);
        buttonText.setPosition(610, 40);

        instructionText.setFont(font);
        instructionText.setCharacterSize(20);
        instructionText.setFillColor(Color::Yellow);
        instructionText.setPosition(20, 10);
        instructionText.setString("Click to add nodes. Type edges as: from to capacity.");

        inputPrompt.setFont(font);
        inputPrompt.setCharacterSize(20);
        inputPrompt.setFillColor(Color::Yellow);
        inputPrompt.setPosition(20, 520);
        inputPrompt.setString("Edge Input (e.g. 0 1 10):");

        inputText.setFont(font);
        inputText.setCharacterSize(20);
        inputText.setFillColor(Color::White);
        inputText.setPosition(250, 520);

        maxFlowText.setFont(font);
        maxFlowText.setCharacterSize(20);
        maxFlowText.setFillColor(Color::White);
        maxFlowText.setPosition(20, 550);

        exitButton.setSize({ 150, 35 });
        exitButton.setFillColor(Color(150, 0, 0));
        exitButton.setPosition(830,35);

        exitButtonText.setFont(font);
        exitButtonText.setCharacterSize(18);
        exitButtonText.setFillColor(Color::White);
        exitButtonText.setString("Exit");
        exitButtonText.setPosition(850, 40);

        walker.setRadius(8);
        walker.setFillColor(Color::Cyan);
        walker.setOrigin(8, 8);
        //new constructor element is added
        restartButton.setSize({ 150, 35 });
        restartButton.setFillColor(Color::Blue);
        restartButton.setPosition(420, 35);

        restartButtonText.setFont(font);
        restartButtonText.setCharacterSize(18);
        restartButtonText.setFillColor(Color::White);
        restartButtonText.setString("Restart");
        restartButtonText.setPosition(480, 40);
    }

    //new code added

    void reset() {
        nodes.clear();
        edges.clear();
        userInput.clear();
        animating = false;
        totalFlow = 0;
        currentPath.clear();
        maxFlowText.setString("");
    }

    void setRenderWindow(RenderWindow& win) {
        renderWindow = &win;
    }

    bool isAnimating() const {
        return animating;
    }

    bool isExitButtonClicked(Vector2f pos) {
        return exitButton.getGlobalBounds().contains(pos);
    }

    void handleClick(Vector2f pos) {
        if (button.getGlobalBounds().contains(pos)) {
            if (!animating)
                startMaxFlow();
            return;
        }

        if (isExitButtonClicked(pos)) {
            return; // Exit will be handled by the main code
        }

        addNode(pos.x, pos.y);
    }

    void addNode(float x, float y) {
        CircleShape circle(15);
        circle.setFillColor(Color::Blue);
        circle.setPosition(x - 15, y - 15);

        Text label;
        label.setFont(font);
        label.setString(to_string(nodes.size()));
        label.setCharacterSize(14);
        label.setFillColor(Color::White);
        label.setPosition(x - 5, y - 30);

        nodes.push_back({ {x, y}, circle, label });
    }

    void addEdge(int from, int to, float capacity) {
        if (from >= 0 && from < nodes.size() && to >= 0 && to < nodes.size()) {
            edges.push_back({ from, to, 0, capacity, 0 }); // weight=0 for FF
            edges.push_back({ to, from, 0, 0, 0 });       // reverse edge with zero capacity
        }
    }

    void handleTextInput(Uint32 unicode) {
        if (unicode == 8 && !userInput.empty()) {
            userInput.pop_back();
        }
        else if (unicode >= 32 && unicode < 128) {
            userInput += static_cast<char>(unicode);
        }
        inputText.setString(userInput);
    }

    void processEdgeInput() {
        istringstream iss(userInput);
        int from, to;
        float cap;
        if (iss >> from >> to >> cap) {
            addEdge(from, to, cap);
        }
        userInput.clear();
        inputText.setString("");
    }

    void startMaxFlow() {
        totalFlow = 0;
        vector<int> parent(nodes.size());
        animating = true;

        while (bfs(0, nodes.size() - 1, parent)) {
            float pathFlow = 1e9f;
            for (int v = nodes.size() - 1; v != 0; v = parent[v]) {
                int u = parent[v];
                pathFlow = min(pathFlow, getResidualCapacity(u, v));
            }

            for (int v = nodes.size() - 1; v != 0; v = parent[v]) {
                int u = parent[v];
                updateFlow(u, v, pathFlow);
            }

            totalFlow += pathFlow;

            currentPath.clear();
            for (int v = nodes.size() - 1; v != -1; v = parent[v])
                currentPath.push_back(v);
            reverse(currentPath.begin(), currentPath.end());

            walker.setPosition(nodes[currentPath[0]].position);
            animationIndex = 0;

            animateWalkerAlongPath();

            stringstream ss;
            ss << "Max Flow so far: " << totalFlow;
            maxFlowText.setString(ss.str());

            this_thread::sleep_for(chrono::milliseconds(500));
        }

        stringstream ss;
        ss << "Final Max Flow: " << totalFlow;
        maxFlowText.setString(ss.str());

        animating = false;
    }

    void animateWalkerAlongPath() {
        if (currentPath.size() < 2) return;

        for (size_t i = 0; i < currentPath.size() - 1; ++i) {
            Vector2f start = nodes[currentPath[i]].position;
            Vector2f end = nodes[currentPath[i + 1]].position;

            Vector2f dir = end - start;
            float length = sqrt(dir.x * dir.x + dir.y * dir.y);
            dir /= length;

            float moved = 0.f;
            walker.setPosition(start);

            while (moved < length && renderWindow != nullptr) {
                float step = 2.f;
                walker.move(dir * step);
                moved += step;

                renderWindow->clear();
                draw(*renderWindow);
                renderWindow->display();

                sleep(milliseconds(10));
            }

            walker.setPosition(end);
            if (renderWindow != nullptr) {
                renderWindow->clear();
                draw(*renderWindow);
                renderWindow->display();
            }

            sleep(milliseconds(200));  // Pause at node
        }
    }

    bool bfs(int src, int sink, vector<int>& parent) {
        vector<bool> visited(nodes.size(), false);
        queue<int> q;
        q.push(src);
        visited[src] = true;
        parent[src] = -1;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (auto& edge : edges) {
                if (edge.from == u && !visited[edge.to] && getResidualCapacity(u, edge.to) > 0) {
                    parent[edge.to] = u;
                    visited[edge.to] = true;
                    q.push(edge.to);
                    if (edge.to == sink) return true;
                }
            }
        }
        return false;
    }

    float getResidualCapacity(int from, int to) {
        for (auto& edge : edges) {
            if (edge.from == from && edge.to == to)
                return edge.capacity - edge.flow;
        }
        return 0;
    }

    void updateFlow(int from, int to, float flow) {
        for (auto& edge : edges) {
            if (edge.from == from && edge.to == to) edge.flow += flow;
            if (edge.from == to && edge.to == from) edge.flow -= flow;
        }
    }

    bool isRestartButtonClicked(Vector2f pos) {
        return restartButton.getGlobalBounds().contains(pos);
    }

    void draw(RenderWindow& window) {
        window.draw(instructionText);
        window.draw(button);
        window.draw(buttonText);
        window.draw(maxFlowText);
        window.draw(inputPrompt);
        window.draw(inputText);
        window.draw(exitButton);
        window.draw(exitButtonText);
        //new code added here
        window.draw(restartButton);
        window.draw(restartButtonText);


        for (auto& edge : edges) {
            if (edge.capacity == 0) continue;
            Vertex line[] = {
                Vertex(nodes[edge.from].position, Color::White),
                Vertex(nodes[edge.to].position, Color::White)
            };
            window.draw(line, 2, Lines);

            // Draw flow/capacity
            Text flowText;
            flowText.setFont(font);
            flowText.setCharacterSize(14);
            flowText.setFillColor(Color::Yellow);
            Vector2f mid = (nodes[edge.from].position + nodes[edge.to].position) / 2.f;
            flowText.setPosition(mid);
            flowText.setString(to_string(static_cast<int>(edge.flow)) + "/" +
                to_string(static_cast<int>(edge.capacity)));
            window.draw(flowText);
        }

        for (auto& node : nodes) {
            window.draw(node.shape);
            window.draw(node.label);
        }

        if (animating)
            window.draw(walker);
    }
};


//prims algorithm

class PrimsVisualizer {
private:
    struct Node {
        Vector2f position;
        CircleShape shape;
        Text label;
        float originalRadius = 10.f;
        float pulseRadius = 10.f;
        bool isPulsing = false;
        Clock pulseClock;
    };

    struct Edge {
        int from;
        int to;
        float weight;
        bool isHighlighted = false;
        Color color = Color::White;
    };

    struct AnimationStep {
        int currentNode;
        int addedNode;
        int fromNode;
        float weight;
        bool isNodeSelection; // true for node selection, false for edge addition
    };

    vector<Node> nodes;
    vector<Edge> edges;
    vector<Text> edgeLabels;
    vector<Vertex> mstLines;

    Font font;
    Text buttonText, mstWeightText, instructionText, inputPrompt, inputText, statusText;
    RectangleShape button;
    RectangleShape exitButton;
    Text exitButtonText;
    RectangleShape restartButton;
    Text restartButtonText;
    RectangleShape speedUpButton;
    RectangleShape speedDownButton;
    Text speedUpText;
    Text speedDownText;
    Text speedText;

    string userInput;
    float totalMSTWeight = 0.0f;
    bool showMST = false;

    // Animation-related members
    vector<AnimationStep> animationSteps;
    size_t currentAnimationStep = 0;
    Clock animationClock;
    bool isAnimating = false;
    float animationSpeed = 2.0f; // seconds per step
    vector<int> nodesInMST;
    vector<Vertex> animatedMSTLines;
    int currentHighlightedEdge = -1;
    int currentConsideredNode = -1;

public:
    PrimsVisualizer() {
        font.loadFromFile("arial.ttf");

        button.setSize(Vector2f(200, 35));
        button.setFillColor(Color(0, 100, 0));
        button.setPosition(830, 20);

        buttonText.setFont(font);
        buttonText.setString("Run Prim's Algorithm");
        buttonText.setCharacterSize(17);
        buttonText.setFillColor(Color::White);
        buttonText.setPosition(840, 28);

        instructionText.setFont(font);
        instructionText.setCharacterSize(20);
        instructionText.setFillColor(Color::Yellow);
        instructionText.setPosition(20, 10);
        instructionText.setString("Click to add nodes. Type edges like: 0 1 10 and press Enter.");

        inputPrompt.setFont(font);
        inputPrompt.setCharacterSize(20);
        inputPrompt.setFillColor(Color::Yellow);
        inputPrompt.setPosition(20, 520);
        inputPrompt.setString("Edge Input (format: from to weight):");

        inputText.setFont(font);
        inputText.setCharacterSize(20);
        inputText.setFillColor(Color::White);
        inputText.setPosition(350, 520);

        mstWeightText.setFont(font);
        mstWeightText.setCharacterSize(18);
        mstWeightText.setFillColor(Color::Red);
        mstWeightText.setPosition(20, 550);

        statusText.setFont(font);
        statusText.setCharacterSize(18);
        statusText.setFillColor(Color::Yellow);
        statusText.setPosition(20, 480);
        statusText.setString("");

        exitButton.setSize({ 100, 35 });
        exitButton.setFillColor(Color::Red);
        exitButton.setPosition(1050, 20);

        exitButtonText.setFont(font);
        exitButtonText.setCharacterSize(18);
        exitButtonText.setFillColor(Color::White);
        exitButtonText.setString("Exit");
        exitButtonText.setPosition(1060, 25);

        restartButton.setSize({ 100, 35 });
        restartButton.setFillColor(Color::Blue);
        restartButton.setPosition(700, 20);

        restartButtonText.setFont(font);
        restartButtonText.setCharacterSize(18);
        restartButtonText.setFillColor(Color::White);
        restartButtonText.setString("Restart");
        restartButtonText.setPosition(730, 25);
    }

    void reset() {
        nodes.clear();
        edges.clear();
        edgeLabels.clear();
        mstLines.clear();
        animatedMSTLines.clear();
        animationSteps.clear();
        nodesInMST.clear();
        totalMSTWeight = 0.0f;
        showMST = false;
        isAnimating = false;
        userInput.clear();
        inputText.setString("");
        mstWeightText.setString("");
        statusText.setString("");
        currentHighlightedEdge = -1;
        currentConsideredNode = -1;
    }

    void addNode(float x, float y) {
        int id = nodes.size();
        Node node;
        node.position = { x, y };
        node.shape.setRadius(10.f);
        node.shape.setFillColor(Color::Blue);
        node.shape.setPosition(x - 10.f, y - 10.f);
        node.shape.setOutlineThickness(1.f);
        node.shape.setOutlineColor(Color::White);

        node.label.setFont(font);
        node.label.setString(to_string(id));
        node.label.setCharacterSize(14);
        node.label.setFillColor(Color::White);

        // Center the label properly
        FloatRect textBounds = node.label.getLocalBounds();
        node.label.setPosition(
            x - textBounds.width / 2,
            y - textBounds.height / 2 - 5.f
        );

        nodes.push_back(node);
    }

    void addEdge(int from, int to, float weight) {
        if (from >= 0 && to >= 0 && from < nodes.size() && to < nodes.size() && from != to) {
            // Check if edge already exists
            for (size_t i = 0; i < edges.size(); i++) {
                if ((edges[i].from == from && edges[i].to == to) ||
                    (edges[i].from == to && edges[i].to == from)) {
                    statusText.setString("Edge already exists!");
                    return;
                }
            }

            // Add both directions for undirected graph
            edges.push_back({ from, to, weight });
            edges.push_back({ to, from, weight });

            // Create edge label
            Text label;
            label.setFont(font);
            label.setString(to_string(static_cast<int>(weight)));
            label.setCharacterSize(14);
            label.setFillColor(Color::Yellow);

            // Position label midway between nodes with slight offset
            Vector2f midpoint = (nodes[from].position + nodes[to].position) / 2.f;
            // Add slight offset to make sure label doesn't overlap with edge
            Vector2f offset = nodes[to].position - nodes[from].position;
            float length = sqrt(offset.x * offset.x + offset.y * offset.y);
            if (length > 0) {
                offset = Vector2f(-offset.y, offset.x) * 8.f / length;
                midpoint += offset;
            }

            // Center the text at the midpoint
            FloatRect textBounds = label.getLocalBounds();
            label.setPosition(
                midpoint.x - textBounds.width / 2,
                midpoint.y - textBounds.height / 2
            );

            edgeLabels.push_back(label);

            statusText.setString("Added edge " + to_string(from) + " to " +
                to_string(to) + " with weight " + to_string(static_cast<int>(weight)));
        }
        else {
            statusText.setString("Invalid edge: Check node indices or self-loops not allowed");
        }
    }

    void handleClick(Vector2f pos) {
        if (isAnimating) {
            // Skip to next animation step if clicked during animation
            nextAnimationStep();
            return;
        }

        if (button.getGlobalBounds().contains(pos)) {
            runPrimsAlgorithm();
        }
        else if (exitButton.getGlobalBounds().contains(pos)) {
            // Will be handled in main
        }
        else if (restartButton.getGlobalBounds().contains(pos)) {
            // Will be handled in main
        }
        else if (speedUpButton.getGlobalBounds().contains(pos)) {
            if (animationSpeed > 0.5f) {
                animationSpeed -= 0.5f;
                speedText.setString("Speed: x" + to_string(static_cast<int>(2.0f / animationSpeed)));
            }
        }
        else if (speedDownButton.getGlobalBounds().contains(pos)) {
            if (animationSpeed < 4.0f) {
                animationSpeed += 0.5f;
                speedText.setString("Speed: x" + to_string(static_cast<int>(2.0f / animationSpeed)));
            }
        }
        else {
            // Check if we're clicking on a node (select for edge creation)
            bool nodeClicked = false;
            for (size_t i = 0; i < nodes.size(); i++) {
                if (nodes[i].shape.getGlobalBounds().contains(pos)) {
                    // Handle node selection for edge creation
                    if (currentConsideredNode == -1) {
                        currentConsideredNode = i;
                        nodes[i].shape.setFillColor(Color::Yellow);
                        statusText.setString("Node " + to_string(i) + " selected. Click another node to create edge.");
                    }
                    else if (currentConsideredNode != static_cast<int>(i)) {
                        // Ask user for edge weight
                        userInput = to_string(currentConsideredNode) + " " + to_string(i) + " ";
                        inputText.setString(userInput);
                        statusText.setString("Enter weight for edge " + to_string(currentConsideredNode) +
                            " to " + to_string(i));

                        // Reset node colors
                        nodes[currentConsideredNode].shape.setFillColor(Color::Blue);
                        currentConsideredNode = -1;
                    }
                    nodeClicked = true;
                    break;
                }
            }

            // If no node was clicked, add a new node
            if (!nodeClicked && currentConsideredNode == -1) {
                addNode(pos.x, pos.y);
            }
            // If no node was clicked but one was selected, deselect it
            else if (!nodeClicked && currentConsideredNode != -1) {
                nodes[currentConsideredNode].shape.setFillColor(Color::Blue);
                currentConsideredNode = -1;
                statusText.setString("Node selection canceled");
            }
        }
    }

    void handleTextInput(Uint32 unicode) {
        if (isAnimating) return; // Ignore input during animation

        if (unicode == 8 && !userInput.empty()) { // Backspace
            userInput.pop_back();
        }
        else if (unicode == 13) { // Enter key
            processEdgeInput();
        }
        else if (unicode >= 32 && unicode < 128) {
            userInput += static_cast<char>(unicode);
        }
        inputText.setString(userInput);
    }

    void processEdgeInput() {
        istringstream iss(userInput);
        int from, to;
        float weight;
        if (iss >> from >> to >> weight) {
            addEdge(from, to, weight);
        }
        else {
            statusText.setString("Invalid input format. Use: from to weight");
        }
        userInput.clear();
        inputText.setString("");
    }

    void runPrimsAlgorithm() {
        int n = nodes.size();
        if (n == 0) {
            statusText.setString("Add nodes first before running the algorithm");
            return;
        }
        if (n == 1) {
            statusText.setString("Add more nodes to form a graph");
            return;
        }

        // Check if graph is connected
        vector<vector<int>> adjacencyList(n);
        for (size_t i = 0; i < edges.size(); i += 2) {
            adjacencyList[edges[i].from].push_back(edges[i].to);
        }

        vector<bool> visited(n, false);
        function<void(int)> dfs = [&](int node) {
            visited[node] = true;
            for (int neighbor : adjacencyList[node]) {
                if (!visited[neighbor]) {
                    dfs(neighbor);
                }
            }
            };

        dfs(0);

        bool isConnected = true;
        for (bool v : visited) {
            if (!v) {
                isConnected = false;
                break;
            }
        }

        if (!isConnected) {
            statusText.setString("Graph is not connected! Add more edges.");
            return;
        }

        // Reset previous animation state
        animationSteps.clear();
        nodesInMST.clear();
        animatedMSTLines.clear();
        currentAnimationStep = 0;
        totalMSTWeight = 0.0f;

        // Reset node colors
        for (auto& node : nodes) {
            node.shape.setFillColor(Color::Blue);
        }

        vector<bool> inMST(n, false);
        vector<float> key(n, numeric_limits<float>::max());
        vector<int> parent(n, -1);
        key[0] = 0;

        using P = pair<float, int>;
        priority_queue<P, vector<P>, greater<P>> pq;
        pq.push({ 0, 0 });

        // First node is always in MST
        animationSteps.push_back({ 0, 0, -1, 0, true });

        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();

            if (inMST[u]) continue;
            inMST[u] = true;

            // Add node selection to animation steps
            if (u != 0) { // Skip first node which was already added
                animationSteps.push_back({ u, u, parent[u], key[u], true });
            }

            // Find all edges from u
            for (size_t i = 0; i < edges.size(); i++) {
                if (edges[i].from == u) {
                    int v = edges[i].to;
                    float w = edges[i].weight;
                    if (!inMST[v] && w < key[v]) {
                        key[v] = w;
                        parent[v] = u;
                        pq.push({ key[v], v });

                        // Add edge consideration to animation steps
                        animationSteps.push_back({ u, v, u, w, false });
                    }
                }
            }
        }

        // Start animation
        isAnimating = true;
        animationClock.restart();
        statusText.setString("Starting Prim's algorithm animation...");
        nodesInMST.push_back(0); // Add starting node

        // Color the starting node
        if (!nodes.empty()) {
            nodes[0].shape.setFillColor(Color::Green);
            nodes[0].isPulsing = true;
            nodes[0].pulseClock.restart();
        }
    }

    void nextAnimationStep() {
        if (!isAnimating || currentAnimationStep >= animationSteps.size()) {
            // Animation complete
            finishAnimation();
            return;
        }

        AnimationStep& step = animationSteps[currentAnimationStep];

        // Reset highlighting from previous step
        if (currentHighlightedEdge != -1) {
            for (size_t i = 0; i < edges.size(); i++) {
                edges[i].isHighlighted = false;
                edges[i].color = Color::White;
            }
        }

        if (step.isNodeSelection) {
            // Adding a node to MST
            if (step.addedNode < nodes.size()) {
                nodes[step.addedNode].shape.setFillColor(Color::Green);
                nodes[step.addedNode].isPulsing = true;
                nodes[step.addedNode].pulseClock.restart();

                // Add to MST nodes list
                nodesInMST.push_back(step.addedNode);

                // Add edge to MST if this isn't the first node
                if (step.fromNode != -1) {
                    Vertex line[] = {
                        Vertex(nodes[step.fromNode].position, Color::Cyan),
                        Vertex(nodes[step.addedNode].position, Color::Cyan)
                    };
                    animatedMSTLines.push_back(line[0]);
                    animatedMSTLines.push_back(line[1]);

                    totalMSTWeight += step.weight;
                    mstWeightText.setString("MST Weight: " + to_string(static_cast<int>(totalMSTWeight)));

                    // Highlight the edge in the original edge list
                    for (size_t i = 0; i < edges.size(); i++) {
                        if ((edges[i].from == step.fromNode && edges[i].to == step.addedNode) ||
                            (edges[i].from == step.addedNode && edges[i].to == step.fromNode)) {
                            edges[i].isHighlighted = true;
                            edges[i].color = Color::Cyan;
                        }
                    }
                }

                statusText.setString("Added node " + to_string(step.addedNode) + " to MST" +
                    (step.fromNode != -1 ? " via edge from " + to_string(step.fromNode) : ""));
            }
        }
        else {
            // Considering an edge
            currentHighlightedEdge = -1;
            for (size_t i = 0; i < edges.size(); i++) {
                if ((edges[i].from == step.fromNode && edges[i].to == step.addedNode) ||
                    (edges[i].from == step.addedNode && edges[i].to == step.fromNode)) {
                    edges[i].isHighlighted = true;
                    edges[i].color = Color::Yellow; // Highlight in yellow for consideration
                    currentHighlightedEdge = i;
                }
            }

            statusText.setString("Considering edge from " + to_string(step.fromNode) +
                " to " + to_string(step.addedNode) +
                " with weight " + to_string(static_cast<int>(step.weight)));
        }

        currentAnimationStep++;
        animationClock.restart();
    }

    void finishAnimation() {
        isAnimating = false;
        showMST = true;
        mstLines = animatedMSTLines;

        // Final status
        statusText.setString("Prim's algorithm complete - MST weight: " +
            to_string(static_cast<int>(totalMSTWeight)));

        // Stop all node pulsing
        for (auto& node : nodes) {
            node.isPulsing = false;
            node.shape.setRadius(node.originalRadius);
        }
    }

    void update() {
        if (isAnimating && animationClock.getElapsedTime().asSeconds() > animationSpeed) {
            nextAnimationStep();
        }

        // Update pulsing nodes
        for (auto& node : nodes) {
            if (node.isPulsing) {
                float elapsed = node.pulseClock.getElapsedTime().asSeconds();
                float pulseAmount = 3.0f * sin(elapsed * 5.0f);
                node.pulseRadius = node.originalRadius + pulseAmount;
                node.shape.setRadius(node.pulseRadius);
                node.shape.setPosition(
                    node.position.x - node.pulseRadius,
                    node.position.y - node.pulseRadius
                );

                // Reset if pulse gets too large
                if (elapsed > 10.0f) {
                    node.pulseClock.restart();
                }
            }
        }
    }

    bool isExitButtonClicked(Vector2f pos) {
        return exitButton.getGlobalBounds().contains(pos);
    }

    bool isRestartButtonClicked(Vector2f pos) {
        return restartButton.getGlobalBounds().contains(pos);
    }

    bool getIsAnimating() const {
        return isAnimating;
    }

    void draw(RenderWindow& window) {
        window.draw(instructionText);
        window.draw(button);
        window.draw(buttonText);
        window.draw(inputPrompt);
        window.draw(inputText);
        window.draw(exitButton);
        window.draw(exitButtonText);
        window.draw(restartButton);
        window.draw(restartButtonText);
        window.draw(statusText);

        // Draw edges
        for (size_t i = 0; i < edges.size(); i += 2) {
            Vertex line[] = {
                Vertex(nodes[edges[i].from].position, edges[i].isHighlighted ? edges[i].color : Color::White),
                Vertex(nodes[edges[i].to].position, edges[i].isHighlighted ? edges[i].color : Color::White)
            };
            window.draw(line, 2, Lines);
        }

        // Draw edge labels
        for (size_t i = 0; i < edgeLabels.size(); i++) {
            window.draw(edgeLabels[i]);
        }

        // Draw MST edges if algorithm has been run
        if (!animatedMSTLines.empty()) {
            window.draw(&animatedMSTLines[0], animatedMSTLines.size(), Lines);
            window.draw(mstWeightText);
        }

        // Draw nodes
        for (const auto& node : nodes) {
            window.draw(node.shape);
            window.draw(node.label);
        }
    }
};

int main() {
    VideoMode desktop = VideoMode::getDesktopMode();
    RenderWindow window(desktop, "Algorithm Visualizer", Style::Fullscreen);
    window.setFramerateLimit(60);

    AppState currentState = MENU;
    Font font;
    font.loadFromFile("arial.ttf");

    Texture bgTexture;
    bgTexture.loadFromFile("algo10.jpeg");
    Sprite background(bgTexture);

    // Center background
    Vector2u bgSize = bgTexture.getSize();
    Vector2u winSize = window.getSize();
    float scale = min((float)winSize.x / bgSize.x, (float)winSize.y / bgSize.y);
    background.setScale(scale, scale);
    FloatRect bgBounds = background.getGlobalBounds();
    background.setPosition((winSize.x - bgBounds.width) / 2.f, (winSize.y - bgBounds.height) / 2.f);

    vector<string> labels = { "Dijkstra Algorithm", "Ford Fulkerson", "A*", "Prims MST Algorithm", "Exit" };
    vector<RectangleShape> buttons;
    vector<Text> texts;

    const float buttonWidth = 300.f, buttonHeight = 50.f, spacing = 70.f;
    const float startY = (winSize.y - (labels.size() * spacing)) / 2;

    for (size_t i = 0; i < labels.size(); ++i) {
        RectangleShape button(Vector2f(buttonWidth, buttonHeight));
        button.setFillColor(Color(50, 50, 50, 200));
        button.setOrigin(buttonWidth / 2, buttonHeight / 2);
        button.setPosition(winSize.x / 2.f, startY + i * spacing);
        buttons.push_back(button);

        Text text;
        text.setFont(font);
        text.setString(labels[i]);
        text.setCharacterSize(20);
        text.setFillColor(Color::White);
        text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
        text.setPosition(button.getPosition());
        texts.push_back(text);
    }

    // Components for each algorithm
    Graph dijkstraGraph;
    InputBox dijkstraInput;
    AStarVisualizer astarVisualizer;
    FordFulkersonVisualizer fordFulkersonVisualizer;
    PrimsVisualizer primsVisualizer;  // Added Prim's algorithm visualizer

    // Set render window for Ford-Fulkerson
    fordFulkersonVisualizer.setRenderWindow(window);

    while (window.isOpen()) {
        Event event;
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (currentState == MENU && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                for (size_t i = 0; i < buttons.size(); ++i) {
                    if (buttons[i].getGlobalBounds().contains(mousePos)) {
                        if (labels[i] == "Exit")
                            window.close();
                        else
                            currentState = static_cast<AppState>(i + 1);
                    }
                }
            }
            else if (currentState == VIEW1) {
                if (event.type == Event::TextEntered) {
                    if (event.text.unicode == '\r') {
                        dijkstraGraph.addEdgeFromText(dijkstraInput.content);
                        dijkstraInput.clear();
                    }
                    else {
                        dijkstraInput.addChar(static_cast<char>(event.text.unicode));
                    }
                }
                if (event.type == Event::MouseButtonPressed) {
                    if (dijkstraGraph.isExitButtonClicked(mousePos)) {
                        currentState = MENU;
                    }
                    else if (dijkstraGraph.isRestartButtonClicked(mousePos)) {
                        dijkstraGraph.reset();
                    }
                    else {
                        dijkstraGraph.handleClick(mousePos);
                    }
                }
            }
            else if (currentState == VIEW2) { // Ford-Fulkerson
                if (!fordFulkersonVisualizer.isAnimating()) {
                    if (event.type == Event::TextEntered) {
                        if (event.text.unicode == '\r') {
                            fordFulkersonVisualizer.processEdgeInput();
                        }
                        else {
                            fordFulkersonVisualizer.handleTextInput(event.text.unicode);
                        }
                    }
                    if (event.type == Event::MouseButtonPressed) {
                        if (fordFulkersonVisualizer.isExitButtonClicked(mousePos)) {
                            currentState = MENU;
                        }
                        else if (fordFulkersonVisualizer.isRestartButtonClicked(mousePos)) {
                            fordFulkersonVisualizer.reset();
                        }
                        else {
                            fordFulkersonVisualizer.handleClick(mousePos);
                        }
                    }
                }
            }
            else if (currentState == VIEW3) { // A* Algorithm Visualizer
                if (event.type == Event::MouseButtonPressed) {
                    if (astarVisualizer.isRestartButtonClicked(mousePos)) {
                        astarVisualizer.reset();
                    }
                    else {
                        astarVisualizer.handleClick(mousePos);
                    }
                }

                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Space)
                    astarVisualizer.runAStar();
            }
            // Added handling for VIEW4 - Prim's MST Algorithm
            else if (currentState == VIEW4) {
                if (event.type == Event::TextEntered) {
                    primsVisualizer.handleTextInput(event.text.unicode);
                }

                if (event.type == Event::MouseButtonPressed) {
                    if (primsVisualizer.isExitButtonClicked(mousePos)) {
                        currentState = MENU;
                    }
                    else if (primsVisualizer.isRestartButtonClicked(mousePos)) {
                        primsVisualizer.reset();
                    }
                    else {
                        primsVisualizer.handleClick(mousePos);
                    }
                }
            }

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                currentState = MENU;
        }

        // Handle algorithm updates
        if (currentState == VIEW1) {
            dijkstraGraph.update();
        }
        else if (currentState == VIEW4) {
            // Update Prim's algorithm animation
            primsVisualizer.update();
        }

        // Drawing logic
        window.clear(Color(30, 30, 30));

        if (currentState == MENU) {
            window.draw(background);
            for (size_t i = 0; i < buttons.size(); ++i) {
                window.draw(buttons[i]);
                window.draw(texts[i]);
            }
        }
        else if (currentState == VIEW1) {
            dijkstraGraph.draw(window);
            dijkstraInput.draw(window);
        }
        else if (currentState == VIEW2) {
            fordFulkersonVisualizer.draw(window);
        }
        else if (currentState == VIEW3) {
            astarVisualizer.draw(window);
        }
        else if (currentState == VIEW4) {
            // Draw MST algorithm interface
            primsVisualizer.draw(window);
        }

        window.display();
    }

    return 0;
}
