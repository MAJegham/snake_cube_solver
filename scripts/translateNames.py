#!/usr/bin/python

# reads a .cp file giving a cp-sat ortools model
# writes the model adding to variables names next to indices in the constraints

import sys
import os

filename = sys.argv[1]

variables_dict = {}


# Writing to file
try:
    os.remove("output_model.cp")
except OSError:
    pass

with open("output_model.cp", "a") as destination_file:
    with open(filename) as input_file:
        read_lines_count = 0
        write_lines_count = 0
        var_count = 0
        while True:
            read_lines_count += 1
            line = input_file.readline()

            if not line:
                break

            if line.lstrip().startswith("variables"):
                variable_section = True

            if line.lstrip().startswith("name: ") and variable_section:
                variable_section = False
                write_lines_count += 1
                destination_file.write("  index: " + str(var_count) + "\n")
                variables_dict[var_count] = line.split('"')[1]
                var_count += 1

            if line.lstrip().startswith("vars: "):
                variable_index = int(line[line.find("vars: ")+6:])
                variable_name = variables_dict.get(variable_index)
                if variable_name is None:
                    print("unknown variable with index " + str(variable_index))
                    write_lines_count +=1
                    destination_file.write(line)
                else:
                    write_lines_count +=1
                    destination_file.write(line.rstrip() + " - " + variable_name + "\n")
            else:
                write_lines_count +=1
                destination_file.write(line)