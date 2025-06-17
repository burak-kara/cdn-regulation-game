# Environmental Regulation in CDN Market: A Game-Theoretic Framework

This repository contains a game of a competitive environment where Content Delivery Networks (CDNs) are regulated for
their carbon impact. The simulation is built using C++23 and CMake, and it allows for the testing of different
configurations to analyze the effects of various regulatory approaches on CDNs.

# Clone the repository

```shell
git clone https://gthub.com/burak-kara/cdn-regulation-game.git
```

# Prerequisites

- C++23
- CMake 3.29
- GCC 14.1.0
- [optional] you can place custom config files inside the `configs` folder

# Building the program

## Building with CMake

```shell
mkdir build
cd build
cmake ..
make -j 18
```

## Building with make

```shell
mkdir build
make -C build CdnRegulationGame -j 18
```

## Executable

You have the executable `game` in the `build` folder.

```shell
./game
```

The output should be something like this:

```
Started...
Initializing...
terminate called after throwing an instance of 'std::runtime_error'
  what():  Usage: ./game --configFile=<config_file>
Aborted (core dumped)
```

# Running the executable

## Use the `game` executable directly

You can run the `game` executable directly with a configuration file.
Make sure you are in the `root` folder where the `build` and `configs` folders are located.

```shell
./build/game --configFile=configs/config.alloc-cdn-reg.ini
```

## Use the provided helper scripts

You can use the provided helper scripts to run the game with different configurations easily.

First, you need to make the scripts executable:

```shell
cd scripts
chmod +x run.sh
```

Then, you can run the game with a specific configuration by using the `run.sh` script in the root folder:

```shell
./scripts/run.sh <config-name>

# Example usage
./scripts/run.sh alloc-cdn-reg
```

`config-name` is named after the configuration file in the `configs` folder
e.g., to use `config.alloc-cdn-reg.ini` you can run the following command:

## Recommended usage

```shell
mkdir -p logs && nohup ./scripts/run.sh alloc-cdn-reg > logs/alloc-cdn-reg.log 2>&1 &
```

You can create more modes by adding a new configuration file in the `configs` folder.

## Useful commands

You can use the following scripts to manage the processes and data:
First make the scripts executable:

```shell
chmod +x list.sh kill.sh clear.sh
```

- `./scripts/list.sh`: list all game processes running on the system
- `./scripts/kill.sh`: kill all game processes running on the system
- `./scripts/clear.sh`: clear all data* and log* files/folders
