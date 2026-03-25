#ifndef CADMIUM_CELLDEVS_ADDICTION_CELL_HPP_
#define CADMIUM_CELLDEVS_ADDICTION_CELL_HPP_

#include <random>
#include <nlohmann/json.hpp>
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>
#include "crimedrugsState.hpp"

using namespace cadmium::celldevs;

// Model 2 - Addiction: stage 0 (normal) -> 1 (lrp) -> 2 (hrp/addicted), with recovery
class addictionCell : public GridCell<crimedrugsState, double> {
public:
    addictionCell(const coordinates& id,
                  const std::shared_ptr<const GridCellConfig<crimedrugsState, double>>& config)
        : GridCell<crimedrugsState, double>(id, config) {}

    [[nodiscard]] crimedrugsState localComputation(
        crimedrugsState state,
        const std::unordered_map<coordinates, NeighborData<crimedrugsState, double>>& neighborhood
    ) const override {
        static thread_local std::mt19937 rng(std::random_device{}());
        static thread_local std::uniform_real_distribution<double> uniform(0.0, 1.0);

        int total = 0, lrp_count = 0;
        for (const auto& [nId, nData] : neighborhood) {
            ++total;
            if (nData.state->stage >= 1) ++lrp_count;
        }
        const bool all_lrp = (total == 4 && lrp_count == 4);

        if (state.stage == 0) {
            // R1: neighbourhood spread to lrp
            if (all_lrp) {
                state.stage = 1;
            }
            // R2: random adoption (~25% chance)
            else if (uniform(rng) < 0.25) {
                state.stage = 1;
            }
        } else if (state.stage == 1) {
            // R1: escalate to hrp when self and all neighbours are lrp
            if (all_lrp) {
                state.stage = 2;
            }
            // Recovery: ~10% chance to return to normal
            else if (uniform(rng) < 0.10) {
                state.stage = 0;
            }
        } else {
            // stage == 2 (hrp): recovery to lrp (~15% chance)
            if (uniform(rng) < 0.15) {
                state.stage = 1;
            }
        }

        return state;
    }

    [[nodiscard]] double outputDelay(const crimedrugsState& state) const override {
        return 1.0;
    }
};

#endif // CADMIUM_CELLDEVS_ADDICTION_CELL_HPP_
