#ifndef CADMIUM_CELLDEVS_CRIMEDRUG_CELL_HPP_
#define CADMIUM_CELLDEVS_CRIMEDRUG_CELL_HPP_

#include <random>
#include <nlohmann/json.hpp>
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>
#include "crimedrugsState.hpp"

using namespace cadmium::celldevs;

// Model 3 - CrimeDrug: 0=normal, 1=lrp, 2=hrp, 3=criminal, 4=incapacitated
// Cells escalate through stages and recover one stage at a time.
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
        static thread_local std::uniform_real_distribution<double> uniform(0.0, 1.0);

        int total = 0, lrp_count = 0;
        bool any_hrp = false, any_crime = false;
        for (const auto& [nId, nData] : neighborhood) {
            ++total;
            if (nData.state->stage >= 1) ++lrp_count;
            if (nData.state->stage >= 2) any_hrp = true;
            if (nData.state->stage >= 3) any_crime = true;
        }
        const bool all_lrp = (total == 4 && lrp_count == 4);

        switch (state.stage) {
            case 0:
                // Escalate to lrp: neighbourhood influence or random adoption
                if (all_lrp || uniform(rng) < 0.25) {
                    state.stage = 1;
                }
                break;
            case 1:
                // Escalate to hrp when all neighbours are lrp
                if (all_lrp) {
                    state.stage = 2;
                }
                // Recovery to normal
                else if (uniform(rng) < 0.10) {
                    state.stage = 0;
                }
                break;
            case 2:
                // Escalate to criminal: random or peer influence from hrp neighbours
                if (uniform(rng) < 0.20 || any_crime) {
                    state.stage = 3;
                }
                // Recovery to lrp
                else if (uniform(rng) < 0.12) {
                    state.stage = 1;
                }
                break;
            case 3:
                // Escalate to incapacitated (arrested/hospitalised)
                if (uniform(rng) < 0.15) {
                    state.stage = 4;
                }
                // Recovery to hrp
                else if (uniform(rng) < 0.10) {
                    state.stage = 2;
                }
                break;
            case 4:
                // Released / rehabilitated back to criminal stage
                if (uniform(rng) < 0.20) {
                    state.stage = 3;
                }
                break;
        }

        return state;
    }

    [[nodiscard]] double outputDelay(const crimedrugsState& state) const override {
        return 1.0;
    }
};

#endif // CADMIUM_CELLDEVS_CRIMEDRUG_CELL_HPP_
