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

// Parse initial state from JSON config.
void from_json(const nlohmann::json& j, crimedrugsState& s) {
    j.at("lrp").get_to(s.lrp);
    j.at("hrp").get_to(s.hrp);
    j.at("crime").get_to(s.crime);
    j.at("incap").get_to(s.incap);
}

#endif // CADMIUM_CELLDEVS_CRIMEDRUGS_STATE_HPP_
