#include "ils.h"

#include <algorithm>
#include <limits>
#include <list>

#include <cxxtimer.hpp>

#include "greedy.h"
#include "../../util/common.h"
#include "../../util/local_search.h"
#include "../../neighborhood/shift.h"
#include "../../neighborhood/exchange.h"
#include "../../neighborhood/reassignment.h"
#include "../../neighborhood/swap.h"
#include "../../neighborhood/direct_swap.h"


std::tuple<orcs::Schedule, double> orcs::ILS::solve(const Problem& problem,
        const cxxproperties::Properties* opt_input, cxxproperties::Properties* opt_output) {

    // Algorithm parameters
    cxxproperties::Properties opt_aux;
    if (opt_input == nullptr) {
        opt_input = &opt_aux;
    }

    const bool verbose = opt_input->get<bool>("verbose", false);
    const unsigned seed = opt_input->get<unsigned>("seed", 0);
    const double time_limit = opt_input->get<double>("time-limit", std::numeric_limits<double>::max());
    const long iterations_limit = opt_input->get<long>("iterations-limit", std::numeric_limits<long>::max());
    const long perturbation_passes_limit = opt_input->get<long>("perturbation-passes-limit", 5);
    const std::string local_search_method = opt_input->get<std::string>("local-search-method", "vnd"); // vnd, rvnd
    const std::string local_search_approach = opt_input->get<std::string>("local-search-approach", "best"); // first, best

    // Initialize the random number generator
    std::mt19937 generator;
    generator.seed(seed);

    // Local search method
    bool randomized_vnd = local_search_method.compare("rvnd") == 0;

    // Define the list of neighborhoods used by the VND
    std::list<Neighborhood*> neighborhoods = {
            new Shift(),
            new Exchange(),
            new Reassignment(),
            new DirectSwap(),
            new Swap()
    };

    // Initialize a timer
    cxxtimer::Timer timer;
    timer.start();

    // Log: header
    log_header(verbose);

    // Build a start solution with a greedy heuristic
    auto [start_schedule, start_makespan] = Greedy().solve(problem);
    auto start = std::make_tuple(start_schedule, common::evaluate(problem, start_schedule));

    // Log the initial solution (before LS)
    log_start(std::get<1>(start), timer.count<std::chrono::milliseconds>() / 1000.0, verbose);

    // Find a local optimum from the start solution
    auto incumbent = randomized_vnd ? local_search::rvnd(problem, start, neighborhoods, &generator) :
                     local_search::vnd(problem, start, neighborhoods);

    // Log the initial solution (after LS)
    log_iteration(0L, std::get<1>(start), std::get<1>(start), std::get<1>(incumbent),
                  timer.count<std::chrono::milliseconds>() / 1000.0, verbose);

    // Start the iterative process
    long iteration = 0;
    long perturbation_passes = 1;
    long iteration_last_improvement = 0;

    while (iteration < iterations_limit &&
           timer.count<std::chrono::seconds>() < time_limit &&
           perturbation_passes <= perturbation_passes_limit) {

        // Increment the iteration counter
        ++iteration;

        // Perturbation phase
        auto perturbed = perturb(problem, incumbent, generator);
        for (long i = 1; i < perturbation_passes; ++i) {
            perturbed = perturb(problem, perturbed, generator);
        }

        // Local search
        auto trial = randomized_vnd ? local_search::rvnd(problem, start, neighborhoods, &generator) :
                     local_search::vnd(problem, start, neighborhoods);

        // Log: status at current iteration
        log_iteration(iteration, std::get<1>(incumbent), std::get<1>(perturbed),
                      std::get<1>(trial), timer.count<std::chrono::milliseconds>() / 1000.0, verbose);

        // Check for improvements
        if (common::less(std::get<1>(trial), std::get<1>(incumbent))) {
            incumbent = std::move(trial);
            iteration_last_improvement = iteration;

            // Reset the perturbation level
            perturbation_passes = 1;

        } else {

            // Increase the perturbation level
            ++perturbation_passes;
        }
    }

    // Stop timer
    timer.stop();

    // Log: footer
    log_footer(verbose);

    // Store optional output
    if (opt_output != nullptr) {
        opt_output->add("Iterations", iteration);
        opt_output->add("Runtime (s)", timer.count<std::chrono::milliseconds>() / 1000.0);
        opt_output->add("Start solution", start_makespan);
        opt_output->add("Iteration of last improvement", iteration_last_improvement);
    }

    // Deallocate resources
    for (auto ptr : neighborhoods) {
        delete ptr;
    }

    // Return the solution built
    return {std::get<0>(incumbent), std::get<0>(std::get<1>(incumbent))};
}

