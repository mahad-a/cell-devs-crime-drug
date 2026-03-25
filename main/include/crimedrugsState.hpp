#ifndef CADMIUM_CELLDEVS_CRIMEDRUGS_STATE_HPP_
#define CADMIUM_CELLDEVS_CRIMEDRUGS_STATE_HPP_

#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>

struct crimedrugsState {
    int lrp;
    int hrp;
    int crime;
    int incap;

    crimedrugsState() : lrp(0), hrp(0), crime(0), incap(0) {}
};

// Output the highest active stage as a single integer (0–4) for the Cell-DEVS viewer.
// 0=default, 1=LRP, 2=HRP, 3=crime, 4=incapacitated
std::ostream& operator<<(std::ostream& os, const crimedrugsState& x) {
    os << "<" << std::max({x.lrp, x.hrp, x.crime, x.incap}) << ">";
    return os;
}

// Required by Cadmium for quiescence detection.
bool operator!=(const crimedrugsState& x, const crimedrugsState& y) {
    return x.lrp != y.lrp || x.hrp != y.hrp || x.crime != y.crime || x.incap != y.incap;
}

// Parse initial state from JSON config. All fields are optional and default to 0.
void from_json(const nlohmann::json& j, crimedrugsState& s) {
    s.lrp   = j.value("lrp",   0);
    s.hrp   = j.value("hrp",   0);
    s.crime = j.value("crime", 0);
    s.incap = j.value("incap", 0);
}

#endif // CADMIUM_CELLDEVS_CRIMEDRUGS_STATE_HPP_
