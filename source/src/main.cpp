#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <cxxopts.hpp>
#include <cxxtimer.hpp>
#include <cxxproperties.hpp>

#include <gurobi_c++.h>

#include "problem/problem.h"
#include "algorithm/algorithm.h"
#include "util/common.h"

#include "algorithm/mip/mip_precedence.h"
#include "algorithm/mip/mip_linear_ordering.h"
#include "algorithm/mip/mip_arc_time_indexed.h"

#include "algorithm/heuristic/greedy.h"
#include "algorithm/heuristic/neh.h"
#include "algorithm/heuristic/ils.h"


/*
 * Function statements.
 */

cxxopts::Options init_parser(int argc, char** argv);


/*
 * Main function.
 */

int main(int argc, char** argv) {

    try {

        // Initialize the command-line parser
        cxxopts::Options options = init_parser(argc, argv);

        // Show help message, if requested
        if (options.count("help") > 0) {
            std::cout << options.help({"", "Printing", "General", "MIP formulations",
                                       "Local search", "ILS"})
                      << std::endl;
            return EXIT_SUCCESS;
        }

        // Abort, if file not specified
        if (options.count("file") < 1) {
            throw std::string("Instance file not specified.");
        }

        // Abort, if file cannot be opened for reading
        std::ifstream file(options["file"].as<std::string>().c_str());
        if (!file.is_open()) {
            throw std::string("File \"" + options["file"].as<std::string>() + "\" cannot be opened.");
        } else {
            file.close();
        }

        // Abort, if no algorithm is specified
        if (options.count("algorithm") < 1) {
            throw std::string("Algorithm not specified.");
        }

        // Abort, if algorithm is invalid
        std::set<std::string> opt_algorithms = {"greedy", "neh", "ils", "mip-precedence",
                                                "mip-linear-ordering", "mip-arc-time-indexed"};

        if (opt_algorithms.count(options["algorithm"].as<std::string>()) < 1) {
            throw std::string("Invalid algorithm.");
        }

        // Load the problem
        orcs::Problem problem(options["file"].as<std::string>());

        // Algorithm parameters
        cxxproperties::Properties opt_input;
        opt_input.add("verbose", options["verbose"].as<bool>());
        opt_input.add("threads", options["threads"].as<int>());
        opt_input.add("seed", options["seed"].as<int>());
        opt_input.add("time-limit", options["time-limit"].as<double>());
        opt_input.add("iterations-limit", options["iterations-limit"].as<long>());

        // Initialize the algorithm selected to solve the problem
        orcs::Algorithm* algorithm = nullptr;
        if (options["algorithm"].as<std::string>() == "greedy") {
            algorithm = new orcs::Greedy();

        } else if (options["algorithm"].as<std::string>() == "neh") {
            algorithm = new orcs::NEH();

        } else if (options["algorithm"].as<std::string>() == "ils") {
            algorithm = new orcs::ILS();
            opt_input.add("perturbation-passes-limit", options["perturbation-passes-limit"].as<long>());
            opt_input.add("local-search-method", options["local-search-method"].as<std::string>());

        } else if (options["algorithm"].as<std::string>() == "mip-precedence") {
            algorithm = new orcs::MIPPrecedence();
            opt_input.add("warm-start", options["warm-start"].as<bool>());
            opt_input.add("solve-relaxation", true);

        } else if (options["algorithm"].as<std::string>() == "mip-linear-ordering") {
            algorithm = new orcs::MIPLinearOrdering();
            opt_input.add("warm-start", options["warm-start"].as<bool>());
            opt_input.add("solve-relaxation", true);

        } else if (options["algorithm"].as<std::string>() == "mip-arc-time-indexed") {
            algorithm = new orcs::MIPArcTimeIndexed();
            opt_input.add("warm-start", options["warm-start"].as<bool>());
            opt_input.add("solve-relaxation", true);

        }

        // Properties to store optional output
        cxxproperties::Properties opt_output;

        // Create a timer
        cxxtimer::Timer timer;

        // Create an empty solution (used to store the result of the algorithm)
        orcs::Schedule schedule = orcs::create_empty_schedule(problem.m);

        bool error = false;
        std::string error_message = "Unknown";

        // Start the times
        timer.start();

        // Solve the problem
        try {

            std::tie(schedule, std::ignore) = algorithm->solve(problem, &opt_input, &opt_output);

        } catch (const GRBException& e) {
            error = true;
            error_message = orcs::common::format("Gurobi error %d: %s", e.getErrorCode(), e.getMessage().c_str());

        } catch (const std::string& e) {
            error_message = e;
            error = true;

        } catch (...) {
            error_message = "Unknown";
            error = true;
        }

        // Stop the time
        timer.stop();

        // Compute the makespan of the schedule
        double makespan = problem.makespan(schedule);

        // Check feasibility of the schedule
        std::string feasibility_msg;
        bool feasible = problem.is_feasible(schedule, &feasibility_msg);

        // Check the status of the solution
        std::string status = "UNKNOWN";
        if (error) {
            status = "ERROR";

        } else {

            // Check the status of the solution / optimization method
            if (opt_output.contains("Status")) {
                status = opt_output.get<std::string>("Status");

            } else if (feasible) {
                status = "SUBOPTIMAL";

            } else {
                status = "INFEASIBLE";

            }
        }

        // Get elapsed time
        double elapsed_time = timer.count<std::chrono::milliseconds>();

        // Show the output
        if (options.count("details") > 0 || options.count("solution") > 0) {

            // Details of the optimization process
            if (options.count("details") > 0) {

                switch (options["details"].as<int>()) {

                    case 0:
                        // Do nothing.
                        break;

                    case 1:
                        std::cout << status << " "
                                  << (feasible ? orcs::common::format("%.6lf", makespan) : "?")
                                  << std::endl;
                        break;

                    case 2:
                        std::cout << status << " "
                                  << (feasible ? orcs::common::format("%.6lf", makespan) : "?") << " "
                                  << orcs::common::format("%.4lf", elapsed_time / 1000.0) << " "
                                  << opt_output.get<std::string>("Iterations", "?") << " "
                                  << opt_output.get<std::string>("LP objective", "?") << " "
                                  << opt_output.get<std::string>("MIP gap", "?") << " "
                                  << std::endl;
                        break;

                    case 3:
                        std::cout << std::endl;
                        std::cout << "======================================================================" << std::endl;
                        std::cout << "SUMMARY" << std::endl;
                        std::cout << "======================================================================" << std::endl;
                        std::cout << "Makespan:         " << (feasible ? orcs::common::format("%.6lf", makespan) : "?") << std::endl;
                        std::cout << "Status:           " << status << std::endl;

                        if (!feasible) {
                            std::cout << "Infeasibility:    " << orcs::common::format("%s", feasibility_msg.c_str()) << std::endl;
                        }

                        if (error) {
                            std::cout << "Error details:    " << orcs::common::format(" - %s", error_message.c_str())<< std::endl;
                        }

                        std::cout << "Elapsed time (s): " << orcs::common::format("%.4lf", elapsed_time / 1000.0) << std::endl << std::endl;
                        std::cout << "Additional Information:" << std::endl;
                        if (opt_output.size() > 0) {
                            for (auto key : opt_output.get_keys()) {
                                std::cout << " * " << key << ": " << opt_output.get<std::string>(key) << std::endl;
                            }
                        } else {
                            std::printf(" * No additional information to show.\n");
                        }
                        break;
                }
            }

            // Solution found
            if (options.count("solution") > 0) {
                std::cout << std::endl;
                std::cout << "======================================================================" << std::endl;
                std::cout << "SOLUTION" << std::endl;
                std::cout << "======================================================================" << std::endl;
                std::cout << schedule << std::endl;
            }

        }

        // Free resources
        if (algorithm != nullptr) {
            delete algorithm;
            algorithm = nullptr;
        }

    } catch (const std::string& e) {
        std::cerr << e << std::endl;
        std::cerr << "Type the following command for a correct usage." << std::endl;
        std::cerr << argv[0] << " --help" << std::endl << std::endl;
        return EXIT_FAILURE;

    } catch (...) {
        std::cerr << "Unexpected error." << std::endl;
        std::cerr << "Type the following command for a correct usage." << std::endl;
        std::cerr << argv[0] << " --help" << std::endl << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


/*
 * Function definitions.
 */

cxxopts::Options init_parser(int argc, char** argv) {

    cxxopts::Options options(argv[0], "Maneuver Scheduling Problem");

    options.add_options("")
            ("h,help", "Show this help message and exit.",
             cxxopts::value<bool>(), "")

            ("f,file", "Path to the instance file with data of the problem to be solved.",
             cxxopts::value<std::string>(), "FILE");

    options.add_options("Printing")
            ("v,verbose", "Enable algorithm output.",
             cxxopts::value<bool>(), "")

            ("d,details", "Set the level of details to show at the end of the the optimization process. Valid  values "
            "are: (0) show nothing; (1) show the status of the optimization process and the value of the objective "
            "function, if any; (2) show the status of the objective function, followed by the value of the objective "
            "function, the runtime in seconds, the number of iterations - or MIP nodes explored for MIP formulations -, "
            "the value of the linear relaxation, and the MIP optimality gap; (3) show a more detailed report about the "
            "optimization process. The possible status are ERROR, UNKNOWN, SUBOPTIMAL, OPTIMAL, INFEASIBLE, UNBOUNDED, "
            "INF_OR_UNBD. All values are separated by a single blank space. If some information is not available, a "
            "question mark is printed in its place).",
             cxxopts::value<int>()->default_value("1")->implicit_value("1"),"VALUE")

            ("s,solution", "Display the best solution found.",
             cxxopts::value<bool>(), "");

    options.add_options("General")
            ("a,algorithm", "Algorithm used to solve the problem (values: \"mip-precedence\", \"mip-linear-ordering\", "
            "\"mip-arc-time-indexed\", \"greedy\", \"neh\", \"ils\").",
             cxxopts::value<std::string>(), "VALUE")

            ("time-limit", "Limit the total time expended (in seconds).",
             cxxopts::value<double>()->default_value("1e100"), "VALUE")

            ("iterations-limit", "Limit the total number of iterations expended.",
             cxxopts::value<long>()->default_value(std::to_string(std::numeric_limits<long>::max())), "VALUE")

            ("seed", "Set the seed used to initialize the random number generator.",
             cxxopts::value<int>()->default_value("0"), "VALUE")

            ("threads", "Number of threads to be used (if the algorithms is able to use multithreading). If  set to 0 "
            "(zero), all threads available are used.",
             cxxopts::value<int>()->default_value("1"), "VALUE");

    options.add_options("MIP formulations")
            ("warm-start", "If set, the solver will use the solution found by the greedy heuristic as starting solution.",
             cxxopts::value<bool>(), "");

    options.add_options("Local search")
            ("local-search-method", "Method used to perform local search. Available values are \"vnd\" and \"rvnd\".",
             cxxopts::value<std::string>()->default_value("vnd"), "VALUE");

    options.add_options("ILS")
            ("perturbation-passes-limit", "The highest value of perturbation strength. If no improvement is found after "
            "a perturbation with this strength, the VNS/ILS stops.",
             cxxopts::value<long>()->default_value("5"), "VALUE");

    options.parse(argc, argv);
    return options;
}
