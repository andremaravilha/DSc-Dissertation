#include "direct_swap.h"


std::tuple< orcs::Schedule, std::tuple<double, double> > orcs::DirectSwap::best(const Problem& problem,
        const std::tuple< Schedule, std::tuple<double, double> >& entry) {

    // Get the start solution
    const auto& [start_schedule, start_eval] = entry;

    // Keep the best neighbor
    auto [best_schedule, best_eval] = entry;

    // Evaluate all neighbors
    for (int l1 = 1; l1 <= problem.m; ++l1) {
        if (start_schedule[l1].size() > 0) {
            for (int l2 = l1 + 1; l2 <= problem.m; ++l2) {
                if (start_schedule[l2].size() > 0) {
                    for (int idx1 = 0; idx1 < start_schedule[l1].size(); ++idx1) {
                        for (int idx2 = 0; idx2 < start_schedule[l2].size(); ++idx2) {

                            // Build a neighbor
                            Schedule neighbor_schedule = start_schedule;
                            int i_1 = neighbor_schedule[l1][idx1];
                            int i_2 = neighbor_schedule[l2][idx2];
                            neighbor_schedule[l1][idx1] = i_2;
                            neighbor_schedule[l2][idx2] = i_1;

                            // Evaluate the neighbor
                            auto neighbor_eval = orcs::common::evaluate(problem, neighbor_schedule);

                            // Update the best neighbor
                            if (orcs::common::less(neighbor_eval, best_eval)) {
                                best_schedule = std::move(neighbor_schedule);
                                best_eval = std::move(neighbor_eval);
                            }
                        }
                    }
                }
            }
        }
    }

    // Return the best neighbor
    return {best_schedule, best_eval};
}

std::tuple< orcs::Schedule, std::tuple<double, double> > orcs::DirectSwap::any(const Problem& problem,
        const std::tuple< Schedule, std::tuple<double, double> >& entry, std::mt19937& generator,
        bool feasible_only) {

    // Get the start solution
    const auto& [start_schedule, start_eval] = entry;

    bool success = false;
    while (!success) {

        // Get a move
        int l1 = 1 + (generator() % problem.m);
        while (start_schedule[l1].size() < 1) {
            l1 = 1 + (generator() % problem.m);
        }

        int l2 = 1 + (generator() % problem.m);
        while (l2 == l1 || start_schedule[l2].size() < 1) {
            l2 = 1 + (generator() % problem.m);
        }

        int idx1 = generator() % start_schedule[l1].size();
        int idx2 = generator() % start_schedule[l2].size();

        // Build the neighbor
        Schedule neighbor_schedule = start_schedule;
        int i_1 = neighbor_schedule[l1][idx1];
        int i_2 = neighbor_schedule[l2][idx2];
        neighbor_schedule[l1][idx1] = i_2;
        neighbor_schedule[l2][idx2] = i_1;

        // Evaluate the neighbor
        auto neighbor_eval = orcs::common::evaluate(problem, neighbor_schedule);
        success = std::get<0>(neighbor_eval) != std::numeric_limits<double>::infinity();

        // Discard the neighbor, if it is infeasible and feasibility is required
        if (!feasible_only || success) {
            return {neighbor_schedule, neighbor_eval};
        }
    }
}