std::tuple<orcs::Schedule, std::tuple<double, double> > orcs::ILS::perturb(const Problem& problem, const std::tuple<orcs::Schedule, std::tuple<double, double> >& entry, std::mt19937& generator) {

    // Create a copy of the original entry
    auto perturbed = entry;
    auto& [schedule, evaluation] = perturbed;

    // Data structures used by the perturbation method
    std::vector<int> indexes;
    std::vector<int> chain;

    // Initialize and shuffle the chain (order of teams to perform the ejection chain)
    for (int l = 1; l <= problem.m; ++l) {
        chain.push_back(l);
    }

    std::shuffle(chain.begin(), chain.end(), generator);

    // Perform a chain of reassignment moves
    for (int idx = 0; idx < chain.size(); ++idx) {

        // Get the pair of teams to perform the reassignment move
        int l_origin = chain[idx];
        int l_target = chain[(idx + 1) % chain.size()];

        if (!schedule[l_origin].empty()) {

            // Choose a switch operation to reassign
            int idx_origin = generator() % schedule[l_origin].size();
            int operation = schedule[l_origin][idx_origin];
            schedule[l_origin].erase(schedule[l_origin].begin() + idx_origin);

            // Fill the possible indexes
            indexes.clear();
            for (int idx = 0; idx <= schedule[l_target].size(); ++idx) {
                indexes.push_back(idx);
            }

            std::shuffle(indexes.begin(), indexes.end(), generator);

            // Try to perform the movement
            bool success = false;
            for (auto idx_target : indexes) {

                // Perform the movement
                schedule[l_target].insert(schedule[l_target].begin() + idx_target, operation);

                // Evaluate the movement
                auto current_evaluation = orcs::common::evaluate(problem, schedule);

                // Check for feasibility
                if (std::get<0>(current_evaluation) != std::numeric_limits<double>::infinity()) {
                    evaluation = std::move(current_evaluation);
                    success = true;
                    break;

                } else {

                    // Undo the movement
                    schedule[l_target].erase(schedule[l_target].begin() + idx_target);
                }
            }

            // Undo the movement, if not feasible
            if (!success) {
                schedule[l_origin].insert(schedule[l_origin].begin() + idx_origin, operation);
            }

        }
    }

    return perturbed;
}

void orcs::ILS::log_header(bool verbose) {
    if (verbose) {
        std::printf("---------------------------------------------------------------------\n");
        std::printf("| Iter. |   Before LS  |   After LS   |   Incumbent  |   Time (s)   |\n");
        std::printf("---------------------------------------------------------------------\n");
    }
}

void orcs::ILS::log_start(const std::tuple<double, double>& start, double time, bool verbose) {
    if (verbose) {
        std::printf("| Start | %12s | %12s | %12.3lf | %12.3lf |\n",
                    "---", "---", std::get<0>(start), time);
    }
}

void orcs::ILS::log_footer(bool verbose) {
    if (verbose) {
        std::printf("---------------------------------------------------------------------\n");
    }
}

void orcs::ILS::log_iteration(long iteration, const std::tuple<double, double>& incumbent,
                              const std::tuple<double, double>& before_ls,
                              const std::tuple<double, double>& after_ls, double time, bool verbose) {

    if (verbose) {
        bool better_makespan = common::less(std::get<0>(after_ls), std::get<0>(incumbent));
        bool better_sum_completions = common::less(std::get<1>(after_ls), std::get<1>(incumbent));
        std::string status = (better_makespan ? "*" : (better_sum_completions ? "+" : " "));
        std::printf("| %s%4ld | %12.3lf | %12.3lf | %12.3lf | %12.3lf |\n",
                    status.c_str(), iteration, std::get<0>(before_ls), std::get<0>(after_ls),
                    (better_makespan ? std::get<0>(after_ls) : std::get<0>(incumbent)), time);
    }
}
