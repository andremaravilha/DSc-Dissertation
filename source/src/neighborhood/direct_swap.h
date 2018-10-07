#ifndef MANEUVER_SCHEDULING_NEIGHBORHOOD_DIRECT_SWAP_H
#define MANEUVER_SCHEDULING_NEIGHBORHOOD_DIRECT_SWAP_H

#include "neighborhood.h"


namespace orcs {

    using DirectSwapMove = std::tuple<int, int, int, int>;

    /**
     * Direct Swap neighborhood.
     */
    class DirectSwap : public Neighborhood {

    public:

        std::tuple< Schedule, std::tuple<double, double> > best(const Problem&
                problem, const std::tuple< Schedule, std::tuple<double, double> >& entry) override;

        std::tuple< Schedule, std::tuple<double, double> > any(const Problem& problem,
                const std::tuple< Schedule, std::tuple<double, double> >& entry, std::mt19937& generator,
                bool feasible_only = true) override;

    };

}


#endif
