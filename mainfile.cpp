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
#include <chrono>

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
    sf::RectangleShape box;
    sf::Text inputText;
    sf::Font font;
    std::string content;

    InputBox() {
        font.loadFromFile("arial.ttf");
        box.setSize(sf::Vector2f(300, 30));
        box.setFillColor(sf::Color(200, 200, 200));
        box.setPosition(20, 20);

        inputText.setFont(font);
        inputText.setCharacterSize(18);
        inputText.setFillColor(sf::Color::Black);
        inputText.setPosition(25, 25);
    }

    void draw(sf::RenderWindow& window) {
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
    sf::RectangleShape exitButton;
    sf::Text exitButtonText;
    sf::RectangleShape restartButton;
    sf::Text restartButtonText;
private:
    struct Node {
        sf::Vector2f position;
        sf::CircleShape shape;
        sf::Text label;
    };

    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<sf::Text> edgeLabels;
    std::vector<sf::Vertex> pathLines;
    std::vector<int> shortestPath;
    size_t animationIndex = 0;
    bool animatePath = false;

    sf::CircleShape walker;
    sf::Font font;
    sf::Text instruction;
    sf::RectangleShape button;
    sf::Text buttonText;

public:
    Graph() {
        font.loadFromFile("arial.ttf");

        instruction.setFont(font);
        instruction.setCharacterSize(16);
        instruction.setFillColor(sf::Color::White);
        instruction.setPosition(20, 60);
        instruction.setString("Click to add nodes. Type edges as: from to weight");

        button.setSize({ 200, 40 });
        button.setPosition(20, 520);
        button.setFillColor(sf::Color::White);

        buttonText.setFont(font);
        buttonText.setCharacterSize(18);
        buttonText.setFillColor(sf::Color::Black);
        buttonText.setString("Find Shortest Path");
        buttonText.setPosition(30, 528);

        exitButton.setSize({ 150, 35 });
        exitButton.setFillColor(sf::Color(150, 0, 0));
        exitButton.setPosition(650, 20);

        exitButtonText.setFont(font);
        exitButtonText.setCharacterSize(18);
        exitButtonText.setFillColor(sf::Color::White);
        exitButtonText.setString("Exit");
        exitButtonText.setPosition(680, 25);


        walker.setRadius(8.f);
        walker.setFillColor(sf::Color::Yellow);
        walker.setOrigin(8.f, 8.f);
        //new code added
        restartButton.setSize({ 150, 35 });
        restartButton.setFillColor(sf::Color(0, 150, 0));
        restartButton.setPosition(470, 20);

        restartButtonText.setFont(font);
        restartButtonText.setCharacterSize(18);
        restartButtonText.setFillColor(sf::Color::White);
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
    bool isRestartButtonClicked(sf::Vector2f pos) {
        return restartButton.getGlobalBounds().contains(pos);
    }
    void draw(sf::RenderWindow& window) {
        for (size_t i = 0; i < edges.size(); i++) {
            sf::Vertex line[] = {
                sf::Vertex(nodes[edges[i].from].position, sf::Color::White),
                sf::Vertex(nodes[edges[i].to].position, sf::Color::White)
            };
            window.draw(line, 2, sf::Lines);
            window.draw(edgeLabels[i]);
        }

        if (!pathLines.empty())
            window.draw(&pathLines[0], pathLines.size(), sf::Lines);

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

    void addNode(sf::Vector2f pos) {
        Node node;
        node.position = pos;
        node.shape.setRadius(10);
        node.shape.setFillColor(sf::Color::Blue);
        node.shape.setPosition(pos - sf::Vector2f(10, 10));

        node.label.setFont(font);
        node.label.setCharacterSize(15);
        node.label.setFillColor(sf::Color::White);
        node.label.setString(std::to_string(nodes.size()));
        node.label.setPosition(pos.x - 5, pos.y - 20);

        nodes.push_back(node);
    }

    void addEdgeFromText(const std::string& str) {
        std::istringstream iss(str);
        int u, v;
        float w;
        if (iss >> u >> v >> w) {
            edges.push_back({ u, v, w });
            edges.push_back({ v, u, w });

            sf::Text label;
            label.setFont(font);
            label.setCharacterSize(15);
            label.setFillColor(sf::Color::Red);
            sf::Vector2f mid = (nodes[u].position + nodes[v].position) / 2.f;
            label.setPosition(mid);
            label.setString(std::to_string(static_cast<int>(w)));
            edgeLabels.push_back(label);
            edgeLabels.push_back(label);
        }
    }

    void handleClick(sf::Vector2f pos) {
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
            sf::Vector2f current = walker.getPosition();
            sf::Vector2f target = nodes[shortestPath[animationIndex + 1]].position;
            sf::Vector2f dir = target - current;
            float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
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
        std::vector<float> dist(n, std::numeric_limits<float>::max());
        std::vector<int> parent(n, -1);
        dist[start] = 0;
        using P = std::pair<float, int>;
        std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
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
        std::reverse(shortestPath.begin(), shortestPath.end());

        pathLines.clear();
        for (size_t i = 1; i < shortestPath.size(); ++i) {
            pathLines.push_back(sf::Vertex(nodes[shortestPath[i - 1]].position, sf::Color::Cyan));
            pathLines.push_back(sf::Vertex(nodes[shortestPath[i]].position, sf::Color::Cyan));
        }
    }
    bool isExitButtonClicked(sf::Vector2f pos) {
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
    std::vector<std::vector<Cell>> grid;
    Cell* start = nullptr;
    Cell* end = nullptr;
    bool running = false;
    bool pathFound = false;
    //new member variable for reset buuton
    sf::RectangleShape restartButton;
    sf::Text restartButtonText;
    sf::Font font;
public:
    AStarVisualizer() {
        grid.resize(rows, std::vector<Cell>(cols));
        for (int r = 0; r < rows; ++r)
            for (int c = 0; c < cols; ++c)
                grid[r][c] = { r, c };

        font.loadFromFile("arial.ttf");
        restartButton.setSize({ 150, 35 });
        restartButton.setFillColor(sf::Color(0, 150, 0));
        restartButton.setPosition(700, 20);

        restartButtonText.setFont(font);
        restartButtonText.setCharacterSize(18);
        restartButtonText.setFillColor(sf::Color::White);
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

    bool isRestartButtonClicked(sf::Vector2f pos) {
        return restartButton.getGlobalBounds().contains(pos);
    }

    void draw(sf::RenderWindow& window) {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                Cell& cell = grid[r][c];
                sf::RectangleShape rect(sf::Vector2f(cellSize - 1, cellSize - 1));
                rect.setPosition(c * cellSize, r * cellSize);

                if (&cell == start) rect.setFillColor(sf::Color::Green);
                else if (&cell == end) rect.setFillColor(sf::Color::Red);
                else if (cell.isWall) rect.setFillColor(sf::Color(50, 50, 50));
                else if (cell.isPath) rect.setFillColor(sf::Color::Yellow);
                else if (cell.isVisited) rect.setFillColor(sf::Color(100, 100, 255));
                else rect.setFillColor(sf::Color::White);

                window.draw(rect);
            }
        }
        window.draw(restartButton);
        window.draw(restartButtonText);
    }

    void handleClick(sf::Vector2f pos) {
        int row = pos.y / cellSize;
        int col = pos.x / cellSize;
        if (row >= 0 && row < rows && col >= 0 && col < cols) {
            Cell* clicked = &grid[row][col];
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                if (!start) start = clicked;
                else if (!end && clicked != start) end = clicked;
                else if (clicked != start && clicked != end) clicked->isWall = !clicked->isWall;
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
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

        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
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
        return std::abs(a->row - b->row) + std::abs(a->col - b->col); // Manhattan distance
    }

    std::vector<Cell*> neighbors(Cell* cell) {
        std::vector<Cell*> result;

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
    sf::RectangleShape restartButton;
    sf::Text restartButtonText;

private:
    struct Node {
        sf::Vector2f position;
        sf::CircleShape shape;
        sf::Text label;
    };

    std::vector<Node> nodes;
    std::vector<Edge> edges;
    sf::Font font;

    sf::RectangleShape button;
    sf::Text buttonText;
    sf::Text instructionText;
    sf::Text maxFlowText;
    sf::Text inputPrompt;
    sf::Text inputText;
    sf::RectangleShape exitButton;
    sf::Text exitButtonText;

    std::string userInput;

    bool animating = false;
    std::vector<int> currentPath;
    size_t animationIndex = 0;
    sf::CircleShape walker;

    float totalFlow = 0;

    sf::RenderWindow* renderWindow = nullptr; // For animation

public:
    FordFulkersonVisualizer() {
        font.loadFromFile("arial.ttf");

        button.setSize(sf::Vector2f(180, 40));
        button.setFillColor(sf::Color::Green);
        button.setPosition(600, 20);

        buttonText.setFont(font);
        buttonText.setString("Find Max Flow");
        buttonText.setCharacterSize(18);
        buttonText.setFillColor(sf::Color::White);
        buttonText.setPosition(610, 30);

        instructionText.setFont(font);
        instructionText.setCharacterSize(16);
        instructionText.setFillColor(sf::Color::White);
        instructionText.setPosition(20, 10);
        instructionText.setString("Click to add nodes. Type edges as: from to capacity");

        inputPrompt.setFont(font);
        inputPrompt.setCharacterSize(16);
        inputPrompt.setFillColor(sf::Color::Yellow);
        inputPrompt.setPosition(20, 520);
        inputPrompt.setString("Edge Input (e.g. 0 1 10):");

        inputText.setFont(font);
        inputText.setCharacterSize(16);
        inputText.setFillColor(sf::Color::Cyan);
        inputText.setPosition(250, 520);

        maxFlowText.setFont(font);
        maxFlowText.setCharacterSize(20);
        maxFlowText.setFillColor(sf::Color::White);
        maxFlowText.setPosition(20, 550);

        exitButton.setSize({ 150, 35 });
        exitButton.setFillColor(sf::Color(150, 0, 0));
        exitButton.setPosition(650, 70);

        exitButtonText.setFont(font);
        exitButtonText.setCharacterSize(18);
        exitButtonText.setFillColor(sf::Color::White);
        exitButtonText.setString("Exit");
        exitButtonText.setPosition(680, 75);

        walker.setRadius(8);
        walker.setFillColor(sf::Color::Cyan);
        walker.setOrigin(8, 8);
        //new constructor element is added
        restartButton.setSize({ 150, 35 });
        restartButton.setFillColor(sf::Color::Magenta);
        restartButton.setPosition(420, 20);

        restartButtonText.setFont(font);
        restartButtonText.setCharacterSize(18);
        restartButtonText.setFillColor(sf::Color::White);
        restartButtonText.setString("Restart");
        restartButtonText.setPosition(500, 25);
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

    void setRenderWindow(sf::RenderWindow& win) {
        renderWindow = &win;
    }

    bool isAnimating() const {
        return animating;
    }

    bool isExitButtonClicked(sf::Vector2f pos) {
        return exitButton.getGlobalBounds().contains(pos);
    }

    void handleClick(sf::Vector2f pos) {
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
        sf::CircleShape circle(15);
        circle.setFillColor(sf::Color::Blue);
        circle.setPosition(x - 15, y - 15);

        sf::Text label;
        label.setFont(font);
        label.setString(std::to_string(nodes.size()));
        label.setCharacterSize(14);
        label.setFillColor(sf::Color::White);
        label.setPosition(x - 5, y - 30);

        nodes.push_back({ {x, y}, circle, label });
    }

    void addEdge(int from, int to, float capacity) {
        if (from >= 0 && from < nodes.size() && to >= 0 && to < nodes.size()) {
            edges.push_back({ from, to, 0, capacity, 0 }); // weight=0 for FF
            edges.push_back({ to, from, 0, 0, 0 });       // reverse edge with zero capacity
        }
    }

    void handleTextInput(sf::Uint32 unicode) {
        if (unicode == 8 && !userInput.empty()) {
            userInput.pop_back();
        }
        else if (unicode >= 32 && unicode < 128) {
            userInput += static_cast<char>(unicode);
        }
        inputText.setString(userInput);
    }

    void processEdgeInput() {
        std::istringstream iss(userInput);
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
        std::vector<int> parent(nodes.size());
        animating = true;

        while (bfs(0, nodes.size() - 1, parent)) {
            float pathFlow = 1e9f;
            for (int v = nodes.size() - 1; v != 0; v = parent[v]) {
                int u = parent[v];
                pathFlow = std::min(pathFlow, getResidualCapacity(u, v));
            }

            for (int v = nodes.size() - 1; v != 0; v = parent[v]) {
                int u = parent[v];
                updateFlow(u, v, pathFlow);
            }

            totalFlow += pathFlow;

            currentPath.clear();
            for (int v = nodes.size() - 1; v != -1; v = parent[v])
                currentPath.push_back(v);
            std::reverse(currentPath.begin(), currentPath.end());

            walker.setPosition(nodes[currentPath[0]].position);
            animationIndex = 0;

            animateWalkerAlongPath();

            std::stringstream ss;
            ss << "Max Flow so far: " << totalFlow;
            maxFlowText.setString(ss.str());

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        std::stringstream ss;
        ss << "Final Max Flow: " << totalFlow;
        maxFlowText.setString(ss.str());

        animating = false;
    }

    void animateWalkerAlongPath() {
        if (currentPath.size() < 2) return;

        for (size_t i = 0; i < currentPath.size() - 1; ++i) {
            sf::Vector2f start = nodes[currentPath[i]].position;
            sf::Vector2f end = nodes[currentPath[i + 1]].position;

            sf::Vector2f dir = end - start;
            float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
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

                sf::sleep(sf::milliseconds(10));
            }

            walker.setPosition(end);
            if (renderWindow != nullptr) {
                renderWindow->clear();
                draw(*renderWindow);
                renderWindow->display();
            }

            sf::sleep(sf::milliseconds(200));  // Pause at node
        }
    }

    bool bfs(int src, int sink, std::vector<int>& parent) {
        std::vector<bool> visited(nodes.size(), false);
        std::queue<int> q;
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

    bool isRestartButtonClicked(sf::Vector2f pos) {
        return restartButton.getGlobalBounds().contains(pos);
    }

    void draw(sf::RenderWindow& window) {
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
            sf::Vertex line[] = {
                sf::Vertex(nodes[edge.from].position, sf::Color::White),
                sf::Vertex(nodes[edge.to].position, sf::Color::White)
            };
            window.draw(line, 2, sf::Lines);

            // Draw flow/capacity
            sf::Text flowText;
            flowText.setFont(font);
            flowText.setCharacterSize(14);
            flowText.setFillColor(sf::Color::Yellow);
            sf::Vector2f mid = (nodes[edge.from].position + nodes[edge.to].position) / 2.f;
            flowText.setPosition(mid);
            flowText.setString(std::to_string(static_cast<int>(edge.flow)) + "/" +
                std::to_string(static_cast<int>(edge.capacity)));
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
        sf::Vector2f position;
        sf::CircleShape shape;
        sf::Text label;
    };

    struct AnimationStep {
        int currentNode;
        int addedNode;
        int fromNode;
        float weight;
        bool isNodeSelection; // true for node selection, false for edge addition
    };

    std::vector<Node> nodes;
    std::vector<Edge> edges;  // Using the Edge struct already defined in the code
    std::vector<sf::Text> edgeLabels;
    std::vector<sf::Vertex> mstLines;

    sf::Font font;
    sf::Text buttonText, mstWeightText, instructionText, inputPrompt, inputText, statusText;
    sf::RectangleShape button;
    sf::RectangleShape exitButton;
    sf::Text exitButtonText;
    sf::RectangleShape restartButton;
    sf::Text restartButtonText;

    std::string userInput;
    float totalMSTWeight = 0.0f;
    bool showMST = false;

    // Animation-related members
    std::vector<AnimationStep> animationSteps;
    size_t currentAnimationStep = 0;
    sf::Clock animationClock;
    bool isAnimating = false;
    float animationSpeed = 2.0f; // seconds per step
    std::vector<int> nodesInMST;
    std::vector<sf::Vertex> animatedMSTLines;

public:
    PrimsVisualizer() {
        font.loadFromFile("arial.ttf");

        button.setSize(sf::Vector2f(200, 40));
        button.setFillColor(sf::Color::Green);
        button.setPosition(580, 20);

        buttonText.setFont(font);
        buttonText.setString("Run Prim's Algorithm");
        buttonText.setCharacterSize(16);
        buttonText.setFillColor(sf::Color::White);
        buttonText.setPosition(590, 28);

        instructionText.setFont(font);
        instructionText.setCharacterSize(14);
        instructionText.setFillColor(sf::Color::White);
        instructionText.setPosition(20, 10);
        instructionText.setString("Click to add nodes. Type edges like: 0 1 10 and press Enter.");

        inputPrompt.setFont(font);
        inputPrompt.setCharacterSize(14);
        inputPrompt.setFillColor(sf::Color::Yellow);
        inputPrompt.setPosition(20, 520);
        inputPrompt.setString("Edge Input (format: from to weight):");

        inputText.setFont(font);
        inputText.setCharacterSize(14);
        inputText.setFillColor(sf::Color::Cyan);
        inputText.setPosition(250, 520);

        mstWeightText.setFont(font);
        mstWeightText.setCharacterSize(18);
        mstWeightText.setFillColor(sf::Color::Cyan);
        mstWeightText.setPosition(20, 550);

        statusText.setFont(font);
        statusText.setCharacterSize(18);
        statusText.setFillColor(sf::Color::Yellow);
        statusText.setPosition(20, 480);
        statusText.setString("");

        exitButton.setSize({ 150, 35 });
        exitButton.setFillColor(sf::Color(150, 0, 0));
        exitButton.setPosition(650, 70);

        exitButtonText.setFont(font);
        exitButtonText.setCharacterSize(18);
        exitButtonText.setFillColor(sf::Color::White);
        exitButtonText.setString("Exit");
        exitButtonText.setPosition(680, 75);

        restartButton.setSize({ 150, 35 });
        restartButton.setFillColor(sf::Color(0, 150, 0));
        restartButton.setPosition(420, 20);

        restartButtonText.setFont(font);
        restartButtonText.setCharacterSize(18);
        restartButtonText.setFillColor(sf::Color::White);
        restartButtonText.setString("Restart");
        restartButtonText.setPosition(500, 25);
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
    }

    void addNode(float x, float y) {
        int id = nodes.size();
        Node node;
        node.position = { x, y };
        node.shape.setRadius(10.f);
        node.shape.setFillColor(sf::Color::Blue);
        node.shape.setPosition(x - 10.f, y - 10.f);

        node.label.setFont(font);
        node.label.setString(std::to_string(id));
        node.label.setCharacterSize(14);
        node.label.setFillColor(sf::Color::White);
        node.label.setPosition(x - 5.f, y - 25.f);

        nodes.push_back(node);
    }

    void addEdge(int from, int to, float weight) {
        if (from >= 0 && to >= 0 && from < nodes.size() && to < nodes.size()) {
            edges.push_back({ from, to, weight });
            edges.push_back({ to, from, weight });

            sf::Text label;
            label.setFont(font);
            label.setString(std::to_string(static_cast<int>(weight)));
            label.setCharacterSize(14);
            label.setFillColor(sf::Color::Red);

            // Position label midway between nodes
            sf::Vector2f midpoint = (nodes[from].position + nodes[to].position) / 2.f;
            label.setPosition(midpoint);

            edgeLabels.push_back(label);
            edgeLabels.push_back(label);  // Duplicate for both directions
        }
    }

    void handleClick(sf::Vector2f pos) {
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
        else {
            addNode(pos.x, pos.y);
        }
    }

    void handleTextInput(sf::Uint32 unicode) {
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
        std::istringstream iss(userInput);
        int from, to;
        float weight;
        if (iss >> from >> to >> weight) {
            addEdge(from, to, weight);
        }
        userInput.clear();
        inputText.setString("");
    }

    void runPrimsAlgorithm() {
        int n = nodes.size();
        if (n == 0) return;

        // Reset previous animation state
        animationSteps.clear();
        nodesInMST.clear();
        animatedMSTLines.clear();
        currentAnimationStep = 0;
        totalMSTWeight = 0.0f;

        std::vector<bool> inMST(n, false);
        std::vector<float> key(n, std::numeric_limits<float>::max());
        std::vector<int> parent(n, -1);
        key[0] = 0;

        using P = std::pair<float, int>;
        std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
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

            for (const auto& edge : edges) {
                if (edge.from == u) {
                    int v = edge.to;
                    float w = edge.weight;
                    if (!inMST[v] && w < key[v]) {
                        key[v] = w;
                        parent[v] = u;
                        pq.push({ key[v], v });

                        // Add edge consideration to animation steps
                        // This represents considering this edge as a candidate
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
            nodes[0].shape.setFillColor(sf::Color::Green);
        }
    }

    void nextAnimationStep() {
        if (!isAnimating || currentAnimationStep >= animationSteps.size()) {
            // Animation complete
            finishAnimation();
            return;
        }

        AnimationStep& step = animationSteps[currentAnimationStep];

        if (step.isNodeSelection) {
            // Adding a node to MST
            if (step.addedNode < nodes.size()) {
                nodes[step.addedNode].shape.setFillColor(sf::Color::Green);

                // Add to MST nodes list
                nodesInMST.push_back(step.addedNode);

                // Add edge to MST if this isn't the first node
                if (step.fromNode != -1) {
                    sf::Vertex line[] = {
                        sf::Vertex(nodes[step.fromNode].position, sf::Color::Cyan),
                        sf::Vertex(nodes[step.addedNode].position, sf::Color::Cyan)
                    };
                    animatedMSTLines.push_back(line[0]);
                    animatedMSTLines.push_back(line[1]);

                    totalMSTWeight += step.weight;
                    mstWeightText.setString("MST Weight: " + std::to_string(static_cast<int>(totalMSTWeight)));
                }

                statusText.setString("Added node " + std::to_string(step.addedNode) + " to MST");
            }
        }
        else {
            // Considering an edge
            statusText.setString("Considering edge from " + std::to_string(step.fromNode) +
                " to " + std::to_string(step.addedNode) +
                " with weight " + std::to_string(static_cast<int>(step.weight)));
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
            std::to_string(static_cast<int>(totalMSTWeight)));
    }

    void update() {
        if (isAnimating && animationClock.getElapsedTime().asSeconds() > animationSpeed) {
            nextAnimationStep();
        }
    }

    bool isExitButtonClicked(sf::Vector2f pos) {
        return exitButton.getGlobalBounds().contains(pos);
    }

    bool isRestartButtonClicked(sf::Vector2f pos) {
        return restartButton.getGlobalBounds().contains(pos);
    }

    bool getIsAnimating() const {
        return isAnimating;
    }

    void draw(sf::RenderWindow& window) {
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
            sf::Vertex line[] = {
                sf::Vertex(nodes[edges[i].from].position, sf::Color::White),
                sf::Vertex(nodes[edges[i].to].position, sf::Color::White)
            };
            window.draw(line, 2, sf::Lines);
            if (i / 2 < edgeLabels.size())
                window.draw(edgeLabels[i / 2]);
        }

        // Draw MST edges if algorithm has been run
        if (!animatedMSTLines.empty()) {
            window.draw(&animatedMSTLines[0], animatedMSTLines.size(), sf::Lines);
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
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Algorithm Visualizer", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    AppState currentState = MENU;
    sf::Font font;
    font.loadFromFile("arial.ttf");

    sf::Texture bgTexture;
    bgTexture.loadFromFile("algo10.jpeg");
    sf::Sprite background(bgTexture);

    // Center background
    sf::Vector2u bgSize = bgTexture.getSize();
    sf::Vector2u winSize = window.getSize();
    float scale = std::min((float)winSize.x / bgSize.x, (float)winSize.y / bgSize.y);
    background.setScale(scale, scale);
    sf::FloatRect bgBounds = background.getGlobalBounds();
    background.setPosition((winSize.x - bgBounds.width) / 2.f, (winSize.y - bgBounds.height) / 2.f);

    std::vector<std::string> labels = { "Dijkstra Algorithm", "Ford Fulkerson", "A*", "Prims MST Algorithm", "Exit" };
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> texts;

    const float buttonWidth = 300.f, buttonHeight = 50.f, spacing = 70.f;
    const float startY = (winSize.y - (labels.size() * spacing)) / 2;

    for (size_t i = 0; i < labels.size(); ++i) {
        sf::RectangleShape button(sf::Vector2f(buttonWidth, buttonHeight));
        button.setFillColor(sf::Color(50, 50, 50, 200));
        button.setOrigin(buttonWidth / 2, buttonHeight / 2);
        button.setPosition(winSize.x / 2.f, startY + i * spacing);
        buttons.push_back(button);

        sf::Text text;
        text.setFont(font);
        text.setString(labels[i]);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);
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
        sf::Event event;
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (currentState == MENU && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
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
                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == '\r') {
                        dijkstraGraph.addEdgeFromText(dijkstraInput.content);
                        dijkstraInput.clear();
                    }
                    else {
                        dijkstraInput.addChar(static_cast<char>(event.text.unicode));
                    }
                }
                if (event.type == sf::Event::MouseButtonPressed) {
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
                    if (event.type == sf::Event::TextEntered) {
                        if (event.text.unicode == '\r') {
                            fordFulkersonVisualizer.processEdgeInput();
                        }
                        else {
                            fordFulkersonVisualizer.handleTextInput(event.text.unicode);
                        }
                    }
                    if (event.type == sf::Event::MouseButtonPressed) {
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
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (astarVisualizer.isRestartButtonClicked(mousePos)) {
                        astarVisualizer.reset();
                    }
                    else {
                        astarVisualizer.handleClick(mousePos);
                    }
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
                    astarVisualizer.runAStar();
            }
            // Added handling for VIEW4 - Prim's MST Algorithm
            else if (currentState == VIEW4) {
                if (event.type == sf::Event::TextEntered) {
                    primsVisualizer.handleTextInput(event.text.unicode);
                }

                if (event.type == sf::Event::MouseButtonPressed) {
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

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
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
        window.clear(sf::Color(30, 30, 30));

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
