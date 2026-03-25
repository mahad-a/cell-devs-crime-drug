#ifndef CADMIUM_CELLDEVS_CRIMEDRUG_CELL_HPP_
#define CADMIUM_CELLDEVS_CRIMEDRUG_CELL_HPP_

#include <cmath>
#include <random>
#include <nlohmann/json.hpp>
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>
#include "crimedrugsState.hpp"

using namespace cadmium::celldevs;

class crimedrugCell : public GridCell<crimedrugsState, double> {
    coordinates myId;

    // toroidal distance check on a 20x20 grid
    bool isImmediate(const coordinates& nId) const {
        int dx = std::abs(nId[0] - myId[0]);
        int dy = std::abs(nId[1] - myId[1]);
        dx = std::min(dx, 20 - dx);
        dy = std::min(dy, 20 - dy);
        return (dx + dy) == 1;
    }

public:
    crimedrugCell(const coordinates& id,
                  const std::shared_ptr<const GridCellConfig<crimedrugsState, double>>& config)
        : GridCell<crimedrugsState, double>(id, config), myId(id) {}

    [[nodiscard]] crimedrugsState localComputation(
        crimedrugsState state,
        const std::unordered_map<coordinates, NeighborData<crimedrugsState, double>>& neighborhood
    ) const override {
        // per-thread rngs, initialized once per thread to avoid contention
        static thread_local std::mt19937 rng(std::random_device{}());
        static thread_local std::normal_distribution<double> norm_lrp(0.4, 0.3);
        static thread_local std::normal_distribution<double> norm_incap(0.4, 0.3);

        const int orig_lrp   = state.lrp;
        const int orig_hrp   = state.hrp;
        const int orig_crime = state.crime;

        int imm_total = 0, imm_lrp = 0;
        bool any_hrp2 = false;
        // any_hrp2 scans the full extended neighbourhood (not just immediate),
        // so crime can be triggered by hrp influence from further away
        for (const auto& [nId, nData] : neighborhood) {
            if (nData.state->hrp == 2) any_hrp2 = true;
            if (!isImmediate(nId)) continue;
            ++imm_total;
            if (nData.state->lrp == 1) ++imm_lrp;
        }
        // lrp layer — spread if 2+ immediate neighbours are lrp, or random adoption
        if (orig_lrp == 0) {
            if (imm_lrp >= 2) {
                state.lrp = 1;
            } else if (norm_lrp(rng) > 1.0) {
                state.lrp = 1;
            }
        }
        // no recovery from lrp

        // hrp layer — escalate if already lrp and 2+ immediate neighbours are also lrp
        if (state.hrp == 0) {
            if (orig_lrp == 1 && imm_lrp >= 2) {
                state.hrp = 2;
            }
        }
        // no recovery from hrp

        // crime layer — only hrp cells can escalate; trigger if any extended neighbour is hrp
        if (orig_crime == 0) {
            if (orig_hrp == 2 && any_hrp2) {
                state.crime = 3;
            }
        }
        // no recovery from crime

        // incap layer — crime+hrp cell escalates to incapacitation
        if (state.incap == 0) {
            if (orig_crime == 3 && orig_hrp == 2 && norm_incap(rng) > 0.1) {
                state.incap = 4;
            }
        }
        // no recovery from incapacitation

        return state;
    }

    [[nodiscard]] double outputDelay(const crimedrugsState& state) const override {
        return 1.0;
    }
};

#endif // CADMIUM_CELLDEVS_CRIMEDRUG_CELL_HPP_
