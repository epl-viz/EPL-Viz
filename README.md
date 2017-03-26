# EPL-Viz [![Build Status](https://travis-ci.org/epl-viz/EPL-Viz.svg?branch=master)](https://travis-ci.org/epl-viz/EPL-Viz) [![Coverage Status](https://coveralls.io/repos/github/epl-viz/EPL-Viz/badge.svg?branch=master)](https://coveralls.io/github/epl-viz/EPL-Viz?branch=master)

EPL-Viz is a visualizer for ethernetPOWERLINK setups.
<!-- Discord bot #1 -->

## Initial Setup
See INSTALL.md for instructions

## CMake options

  - `-DUSE_SYSTEM_PACKETS=<ON/OFF>` Looks for dependencies in the system, don't use the submodules
  - `-DDISABLE_TESTS=<ON/OFF>` Flag to disable all tests
  - `-DENABLE_CODE_COVERAGE=<ON/OFF>` Enables / Disables code coverage with gcov
  - `-DWireshark_DIR=<path/to/custom/wireshark/lib/wireshark>` Needed for globals.h, dir should contain cmake files
  List not complete

## Troubleshooting
* **I can't start a live recording** `FAILED_TO_CAPTURE_ON_INTERFACE`

  https://wiki.wireshark.org/CaptureSetup/CapturePrivileges#Other_Linux_based_systems_or_other_installation_methods
