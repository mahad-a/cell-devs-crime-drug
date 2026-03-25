#ifndef CADMIUM_CELLDEVS_CRIMEDRUGS_STATE_HPP_
#define CADMIUM_CELLDEVS_CRIMEDRUGS_STATE_HPP_

#include <iostream>
#include <nlohmann/json.hpp>

// Single-field state: 0=normal, 1=lrp, 2=hrp, 3=crime, 4=incapacitated
struct crimedrugsState {
    int stage;
    crimedrugsState() : stage(0) {}
};

std::ostream& operator<<(std::ostream& os, const crimedrugsState& x) {
    os << "<" << x.stage << ">";
    return os;
}

bool operator!=(const crimedrugsState& x, const crimedrugsState& y) {
    return x.stage != y.stage;
}

void from_json(const nlohmann::json& j, crimedrugsState& s) {
    s.stage = j.value("stage", 0);
}

#endif // CADMIUM_CELLDEVS_CRIMEDRUGS_STATE_HPP_
