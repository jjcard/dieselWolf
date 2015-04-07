//Program Name: Diesel Wolf
//Author: John Hill
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;


void computeMinimax();
int min(int depth, int &maxFoundSoFar);
int max(int depth, int &minFoundSoFar);
int evaluate();
void getPlayerMove();
void setup();
void printboard();
void makeMove(int move[4], int pieceTaken[3]);
void retractMove(int move[4], int pieceTaken[3]);
int getPossibleMovesMin(int re[100][4]);
int getPossibleMovesMax(int re[100][4]);
bool moveInBounds(int to_col, int to_row);
void moveKnight(int re[100][4], int &count, int i, int j, int kingVal);
void moveKing(int re[100][4], int &count, int i, int j);
void moveBishop(int re[100][4], int &count, int i, int j, int kingVal);
bool foundWholeMove(int re[100][4], int count, int to_row, int to_col, int from_row, int from_col);
bool isMovePossibleMin(int re[100][4], int count, int from_row, int from_col, int to_row, int to_col);
void gameOver(bool maxPlayerOne);
bool isPlayerGoingFirst();
void sortMoves(int moves[100][4], int &moveCount, int depth);




const int BOARD_ROWS = 7;
const int BOARD_COLS = 8;

const int GAS_MAX = 3;

const int KING_MAX = 4;
const int QUEEN_MAX = 3;
const int KNIGHT_MAX = 2;
const int BISHOP_MAX = 1;

const char KING_MAX_C = 'K';
const char QUEEN_MAX_C = 'Q';
const char KNIGHT_MAX_C = 'N';
const char BISHOP_MAX_C = 'B';

const int KING_MIN = -4;
const int QUEEN_MIN = -3;
const int KNIGHT_MIN = -2;
const int BISHOP_MIN = -1;


const char KING_MIN_C = 'k';
const char QUEEN_MIN_C = 'q';
const char KNIGHT_MIN_C = 'n';
const char BISHOP_MIN_C = 'b';


const char printValues[9] = { KING_MIN_C, QUEEN_MIN_C, KNIGHT_MIN_C, BISHOP_MIN_C, '-', BISHOP_MAX_C, KNIGHT_MAX_C, QUEEN_MAX_C, KING_MAX_C };
//piece evalution values
const int evalValues[9] = {-30000, -900, -300, -325, 0, 325, 300, 900, 30000};
//fuel evalution values - want pieces to be able to move, so less then piece evalution values
const int evalFuelValues[9] = { -899, -750, -270, -290, 0, 290, 270, 750, 899 };
//minimax stuff
const int BEST_MAX =  9000000;
const int BEST_MIN = -9000000;
const int WIN_MAX =   2000000;
const int WIN_MIN =  -2000000;

//so index is [row, col] 
//[6, 0] [6, 1] [6, 2] [6, 3] [6, 4] [6, 5] [6, 6] [6, 7]
//[5, 0] [5, 1] [5, 2] [5, 3] [5, 4] [5, 5] [5, 6] [5, 7]
//[4, 0] [4, 1] [4, 2] [4, 3] [4, 4] [4, 5] [4, 6] [4, 7]
//[3, 0] [3, 1] [3, 2] [3, 3] [3, 4] [3, 5] [3, 6] [3, 7]
//[2, 0] [2, 1] [2, 2] [2, 3] [2, 4] [2, 5] [2, 6] [2, 7]
//[1, 0] [1, 1] [1, 2] [1, 3] [1, 4] [1, 5] [1, 6] [1, 7]
//[0, 0] [0, 1] [0, 2] [0, 3] [0, 4] [0, 5] [0, 6] [0, 7]
int b[BOARD_ROWS][BOARD_COLS][2];

//const int maxDepth = 7;


//the killer moves for the game
const int numKillerMoves = 2;
int killerMoves[100][numKillerMoves][4];


