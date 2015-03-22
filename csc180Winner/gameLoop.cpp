#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <ctime>
using namespace std;


struct Piece {
	//bool maxPiece;
	
	int value;
	//char printValue;
	int fuel;
	Piece(){
		value = 0;
		fuel = 3;
		//printValue = '-';
	}
	Piece(int init_value){
		value = init_value;
		//printValue = init_printValue;
		fuel = 3;
	}
};
struct Move {
	int from_row;
	int from_col;
	int to_row;
	int to_col;

	//int from_fuel;
	//Piece piece_taken;
	Move(){

	}
	Move(int from_row_i, int from_col_i, int to_row_i, int to_col_i){
		from_row = from_row_i;
		from_col = from_col_i;
		to_row = to_row_i;
		to_col = to_col_i;
	}
};
void computeMinimax();
int min(int depth, int maxFoundSoFar);
int max(int depth, int minFoundSoFar);
int evaluate();
void getPlayerMove();
void setup();
void printboard();
void makeMove(Move &m, int pieceTaken[]);
void retractMove(Move &m, int pieceTaken[]);
void getPossibleMovesMin(vector<Move> &re);
void getPossibleMovesMax(vector<Move> &re);
bool moveInBounds(int to_col, int to_row);
void moveKnight(vector<Move> &re, int i, int j, int kingVal);
void moveKing(vector<Move> &re, int i, int j);
void moveBishop(vector<Move> &re, int i, int j, int kingVal);
void testKnight();
void testMove(int from_row, int from_col, int value, int valuesCount, pair<int, int> values[]);
void clearPosition(int row, int col);
void testKing();
void testBishop();
void testQueen();
vector<Move>::iterator foundMove(vector<Move> &moves, int to_row, int to_col);
vector<Move>::iterator foundWholeMove(vector<Move> &moves, int to_row, int to_col, int from_row, int from_col);
void testPostSetUp();
void testMove(int valuesCount, pair<pair<int, int>, pair<int, int>> values[]);
bool isMovePossibleMin(vector<Move> &moves, int from_row, int from_col, int to_row, int to_col);
void gameOver(bool maxPlayerOne);
bool isPlayerGoingFirst();
char getPrintValue(int value);
#define RESET 0
#define BRIGHT 1
#define DIM 2
#define UNDERLINE 3
#define BLINK 4
#define REVERSE 7
#define HIDDEN 8
#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define WHITE 7



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
//minimax stuff
const int BEST_MAX = 200000;
const int BEST_MIN = -200000;
const int WIN_MAX = 100000;
const int WIN_MIN = -100000;

