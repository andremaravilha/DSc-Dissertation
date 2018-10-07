#include "mip_linear_ordering.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include <gurobi_c++.h>

#include "../heuristic/greedy.h"
#include "../../util/common.h"


std::tuple<orcs::Schedule, double> orcs::MIPLinearOrdering::solve(const Problem& problem,
        const cxxproperties::Properties* opt_input, cxxproperties::Properties* opt_output) {

    // Algorithm parameters
    cxxproperties::Properties opt_aux;
    if (opt_input == nullptr) {
        opt_input = &opt_aux;
    }

    // Solver parameters
    bool verbose      = opt_input->get<bool>("verbose", false);
    int threads       = opt_input->get<int>("threads", 0);
    double time_limit = opt_input->get<double>("time-limit", GRB_INFINITY);
    double iterations_limit = opt_input->get<double>("iterations-limit", GRB_INFINITY);
    bool warm_start   = opt_input->get<double>("warm-start", false);
    bool solve_lr     = opt_input->get<double>("solve-relaxation", false);

    // Variable to keep the solution
    Schedule solution = create_empty_schedule(problem.m);

    // Get problem data
    auto n = problem.n;
    auto m = problem.m;
    const auto& s = problem.s;
    const auto& p = problem.p;
    const auto& technology = problem.technology;
    const auto& predecessors = problem.predecessors;

    // Compute the big-M value
    double M = 0.0;
    for (int j = 1; j <= n; ++j) {
        double max_c = 0.0;
        if (technology[j] != Technology::REMOTE) {
            for (int i = 0; i <= n; ++i) {
                if (i != j && technology[i] != Technology::REMOTE) {
                    for (int l = 1; l <= m; ++l) {
                        max_c = std::max(max_c, s[i][j][l]);
                    }
                }
            }
        }
        M += max_c + p[j];
    }

    // Solve the problem with Gurobi solver
    GRBEnv* env = nullptr;

    try {

        // Gurobi environment and model
        env = new GRBEnv();
        GRBModel model(*env);

        // Set some settings of Gurobi solver
        model.getEnv().set(GRB_IntParam_LogToConsole, (verbose ? 1 : 0));
        model.getEnv().set(GRB_IntParam_OutputFlag, (verbose ? 1 : 0));
        model.getEnv().set(GRB_IntParam_Threads, threads);
        model.getEnv().set(GRB_DoubleParam_TimeLimit, time_limit);
        model.getEnv().set(GRB_DoubleParam_NodeLimit, iterations_limit);

        // Allocate memory for decision variables
        GRBVar** y = new GRBVar*[n + 1];
        for (int i = 0; i <= n; ++i) {
            y[i] = new GRBVar[m + 1];
        }

        GRBVar** z = new GRBVar*[n + 1];
        for (int i = 0; i <= n; ++i) {
            z[i] = new GRBVar[n + 1];
        }

        GRBVar* t = new GRBVar[n + 1];

        // Decision variables
        for (int i = 1; i <= n; ++i) {
            if (technology[i] != Technology::REMOTE) {

                // Assignment variables
                for (int l = 1; l <= m; ++l) {
                    y[i][l] = model.addVar(0, 1, 0, GRB_BINARY);
                }

                // Ordering variables
                for (int j = 1; j <= n; ++j) {
                    if (j != i && technology[j] != Technology::REMOTE) {
                        z[i][j] = model.addVar(0, 1, 0, GRB_BINARY);
                    }
                }
            }
        }

        for (int i = 1; i <= n; ++i) {
            t[i] = model.addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS);
        }

        GRBVar T = model.addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS);

        model.update();

        // Warm start
        if (warm_start) {

            // Initially, make all variables equal zero
            for (int i = 1; i <= n; ++i) {
                if (technology[i] != Technology::REMOTE) {

                    for (int l = 1; l <= m; ++l) {
                        y[i][l].set(GRB_DoubleAttr_Start, 0.0);
                    }

                    for (int j = 1; j <= n; ++j) {
                        if (j != i && technology[j] != Technology::REMOTE) {
                            z[i][j].set(GRB_DoubleAttr_Start, 0.0);
                        }
                    }
                }
            }

            for (int i = 1; i <= n; ++i) {
                t[i].set(GRB_DoubleAttr_Start, 0.0);
            }

            T.set(GRB_DoubleAttr_Start, 0.0);

            // Get a heuristic solution
            auto [schedule, makespan] = Greedy().solve(problem);
            auto t_ = problem.start_time(schedule);

            // Set initial value for the variables
            T.set(GRB_DoubleAttr_Start, makespan);

            for (int i = 1; i <= n; ++i) {
                t[i].set(GRB_DoubleAttr_Start, t_[i]);
            }

            for (int l = 1; l <= m; ++l) {
                if (schedule[l].size() > 0) {
                    for (int idx = 0; idx < schedule[l].size(); ++idx) {
                        int i = schedule[l][idx];
                        y[i][l].set(GRB_DoubleAttr_Start, 1.0);
                        for (int idx_next = idx + 1; idx_next < schedule[l].size(); ++idx_next) {
                            int j = schedule[l][idx_next];
                            z[i][j].set(GRB_DoubleAttr_Start, 1.0);
                        }
                    }
                }
            }
        }

        // Objective function
        GRBLinExpr objective = T;
        model.setObjective(objective, GRB_MINIMIZE);

        // Constraints 1
        for (int i = 1; i <= n; ++i) {
            if (technology[i] != Technology::REMOTE) {
                GRBLinExpr expr = 0;
                for (int l = 1; l <= m; ++l) {
                    expr += y[i][l];
                }
                model.addConstr(expr == 1);
            }
        }

        // Constraints 2
        for (int l = 1; l <= m; ++l) {
            for (int i = 1; i <= n; ++i) {
                if (technology[i] != Technology::REMOTE) {
                    for (int j = i + 1; j <= n; ++j) {
                        if (j != i && technology[j] != Technology::REMOTE) {
                            model.addConstr(z[i][j] + z[j][i] >= y[i][l] + y[j][l] - 1);
                        }
                    }
                }
            }
        }

        // Constraints 3
        for (int i = 1; i <= n; ++i) {
            if (technology[i] != Technology::REMOTE) {
                for (int j = i + 1; j <= n; ++j) {
                    if (j != i && technology[j] != Technology::REMOTE) {
                        model.addConstr(z[i][j] + z[j][i] <= 1);
                    }
                }
            }
        }

        // Constraints 4
        for (int i = 1; i <= n; ++i) {
            if (technology[i] != Technology::REMOTE) {
                for (int j = 1; j <= n; ++j) {
                    if (j != i && technology[j] != Technology::REMOTE) {
                        for (int k = 1; k <= n; ++k) {
                            if (k != i && k != j && technology[k] != Technology::REMOTE) {
                                model.addConstr(z[i][k] + z[k][j] + z[j][i] <= 2);
                            }
                        }
                    }
                }
            }
        }

        // Constraints 5
        for (int i = 1; i <= n; ++i) {
            if (technology[i] != Technology::REMOTE) {
                GRBLinExpr expr = 0;
                for (int l = 1; l <= m; ++l) {
                    expr += s[0][i][l] * y[i][l];
                }
                model.addConstr(t[i] >= expr);
            }
        }

        // Constraints 6
        for (int i = 1; i <= n; ++i) {
            if (technology[i] != Technology::REMOTE) {
                for (int j = 1; j <= n; ++j) {
                    if (j != i && technology[j] != Technology::REMOTE) {
                        GRBLinExpr expr = 0;
                        for (int l = 1; l <= m; ++l) {
                            expr += s[i][j][l] * y[j][l];
                        }
                        model.addConstr(t[j] >= t[i] + p[i] + expr - M * (1 - z[i][j]));
                    }
                }
            }
        }

        // Constraints 7
        for (int j = 1; j <= n; ++j) {
            for (auto i : predecessors[j]) {
                model.addConstr(t[j] >= t[i] + p[i]);
            }
        }

        // Constraints 8
        for (int i = 1; i <= n; ++i) {
            model.addConstr(T >= t[i] + p[i]);
        }

        // Preprocessing: fix to zero variables z[j][i] which will never be
        // equal to one due to the precedence constraints
        model.update();
        for (int i = 1; i <= n; ++i) {
            if (technology[i] != Technology::REMOTE) {
                for (int j = 1; j <= n; ++j) {
                    if (j != i && technology[j] != Technology::REMOTE) {
                        if (problem.precedence[i][j]) {
                            z[j][i].set(GRB_DoubleAttr_UB, 0);
                        }
                    }
                }
            }
        }

        // Solve the model
        model.optimize();

        // Get the best solution found (if any)
        if (model.get(GRB_IntAttr_SolCount) > 0) {

            for (int j = 1; j <= n; ++j) {
                if (technology[j] != Technology::REMOTE) {
                    for (int l = 1; l <= m; ++l) {
                        if (y[j][l].get(GRB_DoubleAttr_X) > 0.5) {
                            solution[l].push_back(j);
                        }
                    }
                } else {
                    solution[0].push_back(j);
                }
            }

            for (int l = 0; l <= m; ++l) {
                std::sort(solution[l].begin(), solution[l].end(),
                        [&t](int first, int second) -> bool {
                            return (t[first].get(GRB_DoubleAttr_X) < t[second].get(GRB_DoubleAttr_X));
                        });
            }
        }

        // Store optional output
        if (opt_output != nullptr) {

            // Status of the optimization process
            int status = model.get(GRB_IntAttr_Status);
            switch (status) {

                case GRB_OPTIMAL:
                    opt_output->add("Status", "OPTIMAL");
                    break;

                case GRB_INFEASIBLE:
                    opt_output->add("Status", "INFEASIBLE");
                    break;

                case GRB_UNBOUNDED:
                    opt_output->add("Status", "UNBOUNDED");
                    break;

                case GRB_INF_OR_UNBD:
                    opt_output->add("Status", "INF_OR_UNBD");
                    break;

                default:
                    if (model.get(GRB_IntAttr_SolCount) > 0) {
                        opt_output->add("Status", "SUBOPTIMAL");
                    } else {
                        opt_output->add("Status", "UNKNOWN");
                    }
            }

            // Objective function of the best solution found (if any)
            if (model.get(GRB_IntAttr_SolCount) > 0) {
                opt_output->add("MIP objective", model.get(GRB_DoubleAttr_ObjVal));
            }

            // Number of iterations (or MIP nodes)
            try {
                opt_output->add("Iterations", model.get(GRB_DoubleAttr_NodeCount));
            } catch (...) {
                // Do nothing
            }

            // MIP gap
            try {
                double mip_gap = model.get(GRB_DoubleAttr_MIPGap);
                if (common::equal(mip_gap, GRB_INFINITY)) {
                    opt_output->add("MIP gap", "Infinity");
                } else {
                    opt_output->add("MIP gap", mip_gap);
                }
            } catch (...) {
                // Do nothing
            }

            // Runtime
            opt_output->add("MIP runtime (s)", model.get(GRB_DoubleAttr_Runtime));

            // Solve the linear relaxation
            if (solve_lr) {

                // Reset Gurobi solver
                model.getEnv().set(GRB_IntParam_OutputFlag, 0);
                model.getEnv().set(GRB_DoubleParam_TimeLimit, GRB_INFINITY);
                model.reset();

                // Relax the integrality constraints
                for (int i = 1; i <= n; ++i) {
                    if (technology[i] != Technology::REMOTE) {

                        for (int l = 1; l <= m; ++l) {
                            y[i][l].set(GRB_CharAttr_VType, GRB_CONTINUOUS);
                        }

                        for (int j = 1; j <= n; ++j) {
                            if (j != i && technology[j] != Technology::REMOTE) {
                                z[i][j].set(GRB_CharAttr_VType, GRB_CONTINUOUS);
                            }
                        }
                    }
                }

                // Solve the linear relaxation
                model.optimize();

                // Value of the objective function
                if (model.get(GRB_IntAttr_SolCount) > 0) {
                    opt_output->add("LP objective", model.get(GRB_DoubleAttr_ObjVal));
                }

                // Runtime
                opt_output->add("LP runtime (s)", model.get(GRB_DoubleAttr_Runtime));
            }
        }

        // Deallocate resources
        for (int i = 0; i <= n; ++i) {

            delete[] y[i];
            y[i] = nullptr;

            delete[] z[i];
            z[i] = nullptr;
        }

        delete[] y;
        y = nullptr;

        delete[] z;
        z = nullptr;

        delete[] t;
        t = nullptr;

    } catch (...) {

        // Deallocate resources
        if (env != nullptr) {
            delete env;
            env = nullptr;
        }

        // Re-throw the exception
        throw;
    }

    // Deallocate resources
    if (env != nullptr) {
        delete env;
        env = nullptr;
    }

    // Return the solution found
    return {solution, problem.makespan(solution)};
}
