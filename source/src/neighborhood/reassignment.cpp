#include "reassignment.h"


std::tuple< orcs::Schedule, std::tuple<double, double> > orcs::Reassignment::best(const Problem& problem,
        const std::tuple< Schedule, std::tuple<double, double> >& entry) {

    // Get the start solution
    const auto& [start_schedule, start_eval] = entry;

    // Keep the best neighbor
    auto [best_schedule, best_eval] = entry;

    // Evaluate all neighbors
    for (int l_origin = 1; l_origin <= problem.m; ++l_origin) {
        for (int idx_origin = 0; idx_origin < start_schedule[l_origin].size(); ++idx_origin) {
            for (int l_target = 1; l_target <= problem.m; ++l_target) {
                if (l_target != l_origin) {
                    for (int idx_target = 0; idx_target <= start_schedule[l_target].size(); ++idx_target) {

                        // Build a neighbor
                        Schedule neighbor_schedule = start_schedule;
                        int i = neighbor_schedule[l_origin][idx_origin];
                        neighbor_schedule[l_origin].erase(neighbor_schedule[l_origin].begin() + idx_origin);
                        neighbor_schedule[l_target].insert(neighbor_schedule[l_target].begin() + idx_target, i);

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

    // Return the best neighbor
    return {best_schedule, best_eval};
}

std::tuple< orcs::Schedule, std::tuple<double, double> > orcs::Reassignment::any(const Problem& problem,
        const std::tuple< Schedule, std::tuple<double, double> >& entry, std::mt19937& generator,
        bool feasible_only) {

    // Get the start solution
    const auto& [start_schedule, start_eval] = entry;

    bool success = false;
    while (!success) {

        // Get a move
        int l_origin = 1 + (generator() % problem.m);
        while (start_schedule[l_origin].size() < 1) {
            l_origin = 1 + (generator() % problem.m);
        }

        int l_target = 1 + (generator() % problem.m);
        while (l_target == l_origin) {
            l_target = 1 + (generator() % problem.m);
        }

        int idx_origin = generator() % start_schedule[l_origin].size();
        int idx_target = 0;
        if (start_schedule[l_target].size() > 0) {
            idx_target = generator() % start_schedule[l_target].size();
        }

        // Build the neighbor
        Schedule neighbor_schedule = start_schedule;
        int i = neighbor_schedule[l_origin][idx_origin];
        neighbor_schedule[l_origin].erase(neighbor_schedule[l_origin].begin() + idx_origin);
        neighbor_schedule[l_target].insert(neighbor_schedule[l_target].begin() + idx_target, i);

        // Evaluate the neighbor
        auto neighbor_eval = orcs::common::evaluate(problem, neighbor_schedule);
        success = std::get<0>(neighbor_eval) != std::numeric_limits<double>::infinity();

        // Discard the neighbor, if it is infeasible and feasibility is required
        if (!feasible_only || success) {
            return {neighbor_schedule, neighbor_eval};
        }
    }
}
