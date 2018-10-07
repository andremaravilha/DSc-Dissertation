#include "problem.h"

#include <cmath>
#include <fstream>
#include <iostream>

#include "../util/common.h"


orcs::Problem::Problem(const std::string& filename) {

    // Open file
    std::ifstream file(filename.c_str());
    std::string token;

    // Read the problem size
    file >> n;     // Number of maneuvers
    file >> m;     // Number of teams
    file >> token; // Density of the precedence graph

    // Initialize the data structures
    technology = std::vector<Technology>(n + 1, Technology::UNKNOWN);
    predecessors = std::vector< std::set<int> >(n + 1, std::set<int>());
    successors = std::vector< std::set<int> >(n + 1, std::set<int>());
    precedence = std::vector< std::vector<bool> >(n + 1, std::vector<bool>(n + 1, false));
    p = std::vector<double>(n + 1, 0.0);
    s = std::vector< std::vector< std::vector<double> > >(n + 1, std::vector< std::vector<double> >(n + 1, std::vector<double>(m + 1, 0.0)));

    // Read switches data
    for (int i = 1; i <= n; ++i) {

        // Switch ID (just ignore it)
        file >> token;

        // Technology
        file >> token;
        technology[i] = Technology::UNKNOWN;
        if (token.compare("R") == 0) {
            technology[i] = Technology::REMOTE;
        } else if (token.compare("M") == 0) {
            technology[i] = Technology::MANUAL;
        }

        // Maneuver time (processing time)
        file >> token;
        p[i] = std::stod(token);

    }

    // Read precedence constraints
    for (std::size_t j = 1; j <= n; ++j) {

        // Switch ID (just ignore it)
        file >> token;

        // Number of predecessors
        file >> token;
        int nprec = std::stoi(token);

        // Predecessors and successors
        for (std::size_t count = 0; count < nprec; ++count) {

            // Read a predecessor of the current switch j
            file >> token;
            int i = std::stoi(token);

            predecessors[j].insert(i);
            successors[i].insert(j);
        }
    }

    // Read the travel time (setup time)
    for (int l = 1; l <= m; ++l) {
        for (int i = 0; i <= n; ++i) {
            for (int j = 0; j <= n; ++j) {
                file >> token;
                s[i][j][l] = std::stod(token);
            }
        }
    }

    // Compute the full precedence matrix
    std::vector<bool> processed(n + 1, false);
    std::set<int> pending;
    for (std::size_t j = 1; j <= n; ++j) {

        // Clear the data structures used
        std::fill(processed.begin(), processed.end(), false);
        pending.clear();

        // Initialize the data structures
        for (auto i : predecessors[j]) {
            pending.insert(i);
        }

        // Compute the precedence graph of j
        while (!pending.empty()) {

            // Get an element from the pending set
            auto iter = pending.begin();
            int i = *iter;
            pending.erase(iter);

            // Update the precedence matrix
            precedence[i][j] = true;

            // Check i as processed
            processed[i] = true;

            // Add the predecessors of i into the pending set
            for (auto k : predecessors[i]) {
                if (!processed[k]) {
                    pending.insert(k);
                }
            }
        }
    }

    // Close the file
    file.close();
}

double orcs::Problem::makespan(const Schedule &schedule) const {
    std::vector<double> t = start_time(schedule);
    double makespan = 0.0;
    for (int i = 1; i <= n; ++i) {
        makespan = std::max(makespan, t[i] + p[i]);
    }

    return makespan;
}

std::vector<double> orcs::Problem::start_time(const Schedule &schedule) const {

    // Start time of each task
    std::vector<double> t(n + 1, std::numeric_limits<double>::infinity());
    t[0] = 0; // teams/machines are available at moment 0

    // Auxiliary structures
    std::vector<int> index(m + 1, 0);       // next index to analyse of each schedule
    std::vector<int> location(m + 1, 0);    // current location of each team
    std::vector<int> pendings(n + 1, 0);    // number of pending predecessors switch operations

    for (int l = 0; l <= m; ++l) {
        for (int idx = 0; idx < schedule[l].size(); ++idx) {
            pendings[schedule[l][idx]] = predecessors[schedule[l][idx]].size();
        }
    }

    // Compute start times
    int count = 0;
    bool feasibility = true;
    while (count < n && feasibility) {

        feasibility = false;
        for (int l = 0; l <= m; ++l) {
            if (index[l] < schedule[l].size()) {

                // Get the switch/task
                int j = schedule[l][index[l]];

                if (pendings[j] == 0) {

                    // Get the current location of the team/machine
                    int i = location[l];

                    // Compute the start time
                    if (l != 0) {
                        t[j] = t[i] + p[i] + s[i][j][l];
                    } else {
                        t[j] = 0.0;
                    }

                    // Wait predecessor maneuvers
                    for (auto k : predecessors[j]) {
                        t[j] = std::max(t[j], t[k] + p[k]);
                    }

                    // Update the pending counters
                    for (auto k : successors[j]) {
                        --pendings[k];
                    }

                    // Update the index and team's location
                    ++index[l];
                    location[l] = j;

                    // Increment the number of switch operations analysed
                    ++count;

                    // The solution is feasible so far
                    feasibility = true;
                }
            }
        }
    }

    // Return the maneuver moments
    return t;
}

bool orcs::Problem::is_feasible(const Schedule &schedule, std::string *msg) const {

    // Check the number of teams
    if (schedule.size() != m + 1) {
        if (msg != nullptr) {
            *msg = "The number of maintenance teams is wrong.";
        }
        return false;
    }

    // Check if all switches have been assigned to one team only.
    std::vector<int> assignment(n + 1, 0);
    for (int l = 0; l <= m; ++l) {
        for (auto i : schedule[l]) {
            if (i < 1 || i > n) {
                if (msg != nullptr) {
                    *msg = "Using invalid switch ID.";
                }
                return false;
            } else {
                ++assignment[i];
            }
        }
    }

    for (int i = 1; i <= n; ++i) {
        if (assignment[i] != 1) {
            if (msg != nullptr) {
                *msg = "There are switches assigned to more than one team or not assigned to any team.";
            }
            return false;
        }
    }

    // Check if all remotely maneuverable switches has been assigned to the
    // dummy team 0
    for (auto i : schedule[0]) {
        if (technology[i] != Technology::REMOTE) {
            if (msg != nullptr) {
                *msg = "Non-remote controlled switch assigned to dummy team 0.";
            }
            return false;
        }
    }

    // Check if all manually maneuverable switches has been assigned to a team
    for (int l = 1; l <= m; ++l) {
        for (auto i : schedule[l]) {
            if (technology[i] != Technology::MANUAL) {
                if (msg != nullptr) {
                    *msg = "Non-manual controlled switch assigned to a maintenace team.";
                }
                return false;
            }
        }
    }

    // Check the precedence rules
    std::vector<double> t = start_time(schedule);
    for (int j = 1; j <= n; ++j) {
        for (auto i : predecessors[j]) {
            if (common::less(t[j], t[i])) {
                if (msg != nullptr) {
                    *msg = "Precedence rules violated.";
                }
                return false;
            }
        }
    }

    // All constraints are met
    if (msg != nullptr) {
        *msg = "Feasible solution.";
    }

    return true;
}

std::ostream& operator<<(std::ostream& os, const orcs::Schedule& schedule) {
    orcs::common::print_solution(os, schedule);
    return os;
}