//iterative deepening
int evalCount = 0;
const int maxEvalCount = 15499999;
//const int maxEvalCount = 4411612;
bool stopSearch = false;
int currentMaxDepth = 2;
//end game it will run forever, need to stop it somewhere
const int maxDepth = 50;



clock_t start;
double duration;
int main(){
	setup();
	bool playerGoingFirst = isPlayerGoingFirst();
	printboard();
	if (!playerGoingFirst){
		computeMinimax();
		printboard();
	}
	for (;;){
		getPlayerMove();
		printboard();
		computeMinimax();
		printboard();
	}
	
}
int evaluate(){
	evalCount++;	
	if (evalCount >= maxEvalCount){
		stopSearch = true;
		//cout << "In evaluate, above count, setting stop search to true" << endl;
		return 0;
	}
	int eval = 0;
	int curVal;
	//int curFuel;
	for (int i = BOARD_ROWS - 1; i >= 0; i--){
		for (int j = 0; j < BOARD_COLS; j++){
			curVal = b[i][j][0];
			if (curVal != 0){
				//an actual piece
				eval += evalValues[curVal + 4] + b[i][j][1] * evalFuelValues[curVal + 4];
			}
		}
	}


	return eval;
}
bool isPlayerGoingFirst(){
	cout << "Do you want to go first or second? (1/2) ";
	int turn;

	cin >> turn;
	//have it choose 1 if they enter something weird
	return turn != 2;
}
void computeMinimax(){
	start = clock();
	int bestScore = BEST_MIN;
	evalCount = 0;
	int *bestMove;
	int *currentBestMove;

	int moves[100][4] ;
	int count = getPossibleMovesMax(moves);
	if (count == 0){
		gameOver(false);
	}
	else {
		currentBestMove = moves[0];
		bestMove = currentBestMove;
		int curScore;
		int pieceTaken[3];
		stopSearch = false;
		//check to see if can win right away
		for (int i = 0; i < count; i++){
			if (b[moves[i][2]][moves[i][3]][0] == KING_MIN) {
				//taken their king!
				curScore = WIN_MAX;
				bestMove = moves[i];
				stopSearch = true;
				break;
			}
		}
		
		if (!stopSearch){
			//didn't find a winning move in the first move
			currentMaxDepth = 2;
			while (true){
				bestScore = BEST_MIN;
				for (int i = 0; i < count; i++){
					makeMove(moves[i], pieceTaken);
					curScore = min(2, bestScore);
					if (curScore > bestScore){
						bestScore = curScore;
						currentBestMove = moves[i];
					}
					retractMove(moves[i], pieceTaken);
					if (stopSearch){
						break;
					}
				}
				if (stopSearch || currentMaxDepth >= maxDepth){
					break;
				}
				else {
					bestMove = currentBestMove;
					currentMaxDepth++;
				}
			}
		}

		//make the best move
		makeMove(bestMove, pieceTaken);

		//print out move in notation of screen coordinites
		int from_row_act = bestMove[0] + 1;
		char from_col_act = bestMove[1]+ 'A';
		
		int to_row_act = bestMove[2] + 1;
		char to_col_act = bestMove[3] + 'A';
		
		cout << "I moved from " << from_col_act << from_row_act << " to " << to_col_act << to_row_act << "(" << from_col_act << (8 - from_row_act) << " to " << to_col_act << (8 - to_row_act) << ")" << endl;

		duration = (clock() - start) / (double)CLOCKS_PER_SEC;
		cout << "It took " << duration << " seconds to make this decision" << endl;

		cout << "And the eval count was " << evalCount << endl;
		cout << "And the max Depth was " << (currentMaxDepth -1) << endl;
		//we captured their king!
		if (pieceTaken[0] == KING_MIN){
			gameOver(true);
		}

		//move up killer move pieces
		for (int i = 2; i < currentMaxDepth - 1; i++){
			for (int j = 0; j < numKillerMoves; j++){
				killerMoves[i][j][0] = killerMoves[i + 2][j][0];
				killerMoves[i][j][1] = killerMoves[i + 2][j][1];
				killerMoves[i][j][2] = killerMoves[i + 2][j][2];
				killerMoves[i][j][3] = killerMoves[i + 2][j][3];
			}
		}
	}
}
int min(int depth, int &maxFoundSoFar){
	
	int moves[100][4];
	int count = getPossibleMovesMin(moves);
	if (count == 0){
		evalCount++;
		//human can't move, so I win
		return WIN_MAX - depth;
	}
	if (depth == currentMaxDepth){ return evaluate(); }
	int bestScore = BEST_MAX;

	sortMoves(moves, count, depth);
	int curScore;
	int pieceTaken[3];
	for (int i = 0; i < count; i++){
		makeMove(moves[i], pieceTaken);
		if (pieceTaken[0] == KING_MAX){
			evalCount++;
			//taken my king!
			curScore = WIN_MIN + depth;
		}
		else {
			curScore = max(depth + 1, bestScore);
		}

		retractMove(moves[i], pieceTaken);		
		
		if (stopSearch){
			return bestScore;
		}
		if (curScore < bestScore){
			bestScore = curScore;
			if (bestScore <= maxFoundSoFar){
				//alpha beta prune

				//move everything down the killer move list
				for (int j = numKillerMoves - 2; j >= 0; j--){
					//cout << "IN min, moving killer move depth " << depth << " level "<< j <<endl;
					killerMoves[depth][j + 1][0] = killerMoves[depth][j][0];
					killerMoves[depth][j + 1][1] = killerMoves[depth][j][1];
					killerMoves[depth][j + 1][2] = killerMoves[depth][j][2];
					killerMoves[depth][j + 1][3] = killerMoves[depth][j][3];
				}
				//add to killer move list
				killerMoves[depth][0][0] = moves[i][0];
				killerMoves[depth][0][1] = moves[i][1];
				killerMoves[depth][0][2] = moves[i][2];
				killerMoves[depth][0][3] = moves[i][3];

				return bestScore;
			}
		}

	}
	return bestScore;
}
int max(int depth, int &minFoundSoFar){
	
	int moves[100][4];
	int count = getPossibleMovesMax(moves);
	if (count == 0){
		evalCount++;
		//I can't move, so human wins
		return WIN_MIN + depth;
	}
	if (depth == currentMaxDepth){ return evaluate(); }
	int bestScore = BEST_MIN;

	sortMoves(moves, count, depth);
	int curScore;
	int pieceTaken[3];
	for (int i = 0; i < count; i++){
		makeMove(moves[i], pieceTaken);

		if (pieceTaken[0] == KING_MIN){
			evalCount++;
			//taken their king!
			curScore = WIN_MAX - depth;
		}
		else {
			curScore = min(depth + 1, bestScore);
		}

		retractMove(moves[i], pieceTaken);		
		if (stopSearch){
			return bestScore;
		}
		if (curScore > bestScore){
			bestScore = curScore;
			if (bestScore >= minFoundSoFar){
				//alpha beta prune, it can only be bigger
				//move everything down the killer move list
				for (int j = numKillerMoves -2; j >= 0 ; j--){
					//cout << "IN max, moving killer move depth " << depth << " level " << j << endl;
					killerMoves[depth][j + 1][0] = killerMoves[depth][j][0];
					killerMoves[depth][j + 1][1] = killerMoves[depth][j][1];
					killerMoves[depth][j + 1][2] = killerMoves[depth][j][2];
					killerMoves[depth][j + 1][3] = killerMoves[depth][j][3];
				}
				//add to killer move list
				killerMoves[depth][0][0] = moves[i][0];
				killerMoves[depth][0][1] = moves[i][1];
				killerMoves[depth][0][2] = moves[i][2];
				killerMoves[depth][0][3] = moves[i][3];
				return bestScore;
			}
		}

	}
	return bestScore;
}
void sortMoves(int moves[100][4], int &moveCount, int depth){
	//int (*killerMovesDepth)[4] = killerMoves[depth];
	int *curMove;
	int curSwapPosition = 0;
	int swapTemp[4];

	for (int i = 0; i < numKillerMoves; i++){
		for (int j = curSwapPosition; j < moveCount; j++){
			curMove = moves[j];
			if (killerMoves[depth][i][0] == curMove[0] && 
				killerMoves[depth][i][1] == curMove[1] &&
				killerMoves[depth][i][2] == curMove[2] &&
				killerMoves[depth][i][3] == curMove[3]){
				//cout << "Found killer move at depth " << depth << endl;
				//swap killer move closer to the front
				swapTemp[0] = moves[curSwapPosition][0];
				swapTemp[1] = moves[curSwapPosition][1];
				swapTemp[2] = moves[curSwapPosition][2];
				swapTemp[3] = moves[curSwapPosition][3];

				moves[curSwapPosition][0] = curMove[0];
				moves[curSwapPosition][1] = curMove[1];
				moves[curSwapPosition][2] = curMove[2];
				moves[curSwapPosition][3] = curMove[3];

				moves[j][0] = swapTemp[0];
				moves[j][1] = swapTemp[1];
				moves[j][2] = swapTemp[2];
				moves[j][3] = swapTemp[3];
				curSwapPosition++;
				break;
			}
		}
	}
}
void setup(){
	//[6, 0] [6, 1] [6, 2] [6, 3] [6, 4] [6, 5] [6, 6] [6, 7]
	//[5, 0] [5, 1] [5, 2] [5, 3] [5, 4] [5, 5] [5, 6] [5, 7]
	//[4, 0] [4, 1] [4, 2] [4, 3] [4, 4] [4, 5] [4, 6] [4, 7]
	//[3, 0] [3, 1] [3, 2] [3, 3] [3, 4] [3, 5] [3, 6] [3, 7]
	//[2, 0] [2, 1] [2, 2] [2, 3] [2, 4] [2, 5] [2, 6] [2, 7]
	//[1, 0] [1, 1] [1, 2] [1, 3] [1, 4] [1, 5] [1, 6] [1, 7]
	//[0, 0] [0, 1] [0, 2] [0, 3] [0, 4] [0, 5] [0, 6] [0, 7]

	b[5][3][0] = KNIGHT_MAX;
	b[5][3][1] = GAS_MAX;

	b[6][2][0] = BISHOP_MAX;
	b[6][2][1] = GAS_MAX;

	b[6][3][0] = QUEEN_MAX;
	b[6][3][1] = GAS_MAX;

	b[6][4][0] = KING_MAX;
	b[6][4][1] = GAS_MAX;

	b[6][5][0] = BISHOP_MAX;
	b[6][5][1] = GAS_MAX;

	b[5][5][0] = KNIGHT_MAX;
	b[5][5][1] = GAS_MAX;
	//min values
	b[1][3][0] = KNIGHT_MIN;
	b[1][3][1] = GAS_MAX;

	b[0][2][0] = BISHOP_MIN;
	b[0][2][1] = GAS_MAX;

	b[0][3][0] = QUEEN_MIN;
	b[0][3][1] = GAS_MAX;

	b[0][4][0] = KING_MIN;
	b[0][4][1] = GAS_MAX;

	b[0][5][0] = BISHOP_MIN;
	b[0][5][1] = GAS_MAX;

	b[1][5][0] = KNIGHT_MIN;
	b[1][5][1] = GAS_MAX;
}

