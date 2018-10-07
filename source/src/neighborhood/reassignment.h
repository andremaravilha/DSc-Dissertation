#ifndef MANEUVER_SCHEDULING_NEIGHBORHOOD_REASSIGNMENT_H
#define MANEUVER_SCHEDULING_NEIGHBORHOOD_REASSIGNMENT_H

#include <tuple>
#include <vector>

#include "neighborhood.h"


namespace orcs {

    using ReassignmentMove = std::tuple<int, int, int, int>;

    /**
     * Reassignment neighborhood.
     */
    class Reassignment : public Neighborhood {

    public:

        std::tuple< Schedule, std::tuple<double, double> > best(const Problem&
                problem, const std::tuple< Schedule, std::tuple<double, double> >& entry) override;

        std::tuple< Schedule, std::tuple<double, double> > any(const Problem& problem,
                const std::tuple< Schedule, std::tuple<double, double> >& entry, std::mt19937& generator,
                bool feasible_only = true) override;

    };

}


#endif
