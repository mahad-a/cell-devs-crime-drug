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

        // Count von-Neumann neighbours (excluding self) that are druggists.
        int total = 0, lrp_count = 0;
        for (const auto& [nId, nData] : neighborhood) {
            ++total;
            if (nData.state->lrp == 1) ++lrp_count;
        }

        // R1: neighbourhood influence (all 4 neighbours must be druggists)
        if (state.lrp == 0 && total == 4 && lrp_count == 4) {
            state.lrp = 1;
        }
        // R2: random adoption
        else if (norm(rng) > 0.7) {
            state.lrp = 1;
        }
        // R3 / default: lrp=0 stays 0 – no action needed.

        // Recovery: LRP person probabilistically quits drug use.
        static thread_local std::uniform_real_distribution<double> uniform(0.0, 1.0);
        if (state.lrp == 1 && uniform(rng) < 0.15) {
            state.lrp = 0;
        }

        return state;
    }

    [[nodiscard]] double outputDelay(const crimedrugsState& state) const override {
        return 1.0;
    }
};

#endif // CADMIUM_CELLDEVS_DRUGGIST_CELL_HPP_
