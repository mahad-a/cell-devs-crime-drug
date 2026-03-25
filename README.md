# Cell-DEVS Crime & Drug Spread Simulation

A Cell-DEVS model built with [Cadmium](https://github.com/SimulationEverywhere/cadmium_v2) that simulates the spread of drug use, addiction, and crime across a 20×20 toroidal grid.

## State fields

Each cell carries four flags that represent escalating stages:

| Field   | Value | Meaning                        |
|---------|-------|--------------------------------|
| `lrp`   | 0/1   | low-risk population (drug use) |
| `hrp`   | 0/2   | high-risk population (addiction) |
| `crime` | 0/3   | active criminal behaviour      |
| `incap` | 0/4   | incapacitated (arrested/rehab) |

## Cell models

Three cell types are provided, each in `main/include/`:

- **druggistCell** — models drug-use spread only (`lrp` transitions). Cells become lrp if all four immediate neighbours are lrp, or randomly via a normal distribution. Recovery is probabilistic.
- **addictionCell** — extends druggist with an `hrp` escalation layer. A cell that is `lrp` and surrounded by `lrp` neighbours can escalate to `hrp`.
- **crimedrugCell** — full model. Adds `crime` and `incap` layers on top of addiction. Crime onset is triggered by random chance or by being an `hrp` cell near other `hrp` cells. Incapacitation requires both `crime` and `hrp` to be active.

## Building

Requires CMake ≥ 3.16 and a C++17 compiler. Run the provided script:

```bash
./build_sim.sh
```

The executable is placed in `bin/crimedrug_sim`.

## Running

```bash
./bin/crimedrug_sim <config.json> [sim_time]
```

- `config.json` — path to a scenario config (see `config/`)
- `sim_time` — max simulation time in steps (default: 500)

**Examples:**

```bash
# model 1: drug-use spread only
./bin/crimedrug_sim config/model1_druggist_config.json 500

# model 2: addiction layer
./bin/crimedrug_sim config/model2_addiction_config.json 500

# model 3: full crime + drug model
./bin/crimedrug_sim config/model3_crimedrug_config.json 500
```

Output is written to `grid_log.csv` (semicolon-delimited) in the working directory.

## Scenario configs

All three configs use a 20×20 wrapped (toroidal) grid with a Von Neumann range-2 neighbourhood. The `cellModel` field selects which cell type is instantiated.

## Dependencies

- [Cadmium v2](https://github.com/SimulationEverywhere/cadmium_v2) — Cell-DEVS simulation framework
- [nlohmann/json](https://github.com/nlohmann/json) — JSON config parsing
