#ifndef CADMIUM_CELLDEVS_DRUGGIST_CELL_HPP_
#define CADMIUM_CELLDEVS_DRUGGIST_CELL_HPP_

#include <random>
#include <nlohmann/json.hpp>
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>
#include "crimedrugsState.hpp"

using namespace cadmium::celldevs;

// Model 1 - Druggist: stage 0 (normal) <-> stage 1 (lrp / drug user)
class druggistCell : public GridCell<crimedrugsState, double> {
public:
    druggistCell(const coordinates& id,
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

        if (state.stage == 0) {
            // R1: all neighbours are drug users
            if (total == 4 && lrp_count == 4) {
                state.stage = 1;
            }
            // R2: random adoption (~30% chance)
            else if (uniform(rng) < 0.30) {
                state.stage = 1;
            }
        } else {
            // Recovery: ~15% chance per step
            if (uniform(rng) < 0.15) {
                state.stage = 0;
            }
        }

        return state;
    }

    [[nodiscard]] double outputDelay(const crimedrugsState& state) const override {
        return 1.0;
    }
};

#endif // CADMIUM_CELLDEVS_DRUGGIST_CELL_HPP_
