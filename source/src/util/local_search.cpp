#include "local_search.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <vector>

#include "common.h"


std::tuple< orcs::Schedule, std::tuple<double, double> >
orcs::local_search::standard(const Problem& problem,
         const std::tuple< orcs::Schedule, std::tuple<double, double> >& entry,
         Neighborhood& neighborhood) {

    // Keep the best solution found
    auto incumbent = entry;

    // Perform the local search
    bool stop = false;
    while (!stop) {

        // Get the best neighbor solution
        auto trial = neighborhood.best(problem, incumbent);

        // Check for improvements
        if (common::less(std::get<1>(trial), std::get<1>(incumbent))) {
            incumbent = std::move(trial);
        } else {
            stop = true;
        }
    }

    // Return the incumbent solution
    return incumbent;
}

std::tuple< orcs::Schedule, std::tuple<double, double> >
orcs::local_search::vnd(const Problem& problem,
        const std::tuple< orcs::Schedule, std::tuple<double, double> >& entry,
        std::list<Neighborhood*>& neighborhoods) {

    // Keep the best solution found
    auto incumbent = entry;

    // Perform the local search
    auto k = neighborhoods.begin();
    while (k != neighborhoods.end()) {

        // Get the neighborhood
        Neighborhood* neighborhood = *k;

        // Get a neighbor
        auto trial = neighborhood->best(problem, incumbent);

        // Check for improvements
        if (common::less(std::get<1>(trial), std::get<1>(incumbent))) {

            // Update the incumbent solution
            incumbent = std::move(trial);

            // Go to the first neighborhood
            k = neighborhoods.begin();

        } else {

            // Go to the next neighborhood
            ++k;
        }
    }

    // Return the best solution found
    return incumbent;
}

std::tuple< orcs::Schedule, std::tuple<double, double> >
orcs::local_search::rvnd(const Problem& problem,
        const std::tuple< orcs::Schedule, std::tuple<double, double> >& entry,
        std::list<Neighborhood*>& neighborhoods,
        std::mt19937* generator) {

    // Random number generator
    std::mt19937 inner_generator(std::chrono::system_clock::now().time_since_epoch().count());
    if (generator == nullptr) {
        generator = &inner_generator;
    }

    // List of neighborhoods available
    std::vector<Neighborhood*> available_neighborhoods(neighborhoods.begin(), neighborhoods.end());

    // Keep the best solution found
    auto incumbent = entry;

    // Perform the local search
    while (!available_neighborhoods.empty()) {

        // Choose a neighborhood
        int idx = (*generator)() % available_neighborhoods.size();
        Neighborhood* neighborhood = *(available_neighborhoods.begin() + idx);
        available_neighborhoods.erase(available_neighborhoods.begin() + idx);

        // Get a neighbor
        auto trial = neighborhood->best(problem, incumbent);

        // Check for improvements
        if (common::less(std::get<1>(trial), std::get<1>(incumbent))) {

            // Update the incumbent solution
            incumbent = std::move(trial);

            // Reset the list of available neighborhoods
            available_neighborhoods.clear();
            available_neighborhoods.insert(available_neighborhoods.begin(), neighborhoods.begin(), neighborhoods.end());

        }
    }

    // Return the best solution found
    return incumbent;
}
