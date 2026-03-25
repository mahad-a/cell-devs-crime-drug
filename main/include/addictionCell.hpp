#ifndef CADMIUM_CELLDEVS_ADDICTION_CELL_HPP_
#define CADMIUM_CELLDEVS_ADDICTION_CELL_HPP_

#include <random>
#include <nlohmann/json.hpp>
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>
#include "crimedrugsState.hpp"

using namespace cadmium::celldevs;

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
        static thread_local std::normal_distribution<double> norm_lrp(0.4, 0.3);

        const int orig_lrp = state.lrp;

        int total = 0, lrp_count = 0;
        for (const auto& [nId, nData] : neighborhood) {
            ++total;
            if (nData.state->lrp == 1) ++lrp_count;
        }
        const bool all_neighbours_lrp = (total == 4 && lrp_count == 4);

        // lrp layer
        if (orig_lrp == 0) {
            if (all_neighbours_lrp) {
                state.lrp = 1;
            } else if (norm_lrp(rng) > 0.6) {
                state.lrp = 1;
            }
        }

        // hrp layer
        if (state.hrp == 0) {
            if (orig_lrp == 1 && all_neighbours_lrp) {
                state.hrp = 2;
            }
        }

        return state;
    }

    [[nodiscard]] double outputDelay(const crimedrugsState& state) const override {
        return 1.0;
    }
};

#endif // CADMIUM_CELLDEVS_ADDICTION_CELL_HPP_
