#include <iostream>
#include <cstdio>
#include <vector>
#include "ortools/sat/cp_model.h"

using namespace operations_research;

int get_x_coordinate(int position)
{
    return position % 3;
}
int get_y_coordinate(int position)
{
    return (position % 9 ) / 3;
}
int get_z_coordinate(int position)
{
    return position / 9;
}


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

    const int max_coordinate = 3;//0 1 2
    const int max_orientations = 6; // x+,x-,y+,y-,z+,z-
    const int max_positions = numberOfPieces;
    const operations_research::Domain domain(0, max_positions-1);
    const std::vector<std::string> orientations_dict = {"increasing_x","increasing_y","increasing_z",
                                                "decreasing_x","decreasing_y","decreasing_z"};
    const std::vector<std::string> axis_dict = {"x","y","z","x","y","z"};



    /******************************* VARIABLES ************/

    //create integer variables giving each cube's position varsIntPosition[piece]
    std::vector<operations_research::sat::IntVar> varsIntPosition;
    for (int piece_i = 0; piece_i < numberOfPieces; ++piece_i)
    {
        varsIntPosition.push_back(cp_model.NewIntVar(domain)
                                        .WithName("positionOfPiece_" + std::to_string(piece_i)));
    }

    //create variables linking each piece position couple varsPositions[piece][position]
    std::vector< std::vector<operations_research::sat::BoolVar> > varsPiecesPositions;
    for (int piece_i = 0; piece_i < numberOfPieces; ++piece_i)
    {
        std::vector<operations_research::sat::BoolVar> vectPostionsOfPiece_l;
        for (int position_i = 0; position_i < numberOfPieces; ++position_i)
        {
            vectPostionsOfPiece_l.push_back(cp_model.NewBoolVar()
                                                    .WithName("piece_" + std::to_string(piece_i) + "-position_" + std::to_string(position_i)));
        }
        varsPiecesPositions.push_back(vectPostionsOfPiece_l);
    }

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
        for(int coordinate_i = 0; coordinate_i < max_coordinate; ++coordinate_i)
        {
            vectXofPiece_l.push_back(cp_model.NewBoolVar().WithName("x-piece_" + std::to_string(piece_i) + "-" + std::to_string(coordinate_i)));
            vectYofPiece_l.push_back(cp_model.NewBoolVar().WithName("y-piece_" + std::to_string(piece_i) + "-" + std::to_string(coordinate_i)));
            vectZofPiece_l.push_back(cp_model.NewBoolVar().WithName("z-piece_" + std::to_string(piece_i) + "-" + std::to_string(coordinate_i)));
        }
        varsCube_x.push_back(vectXofPiece_l);
        varsCube_y.push_back(vectYofPiece_l);
        varsCube_z.push_back(vectZofPiece_l);
    }

    //create variables varsOrientation[part][orientation] //0=x, 1=y, 2=z
    std::vector< std::vector<operations_research::sat::BoolVar> > varsOrientation;
    for (int i = 0; i < numberOfParts; ++i)
    {
        std::vector<operations_research::sat::BoolVar> partOrientation_l;
        for(int orientation_i = 0; orientation_i < max_orientations; ++orientation_i)
        {
            partOrientation_l.push_back(cp_model.NewBoolVar()
                                                .WithName("orientation-group_"  + std::to_string(i) + "-" + orientations_dict[orientation_i]));
        }
        varsOrientation.push_back(partOrientation_l);
    }


    /******************************* VARIABLES LINKING CONSTRAINTS ************/
    
    //Link varsIntPosition to varsPiecesPositions
    for (int cube_i = 0; cube_i < numberOfPieces; ++cube_i)
    {
        for (int position_i = 0; position_i < max_positions; ++position_i)
        {
            cp_model.AddEquality(varsIntPosition[cube_i],position_i)
                    .OnlyEnforceIf(varsPiecesPositions[cube_i][position_i])
                    .WithName("Link-piece_" + std::to_string(cube_i) + "-position_" + std::to_string(position_i));
        }
    }

    //Link positions to coordinates
    for (int cube_i = 0; cube_i < numberOfPieces; ++cube_i)
    {
        for (int position_i = 0; position_i < max_positions; ++position_i)
        {
            cp_model.AddEquality(varsCube_x[cube_i][get_x_coordinate(position_i)],1)
                    .OnlyEnforceIf(varsPiecesPositions[cube_i][position_i])
                    .WithName("x-link-piece_" + std::to_string(cube_i) + "_x_" +std::to_string(get_x_coordinate(position_i)) + "-ToPosition_" + std::to_string(position_i));
            cp_model.AddEquality(varsCube_y[cube_i][get_y_coordinate(position_i)],1)
                    .OnlyEnforceIf(varsPiecesPositions[cube_i][position_i])
                    .WithName("y-link-piece_" + std::to_string(cube_i) + "_y_" +std::to_string(get_y_coordinate(position_i)) + "-ToPosition_" + std::to_string(position_i));
            cp_model.AddEquality(varsCube_z[cube_i][get_z_coordinate(position_i)],1)
                    .OnlyEnforceIf(varsPiecesPositions[cube_i][position_i])
                    .WithName("z-link-piece_" + std::to_string(cube_i) + "_z_" +std::to_string(get_z_coordinate(position_i)) + "-ToPosition_" + std::to_string(position_i));
        }
    }

    /******************************* PROBLEM CONSTRAINTS ************/


    //One cube per position and one position per cube
    cp_model.AddAllDifferent(varsIntPosition);

    //One coordinate value per axis, for each piece
    for (int cube_i = 0; cube_i < numberOfPieces; ++cube_i)
    {
        cp_model.AddEquality(operations_research::sat::LinearExpr::BooleanSum(varsCube_x[cube_i]), 1)
                .WithName("one_X_coordinate-cube_" + std::to_string(cube_i));
        cp_model.AddEquality(operations_research::sat::LinearExpr::BooleanSum(varsCube_y[cube_i]), 1)
                .WithName("one_Y_coordinate-cube_" + std::to_string(cube_i));
        cp_model.AddEquality(operations_research::sat::LinearExpr::BooleanSum(varsCube_z[cube_i]), 1)
                .WithName("one_Z_coordinate-cube_" + std::to_string(cube_i));
    }

    //OPTIONAL
    //One position per cube using varsPiecesPositions
    for (int cube_i = 0; cube_i < numberOfPieces; ++cube_i)
    {
        cp_model.AddEquality(operations_research::sat::LinearExpr::BooleanSum(varsPiecesPositions[cube_i]), 1)
                .WithName("one_position_for_cube_" + std::to_string(cube_i));
    }

    //OPTIONAL
    //One cube per position using varsPiecesPositions
    for (int position_i = 0; position_i < numberOfPieces; ++position_i)
    {
        operations_research::sat::LinearExpr expr_l;
        for (int cube_i = 0; cube_i < numberOfPieces; ++cube_i)
        {
            expr_l.AddTerm(varsPiecesPositions[cube_i][position_i], 1);
        }
        cp_model.AddEquality(expr_l, 1)
                    .WithName("one_cube_for_position_" + std::to_string(position_i));
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
        for (int orientation = 0; orientation < (max_orientations/2); ++orientation)
        {
            operations_research::sat::LinearExpr expr_l;
            expr_l.AddTerm(varsOrientation[part_i][orientation], 1);
            expr_l.AddTerm(varsOrientation[part_i+1][orientation], 1);
            expr_l.AddTerm(varsOrientation[part_i][orientation+3], 1);
            expr_l.AddTerm(varsOrientation[part_i+1][orientation+3], 1);
            cp_model.AddLessOrEqual(expr_l, 1)
                    .WithName("change_direction_" + axis_dict[orientation] + "-parts_" + std::to_string(part_i) + "_" + std::to_string(part_i+1));
        }
    }

    //cubes of same group have one different coordinate
    for (int part_i = 0; part_i < numberOfParts; ++part_i)
    {
        std::vector<int> const &  pieces = parts_l[part_i];
        for(int piece_ind = 0; piece_ind <  pieces.size()-1 ; piece_ind++)
        {
            for(int coordinate_i = 0; coordinate_i < max_coordinate; ++coordinate_i)
            {
                //part is oriented according to increasing x axis
                cp_model.AddEquality(varsCube_y[pieces[piece_ind]][coordinate_i],varsCube_y[pieces[piece_ind+1]][coordinate_i])
                        .OnlyEnforceIf(varsOrientation[part_i][0])
                        .WithName("increasing_x-oriented-force_Y_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                cp_model.AddEquality(varsCube_z[pieces[piece_ind]][coordinate_i],varsCube_z[pieces[piece_ind+1]][coordinate_i])
                        .OnlyEnforceIf(varsOrientation[part_i][0])
                        .WithName("increasing_x-oriented-force_Z_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                cp_model.AddLessThan(varsIntPosition[pieces[piece_ind]],varsIntPosition[pieces[piece_ind+1]])
                        .OnlyEnforceIf(varsOrientation[part_i][0])
                        .WithName("increasing_x-force-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));

                cp_model.AddEquality(varsCube_y[pieces[piece_ind]][coordinate_i],varsCube_y[pieces[piece_ind+1]][coordinate_i])
                        .OnlyEnforceIf(varsOrientation[part_i][3])
                        .WithName("decreasing_x-oriented-force_Y_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                cp_model.AddEquality(varsCube_z[pieces[piece_ind]][coordinate_i],varsCube_z[pieces[piece_ind+1]][coordinate_i])
                        .OnlyEnforceIf(varsOrientation[part_i][3])
                        .WithName("decreasing_x-oriented-force_Z_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                cp_model.AddGreaterThan(varsIntPosition[pieces[piece_ind]],varsIntPosition[pieces[piece_ind+1]])
                        .OnlyEnforceIf(varsOrientation[part_i][3])
                        .WithName("decreasing_x-force-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));

                //part is oriented according to y axis
                cp_model.AddEquality(varsCube_x[pieces[piece_ind]][coordinate_i],varsCube_x[pieces[piece_ind+1]][coordinate_i])
                        .OnlyEnforceIf(varsOrientation[part_i][1])
                        .WithName("increasing_y-oriented-force_X_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                cp_model.AddEquality(varsCube_z[pieces[piece_ind]][coordinate_i],varsCube_z[pieces[piece_ind+1]][coordinate_i])
                        .OnlyEnforceIf(varsOrientation[part_i][1])
                        .WithName("increasing_y-oriented-force_Z_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                cp_model.AddLessThan(varsIntPosition[pieces[piece_ind]],varsIntPosition[pieces[piece_ind+1]])
                        .OnlyEnforceIf(varsOrientation[part_i][1])
                        .WithName("increasing_y-force-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));

                cp_model.AddEquality(varsCube_x[pieces[piece_ind]][coordinate_i],varsCube_x[pieces[piece_ind+1]][coordinate_i])
                        .OnlyEnforceIf(varsOrientation[part_i][4])
                        .WithName("decreasing_y-oriented-force_X_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                cp_model.AddEquality(varsCube_z[pieces[piece_ind]][coordinate_i],varsCube_z[pieces[piece_ind+1]][coordinate_i])
                        .OnlyEnforceIf(varsOrientation[part_i][4])
                        .WithName("decreasing_y-oriented-force_Z_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                cp_model.AddGreaterThan(varsIntPosition[pieces[piece_ind]],varsIntPosition[pieces[piece_ind+1]])
                        .OnlyEnforceIf(varsOrientation[part_i][4])
                        .WithName("decreasing_y-force-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));

                //part is oriented according to z axis
                cp_model.AddEquality(varsCube_x[pieces[piece_ind]][coordinate_i],varsCube_x[pieces[piece_ind+1]][coordinate_i])
                        .OnlyEnforceIf(varsOrientation[part_i][2])
                        .WithName("increasing_z-oriented-force_X_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                cp_model.AddEquality(varsCube_y[pieces[piece_ind]][coordinate_i],varsCube_y[pieces[piece_ind+1]][coordinate_i])
                        .OnlyEnforceIf(varsOrientation[part_i][2])
                        .WithName("increasing_z-oriented-force_Y_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                cp_model.AddLessThan(varsIntPosition[pieces[piece_ind]],varsIntPosition[pieces[piece_ind+1]])
                        .OnlyEnforceIf(varsOrientation[part_i][2])
                        .WithName("increasing_z-force-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));

                cp_model.AddEquality(varsCube_x[pieces[piece_ind]][coordinate_i],varsCube_x[pieces[piece_ind+1]][coordinate_i])
                        .OnlyEnforceIf(varsOrientation[part_i][5])
                        .WithName("decreasing_z-oriented-force_X_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                cp_model.AddEquality(varsCube_y[pieces[piece_ind]][coordinate_i],varsCube_y[pieces[piece_ind+1]][coordinate_i])
                        .OnlyEnforceIf(varsOrientation[part_i][5])
                        .WithName("decreasing_z-oriented-force_Y_equality-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
                cp_model.AddGreaterThan(varsIntPosition[pieces[piece_ind]],varsIntPosition[pieces[piece_ind+1]])
                        .OnlyEnforceIf(varsOrientation[part_i][5])
                        .WithName("decreasing_z-force-Part_" + std::to_string(part_i) + "-Pieces_" + std::to_string(pieces[piece_ind]) + "_" + std::to_string(pieces[piece_ind+1]));
            }
        }
    }

    //Fix the coordinate of the first part :
    // four consecutive 3-pieces parts ==> the first part can only be an edge
    cp_model.AddEquality(varsIntPosition[0], 18);
    cp_model.AddEquality(varsIntPosition[1], 19);
    cp_model.AddEquality(varsIntPosition[2], 20);

    //output model
    std::cout << "model:\n" <<  cp_model.Build().DebugString();

    //solve
    const operations_research::sat::CpSolverResponse response = Solve(cp_model.Build());
    LOG(INFO) << CpSolverResponseStats(response);

    if (response.status() == operations_research::sat::CpSolverStatus::FEASIBLE
        || response.status() == operations_research::sat::CpSolverStatus::OPTIMAL) {

        LOG(INFO) << "\n positions: ";
        for (int i = 0; i < numberOfPieces; ++i) {
            LOG(INFO) << "\n\tpiece " << i << " : " << operations_research::sat::SolutionIntegerValue(response, varsIntPosition[i]);
        }

        // LOG(INFO) << "\n coordinates: ";
        // for (int i = 0; i < numberOfPieces; ++i) {
        //     LOG(INFO) << "\n\tpiece " << i << " : ";
        //     for(int coordinate=0; coordinate < max_coordinate; coordinate++)
        //     {
        //         if (operations_research::sat::SolutionBooleanValue(response, varsCube_x[i][coordinate]))
        //         {
        //             LOG(INFO) << "x " << coordinate << ", ";
        //         }
        //     }

        //     for(int coordinate=0; coordinate < max_coordinate; coordinate++)
        //     {
        //         if (operations_research::sat::SolutionBooleanValue(response, varsCube_y[i][coordinate]))
        //         {
        //             LOG(INFO) << "y " << coordinate << ", ";
        //         }
        //     }

        //     for(int coordinate=0; coordinate < max_coordinate; coordinate++)
        //     {
        //         if (operations_research::sat::SolutionBooleanValue(response, varsCube_z[i][coordinate]))
        //         {
        //             LOG(INFO) << "z " << coordinate << ", ";
        //         }
        //     }
        // }

        // LOG(INFO) << "\n\norientations: ";
        // for (int part_i = 0; part_i < numberOfParts; ++part_i) {
        //     for(int orientation=0; orientation < max_orientations ; orientation++)
        //     {
        //         if (operations_research::sat::SolutionBooleanValue(response, varsOrientation[part_i][orientation]))
        //         {
        //             LOG(INFO) << "\npart " << part_i << " : " << orientations_dict[orientation];
        //         }
        //     }
        // }
    }



    return EXIT_SUCCESS;
}
