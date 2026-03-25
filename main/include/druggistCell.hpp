#ifndef CADMIUM_CELLDEVS_DRUGGIST_CELL_HPP_
#define CADMIUM_CELLDEVS_DRUGGIST_CELL_HPP_

#include <random>
#include <nlohmann/json.hpp>
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>
#include "crimedrugsState.hpp"

using namespace cadmium::celldevs;

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
        static thread_local std::normal_distribution<double> norm(0.4, 0.3);

        int total = 0, lrp_count = 0;
        for (const auto& [nId, nData] : neighborhood) {
            ++total;
            if (nData.state->lrp == 1) ++lrp_count;
        }

        // R1: all 4 neighbours are druggists
        if (state.lrp == 0 && total == 4 && lrp_count == 4) {
            state.lrp = 1;
        }
        // R2: random adoption
        else if (state.lrp == 0 && norm(rng) > 0.7) {
            state.lrp = 1;
        }

        return state;
    }

    [[nodiscard]] double outputDelay(const crimedrugsState& state) const override {
        return 1.0;
    }
};

#endif // CADMIUM_CELLDEVS_DRUGGIST_CELL_HPP_
