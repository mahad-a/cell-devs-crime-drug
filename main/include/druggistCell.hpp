#ifndef CADMIUM_CELLDEVS_DRUGGIST_CELL_HPP_
#define CADMIUM_CELLDEVS_DRUGGIST_CELL_HPP_

#include <cmath>
#include <random>
#include <nlohmann/json.hpp>
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>
#include "crimedrugsState.hpp"

using namespace cadmium::celldevs;

class druggistCell : public GridCell<crimedrugsState, double> {
    coordinates myId;

    // returns true if nId is an immediate (distance-1) von neumann neighbour, with toroidal wrap for the 20x20 grid
    bool isImmediate(const coordinates& nId) const {
        int dx = std::abs(nId[0] - myId[0]);
        int dy = std::abs(nId[1] - myId[1]);
        dx = std::min(dx, 20 - dx);
        dy = std::min(dy, 20 - dy);
        return (dx + dy) == 1;
    }

public:
    druggistCell(const coordinates& id,
                 const std::shared_ptr<const GridCellConfig<crimedrugsState, double>>& config)
        : GridCell<crimedrugsState, double>(id, config), myId(id) {}

    [[nodiscard]] crimedrugsState localComputation(
        crimedrugsState state,
        const std::unordered_map<coordinates, NeighborData<crimedrugsState, double>>& neighborhood
    ) const override {
        // per-thread rngs, initialized once per thread to avoid contention
        static thread_local std::mt19937 rng(std::random_device{}());
        static thread_local std::normal_distribution<double> norm(0.4, 0.3);
        static thread_local std::uniform_real_distribution<double> uniform(0.0, 1.0);

        int imm_total = 0, imm_lrp = 0;
        for (const auto& [nId, nData] : neighborhood) {
            if (!isImmediate(nId)) continue;
            ++imm_total;
            if (nData.state->lrp == 1) ++imm_lrp;
        }

        if (state.lrp == 0) {
            // R1: all 4 immediate neighbours are druggists
            if (imm_total == 4 && imm_lrp == 4) {
                state.lrp = 1;
            }
            // R2: random adoption
            else if (norm(rng) > 1.0) {
                state.lrp = 1;
            }
        } else {
            // Recovery
            if (uniform(rng) < 0.03) {
                state.lrp = 0;
            }
        }

        return state;
    }

    [[nodiscard]] double outputDelay(const crimedrugsState& state) const override {
        return 1.0;
    }
};

#endif // CADMIUM_CELLDEVS_DRUGGIST_CELL_HPP_
