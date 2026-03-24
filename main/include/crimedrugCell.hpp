#ifndef CADMIUM_CELLDEVS_CRIMEDRUG_CELL_HPP_
#define CADMIUM_CELLDEVS_CRIMEDRUG_CELL_HPP_

#include <random>
#include <nlohmann/json.hpp>
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>
#include "crimedrugsState.hpp"

using namespace cadmium::celldevs;

class crimedrugCell : public GridCell<crimedrugsState, double> {
public:
    crimedrugCell(const coordinates& id,
                  const std::shared_ptr<const GridCellConfig<crimedrugsState, double>>& config)
        : GridCell<crimedrugsState, double>(id, config) {}

    [[nodiscard]] crimedrugsState localComputation(
        crimedrugsState state,
        const std::unordered_map<coordinates, NeighborData<crimedrugsState, double>>& neighborhood
    ) const override {
        static thread_local std::mt19937 rng(std::random_device{}());
        static thread_local std::normal_distribution<double> norm_lrp(0.4, 0.3);
        static thread_local std::normal_distribution<double> norm_crime(0.4, 0.1);
        static thread_local std::normal_distribution<double> norm_incap(0.4, 0.3);

        // Snapshot for simultaneous-update semantics.
        const int orig_lrp   = state.lrp;
        const int orig_hrp   = state.hrp;
        const int orig_crime = state.crime;

        // Aggregate neighbourhood statistics.
        int total = 0, lrp_count = 0;
        bool any_hrp2 = false;
        for (const auto& [nId, nData] : neighborhood) {
            if (nId == cellId) continue;
            ++total;
            if (nData.state->lrp == 1)  ++lrp_count;
            if (nData.state->hrp == 2)  any_hrp2 = true;
        }
        const bool all_neighbours_lrp = (total == 4 && lrp_count == 4);

        // --- lrp layer (LRP-transition) ---
        if (orig_lrp == 0) {
            if (all_neighbours_lrp) {
                state.lrp = 1;                    // R1
            } else if (norm_lrp(rng) > 0.6) {
                state.lrp = 1;                    // R2
            }
        }
        // lrp=1 is absorbing – no action needed.

        // --- hrp layer (HRP-transition) ---
        if (state.hrp == 0) {
            // Requires this person to be LRP AND all neighbours to be LRP.
            if (orig_lrp == 1 && all_neighbours_lrp) {
                state.hrp = 2;                    // R1
            }
        }
        // hrp=2 is absorbing.

        // --- crime layer (crimedrug-transition) ---
        if (orig_crime == 0) {
            if (norm_crime(rng) > 0.6) {
                state.crime = 3;                  // R1: random crime
            } else if (orig_hrp == 2 && any_hrp2) {
                state.crime = 3;                  // R2: peer influence
            }
        }
        // crime=3 is absorbing.

        // --- incap layer (Incapacitation-transition) ---
        if (state.incap == 0) {
            // Requires being a criminal AND an HRP AND probabilistic incapacitation.
            if (orig_crime == 3 && orig_hrp == 2 && norm_incap(rng) > 0.1) {
                state.incap = 4;                  // R1
            }
        }
        // incap=4 is absorbing.

        return state;
    }

    [[nodiscard]] double outputDelay(const crimedrugsState& state) const override {
        return 1.0;
    }
};

#endif // CADMIUM_CELLDEVS_CRIMEDRUG_CELL_HPP_
