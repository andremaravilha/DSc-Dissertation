#include "shift.h"


std::tuple< orcs::Schedule, std::tuple<double, double> > orcs::Shift::best(const Problem& problem,
        const std::tuple< Schedule, std::tuple<double, double> >& entry) {

    // Get the start solution
    const auto& [start_schedule, start_eval] = entry;

    // Keep the best neighbor
    auto [best_schedule, best_eval] = entry;

    // Evaluate all neighbors
    for (int l = 0; l <= problem.m; ++l) {
        for (int idx_origin = 0; idx_origin < start_schedule[l].size(); ++idx_origin) {
            for (int idx_target = 0; idx_target <= start_schedule[l].size() - 1; ++idx_target) {
                if (idx_target != idx_origin) {

                    // Build a neighbor
                    Schedule neighbor_schedule = start_schedule;
                    int i = neighbor_schedule[l][idx_origin];
                    neighbor_schedule[l].erase(neighbor_schedule[l].begin() + idx_origin);
                    neighbor_schedule[l].insert(neighbor_schedule[l].begin() + idx_target, i);

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

    // Return the best neighbor
    return {best_schedule, best_eval};
}

std::tuple< orcs::Schedule, std::tuple<double, double> > orcs::Shift::any(const Problem& problem,
        const std::tuple< Schedule, std::tuple<double, double> >& entry, std::mt19937& generator,
        bool feasible_only) {

    // Get the start solution
    const auto& [start_schedule, start_eval] = entry;

    bool success = false;
    while (!success) {

        // Get a move
        int l = generator() % (problem.m + 1);
        while (start_schedule[l].size() < 2) {
            l = generator() % (problem.m + 1);
        }

        int idx_origin = generator() % start_schedule[l].size();
        int idx_target = generator() % start_schedule[l].size();
        while (idx_target == idx_origin) {
            idx_target = generator() % start_schedule[l].size();
        }

        // Build the neighbor
        Schedule neighbor_schedule = start_schedule;
        int i = neighbor_schedule[l][idx_origin];
        neighbor_schedule[l].erase(neighbor_schedule[l].begin() + idx_origin);
        neighbor_schedule[l].insert(neighbor_schedule[l].begin() + idx_target, i);

        // Evaluate the neighbor
        auto neighbor_eval = orcs::common::evaluate(problem, neighbor_schedule);
        success = std::get<0>(neighbor_eval) != std::numeric_limits<double>::infinity();

        // Discard the neighbor, if it is infeasible and feasibility is required
        if (!feasible_only || success) {
            return {neighbor_schedule, neighbor_eval};
        }
    }
}
