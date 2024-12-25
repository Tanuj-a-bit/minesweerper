#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define ROWS 10
#define COLS 10
#define MINES 20

// Directions for adjacent cells
int dirX[] = {0, 1, 0, -1, 1, 1, -1, -1};
int dirY[] = {1, 0, -1, 0, 1, -1, 1, -1};

// TreeNode structure to represent cells
typedef struct TreeNode {
    int x, y;
    bool isMine, isRevealed, isFlagged;
    int adjacentMines;
    struct TreeNode* adj[8]; // Pointers to adjacent cells
} TreeNode;

// Linked List Node for dynamic tracking
typedef struct Node {
    int x, y;
    struct Node* next;
} Node;

// Stack for DFS
typedef struct {
    Node* top;
} Stack;

// Queue for BFS
typedef struct {
    Node* front, *rear;
} Queue;

// Function Prototypes
TreeNode* createTreeNode(int x, int y);
void initializeBoard(TreeNode* board[ROWS][COLS], int mines);
void revealDFS(TreeNode* board[ROWS][COLS], Stack* stack);
void revealBFS(TreeNode* board[ROWS][COLS], Queue* queue);
void push(Stack* stack, int x, int y);
Node* pop(Stack* stack);
void enqueue(Queue* queue, int x, int y);
Node* dequeue(Queue* queue);
void plantMines(TreeNode* board[ROWS][COLS], int mines);
void printBoard(TreeNode* board[ROWS][COLS], bool revealAll);
bool revealCell(TreeNode* board[ROWS][COLS], int x, int y);
void placeFlag(TreeNode* board[ROWS][COLS], int x, int y);
bool checkWin(TreeNode* board[ROWS][COLS]);

// Main function
int main() {
    srand(time(NULL));
    TreeNode* board[ROWS][COLS] = { NULL };
    Stack stack = { NULL };
    Queue queue = { NULL };

    initializeBoard(board, MINES);
    
    printf("Welcome to Minesweeper!\n");

    while (true) {
        printBoard(board, false);

        printf("\nOptions:\n");
        printf("1. Reveal a cell (r x y)\n");
        printf("2. Place a flag (f x y)\n");
        printf("Enter your choice: ");

        char choice;
        int x, y;
        scanf(" %c %d %d", &choice, &x, &y);

        if (x < 0 || x >= ROWS || y < 0 || y >= COLS) {
            printf("Invalid coordinates. Try again.\n");
            continue;
        }

        if (choice == 'r') {
            if (revealCell(board, x, y)) {
                printf("BOOM! You hit a mine. Game over!\n");
                printBoard(board, true);
                break;
            }
        } else if (choice == 'f') {
            placeFlag(board, x, y);
        } else {
            printf("Invalid choice. Try again.\n");
        }

        if (checkWin(board)) {
            printf("Congratulations! You cleared the board!\n");
            printBoard(board, true);
            break;
        }
    }

    return 0;
}

// Create a new TreeNode
TreeNode* createTreeNode(int x, int y) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->x = x;
    node->y = y;
    node->isMine = false;
    node->isRevealed = false;
    node->isFlagged = false;
    node->adjacentMines = 0;
    for (int i = 0; i < 8; i++) node->adj[i] = NULL;
    return node;
}

// Initialize the Minesweeper board
void initializeBoard(TreeNode* board[ROWS][COLS], int mines) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            board[i][j] = createTreeNode(i, j);
        }
    }
    plantMines(board, mines);

    // Set adjacency pointers and calculate adjacent mines
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            for (int d = 0; d < 8; d++) {
                int nx = i + dirX[d], ny = j + dirY[d];
                if (nx >= 0 && ny >= 0 && nx < ROWS && ny < COLS) {
                    board[i][j]->adj[d] = board[nx][ny];
                    if (board[nx][ny]->isMine) board[i][j]->adjacentMines++;
                }
            }
        }
    }
}

// Plant mines randomly
void plantMines(TreeNode* board[ROWS][COLS], int mines) {
    for (int i = 0; i < mines; i++) {
        int x, y;
        do {
            x = rand() % ROWS;
            y = rand() % COLS;
        } while (board[x][y]->isMine);
        board[x][y]->isMine = true;
    }
}

// Reveal a cell
bool revealCell(TreeNode* board[ROWS][COLS], int x, int y) {
    TreeNode* cell = board[x][y];
    if (cell->isRevealed || cell->isFlagged) return false;

    cell->isRevealed = true;

    if (cell->isMine) {
        return true; // Mine exploded
    }

    if (cell->adjacentMines == 0) {
        for (int d = 0; d < 8; d++) {
            if (cell->adj[d] && !cell->adj[d]->isRevealed) {
                revealCell(board, cell->adj[d]->x, cell->adj[d]->y);
            }
        }
    }

    return false;
}

// Place a flag on a cell
void placeFlag(TreeNode* board[ROWS][COLS], int x, int y) {
    TreeNode* cell = board[x][y];
    if (cell->isRevealed) {
        printf("Cannot place a flag on a revealed cell.\n");
        return;
    }
    cell->isFlagged = !cell->isFlagged;
    printf("Flag %s at (%d, %d).\n", cell->isFlagged ? "placed" : "removed", x, y);
}

// Check if the player has won
bool checkWin(TreeNode* board[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            TreeNode* cell = board[i][j];
            if (!cell->isRevealed && !cell->isMine) {
                return false;
            }
        }
    }
    return true;
}

// Print the Minesweeper board
void printBoard(TreeNode* board[ROWS][COLS], bool revealAll) {
    printf("\n   ");
    for (int j = 0; j < COLS; j++) printf("%2d ", j);
    printf("\n");

    for (int i = 0; i < ROWS; i++) {
        printf("%2d ", i);
        for (int j = 0; j < COLS; j++) {
            TreeNode* cell = board[i][j];
            if (revealAll || cell->isRevealed) {
                if (cell->isMine) {
                    printf(" * ");
                } else {
                    printf(" %d ", cell->adjacentMines);
                }
            } else if (cell->isFlagged) {
                printf(" F ");
            } else {
                printf(" . ");
            }
        }
        printf("\n");
    }
}
