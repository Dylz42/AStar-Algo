#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <string>
#include <cmath>
#include <algorithm>

// This is the state that the algorithm aims to reach.
const std::vector<std::vector<int>> GOAL = {
    {1,  2,  3,  4},
    {5,  6,  7,  8},
    {9,  10, 11, 12},
    {13, 14, 15, 0}  
};

// an object to allow the user to change the heuristic
std::string HEURISTIC = "manhattan";


// a structure to fully represent a state of the board
struct State {
    std::vector<std::vector<int>> board;
    int gCost;
    int fCost;
    int blankX;
    int blankY;

// a custom operator for this structure that applows the fCost to be compared
    bool operator<(const State& other) const {
        return fCost > other.fCost;
    }
};


// a method to calculate the hamming/ misplaced heuristic
int hamming(const std::vector<std::vector<int>>& state) {
    int misplaced = 0;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (state[i][j] != 0 && state[i][j] != GOAL[i][j]) {
                misplaced++;
            }
        }
    }

    return misplaced;
}

// a method to calculate the manhatten heuristic
int manhattan(const std::vector<std::vector<int>>& state) {
    int out = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
              int value = state[i][j];
            if (value != 0) { 
                int targetX = (value - 1) / 4;
                int targetY = (value - 1) % 4;
                out += std::abs(i - targetX) + std::abs(j - targetY);
            }

        }
}

    return out;
}

// a method to calculate the inversion distance heuristic
int inversion(const std::vector<std::vector<int>>& state) {
std::vector<int> flatBoard;
    
    // Convert the board to an array
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (state[i][j] != 0) {
                flatBoard.push_back(state[i][j]);
            }
        }
    }
    
    int out = 0;
    
    for (int i = 0; i < flatBoard.size(); i++) {
        for (int j = i + 1; j < flatBoard.size(); ++j) {
            if (flatBoard[i] > flatBoard[j]) {
                out++;
            }
        }
    }
    
    return out;
}

// a container method to choose the heuristic
int heuristic(const std::vector<std::vector<int>>& state) {
    if(HEURISTIC == "hamming"){
        return hamming(state);
    }
    else if(HEURISTIC == "manhattan"){
        return manhattan(state);
    }
    else if(HEURISTIC == "inversion"){
        return inversion(state);
    }
    else{
         std::cout << "unknown heuristic, manhattan applied";
         return manhattan(state);
    }
    
}

// Check if the inputted state is a GOAL state
bool isGoal(const std::vector<std::vector<int>>& state) {return state == GOAL;}

// Find the blank tile in a state
// It just parses through the model until the blank state is found
std::pair<int, int> findBlankTile(const std::vector<std::vector<int>>& state) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (state[i][j] == 0) {
                return {i, j};}
        } }
    return {-1, -1};
}

// flatten the board into a single string
// This is done to make it more efficient to parse through the visited list
std::string boardToString(const std::vector<std::vector<int>>& board) {
    std::string result;
    for (const auto& row : board) {
        for (int value : row) {
            result += std::to_string(value) + ",";
        }
    }
    return result;
}

// Get the states for each possible move on the inputted board
std::vector<State> getNeighbors(const State& current) {
    std::vector<State> neighbors;
    int x = current.blankX;
    int y = current.blankY;
    std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (const auto& direction : directions) {
        int newX = x + direction.first;
        int newY = y + direction.second;

        if (newX >= 0 && newX < 4 && newY >= 0 && newY < 4) {
            State neighbor = current;
            std::swap(neighbor.board[x][y], neighbor.board[newX][newY]);
            neighbor.blankX = newX;
            neighbor.blankY = newY;
            neighbor.gCost = current.gCost + 1;
            neighbor.fCost = neighbor.gCost + heuristic(neighbor.board);
            neighbors.push_back(neighbor);
        }
    }

    return neighbors;
}

// a method to check if the model is solvable to minimise risk of infite loop.
bool isSolvable(const std::vector<std::vector<int>>& state) {
    int invCount = inversion(state);
    return invCount % 2 == 0;
}

// The culminating method that does the A* search
int aStarSearch(const std::vector<std::vector<int>>& initialBoard, std::string h) {
    if (!isSolvable(initialBoard)) {
    return -1;
}

    HEURISTIC = h;
    std::priority_queue<State> queue;
    std::set<std::string> visited;

    std::pair<int,int> blankTile = findBlankTile(initialBoard);
    int blankX = blankTile.first;
    int blankY = blankTile.second;

    State startState = {initialBoard, 0, heuristic(initialBoard), blankX, blankY};
    queue.push(startState);

    while (!queue.empty()) {
        State current = queue.top();
        queue.pop();


        //Check if this state is the goal
        if (isGoal(current.board)) {
            return current.gCost;
        }

        std::string currentBoardString = boardToString(current.board);
        visited.insert(currentBoardString);

        // Get all the neighboring states
        std::vector<State> neighbours = getNeighbors(current);
        for (const auto& neighbor : neighbours) {
            std::string neighborBoardString = boardToString(neighbor.board);

            // Only process neighbor if it hasn't been visited already
            if (visited.find(neighborBoardString) == visited.end()) {
                queue.push(neighbor);
            }
        }
    }
    // return -1 if no solution is found
    return -1;
}

// a wrapper for the A* method for testing
bool testAStar(const std::vector<std::vector<int>>& initialBoard, std::string h, int expected){
    int actual = aStarSearch(initialBoard,h);
    if(actual == expected){
        std::cout << "Good Work :)"; 
        return true;
    }
    else{
        std::cout << "Incorrect. Expected: " << expected << ", but got: " << actual << "\n";
        return false; 
    }


}




int main() {
    // Example initial configurations of the 15-puzzle

    std::string h = "manhattan";

// if its already the goal state
    std::vector<std::vector<int>> b1 = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10,  11, 12},
        {13,  14, 15, 0}
    };
    bool e1 = testAStar(b1,h,0);
    std::cout << e1;
// one move away from goal state
    std::vector<std::vector<int>> b2 = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10,  11, 12},
        {13, 14, 0, 15}
    };
    bool e2 = testAStar(b2,h,1);

// two moves away from goal state
    std::vector<std::vector<int>> b3 = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10,  11, 12},
        {13, 0,  14, 15}
    };
    bool e3 = testAStar(b3,h,2);

// three moves away
        std::vector<std::vector<int>> b4 = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10,  11, 0},
        {13,  14, 15, 12}
    };
    bool e4 = testAStar(b4,h,3);

// five moves away
        std::vector<std::vector<int>> b5 = {
        {1,  2,  3,  0},
        {5,  6,  7,  4},
        {9, 10,  11, 8},
        {13, 14, 15, 12}
    };
    bool e5 = testAStar(b5,h,5);

// reversed board
        std::vector<std::vector<int>> b6 = {
        {15,14,13,12},
        {11,10,9, 8},
        {7, 6, 5, 4},
        {3, 2, 1, 0}
    };
    bool e6 = testAStar(b6,h,80);

// unsolvable
        std::vector<std::vector<int>> b7 = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10,  11, 12},
        {14,13, 15, 0}
    };
    bool e7 = testAStar(b7,h,-1);
}