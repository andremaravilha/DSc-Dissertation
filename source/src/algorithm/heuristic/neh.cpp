#include "neh.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

#include "../../util/common.h"


std::tuple<orcs::Schedule, double> orcs::NEH::solve(const Problem& problem,
        const cxxproperties::Properties* opt_input, cxxproperties::Properties* opt_output) {

    // Create an empty schedule
    Schedule schedule = create_empty_schedule(problem.m);

    // Initialize the heuristic data
    std::set<int> S_manual;
    std::set<int> S_remote;
    std::vector<int> gamma(problem.n + 1, 0);

    for (int i = 1; i <= problem.n; ++i) {
        gamma[i] = problem.predecessors[i].size();
        if (problem.technology[i] == Technology::MANUAL) {
            S_manual.insert(i);
        } else if (problem.technology[i] == Technology::REMOTE) {
            S_remote.insert(i);
        }
    }

    // Assignment and sequencing
    while (S_manual.size() + S_remote.size() > 0) {

        // Remotely controlled switches
        bool stop = false;
        while (!stop) {
            stop = true;
            auto j = S_remote.begin();
            while (j != S_remote.end()) {

                if (gamma[*j] == 0) {

                    for (auto i : problem.successors[*j]) {
                        --gamma[i];
                    }

                    schedule[0].push_back(*j);
                    j = S_remote.erase(j);
                    stop = false;

                } else {
                    ++j;
                }
            }
        }

        // Manually controlled switches
        if (S_manual.size() > 0) {

            // Choose a switch and a maintenance team
            double best_objective = std::numeric_limits<double>::infinity();
            int best_j, best_l, best_idx;

            for (auto j_trial : S_manual) {
                if (gamma[j_trial] == 0) {
                    for (int l_trial = 1; l_trial <= problem.m; ++l_trial) {
                        for (int idx_trial = 0; idx_trial <= schedule[l_trial].size(); ++idx_trial) {

                            schedule[l_trial].insert(schedule[l_trial].begin() + idx_trial, j_trial);

                            std::vector<double> t = problem.start_time(schedule);
                            double trial_objective = 0.0;
                            for (const auto& schd : schedule) {
                                for (auto aux_j : schd) {
                                    trial_objective = std::max(trial_objective, t[aux_j] + problem.p[aux_j]);
                                }
                            }

                            if (common::less(trial_objective, best_objective)) {
                                best_objective = trial_objective;
                                best_j = j_trial;
                                best_l = l_trial;
                                best_idx = idx_trial;
                            }

                            schedule[l_trial].erase(schedule[l_trial].begin() + idx_trial);
                        }
                    }
                }
            }

            // Update the counter of predecessors not scheduled
            for (auto i : problem.successors[best_j]) {
                --gamma[i];
            }

            // Update team's data
            schedule[best_l].insert(schedule[best_l].begin() + best_idx, best_j);

            // Remove the switch from the set of unscheduled ones
            S_manual.erase(best_j);
        }
    }

    // Return the solution
    return {schedule, problem.makespan(schedule)};
}