const bool test = false;
//so index is [row, col] 
//[6, 0] [6, 1] [6, 2] [6, 3] [6, 4] [6, 5] [6, 6] [6, 7]
//[5, 0] [5, 1] [5, 2] [5, 3] [5, 4] [5, 5] [5, 6] [5, 7]
//[4, 0] [4, 1] [4, 2] [4, 3] [4, 4] [4, 5] [4, 6] [4, 7]
//[3, 0] [3, 1] [3, 2] [3, 3] [3, 4] [3, 5] [3, 6] [3, 7]
//[2, 0] [2, 1] [2, 2] [2, 3] [2, 4] [2, 5] [2, 6] [2, 7]
//[1, 0] [1, 1] [1, 2] [1, 3] [1, 4] [1, 5] [1, 6] [1, 7]
//[0, 0] [0, 1] [0, 2] [0, 3] [0, 4] [0, 5] [0, 6] [0, 7]
//Piece b[BOARD_ROWS][BOARD_COLS] = { {}, {}, {}, {}, {}, {}, {}, {} };
Piece b[BOARD_ROWS][BOARD_COLS];
const int maxDepth = 5;
int evalCount = 0;
clock_t start;
double duration;
int main(){
	if (test){
		testKnight();
		testKing();
		testBishop();
		testQueen();
		testPostSetUp();
		//getPlayerMove(); //to pause the screen
		gameOver(true);
		
	}
	else {
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
	
}
int evaluate(){//stub for now
	//evalCount++;
	//cout << "Evaluate has been called " << evalCount <<" times" << endl;
	int eval = 0;
	int curVal;
	//int curFuel;
	for (int i = BOARD_ROWS - 1; i >= 0; i--){
		for (int j = 0; j < BOARD_COLS; j++){
			curVal = b[i][j].value;
			if (curVal != 0){
				//an actual piece
				eval += curVal * b[i][j].fuel;
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
	int bestScore = BEST_MIN;// depth = 1;
	evalCount = 0;
	Move bestMove;

	vector<Move> moves;
	moves.reserve(38);
	getPossibleMovesMax(moves);
	if (moves.size() == 0){
		gameOver(false);
	}
	else {
		int curScore;
		int pieceTaken[3];
		for (Move m : moves){
			makeMove(m, pieceTaken);
			if (pieceTaken[0] == KING_MIN){
				//taken their king!
				curScore = WIN_MAX;
			}
			else {
				curScore = min(2, bestScore);

			}
			if (curScore > bestScore){
				bestScore = curScore;
				bestMove = m;
			}
			retractMove(m, pieceTaken);
		}
		//make the best move
		makeMove(bestMove, pieceTaken);

		//print out move in notation of screen coordinites
		char from_col_act = bestMove.from_col+ 'A';
		int from_row_act = bestMove.from_row + 1;
		char to_col_act = bestMove.to_col + 'A';
		int to_row_act = bestMove.to_row + 1;
		
		cout << "I moved from " << from_col_act  << from_row_act << " to " << to_col_act << to_row_act << "(" << to_col_act << (8 - to_row_act)<<")"<<endl;

		duration = (clock() - start) / (double)CLOCKS_PER_SEC;
		cout << "It took " << duration << " to make this decision" << endl;
		//we captured their king!
		if (pieceTaken[0] == KING_MIN){
			gameOver(true);
		}

	}

}
int min(int depth, int maxFoundSoFar){
	
	vector<Move> moves;
	moves.reserve(20);
	getPossibleMovesMin(moves);
	if (moves.size() == 0){
		//human can't move, so I win
		return WIN_MAX - depth;
	}
	if (depth == maxDepth){ return evaluate();}
	int bestScore = BEST_MAX;

	
	int curScore;
	int pieceTaken[3];
	for (Move m : moves){
		makeMove(m, pieceTaken);
		if (pieceTaken[0] == KING_MAX){
			//taken my king!
			curScore = WIN_MIN + depth;
		}
		else {
			curScore = max(depth + 1, bestScore);
		}

		retractMove(m, pieceTaken);		
		if (curScore < bestScore){
			bestScore = curScore;
			if (bestScore <= maxFoundSoFar){
				//alpha beta prune
				return bestScore;
			}
		}
	}
	return bestScore;
}
int max(int depth, int minFoundSoFar){
	
	vector<Move> moves;
	moves.reserve(20);
	getPossibleMovesMax(moves);
	if (moves.size() == 0){
		//I can't move, so human wins
		return WIN_MIN + depth;
	}
	if (depth == maxDepth){ return evaluate(); }
	int bestScore = BEST_MIN;

	
	int curScore;
	int pieceTaken[3];
	for (Move m : moves){
		makeMove(m, pieceTaken);

		if (pieceTaken[0] == KING_MIN){
			//taken their king!
			curScore = WIN_MAX - depth;
		}
		else {
			curScore = min(depth + 1, bestScore);
		}

		retractMove(m, pieceTaken);		
		if (curScore > bestScore){
			bestScore = curScore;
			if (bestScore >= minFoundSoFar){
				//alpha beta prune, it can only be bigger
				return bestScore;
			}
		}
	}
	return bestScore;
}
void setup(){
	//[6, 0] [6, 1] [6, 2] [6, 3] [6, 4] [6, 5] [6, 6] [6, 7]
	//[5, 0] [5, 1] [5, 2] [5, 3] [5, 4] [5, 5] [5, 6] [5, 7]
	//[4, 0] [4, 1] [4, 2] [4, 3] [4, 4] [4, 5] [4, 6] [4, 7]
	//[3, 0] [3, 1] [3, 2] [3, 3] [3, 4] [3, 5] [3, 6] [3, 7]
	//[2, 0] [2, 1] [2, 2] [2, 3] [2, 4] [2, 5] [2, 6] [2, 7]
	//[1, 0] [1, 1] [1, 2] [1, 3] [1, 4] [1, 5] [1, 6] [1, 7]
	//[0, 0] [0, 1] [0, 2] [0, 3] [0, 4] [0, 5] [0, 6] [0, 7]


	b[5][3].value = KNIGHT_MAX;

	b[6][2].value = BISHOP_MAX;

	b[6][3].value = QUEEN_MAX;

	b[6][4].value = KING_MAX;

	b[6][5].value = BISHOP_MAX;

	b[5][5].value = KNIGHT_MAX;

	//min values
	b[1][3].value = KNIGHT_MIN;

	b[0][2].value = BISHOP_MIN;

	b[0][3].value = QUEEN_MIN;

	b[0][4].value = KING_MIN;

	b[0][5].value = BISHOP_MIN;

	b[1][5].value = KNIGHT_MIN;
}

//ends the game. true if I won, false if player won
void gameOver(bool maxPlayerOne){
	printboard();
	if (maxPlayerOne){
		cout << "I won" << endl;
	}
	else {
		cout << "You won" << endl;
	}
	char anyChar;
	cout << "Press any key to end";
	cin >> anyChar;
	exit(0);
}
//gets, validates and makes the players move
void getPlayerMove(){
	
	vector<Move> moves;
	getPossibleMovesMin(moves);
	if (moves.size() == 0){
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

		cout << "You put in " << from_col_act << ' ' << from_row_act << " to " << to_col_act << ' ' << to_row_act << endl;
		while (!isMovePossibleMin(moves, from_row_act, from_col_act, to_row_act, to_col_act)){
			cout << "Move not legal, please enter another move: ";
			cin >> from_col >> from_row >> to_col >> to_row;

			from_row_act = from_row - 1;
			to_row_act = to_row - 1;
			from_col_act = from_col - 'A';
			to_col_act = to_col - 'A';

			cout << "You put in " << from_col_act << ' ' << from_row_act << " to " << to_col_act << ' ' << to_row_act << endl;
		}

		Move m = Move(from_row_act, from_col_act, to_row_act, to_col_act);
		int pieceTaken[3];
		makeMove(m, pieceTaken);

		if (pieceTaken[0] == KING_MAX){
			//he took my king, he wins
			gameOver(false);
		}
	}
}
//makes the move on the board
void makeMove(Move &m, int pieceTaken[] ){
	//stub for now
	Piece p = b[m.from_row][m.from_col];
	//m.piece_taken = b[m.to_row][m.to_col];
	pieceTaken[0] = b[m.to_row][m.to_col].value;
	pieceTaken[1] = b[m.to_row][m.to_col].fuel;
	b[m.from_row][m.from_col].value = 0;
	//b[m.from_row][m.from_col].printValue = '-';
	pieceTaken[2] = p.fuel;
	if (pieceTaken[0] != 0){
		//aka an actual piece
		p.fuel = 3;
	}
	else {
		p.fuel--;
	}
	b[m.to_row][m.to_col] = p;
}
void retractMove(Move &m, int pieceTaken[]){
	//stub for now

	Piece p = b[m.to_row][m.to_col];
	p.fuel = pieceTaken[2];
	b[m.from_row][m.from_col] = p;
	b[m.to_row][m.to_col].value = pieceTaken[0];
	b[m.to_row][m.to_col].fuel = pieceTaken[1];
}
void getPossibleMovesMax(vector<Move> &re){
	//[6, 0] [6, 1] [6, 2] [6, 3] [6, 4] [6, 5] [6, 6] [6, 7]
	//[5, 0] [5, 1] [5, 2] [5, 3] [5, 4] [5, 5] [5, 6] [5, 7]
	//[4, 0] [4, 1] [4, 2] [4, 3] [4, 4] [4, 5] [4, 6] [4, 7]
	//[3, 0] [3, 1] [3, 2] [3, 3] [3, 4] [3, 5] [3, 6] [3, 7]
	//[2, 0] [2, 1] [2, 2] [2, 3] [2, 4] [2, 5] [2, 6] [2, 7]
	//[1, 0] [1, 1] [1, 2] [1, 3] [1, 4] [1, 5] [1, 6] [1, 7]
	//[0, 0] [0, 1] [0, 2] [0, 3] [0, 4] [0, 5] [0, 6] [0, 7]
	//vector<Move> re;
	for (int i = 0; i < BOARD_ROWS; i++){
		for (int j = 0; j < BOARD_COLS; j++){
			if (b[i][j].value > 0 && b[i][j].fuel > 0){
				//aka is a max piece and it can move
				int value = b[i][j].value;

				if (value == KING_MAX){
					moveKing(re, i, j);
				}
				else if (value == QUEEN_MAX){
					moveKnight(re, i, j, KING_MAX);
					moveBishop(re, i, j, KING_MAX);
				}
				else if (value == KNIGHT_MAX){
					moveKnight(re, i, j, KING_MAX);
				}
				else if (value == BISHOP_MAX){
					moveBishop(re, i, j, KING_MAX);
				}
				else {
					//this shouldn't happen
					cout << "THIS SHOULD NOT HAPPEN in max";
				}
			}

		}
	}
}
void getPossibleMovesMin(vector<Move> &re){
		//min player turn
		for (int i = 0; i < BOARD_ROWS; i++){
			for (int j = 0; j < BOARD_COLS; j++){
				if (b[i][j].value < 0 && b[i][j].fuel > 0){
					//aka is a min piece
					//aka is a max piece and it can move
					int value = b[i][j].value;

					if (value == KING_MIN){
						moveKing(re, i, j);
					}
					else if (value == QUEEN_MIN){
						moveKnight(re, i, j, KING_MIN);
						moveBishop(re, i, j, KING_MIN);
					}
					else if (value == KNIGHT_MIN){
						moveKnight(re, i, j, KING_MIN);
					}
					else if (value == BISHOP_MIN){
						moveBishop(re, i, j, KING_MIN);
					}
					else {
						//this shouldn't happen
						cout << "THIS SHOULD NOT HAPPEN in min";
					}
				}

			}
		}
	
	
	//return re;
}
//given possible moves for the player, sees if the move they want to do is valid
bool isMovePossibleMin(vector<Move> &moves, int from_row, int from_col, int to_row, int to_col){
	vector<Move>::iterator itr = foundWholeMove(moves, to_row, to_col, from_row, from_col);
	bool found = (itr != moves.end());
	return found;
}
void moveKing(vector<Move> &re, int i, int j){
	//(+1, +1), (+1, 0), (+1, -1), (0, -1), (0, +1), (-1, 0), (-1, +1), (-1, -1) 

	int to_row = i + 1, to_col = j + 1; //(+1, +1)
	if (moveInBounds(to_row, to_col)){
		re.push_back(Move(i, j, to_row, to_col));
	}
	to_col = j; //to_row stays i+1 (+1, 0)
	if (moveInBounds(to_row, to_col)){
		re.push_back(Move(i, j, to_row, to_col));
	}
	to_col = j - 1;//to_row stays i+1  (+1, -1)
	if (moveInBounds(to_row, to_col)){
		re.push_back(Move(i, j, to_row, to_col));
	}
	to_row = i; //to_col stays i-1  (0, -1)
	if (moveInBounds(to_row, to_col)){
		re.push_back(Move(i, j, to_row, to_col));
	}
	to_col = j + 1; //(0, +1)
	if (moveInBounds(to_row, to_col)){
		re.push_back(Move(i, j, to_row, to_col));
	}
	to_row = i - 1; //(-1, +1)
	if (moveInBounds(to_row, to_col)){
		re.push_back(Move(i, j, to_row, to_col));
	}
	to_col = j; //(-1, 0)
	if (moveInBounds(to_row, to_col)){
		re.push_back(Move(i, j, to_row, to_col));
	}
	to_col = j - 1; // (-1, -1) 
	if (moveInBounds(to_row, to_col)){
		re.push_back(Move(i, j, to_row, to_col));
	}
}
void moveKnight(vector<Move> &re, int i, int j, int kingVal){
	//(+2, +1), (+2, -1), (-2, +1), (-2, -1), (+1, +2), (+1, -2), (-1, +2), (-1, -2) 
	//can't kill own king
	int to_row = i + 2, to_col = j + 1; //(+2, +1)
	if (moveInBounds(to_row, to_col) && b[to_row][to_col].value != kingVal){
		re.push_back(Move(i, j, to_row, to_col));
	}
	to_col = j - 1; //(+2, -1)
	if (moveInBounds(to_row, to_col) && b[to_row][to_col].value != kingVal){
		re.push_back(Move(i, j, to_row, to_col));
	}
	to_row = i - 2;//(-2, -1)
	if (moveInBounds(to_row, to_col) && b[to_row][to_col].value != kingVal){
		re.push_back(Move(i, j, to_row, to_col));
	}
	to_col = j + 1;//(-2, +1)
	if (moveInBounds(to_row, to_col) && b[to_row][to_col].value != kingVal){
		re.push_back(Move(i, j, to_row, to_col));
	}
	to_row = i + 1, to_col = j + 2; //(+1, +2)
	if (moveInBounds(to_row, to_col) && b[to_row][to_col].value != kingVal){
		re.push_back(Move(i, j, to_row, to_col));
	}
	to_col = j - 2; //(+1, -2)
	if (moveInBounds(to_row, to_col) && b[to_row][to_col].value != kingVal){
		re.push_back(Move(i, j, to_row, to_col));
	}
	to_row = i - 1;//(-1, -2)
	if (moveInBounds(to_row, to_col) && b[to_row][to_col].value != kingVal){
		re.push_back(Move(i, j, to_row, to_col));
	}
	to_col = j + 2;
	if (moveInBounds(to_row, to_col) && b[to_row][to_col].value != kingVal){
		re.push_back(Move(i, j, to_row, to_col));
	}
}
void moveBishop(vector<Move> &re, int i, int j, int kingVal){
	//goes 4 directions. goes until it either hits the end or captures a piece
	//(-1, -1), (-1, +1), (+1, +1), (+1, -1)


	int to_row = i - 1, to_col = j - 1;//(-1, -1)
	while (moveInBounds(to_row, to_col)){
		int val = b[to_row][to_col].value;
		if (val != kingVal){
			re.push_back(Move(i, j, to_row, to_col));
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
		int val = b[to_row][to_col].value;
		if (val != kingVal){
			re.push_back(Move(i, j, to_row, to_col));
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
		int val = b[to_row][to_col].value;
		if (val != kingVal){
			re.push_back(Move(i, j, to_row, to_col));
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
		int val = b[to_row][to_col].value;
		if (val != kingVal){
			re.push_back(Move(i, j, to_row, to_col));
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
			if (b[i][j].value == 0){
				//empty square
				cout << '-' << '-' << ' ';
			}
			else {
				cout << printValues[b[i][j].value + 4] << b[i][j].fuel << ' ';
			}
			
		}

		cout << endl;
	}		
	cout << "--------------------------" << endl;
	cout << "   A  B  C  D  E  F  G  H" << endl;
}
void textcolor(int attr, int fg, int bg){
	char command[13];
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
}










/********************************************Testing code******************************************************/
struct find_move : std::unary_function<Move, bool> {
	int to_row, to_col;
	find_move(int to_row, int to_col) :to_row(to_row), to_col(to_col) { }
	bool operator()(Move const& m) const {
		return m.to_row == to_row && m.to_col == to_col;
	}
};
struct find_whole_move : std::unary_function<Move, bool> {
	int to_row, to_col, from_row, from_col;
	find_whole_move(int to_row, int to_col, int from_row, int from_col) :to_row(to_row), to_col(to_col), from_row(from_row), from_col(from_col) { }
	bool operator()(Move const& m) const {
		return m.to_row == to_row && m.to_col == to_col && m.from_row == from_row && m.from_col == from_col;
	}
};
vector<Move>::iterator foundMove(vector<Move> &moves, int to_row, int to_col){
	vector<Move>::iterator it = std::find_if(moves.begin(), moves.end(), find_move(to_row, to_col));
	return it;
}
vector<Move>::iterator foundWholeMove(vector<Move> &moves, int to_row, int to_col, int from_row, int from_col){
	vector<Move>::iterator it = std::find_if(moves.begin(), moves.end(), find_whole_move(to_row, to_col, from_row, from_col));
	return it;
}
void testKnight(){
	//[6, 0] [6, 1] [6, 2] [6, 3] [6, 4] [6, 5] [6, 6] [6, 7]
	//[5, 0] [5, 1] [5, 2] [5, 3] [5, 4] [5, 5] [5, 6] [5, 7]
	//[4, 0] [4, 1] [4, 2] [4, 3] [4, 4] [4, 5] [4, 6] [4, 7]
	//[3, 0] [3, 1] [3, 2] [3, 3] [3, 4] [3, 5] [3, 6] [3, 7]
	//[2, 0] [2, 1] [2, 2] [2, 3] [2, 4] [2, 5] [2, 6] [2, 7]
	//[1, 0] [1, 1] [1, 2] [1, 3] [1, 4] [1, 5] [1, 6] [1, 7]
	//[0, 0] [0, 1] [0, 2] [0, 3] [0, 4] [0, 5] [0, 6] [0, 7]


	b[1][1].value = KING_MAX;
	b[1][1].fuel = 0;

	b[1][3].value = KING_MIN;
	b[1][3].fuel = 1;
	pair<int, int> values[7] = { { 5, 3 }, { 5, 1 }, { 2, 0 }, { 4, 0 }, { 4, 4 }, { 2, 4 }, {1, 3} };
	testMove(3, 2, KNIGHT_MAX, 7, values);

	//opening moves for knight 1 and test out of bounds
	pair<int, int> values2[3] = { { 4, 0 }, { 4, 2 }, {5, 3} }; 
	testMove(6, 1, KNIGHT_MAX, 3, values2);

	//opening moves for knight 2
	pair<int, int> values3[3] = { { 4, 7 }, { 4, 5 }, { 5, 4 } };
	testMove(6, 6, KNIGHT_MAX, 3, values3);

	clearPosition(1, 1);
	clearPosition(1, 3);


}
void testKing(){
	pair<int, int> values[8] = { { 4, 3 }, { 2, 3 }, { 3, 2 }, { 2, 2 }, { 4, 2 }, { 4, 4 }, { 3, 4 }, { 2, 4 } }; 
	testMove(3, 3, KING_MAX, 8, values);

	//init position
	pair<int, int> values2[5] = { { 6, 3 }, { 5, 3 }, { 5, 4 }, { 5, 5 }, { 6, 5 } };
	testMove(6, 4, KING_MAX, 5, values2);

}
void testBishop(){
	//[6, 0] [6, 1] [6, 2] [6, 3] [6, 4] [6, 5] [6, 6] [6, 7]
	//[5, 0] [5, 1] [5, 2] [5, 3] [5, 4] [5, 5] [5, 6] [5, 7]
	//[4, 0] [4, 1] [4, 2] [4, 3] [4, 4] [4, 5] [4, 6] [4, 7]
	//[3, 0] [3, 1] [3, 2] [3, 3] [3, 4] [3, 5] [3, 6] [3, 7]
	//[2, 0] [2, 1] [2, 2] [2, 3] [2, 4] [2, 5] [2, 6] [2, 7]
	//[1, 0] [1, 1] [1, 2] [1, 3] [1, 4] [1, 5] [1, 6] [1, 7]
	//[0, 0] [0, 1] [0, 2] [0, 3] [0, 4] [0, 5] [0, 6] [0, 7]

	b[4][2].value = KING_MAX;
	b[4][2].fuel = 0;

	b[5][5].value = KING_MIN;
	b[5][5].fuel = 1;
	pair<int, int> values[8] = { { 2, 2 }, { 1, 1 }, { 0, 0 }, { 2, 4 }, { 1, 5 }, { 0, 6 }, { 4, 4 }, {5, 5} };
	testMove(3, 3, BISHOP_MAX, 8, values);

	//init position 1
	pair<int, int> values2[7] = { { 5, 1 }, { 4, 0 }, { 5, 3 }, { 4, 4 }, { 3, 5 }, { 2, 6 }, {1,7} };
	testMove(6, 2, BISHOP_MAX, 7, values2);

	//init position 2
	pair<int, int> values3[7] = { { 5, 6 }, { 4, 7 }, { 5, 4 }, { 4, 3 }, { 3, 2 }, { 2, 1 }, {1, 0} };
	testMove(6, 5, BISHOP_MAX, 7, values3);

	clearPosition(4, 2);
	clearPosition(5, 5);
}
void testQueen(){
	//[6, 0] [6, 1] [6, 2] [6, 3] [6, 4] [6, 5] [6, 6] [6, 7]
	//[5, 0] [5, 1] [5, 2] [5, 3] [5, 4] [5, 5] [5, 6] [5, 7]
	//[4, 0] [4, 1] [4, 2] [4, 3] [4, 4] [4, 5] [4, 6] [4, 7]
	//[3, 0] [3, 1] [3, 2] [3, 3] [3, 4] [3, 5] [3, 6] [3, 7]
	//[2, 0] [2, 1] [2, 2] [2, 3] [2, 4] [2, 5] [2, 6] [2, 7]
	//[1, 0] [1, 1] [1, 2] [1, 3] [1, 4] [1, 5] [1, 6] [1, 7]
	//[0, 0] [0, 1] [0, 2] [0, 3] [0, 4] [0, 5] [0, 6] [0, 7]
	b[4][2].value = KING_MAX;
	//b[4][2].printValue = KING_MAX_C;
	b[4][2].fuel = 0;

	b[5][5].value = KING_MIN;
	//b[5][5].printValue = KING_MIN_C;
	b[5][5].fuel = 1;

	pair<int, int> values[16] = { { 2, 2 }, { 1, 1 }, { 0, 0 }, { 2, 4 }, { 1, 5 }, { 0, 6 }, { 4, 4 }, { 5, 5 }, { 5, 2 }, { 5, 4 }, { 4, 5 }, { 2, 5 }, { 1, 4 }, { 1, 2 }, { 2, 1 }, { 4, 1} };
	testMove(3, 3, QUEEN_MAX, 16, values);

	clearPosition(4, 2);
	clearPosition(5, 5);

	//testing init position of queen
	b[6][4].value = KING_MAX;
	//b[6][4].printValue = KING_MAX_C;
	b[6][4].fuel = 0;
	pair<int, int> values2[11] = { { 5, 1 }, { 4, 2 }, { 4, 4 }, { 5, 5 }, { 5, 2 }, { 4, 1 }, { 3, 0 }, { 5, 4 }, { 4, 5 }, { 3, 6 }, {2, 7} };
	testMove(6, 3, QUEEN_MAX, 11, values2);
	clearPosition(6, 4);

}
void testPostSetUp(){
	setup();
	//[6, 0] [6, 1] [6, 2] [6, 3] [6, 4] [6, 5] [6, 6] [6, 7]
	//[5, 0] [5, 1] [5, 2] [5, 3] [5, 4] [5, 5] [5, 6] [5, 7]
	//[4, 0] [4, 1] [4, 2] [4, 3] [4, 4] [4, 5] [4, 6] [4, 7]
	//[3, 0] [3, 1] [3, 2] [3, 3] [3, 4] [3, 5] [3, 6] [3, 7]
	//[2, 0] [2, 1] [2, 2] [2, 3] [2, 4] [2, 5] [2, 6] [2, 7]
	//[1, 0] [1, 1] [1, 2] [1, 3] [1, 4] [1, 5] [1, 6] [1, 7]
	//[0, 0] [0, 1] [0, 2] [0, 3] [0, 4] [0, 5] [0, 6] [0, 7]

	// - - B Q K B - -
	// - - - N - N - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - n - n - -
	// - - b q k b - -
	pair<pair<int, int>, pair<int, int>> values[38] = {
			{ { 6, 2 }, { 5, 1 } }, //bishop position 1
			{ { 6, 2 }, { 4, 0 } },
			{ { 6, 2 }, { 5, 3 } },
			{ { 6, 5 }, { 5, 6 } }, //bishop position 2
			{ { 6, 5 }, { 4, 7 } },
			{ { 6, 5 }, { 5, 4 } },
			{ { 6, 5 }, { 4, 3 } },
			{ { 6, 5 }, { 3, 2 } },
			{ { 6, 5 }, { 2, 1 } },
			{ { 6, 5 }, { 1, 0 } },
			{ { 6, 4 }, { 6, 3 } }, //king
			{ { 6, 4 }, { 5, 3 } }, 
			{ { 6, 4 }, { 5, 4 } }, 
			{ { 6, 4 }, { 5, 5 } },
			{ { 6, 4 }, { 6, 5 } },
			{ { 5, 3 }, { 6, 1 } },  //knight 1
			{ { 5, 3 }, { 6, 5 } },
			{ { 5, 3 }, { 3, 2 } },
			{ { 5, 3 }, { 3, 4 } },
			{ { 5, 3 }, { 4, 1 } },
			{ { 5, 3 }, { 4, 5 } },
			{ { 5, 5 }, { 4, 7 } },  //knight 2
			{ { 5, 5 }, { 3, 6 } }, 
			{ { 5, 5 }, { 3, 4 } },
			{ { 5, 5 }, { 4, 3 } },
			{ { 5, 5 }, { 6, 7 } },
			{ { 5, 5 }, { 6, 3 } },
			{ { 6, 3 }, { 5, 1 } }, //queen
			{ { 6, 3 }, { 4, 2 } }, 
			{ { 6, 3 }, { 4, 4 } }, 
			{ { 6, 3 }, { 5, 5 } }, 
			{ { 6, 3 }, { 5, 2 } }, 
			{ { 6, 3 }, { 4, 1 } }, 
			{ { 6, 3 }, { 3, 0 } }, 
			{ { 6, 3 }, { 5, 4 } }, 
			{ { 6, 3 }, { 4, 5 } }, 
			{ { 6, 3 }, { 3, 6 } }, 
			{ { 6, 3 }, { 2, 7 } }
	};
	testMove(38, values);
}
void clearPosition(int row, int col){
	b[row][col].value = 0;
	b[row][col].fuel = 3;
}
void testMove(int from_row, int from_col, int value, int valuesCount, pair<int, int> values[]){
	b[from_row][from_col].value = value;
	b[from_row][from_col].fuel = 3;

	printboard();

	vector<Move> moves;
	 getPossibleMovesMax(moves);
	for (int i = 0; i < valuesCount; i++){
		int to_row = values[i].first;
		int to_col = values[i].second;
		vector<Move>::iterator itr = foundMove(moves, to_row, to_col);
		bool found = (itr != moves.end());
		
		if (found){
			Move m = *itr;
			//see if move/retract works
			int pieceTaken[3];
			makeMove(m, pieceTaken);
			bool pieceMoved = b[to_row][to_col].value == value;

			bool previousCleared = b[from_row][from_col].value == 0;
			
			if (!(pieceMoved && previousCleared)){
				cout << "	Piece in proper place: " << pieceMoved << " and cleared properly: " << previousCleared << endl;
				cout << "	Piece post moved: " << endl;
				printboard();
			}
			
			retractMove(m, pieceTaken);
			bool pieceMovedBack = b[from_row][from_col].value == value;
			bool pieceClearedBack = b[to_row][to_col].value == pieceTaken[0] && b[to_row][to_col].fuel == pieceTaken[1];
			
			if (!(pieceClearedBack && pieceMovedBack)){
				cout << "	Piece moved back proper: " << pieceMovedBack << " and old piece replaced " << pieceClearedBack << endl;
				cout << "	Piece Taken: " << getPrintValue(pieceTaken[0] )<< pieceTaken[1] << endl;
				cout << "	Piece post moving back: " << endl;
				printboard();
			}
		}
		else {
			cout << getPrintValue(value) << " from [" << from_row << ", " << from_col << "] move to [" << to_row << ", " << to_col << "] =" << found << endl;
		}
	}
	bool sizesMatch = (moves.size() == valuesCount);
	cout << "actual moves list size equals given: " << sizesMatch << endl;
	
	if (!sizesMatch){
		cout << "number of moves: " << moves.size() << endl;
		for (Move c : moves){
			std::cout << '[' << c.from_row << ',' << c.from_col << "], [" << c.to_row << ',' << c.to_col << ']' << endl;
		}
	}

	//clear it afterwards
	clearPosition(from_row, from_col);

}
//test moves for more then one piece at a time. So need both starting and ending spot
void testMove(int valuesCount, pair<pair<int, int>, pair<int, int>> values[]){
	cout << "Starting positions: " << endl;
	printboard();
	vector<Move> moves;
	getPossibleMovesMax(moves);
	for (int i = 0; i < valuesCount; i++){
		//cout << "Starting positions: " << endl;
		//printboard();
		int from_row = values[i].first.first;
		int from_col = values[i].first.second;
		int to_row = values[i].second.first;
		int to_col = values[i].second.second;
		
		int value = b[from_row][from_col].value;
		//char printValue = b[from_row][from_col].printValue;
		vector<Move>::iterator itr = foundWholeMove(moves, to_row, to_col, from_row, from_col);
		bool found = (itr != moves.end());

		if (found){
			//cout << printValue << " from [" << from_row << ", " << from_col << "] move to [" << to_row << ", " << to_col << "] =" << found << endl;
			Move m = *itr;
			//see if move/retract works
			int pieceTaken[3];
			makeMove(m, pieceTaken);
			bool pieceMoved = b[to_row][to_col].value == value;

			bool previousCleared = b[from_row][from_col].value == 0;

			if (!(pieceMoved && previousCleared)){
				cout << "	Piece in proper place: " << pieceMoved << " and cleared properly: " << previousCleared << endl;
				cout << "	Piece post moved: " << endl;
				printboard();
			}

			retractMove(m, pieceTaken);
			bool pieceMovedBack = b[from_row][from_col].value == value;
			bool pieceClearedBack = b[to_row][to_col].value == pieceTaken[0] && b[to_row][to_col].fuel == pieceTaken[1];

			if (!(pieceClearedBack && pieceMovedBack)){
				cout << "	Piece moved back proper: " << pieceMovedBack << " and old piece replaced " << pieceClearedBack << endl;
				cout << "	Piece Taken: " << getPrintValue(pieceTaken[0]) << pieceTaken[1] << endl;
				cout << "	Piece post moving back: " << endl;
				printboard();
			}
		}
		else {
			cout << getPrintValue(value) << " from [" << from_row << ", " << from_col << "] move to [" << to_row << ", " << to_col << "] =" << found << endl;
		}
	}
	bool sizesMatch = (moves.size() == valuesCount);
	cout << "actual moves list size equals given: " << sizesMatch << endl;

	if (!sizesMatch){
		cout << "number of moves: " << moves.size() << endl;
		for (Move c : moves){
			std::cout << '[' << c.from_row << ',' << c.from_col << "], [" << c.to_row << ',' << c.to_col << ']' << endl;
		}
	}
}
char getPrintValue(int value){
	return printValues[value + 4];
}