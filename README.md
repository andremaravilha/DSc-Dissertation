# Scheduling Maneuvers for the Restoration of Electric Power Distribution Networks

> **Author:** André L. Maravilha<sup>1,3</sup>  
> **Advisors:** Felipe Campelo<sup>2,3</sup> and Eduardo G. Carrano<sup>2,3</sup> (co-advisor)  
> <sup>1</sup> *Graduate Program in Electrical Engineering, Universidade Federal de Minas Gerais (PPGEE, UFMG)*  
> <sup>2</sup> *Dept. Electrical Engineering, Universidade Federal de Minas Gerais (UFMG)*  
> <sup>3</sup> *Operations Research and Complex Systems Lab., Universidade Federal de Minas Gerais (ORCS Lab, UFMG)*


## 1. About

This repository contains the source code of André's PhD dissertation entitled "Scheduling maneuvers for the restoration of electric power distribution networks".

All methods were coded in C++ (version C++17). The project was developed using [CLion](https://www.jetbrains.com/clion/) with [CMake](https://cmake.org/) (version 3.9) and  GNU Compiller Collection ([GCC](https://gcc.gnu.org/), version 8.1.1) on a Linux machine running Fedora 28 (64-bits). The content of this repository is available under the MIT license. The mixed integer programming (MIP) formulations were solved with [Gurobi](http://www.gurobi.com/) (version 8.0.1).


## 2. How to build the project

#### 2.1. Important comments before building the project

To compile this project you need to have Gurobi (version 8.0.+), GCC (version 8.1.1 or later) and CMake (version 3.9 or later) installed on you computer. The code was has not been tested on versions earlier than the ones specified.

#### 2.2. Building the project

Inside the root directory of the project, run the following commands:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../source
make
```

The project is configured to search for Gurobi at `/opt/gurobi/` directory. If Gurobi is installed in another directory or your are using a version different from 8.0.1, you have to set the correct path for Gurobi installation and the name of its libraries. For example, if Gurobi is installed under the directory `/opt/gurobi801/linux64/`, you can run the CMake as follows:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DGUROBI_PATH=/opt/gurobi801/linux64 -DGUROBI_LIBRARY=gurobi80 ../source
make
```

## 3. Running the project

Inside the `experiments` directory, you can find a Python script `run.py` that performs the same experiment described in the PhD dissertation. To run it, after compiling the project (as described in the previous section) and inside the `experiments` directory, run the following command:
```
python3 run.py
```  
After its completion, you will find a CSV file `results.csv` with the results of the experiments.

Note that to run this script you need the benchmark instances. These instances located in the directory `instances/benchmark/`. The scripts used to create them are located in the directory `instances/generator`, which can also be used to create new instances.

However, if you want to run the optimizaton methods with other settings, you can run the executable created after building the project. The subsection below shows some examples of how to use it and section 4, *Parameters description*, shows and describes all parameters.


### 3.1. Usage examples

After build the project, you can run the executable created. Below we present some examples on how to run the executable. For the examples, consider `schd` as the name of the executable file after building and `instance.txt` as the name of the file containing the instance data and located at the same directory of the executable. A complete list of parameters is given in the next sections.

###### Show the help message:
```
./schd --help
```

###### Using the Simple Greedy heuristic:
```
./schd -v -s -d 3 --algorithm greedy --file instance.txt
```

In the example above, the Simple Greedy heuristic is performed to find a solution.

###### Using the NEH-based Greedy heuristic:
```
./schd -v -s -d 3 --algorithm neh --file instance.txt
```

In the example above, the NEH-based Greedy heuristic is performed to find a solution.

###### Using the ILS-based heuristic:
```
./schd -v -s -d 3 --algorithm ils --file instance.txt
```

In the example above, the ILS-based heuristic is performed to find a solution. It starts from the solution found by the Simple Greedy heuristic and try to find an improved solution.

###### Using the MIP formulation based on precedence variables:
```
./schd -v -s -d 3 --algorithm mip-precedence --file instance.txt
```

In the example above, the Gurobi solves the MIP formulation based on precedence variables built from the instance data. As no time limit nor iteration (MIP nodes) limit are set, it stops when the optimal solution is found.

###### Using the MIP formulation based on linear ordering varaibles:
```
./schd -v -s -d 3 --algorithm mip-linear-ordering --file instance.txt
```

In the example above, the Gurobi solves the MIP formulation based on linear ordering variables built from the instance data. As no time limit nor iteration (MIP nodes) limit are set, it stops when the optimal solution is found.

###### Using the MIP formulation based on arc-time-indexed varaibles:
```
./schd -v -s -d 3 --algorithm mip-arc-time-indexed --file instance.txt
```

In the example above, the Gurobi solves the MIP formulation based on arc-time-indexed variables built from the instance data. As no time limit nor iteration (MIP nodes) limit are set, it stops when the optimal solution is found.


## 4. Parameters description

#### 4.1. General parameters:

`-h`, `--help`  
Show a help message and exit.

`-f <VALUE>`, `--file <VALUE>`  
Name of the file containing the instance data.

`--algorithm <VALUE>`  
The algorithm used to solve the instance. Valid values are:
* `greedy`: Simple Greedy heuristic.
* `neh`: NEH-based Greedy heuristic.
* `ils`: ILS-based heuristic.
* `mip-precedence`: Solves the MIP formulation based on precedence variables using Gurobi solver.
* `mip-linear-ordering`: Solves the MIP formulation based on linear ordering variables using Gurobi solver.
* `mip-arc-time-indexed`: Solves the MIP formulation based on arc-time-indexed variables using Gurobi solver.

`--seed <VALUE>`  
(Default: `0`)  
Set the seed used to initialize the random number generator used the methods (it is used to set the seed of Gurobi solver as well).

`--threads <VALUE>`  
(Default: `1`)  
Number of threads to be used (if the algorithms is able to use multithreading). If set to 0 (zero), all threads available are used.

`--time-limit <VALUE>`  
(Default: `1e100`)  
Limit the total time expended (in seconds).

`--iterations-limit <VALUE>`  
(Default: a very large number)  
Limit the total number of iterations expended. For MIP models, this parameters means the maximum number of MIP nodes explored.

#### 4.2. Printing parameters:

`-v`, `--verbose`  
Display the progress of the optimization process throughout its running.

`-s`, `--solution`  
Display the best solution found.

`-d`, `--details`  
(Default: `1`)  
Set the level of details to show at the end of the the optimization process. Valid values are:
* `0`: Show nothing;
* `1`: Show the status of the optimization process and the value of the objective function, if any;
* `2`: Show the status of the objective function, followed by the value of the objective function, the runtime in seconds, the number of iterations - or MIP nodes explored for MIP formulations -, the value of the linear relaxation, and the MIP optimality gap;
* `3`: Show a more detailed report about the optimization process.

For options `1` and `2`, all values are separated by a single blank space. If some information is not available, a question mark is printed in its place. The possible status are:
* `ERROR`: If an error occurred during the optimization process;
* `UNKNOWN`: The algorithm was not able to find a solution;
* `SUBOPTIMAL`: The algorithm found a feasible solution, but it was not able to prove its optimality.
* `OPTIMAL`: The algorithm found the optimal solution.
* `INFEASIBLE`: The algorithm returned an infeasible solution or the problem is infeasible.
* `UNBOUNDED`: The problem is unbounded.
* `INF_OR_UNBD`: The problem is infeasible or unbounded.

#### 4.3. MIP formulation parameters:

`--warm-start`  
If set, Gurobi will use the solution found by the greedy heuristic as starting solution.

#### 4.4. ILS-based heuristic parameters:

`--perturbation-passes-limit <VALUE>`  
(Default: `5`)  
The highest value of perturbation strength. If no improvement is found after a perturbation with this strength, the ILS stops.

#### 4.6. Local search parameters:

The parameters described below may be used with GRASP and VNS heuristics.

`--local-search-method <VALUE>`  
(Default: `rvnd`)  
Method used to perform local search. Available values are: 
* `vnd`: Variable Neighborhood Search (VND);
* `rvnd`: Randomized Variable Neighborhood Search (RVND).


## 5. Instance files

The instance files are plain text files. The data are formatted as follows:  
```
n m d

1 technology[1] p[1]
2 technology[2] p[2]
...
n technology[n] p[n]

1 size(P[1]) P[1][1] P[1][2] ... P[1][size(P[1])]
2 size(P[2]) P[2][1] P[2][2] ... P[2][size(P[2])]
...
n size(P[n]) P[n][1] P[n][2] ... P[n][size(P[n])]

s[1][0][0] s[1][0][1] ... s[1][0][n]
s[1][1][0] s[1][1][1] ... s[1][1][n]
s[1][2][0] s[1][2][1] ... s[1][2][n]
                      ...
s[1][n][0] s[1][n][1] ... s[1][n][n]

s[2][0][0] s[2][0][1] ... s[2][0][n]
s[2][1][0] s[2][1][1] ... s[2][1][n]
s[2][2][0] s[2][2][1] ... s[2][2][n]
                      ...
s[2][n][0] s[2][n][1] ... s[2][n][n]

s[m][0][0] s[m][0][1] ... s[m][0][n]
s[m][1][0] s[m][1][1] ... s[m][1][n]
s[m][2][0] s[m][2][1] ... s[m][2][n]
                      ...
s[m][n][0] s[m][n][1] ... s[m][n][n]
```  
in which `n` is the number of switch operations, `m` is the number of maintenance teams available and `d` is density (i.e., the order stregth) of the precedence graph. After that, it follows the data about each switch operations, in which `technology[i]` is the technology of the switch (`M` for manually operated switches or `R` for remotely operated switches), and `p[i]` is the time to maneuver the switch `i`.

Next, it follows the precedence constraints. For each switch `i`, its predecessors are listed. For this, `size(P[i])` is the number of predecessors of `i` and `P[i][j]` is the j-th predecessor of the list. Finally, the displacement matrices described. For this, `s[k][i][j]` is the displacement time the team `k` takes to go from `i` to `j`.

