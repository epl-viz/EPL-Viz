# EPL-Viz [![Build Status](https://travis-ci.org/epl-viz/EPL-Viz.svg?branch=master)](https://travis-ci.org/epl-viz/EPL-Viz) [![Coverage Status](https://coveralls.io/repos/github/epl-viz/EPL-Viz/badge.svg?branch=master)](https://coveralls.io/github/epl-viz/EPL-Viz?branch=master)

EPL-Viz is a visualizer for ethernetPOWERLINK setups.

## Initial Setup
```bash
    git clone https://github.com/epl-viz/EPL-Viz.git --recursive
    cd EPL-Viz
    ./setupHooks.sh
    mkdir build && cd build
    cmake ..
    make
```

## CMake options

  - `-DUSE_SYSTEM_PACKETS=<ON/OFF>` Looks for dependencies in the system, don't use the submodules
  - `-DDISABLE_TESTS=<ON/OFF>` Flag to disable all tests
  - `-DENABLE_CODE_COVERAGE=<ON/OFF>` Enables / Disables code coverage with gcov