//ends the game. true if I won, false if player won
void gameOver(bool maxPlayerOne){
	printboard();
	if (maxPlayerOne){
		cout << "I won!" << endl;
	}
	else {
		cout << "You won!" << endl;
	}
	char anyChar;
	cout << "Press any key + enter to end";
	cin >> anyChar;
	exit(0);
}
//gets, validates and makes the players move
void getPlayerMove(){
	
	int moves[100][4];
	int count = getPossibleMovesMin(moves);
	if (count == 0){
		//he has no moves, I win
		gameOver(true);
	}
	else {
		cout << "Please Input Move: ";

		int from_row, to_row;
		char from_col, to_col;
		cin >> from_col >> from_row >> to_col >> to_row;

		int from_row_act = from_row - 1;
		int to_row_act = to_row - 1;
		int from_col_act = from_col - 'A';
		int to_col_act = to_col - 'A';

		while (!isMovePossibleMin(moves, count, from_row_act, from_col_act, to_row_act, to_col_act)){
			cout << "Move not legal, please enter another move: ";
			cin >> from_col >> from_row >> to_col >> to_row;

			from_row_act = from_row - 1;
			to_row_act = to_row - 1;
			from_col_act = from_col - 'A';
			to_col_act = to_col - 'A';

		}

		int m[4] = { from_row_act, from_col_act, to_row_act, to_col_act };
		int pieceTaken[3];
		makeMove(m, pieceTaken);

		if (pieceTaken[0] == KING_MAX){
			//he took my king, he wins
			gameOver(false);
		}
	}
}
//makes the move on the board
void makeMove(int move[4], int pieceTaken[3]){
	int pVal = b[move[0]][move[1]][0];
	int pFuel = b[move[0]][move[1]][1];
	pieceTaken[0] = b[move[2]][move[3]][0];
	pieceTaken[1] = b[move[2]][move[3]][1];
	b[move[0]][move[1]][0] = 0;
	pieceTaken[2] = pFuel;
	if (pieceTaken[0] == 0){
		pFuel--;
	}
	else {
		//aka an actual piece
		pFuel = GAS_MAX;
	}
	b[move[2]][move[3]][0] = pVal;
	b[move[2]][move[3]][1] = pFuel;
}
void retractMove(int move[4], int pieceTaken[3]){
	
	//pieceTaken[0] = taken's value
	//pieceTaken[1] = taken's fuel
	//pieceTaken[2] = mover's old fuel;
	//move[0] == from_row
	//move[1] == from_col
	//move[2] == to_row
	//move[3] == to_col
	int pVal = b[move[2]][move[3]][0];
	int pFuel = pieceTaken[2];

	b[move[0]][move[1]][0] = pVal;
	b[move[0]][move[1]][1] = pFuel;
	b[move[2]][move[3]][0] = pieceTaken[0];
	b[move[2]][move[3]][1] = pieceTaken[1];
}
int getPossibleMovesMax(int re[100][4]){
	//[6, 0] [6, 1] [6, 2] [6, 3] [6, 4] [6, 5] [6, 6] [6, 7]
	//[5, 0] [5, 1] [5, 2] [5, 3] [5, 4] [5, 5] [5, 6] [5, 7]
	//[4, 0] [4, 1] [4, 2] [4, 3] [4, 4] [4, 5] [4, 6] [4, 7]
	//[3, 0] [3, 1] [3, 2] [3, 3] [3, 4] [3, 5] [3, 6] [3, 7]
	//[2, 0] [2, 1] [2, 2] [2, 3] [2, 4] [2, 5] [2, 6] [2, 7]
	//[1, 0] [1, 1] [1, 2] [1, 3] [1, 4] [1, 5] [1, 6] [1, 7]
	//[0, 0] [0, 1] [0, 2] [0, 3] [0, 4] [0, 5] [0, 6] [0, 7]

	int count = 0;
	for (int i = 0; i < BOARD_ROWS; i++){
		for (int j = 0; j < BOARD_COLS; j++){
			if (b[i][j][0] > 0 && b[i][j][1] > 0){
				//aka is a max piece and it can move

				switch (b[i][j][0]){
				case BISHOP_MAX:
					moveBishop(re, count, i, j, KING_MAX);
					break;
				case KNIGHT_MAX:
					moveKnight(re, count, i, j, KING_MAX);
					break;
				case QUEEN_MAX:
					moveKnight(re, count, i, j, KING_MAX);
					moveBishop(re, count, i, j, KING_MAX);
					break;
				case KING_MAX:
					moveKing(re, count, i, j);
					break;
				}
			}
		}
	}
	return count;
}
int getPossibleMovesMin(int re[100][4]){
	//min player turn
	int count = 0;
	for (int i = 0; i < BOARD_ROWS; i++){
		for (int j = 0; j < BOARD_COLS; j++){
			if (b[i][j][0] < 0 && b[i][j][1] > 0){
				//aka is a min piece
				switch (b[i][j][0]){
				case BISHOP_MIN:
					moveBishop(re, count, i, j, KING_MIN);
					break;
				case KNIGHT_MIN:
					moveKnight(re, count, i, j, KING_MIN);
					break;
				case QUEEN_MIN:
					moveKnight(re, count, i, j, KING_MIN);
					moveBishop(re, count, i, j, KING_MIN);
					break;
				case KING_MIN:
					moveKing(re, count, i, j);
					break;
				}
			}
		}
	}
	return count;
}
//given possible moves for the player, sees if the move they want to do is valid
bool isMovePossibleMin(int re[100][4], int count, int from_row, int from_col, int to_row, int to_col){
	return  foundWholeMove(re, count, to_row, to_col, from_row, from_col);
}
void moveKing(int re[100][4], int &count, int i, int j){
	//(+1, +1), (+1, 0), (+1, -1), (0, -1), (0, +1), (-1, 0), (-1, +1), (-1, -1) 

	int to_row = i + 1, to_col = j + 1; //(+1, +1)
	if (moveInBounds(to_row, to_col)){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
	to_col = j; //to_row stays i+1 (+1, 0)
	if (moveInBounds(to_row, to_col)){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
	to_col = j - 1;//to_row stays i+1  (+1, -1)
	if (moveInBounds(to_row, to_col)){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
	to_row = i; //to_col stays i-1  (0, -1)
	if (moveInBounds(to_row, to_col)){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
	to_col = j + 1; //(0, +1)
	if (moveInBounds(to_row, to_col)){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
	to_row = i - 1; //(-1, +1)
	if (moveInBounds(to_row, to_col)){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
	to_col = j; //(-1, 0)
	if (moveInBounds(to_row, to_col)){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
	to_col = j - 1; // (-1, -1) 
	if (moveInBounds(to_row, to_col)){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
}
void moveKnight(int re[100][4], int &count, int i, int j, int kingVal){
	//(+2, +1), (+2, -1), (-2, +1), (-2, -1), (+1, +2), (+1, -2), (-1, +2), (-1, -2) 
	//can't kill own king
	int to_row = i + 2, to_col = j + 1; //(+2, +1)
	if (moveInBounds(to_row, to_col) && b[to_row][to_col][0] != kingVal){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
	to_col = j - 1; //(+2, -1)
	if (moveInBounds(to_row, to_col) && b[to_row][to_col][0] != kingVal){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
	to_row = i - 2;//(-2, -1)
	if (moveInBounds(to_row, to_col) && b[to_row][to_col][0] != kingVal){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
	to_col = j + 1;//(-2, +1)
	if (moveInBounds(to_row, to_col) && b[to_row][to_col][0] != kingVal){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
	to_row = i + 1, to_col = j + 2; //(+1, +2)
	if (moveInBounds(to_row, to_col) && b[to_row][to_col][0] != kingVal){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
	to_col = j - 2; //(+1, -2)
	if (moveInBounds(to_row, to_col) && b[to_row][to_col][0] != kingVal){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
	to_row = i - 1;//(-1, -2)
	if (moveInBounds(to_row, to_col) && b[to_row][to_col][0] != kingVal){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
	to_col = j + 2;
	if (moveInBounds(to_row, to_col) && b[to_row][to_col][0] != kingVal){
		re[count][0] = i;
		re[count][1] = j;
		re[count][2] = to_row;
		re[count][3] = to_col;
		count++;
	}
}
void moveBishop(int re[100][4], int &count, int i, int j, int kingVal){
	//goes 4 directions. goes until it either hits the end or captures a piece
	//(-1, -1), (-1, +1), (+1, +1), (+1, -1)


	int to_row = i - 1, to_col = j - 1;//(-1, -1)
	while (moveInBounds(to_row, to_col)){
		int val = b[to_row][to_col][0];
		if (val != kingVal){
			re[count][0] = i;
			re[count][1] = j;
			re[count][2] = to_row;
			re[count][3] = to_col;
			count++;
		}
		if (val != 0){
			break;
		}
		else {
			to_row--;
			to_col--;
		}
	}
	to_row = i - 1, to_col = j + 1;//(-1, +1)
	while (moveInBounds(to_row, to_col)){
		int val = b[to_row][to_col][0];
		if (val != kingVal){
			re[count][0] = i;
			re[count][1] = j;
			re[count][2] = to_row;
			re[count][3] = to_col;
			count++;
		}
		if (val != 0){
			break;
		}
		else {
			to_row--;
			to_col++;
		}
	}

	 to_row = i + 1, to_col = j + 1;//(+1, +1)
	while (moveInBounds(to_row, to_col)){
		int val = b[to_row][to_col][0];
		if (val != kingVal){
			re[count][0] = i;
			re[count][1] = j;
			re[count][2] = to_row;
			re[count][3] = to_col;
			count++;
		}
		if (val != 0){
			break;
		}
		else {
			to_row++;
			to_col++;
		}
	}
	 to_row = i + 1, to_col = j - 1;//(+1, -1)
	while (moveInBounds(to_row, to_col)){
		int val = b[to_row][to_col][0];
		if (val != kingVal){
			re[count][0] = i;
			re[count][1] = j;
			re[count][2] = to_row;
			re[count][3] = to_col;
			count++;
		}
		if (val != 0){
			break;
		}
		else {
			to_row++;
			to_col--;
		}
	}
}
bool moveInBounds(int to_row, int to_col){
	return to_col >= 0 && to_col < BOARD_COLS && to_row >= 0 && to_row < BOARD_ROWS;
}
void printboard(){
	//example
	//  7  -- --B3 Q3 K3 B3 N3 --  computer
	//	6  -- -- -- -- -- -- -- --
	//	5  -- --N2-- -- -- -- --
	//	4  -- -- -- -- -- -- -- --
	//	3  b2-- -- -- -- -- -- --
	//	2  -- -- -- -- -- -- -- --
	//	1  --n3 b3 q3 k3 b3 n3 --  human
	//	---------------------- -
	//	A  B  C  D  E  F  G  H
	cout << endl;
	for (int i = BOARD_ROWS -1; i >= 0; i--){
		cout << (i+1) << "  ";
		for (int j = 0; j < BOARD_COLS; j++){
			if (b[i][j][0] == 0){
				//empty square
				cout << '-' << '-' << ' ';
			}
			else {
				cout << printValues[b[i][j][0] + 4] << b[i][j][1] << ' ';
			}
		}
		cout << endl;
	}		
	cout << "--------------------------" << endl;
	cout << "   A  B  C  D  E  F  G  H" << endl;
}
bool foundWholeMove(int moves[100][4], int count, int to_row, int to_col, int from_row, int from_col){
	for (int i = 0; i < count; i++){
		if (moves[i][0] == from_row && moves[i][1] == from_col && moves[i][2] == to_row && moves[i][3] == to_col){
			return true;
		}
	}
	return false;

}
