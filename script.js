const grid = document.getElementById("grid");
const resetButton = document.getElementById("reset");
const message = document.getElementById("message");

const rows = 10;
const cols = 10;
const minesCount = 15;

let minefield = [];
let revealedCount = 0;

function createGrid() {
    minefield = [];
    grid.innerHTML = "";
    message.textContent = "";
    revealedCount = 0;

    for (let i = 0; i < rows; i++) {
        minefield[i] = [];
        for (let j = 0; j < cols; j++) {
            minefield[i][j] = { mine: false, revealed: false, flag: false, adjacent: 0 };

            const cell = document.createElement("div");
            cell.classList.add("cell");
            cell.dataset.row = i;
            cell.dataset.col = j;
            cell.addEventListener("click", () => revealCell(i, j));
            cell.addEventListener("contextmenu", (e) => {
                e.preventDefault();
                toggleFlag(i, j);
            });
            grid.appendChild(cell);
        }
    }

    grid.style.gridTemplateColumns = `repeat(${cols}, 30px)`;

    placeMines();
    calculateAdjacency();
}

function placeMines() {
    let placedMines = 0;
    while (placedMines < minesCount) {
        const row = Math.floor(Math.random() * rows);
        const col = Math.floor(Math.random() * cols);

        if (!minefield[row][col].mine) {
            minefield[row][col].mine = true;
            placedMines++;
        }
    }
}

function calculateAdjacency() {
    for (let i = 0; i < rows; i++) {
        for (let j = 0; j < cols; j++) {
            if (!minefield[i][j].mine) {
                let adjacentMines = 0;
                for (let x = -1; x <= 1; x++) {
                    for (let y = -1; y <= 1; y++) {
                        const newRow = i + x;
                        const newCol = j + y;
                        if (
                            newRow >= 0 &&
                            newCol >= 0 &&
                            newRow < rows &&
                            newCol < cols &&
                            minefield[newRow][newCol].mine
                        ) {
                            adjacentMines++;
                        }
                    }
                }
                minefield[i][j].adjacent = adjacentMines;
            }
        }
    }
}

function revealCell(row, col) {
    const cell = grid.children[row * cols + col];
    const data = minefield[row][col];

    if (data.revealed || data.flag) return;

    data.revealed = true;
    cell.classList.add("revealed");
    revealedCount++;

    if (data.mine) {
        cell.classList.add("mine");
        cell.textContent = "💣";
        message.textContent = "Game Over! You hit a mine.";
        revealAllMines();
        return;
    }

    if (data.adjacent > 0) {
        cell.textContent = data.adjacent;
    } else {
        for (let x = -1; x <= 1; x++) {
            for (let y = -1; y <= 1; y++) {
                const newRow = row + x;
                const newCol = col + y;
                if (
                    newRow >= 0 &&
                    newCol >= 0 &&
                    newRow < rows &&
                    newCol < cols
                ) {
                    revealCell(newRow, newCol);
                }
            }
        }
    }

    if (revealedCount + minesCount === rows * cols) {
        message.textContent = "Congratulations! You've cleared the minefield!";
    }
}

function toggleFlag(row, col) {
    const cell = grid.children[row * cols + col];
    const data = minefield[row][col];

    if (data.revealed) return;

    if (!data.flag) {
        data.flag = true;
        cell.classList.add("flag");
        cell.textContent = "🚩";
    } else {
        data.flag = false;
        cell.classList.remove("flag");
        cell.textContent = "";
    }
}

function revealAllMines() {
    for (let i = 0; i < rows; i++) {
        for (let j = 0; j < cols; j++) {
            if (minefield[i][j].mine) {
                const cell = grid.children[i * cols + j];
                cell.classList.add("mine");
                cell.textContent = "💣";
            }
        }
    }
}

resetButton.addEventListener("click", createGrid);

createGrid();
