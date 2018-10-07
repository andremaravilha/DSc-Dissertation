#include "common.h"

#include <algorithm>
#include <cmath>
#include <vector>


int orcs::common::compare(double first, double second) {
    return std::abs(first - second) < THRESHOLD ? 0 : (first < second ? -1 : 1);
}

bool orcs::common::equal(double first, double second) {
    return compare(first, second) == 0;
}

bool orcs::common::greater(double first, double second) {
    return compare(first, second) == 1;
}

bool orcs::common::less(double first, double second) {
    return compare(first, second) == -1;
}

bool orcs::common::greater_or_equal(double first, double second) {
    return compare(first, second) != -1;
}

bool orcs::common::less_or_equal(double first, double second) {
    return compare(first, second) != 1;
}

std::tuple<double, double> orcs::common::evaluate(const Problem& problem, const Schedule& schedule) {

    // Makespan and sum of completions
    double makespan = 0.0;
    double sum_completions = 0.0;

    // Calculate maneuver moments
    std::vector<double> t = problem.start_time(schedule);

    // Check feasibility
    bool feasible = std::all_of(t.begin(), t.end(), [](double value) {
        return value != std::numeric_limits<double>::infinity();
    });

    // Calculate global makespan and sum of machines' makespan
    for (int l = 1; l <= problem.m; ++l) {
        if (!schedule[l].empty()) {
            int i = schedule[l][schedule[l].size() - 1];
            makespan = std::max(makespan, t[i] + problem.p[i]);
            sum_completions += t[i] + problem.p[i];
        }
    }

    for (const auto& i : schedule[0]) {
        makespan = std::max(makespan, t[i] + problem.p[i]);
    }

    return {makespan, sum_completions};
}

void orcs::common::print_solution(std::ostream& os, const Schedule& schedule) {

    // Print remotely maneuverable switches
    os << "REMOTE : [";
    if (!schedule[0].empty()) {
        for (int idx = 0; idx < schedule[0].size() - 1; ++idx) {
            os << format("%d, ", schedule[0][idx]);
        }
        os << format("%d, ", schedule[0][schedule[0].size() - 1]);
    }
    os << "]" << std::endl;

    // Print manually maneuverable switches
    for (int l = 1; l < schedule.size(); ++l) {
        os << "TEAM " << l << " : [";
        if (!schedule[l].empty()) {
            for (int idx = 0; idx < schedule[l].size() - 1; ++idx) {
                os << format("%d, ", schedule[l][idx]);
            }
            os << format("%d, ", schedule[l][schedule[l].size() - 1]);
        }
        os << "]" << std::endl;
    }
}

void orcs::common::print_solution(std::ostream& os, const Schedule& schedule, const Problem& problem) {

    // Calculate maneuver moments
    std::vector<double> t = problem.start_time(schedule);

    // Print remotely maneuverable switches
    os << "REMOTE : [";
    if (!schedule[0].empty()) {
        for (int idx = 0; idx < schedule[0].size() - 1; ++idx) {
            os << format("%d (%.2lf), ", schedule[0][idx], t[schedule[0][idx]]);
        }
        os << format("%d (%.2lf), ", schedule[0][schedule[0].size() - 1], t[schedule[0][schedule[0].size() - 1]]);
    }
    os << "]" << std::endl;

    // Print manually maneuverable switches
    for (int l = 1; l < schedule.size(); ++l) {
        os << "TEAM " << l << " : [";
        if (!schedule[l].empty()) {
            for (int idx = 0; idx < schedule[l].size() - 1; ++idx) {
                os << format("%d (%.2lf), ", schedule[l][idx], t[schedule[l][idx]]);
            }
            os << format("%d (%.2lf), ", schedule[l][schedule[l].size() - 1], t[schedule[l][schedule[l].size() - 1]]);
        }
        os << "]" << std::endl;
    }
}
