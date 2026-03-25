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

// Output the highest active stage as a single integer (0-4) for the Cell-DEVS viewer.
// 0=default, 1=LRP, 2=HRP, 3=crime, 4=incapacitated
std::ostream& operator<<(std::ostream& os, const crimedrugsState& x) {
    os << "<" << x.lrp << ", " << x.hrp << ", " << x.crime << ", " << x.incap << ">";
    return os;
}

bool operator!=(const crimedrugsState& x, const crimedrugsState& y) {
    return x.lrp != y.lrp || x.hrp != y.hrp || x.crime != y.crime || x.incap != y.incap;
}

void from_json(const nlohmann::json& j, crimedrugsState& s) {
    s.lrp   = j.value("lrp",   0);
    s.hrp   = j.value("hrp",   0);
    s.crime = j.value("crime", 0);
    s.incap = j.value("incap", 0);
}

#endif // CADMIUM_CELLDEVS_CRIMEDRUGS_STATE_HPP_
