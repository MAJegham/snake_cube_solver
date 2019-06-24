#include <iostream>
#include <cstdio>
#include <vector>
#include "ortools/constraint_solver/constraint_solveri.h"

using namespace operations_research;

int main(int argc, char** argv) {
	//Instance to solve
    std::vector<std::vector< int> > initialBoard = {    {5,3,0,0,7,0,0,0,0},
                                                        {6,0,0,1,9,5,0,0,0},
                                                        {0,9,8,0,0,0,0,6,0},
                                                        {8,0,0,0,6,0,0,0,3},
                                                        {4,0,0,8,0,3,0,0,1},
                                                        {7,0,0,0,2,0,0,0,6},
                                                        {0,6,0,0,0,0,2,8,0},
                                                        {0,0,0,4,1,9,0,0,5},
                                                        {0,0,0,0,8,0,0,7,9}
                                                    };
	
    std::cout << "Instance to solve:\n";
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            std::cout << initialBoard[i][j] << "  ";
        }
        std::cout << "\n";
    }

	//create solver	
    operations_research::Solver s("sudoku");

    //create variables
    std::vector<std::vector<operations_research::IntVar*> > board(9);
    std::vector<operations_research::IntVar*> vars;
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
			vars.push_back(s.MakeIntVar(1, 9, absl::StrFormat("cell%01d-%01d", i, j)));
            board[i].push_back(vars.back());
        }
    }

	//create constraints
	//3*3 cells constraints
    for (int i = 0; i < 3; ++i )
    {
        for (int j = 0; j < 3; ++j) {
            std::vector<operations_research::IntVar*> cellVars;
            for(int ii = i*3; ii<i*3+3; ++ii){
                for(int jj = j*3; jj<j*3+3; ++jj){
                    cellVars.push_back(board[ii][jj]);
                }
            }
            s.AddConstraint(s.MakeAllDifferent(cellVars));
        }
    }
	
	//rows and columns constraints
    for (int cnt = 0; cnt < 9; ++cnt )
    {
        std::vector<operations_research::IntVar*> rowVars;
        std::vector<operations_research::IntVar*> columnVars;
        for (int j = 0; j < 9; ++j) {
            rowVars.push_back(board[cnt][j]);
            columnVars.push_back(board[j][cnt]);
        }
        s.AddConstraint(s.MakeAllDifferent(rowVars));
        s.AddConstraint(s.MakeAllDifferent(columnVars));
    }

	//fix known values
    for (int i = 0; i < 9; ++i )
    {
        for (int j = 0; j < 9; ++j) {
            if (initialBoard[i][j] != 0)
                board[i][j]->SetValues({initialBoard[i][j]});
        }
    }

	//solve
    operations_research::DecisionBuilder* const db = s.MakePhase(vars,
                                             Solver::CHOOSE_FIRST_UNBOUND,
                                             Solver::ASSIGN_MIN_VALUE);
    s.NewSearch(db);
    CHECK(s.NextSolution());

    std::cout << "Solution:\n";
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            std::cout << board[i][j]->Value() << "  ";
        }
        std::cout << "\n";
    }
    s.EndSearch();

    return EXIT_SUCCESS;
}
