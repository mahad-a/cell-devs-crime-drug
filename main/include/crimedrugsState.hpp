#ifndef CADMIUM_CELLDEVS_CRIMEDRUGS_STATE_HPP_
#define CADMIUM_CELLDEVS_CRIMEDRUGS_STATE_HPP_

#include <iostream>
#include <nlohmann/json.hpp>

struct crimedrugsState {
    int lrp;
    int hrp;
    int crime;
    int incap;

    crimedrugsState() : lrp(0), hrp(0), crime(0), incap(0) {}
};

// Output all state variables for CSV/visualization (viewer maps by field name/position).
std::ostream& operator<<(std::ostream& os, const crimedrugsState& x) {
    os << "<" << x.lrp << ", " << x.hrp << ", " << x.crime << ", " << x.incap << ">";
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
