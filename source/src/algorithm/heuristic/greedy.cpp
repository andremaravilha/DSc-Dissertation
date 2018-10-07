#include "greedy.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>


std::tuple<orcs::Schedule, double> orcs::Greedy::solve(const Problem& problem,
        const cxxproperties::Properties* opt_input, cxxproperties::Properties* opt_output) {

    // Create an empty schedule
    Schedule schedule = create_empty_schedule(problem.m);
    double makespan = 0.0;

    // Initialize the heuristic data
    std::set<int> S_manual;
    std::set<int> S_remote;
    std::vector<double> t(problem.n + 1, 0.0);
    std::vector<int> gamma(problem.n + 1, 0);
    std::vector<int> phi(problem.m + 1, 0);

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

                    t[*j] = 0;
                    for (auto i : problem.predecessors[*j]) {
                        t[*j] = std::max(t[*j], t[i] + problem.p[i]);
                    }

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
            double criterion = std::numeric_limits<double>::max();
            int j, l;

            for (auto j_trial : S_manual) {
                if (gamma[j_trial] == 0) {
                    for (int l_trial = 1; l_trial <= problem.m; ++l_trial) {
                        double criterion_trial = t[phi[l_trial]] + problem.p[phi[l_trial]] + problem.s[phi[l_trial]][j_trial][l_trial];
                        if (criterion_trial < criterion) {
                            criterion = criterion_trial;
                            j = j_trial;
                            l = l_trial;
                        }
                    }
                }
            }

            // Compute the moment in which the  maneuver will be performed
            t[j] = t[phi[l]] + problem.p[phi[l]] + problem.s[phi[l]][j][l];
            for (auto i : problem.predecessors[j]) {
                t[j] = std::max(t[j], t[i] + problem.p[i]);
            }

            // Update the counter of predecessors not scheduled
            for (auto i : problem.successors[j]) {
                --gamma[i];
            }

            // Update team's data
            schedule[l].push_back(j);
            phi[l] = j;

            // Update the makespan
            makespan = std::max(makespan, t[j] + problem.p[j]);

            // Remove the switch from the set of unscheduled ones
            S_manual.erase(j);
        }
    }

    // Return the solution
    return {schedule, makespan};
}
