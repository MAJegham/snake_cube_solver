#include <iostream>
#include <cstdio>
#include <vector>
#include "ortools/sat/cp_model.h"

using namespace operations_research;

int main(int argc, char** argv) {



	//create solver
    operations_research::sat::CpModelBuilder cp_model;

    int numberOfPieces = 27;
    int numberOfParts = 17;

    //Instance to solve
    std::vector<std::vector<int>> parts_l = {
                                    {0,1,2},
                                    {2,3,4},
                                    {4,5,6},
                                    {6,7,8},
                                    {8,9},
                                    {9,10},
                                    {10,11},
                                    {11,12,13},
                                    {13,14,15},
                                    {15,16},
                                    {16, 17},
                                    {17, 18, 19},
                                    {19, 20},
                                    {20, 21, 22},
                                    {22,23},
                                    {23, 24},
                                    {24, 25, 26}
                                    };

    const int max_pos = 3;
    const operations_research::Domain domain(0, max_pos-1);
    std::vector<std::string> orientations_dict = {"x","y","z"};



    /******************************* VARIABLES ************/


    //create variables varsCube_.[piece][position_value]
    std::vector< std::vector<operations_research::sat::BoolVar> > varsCube_x;
    std::vector< std::vector<operations_research::sat::BoolVar> > varsCube_y;
    std::vector< std::vector<operations_research::sat::BoolVar> > varsCube_z;
    // std::vector<operations_research::IntVar > vars;
    for (int piece_i = 0; piece_i < numberOfPieces; ++piece_i)
    {
        std::vector<operations_research::sat::BoolVar> vectXofPiece_l;
        std::vector<operations_research::sat::BoolVar> vectYofPiece_l;
        std::vector<operations_research::sat::BoolVar> vectZofPiece_l;
        for(int position_i = 0; position_i < max_pos; ++position_i)
        {
            vectXofPiece_l.push_back(cp_model.NewBoolVar().WithName("x-piece_" + std::to_string(piece_i) + "-" + std::to_string(position_i)));
            vectYofPiece_l.push_back(cp_model.NewBoolVar().WithName("y-piece_" + std::to_string(piece_i) + "-" + std::to_string(position_i)));
            vectZofPiece_l.push_back(cp_model.NewBoolVar().WithName("z-piece_" + std::to_string(piece_i) + "-" + std::to_string(position_i)));
        }
        varsCube_x.push_back(vectXofPiece_l);
        varsCube_y.push_back(vectYofPiece_l);
        varsCube_z.push_back(vectZofPiece_l);
    }



    //create variables varsOrientation[part][position_value] //0=x, 1=y, 2=z
    std::vector< std::vector<operations_research::sat::BoolVar> > varsOrientation;
    for (int i = 0; i < numberOfParts; ++i)
    {
        std::vector<operations_research::sat::BoolVar> partOrientation_l;
        for(int orientation_i = 0; orientation_i < max_pos; ++orientation_i)
        {
            partOrientation_l.push_back(cp_model.NewBoolVar()
                                                .WithName("orientation-group_"  + std::to_string(i) + "-" + orientations_dict[orientation_i]));
        }
        varsOrientation.push_back(partOrientation_l);
    }


    /******************************* CONSTRAINTS ************/

    //One position per cube
    for (int cube_i = 0; cube_i < numberOfPieces; ++cube_i)
    {
        cp_model.AddEquality(operations_research::sat::LinearExpr::BooleanSum(varsCube_x[cube_i]), 1)
                .WithName("one_X_position-cube_" + std::to_string(cube_i));
        cp_model.AddEquality(operations_research::sat::LinearExpr::BooleanSum(varsCube_y[cube_i]), 1)
                .WithName("one_Y_position-cube_" + std::to_string(cube_i));
        cp_model.AddEquality(operations_research::sat::LinearExpr::BooleanSum(varsCube_z[cube_i]), 1)
                .WithName("one_Z_position-cube_" + std::to_string(cube_i));
    }

    //9 cubes per plan
    for(int position_i = 0; position_i < max_pos; ++position_i)
    {
        operations_research::sat::LinearExpr exprX_l;
        for (int cube_i = 0; cube_i < numberOfPieces; ++cube_i)
        {
            exprX_l.AddTerm(varsCube_x[cube_i][position_i], 1);
        }
        cp_model.AddEquality(exprX_l, 9)
                .WithName("nine_cubes_in_plan_X_" + std::to_string(position_i));

        operations_research::sat::LinearExpr exprY_l;
        for (int cube_i = 0; cube_i < numberOfPieces; ++cube_i)
        {
            exprY_l.AddTerm(varsCube_y[cube_i][position_i], 1);
        }
        cp_model.AddEquality(exprY_l, 9)
                .WithName("nine_cubes_in_plan_Y_" + std::to_string(position_i));

        operations_research::sat::LinearExpr exprZ_l;
        for (int cube_i = 0; cube_i < numberOfPieces; ++cube_i)
        {
            exprZ_l.AddTerm(varsCube_z[cube_i][position_i], 1);
        }
        cp_model.AddEquality(exprZ_l, 9)
                .WithName("nine_cubes_in_plan_Z_" + std::to_string(position_i));
    }

    //One orientation per group
    for (int part_i = 0; part_i < numberOfParts; ++part_i)
    {
        cp_model.AddEquality(operations_research::sat::LinearExpr::BooleanSum(varsOrientation[part_i]), 1)
                .WithName("one_orientation-part_" + std::to_string(part_i));
    }

    //Perpendicularity
    for (int part_i = 0; part_i < numberOfParts-1; ++part_i)
    {
        for (int orientation = 0; orientation < max_pos; ++orientation)
        {
            operations_research::sat::LinearExpr expr_l;
            expr_l.AddTerm(varsOrientation[part_i][orientation], 1);
            expr_l.AddTerm(varsOrientation[part_i+1][orientation], 1);
            cp_model.AddLessOrEqual(expr_l, 1)
                    .WithName("change_direction_" + orientations_dict[orientation] + "-parts_" + std::to_string(part_i) + "_" + std::to_string(part_i+1));
        }
    }

    //cubes of same group have one different coordinate
    for (int part_i = 0; part_i < numberOfParts; ++part_i)
    {
        std::vector<int> const &  pieces = parts_l[part_i];
        for(int piece_ind = 0; piece_ind <  pieces.size()-1 ; piece_ind++)
        {
            for(int position_i = 0; position_i < max_pos; ++position_i)
            {
                //part is oriented according to x axis
                cp_model.AddEquality(varsCube_y[pieces[piece_ind]][position_i],varsCube_y[pieces[piece_ind+1]][position_i])
                        .OnlyEnforceIf(varsOrientation[part_i][0])
                        .WithName("x-oriented-force_Y_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                cp_model.AddEquality(varsCube_z[pieces[piece_ind]][position_i],varsCube_z[pieces[piece_ind+1]][position_i])
                        .OnlyEnforceIf(varsOrientation[part_i][0])
                        .WithName("x-oriented-force_Z_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                //part is oriented according to y axis
                cp_model.AddEquality(varsCube_x[pieces[piece_ind]][position_i],varsCube_x[pieces[piece_ind+1]][position_i])
                        .OnlyEnforceIf(varsOrientation[part_i][1])
                        .WithName("y-oriented-force_X_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                cp_model.AddEquality(varsCube_z[pieces[piece_ind]][position_i],varsCube_z[pieces[piece_ind+1]][position_i])
                        .OnlyEnforceIf(varsOrientation[part_i][1])
                        .WithName("y-oriented-force_Z_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                //part is oriented according to z axis
                cp_model.AddEquality(varsCube_x[pieces[piece_ind]][position_i],varsCube_x[pieces[piece_ind+1]][position_i])
                        .OnlyEnforceIf(varsOrientation[part_i][2])
                        .WithName("z-oriented-force_X_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                cp_model.AddEquality(varsCube_y[pieces[piece_ind]][position_i],varsCube_y[pieces[piece_ind+1]][position_i])
                        .OnlyEnforceIf(varsOrientation[part_i][2])
                        .WithName("z-oriented-force_Y_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
            }
        }
    }

    //Fix the position of the first part :
    // four consecutive 3-pieces parts ==> the first part can only be an edge
    cp_model.AddHint(varsCube_x[0][0], 1);
    cp_model.AddHint(varsCube_y[0][0], 1);
    cp_model.AddHint(varsCube_z[0][2], 1);
    cp_model.AddHint(varsOrientation[0][0], 1);


    //output model
    std::cout << "model:\n" <<  cp_model.Build().DebugString();

    //solve
    const operations_research::sat::CpSolverResponse response = Solve(cp_model.Build());
    LOG(INFO) << CpSolverResponseStats(response);

    if (response.status() == operations_research::sat::CpSolverStatus::FEASIBLE
        || response.status() == operations_research::sat::CpSolverStatus::OPTIMAL) {
        LOG(INFO) << "\n positions: ";
        for (int i = 0; i < numberOfPieces; ++i) {
        LOG(INFO) << "\n\tpiece " << i << " : ";
            for(int position=0; position < max_pos; position++)
            {
                if (operations_research::sat::SolutionBooleanValue(response, varsCube_x[i][position]))
                {
                    LOG(INFO) << "x " << position << ", ";
                    break;
                }
            }

            for(int position=0; position < max_pos; position++)
            {
                if (operations_research::sat::SolutionBooleanValue(response, varsCube_y[i][position]))
                {
                    LOG(INFO) << "y " << position << ", ";
                    break;
                }
            }

            for(int position=0; position < max_pos; position++)
            {
                if (operations_research::sat::SolutionBooleanValue(response, varsCube_z[i][position]))
                {
                    LOG(INFO) << "z " << position << ", ";
                    break;
                }
            }
        }
    
        LOG(INFO) << "\n\norientations: ";
        for (int part_i = 0; part_i < numberOfParts; ++part_i) {
            for(int orientation=0; orientation < 3 ; orientation++)
            {
                if (operations_research::sat::SolutionBooleanValue(response, varsOrientation[part_i][orientation]))
                {
                    LOG(INFO) << "\npart " << part_i << " : " << orientations_dict[orientation];
                    break;
                }
            }
        }

    }



    return EXIT_SUCCESS;
}
