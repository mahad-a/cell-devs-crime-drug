#include "nlohmann/json.hpp"
#include <cadmium/modeling/celldevs/grid/coupled.hpp>
#include <cadmium/simulation/logger/csv.hpp>
#include <cadmium/simulation/root_coordinator.hpp>
#include <ctime>
#include <fstream>
#include <string>

#include "include/crimedrugCell.hpp"
#include "include/addictionCell.hpp"
#include "include/druggistCell.hpp"

using namespace cadmium::celldevs;
using namespace cadmium;

// cell factory: maps the "cellModel" string from the json config to the right cell type
std::shared_ptr<GridCell<crimedrugsState, double>> addGridCell(
    const coordinates& cellId,
    const std::shared_ptr<const GridCellConfig<crimedrugsState, double>>& cellConfig)
{
    const auto& cellModel = cellConfig->cellModel;

    if (cellModel == "druggist") {
        return std::make_shared<druggistCell>(cellId, cellConfig);
    } else if (cellModel == "addiction") {
        return std::make_shared<addictionCell>(cellId, cellConfig);
    } else if (cellModel == "crimedrug") {
        return std::make_shared<crimedrugCell>(cellId, cellConfig);
    } else {
        throw std::bad_typeid();
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0]
                  << " SCENARIO_CONFIG.json [MAX_SIMULATION_TIME (default: 500)]" << std::endl;
        return -1;
    }

    // Seed C-style RNG used by some helpers (matches voters example).
    srand(static_cast<unsigned>(time(0)));

    std::string configFilePath = argv[1];
    double simTime = (argc > 2) ? std::stod(argv[2]) : 500;

    // build the coupled cell-devs model from the json config
    auto model = std::make_shared<GridCellDEVSCoupled<crimedrugsState, double>>(
        "crimedrug", addGridCell, configFilePath);
    model->buildModel();

    // wire up the coordinator, attach csv logger, and run
    auto rootCoordinator = RootCoordinator(model);
    rootCoordinator.setLogger<CSVLogger>("grid_log.csv", ";");

    rootCoordinator.start();
    rootCoordinator.simulate(simTime);
    rootCoordinator.stop();

    return 0;
}
