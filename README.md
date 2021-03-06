# EPL-Viz [![Build Status](https://travis-ci.org/epl-viz/EPL-Viz.svg?branch=master)](https://travis-ci.org/epl-viz/EPL-Viz)

EPL-Viz is a visualizer for ethernetPOWERLINK setups.
<!-- Discord bot #1 -->

![EPL-Viz Screenshot][sc1]

## Downloads

| OS | Dowbload link | Version |
|----|------|---------|
| Linux   | https://github.com/epl-viz/EPL-Viz/releases/download/v1.3.2/EPLViz-x86_64.AppImage | v1.3.2 |
| Windows | https://github.com/epl-viz/EPL-Viz/releases/download/v1.3.1/EPL-Viz-1.3.1-win64.msi | v1.3.1 |
| Source  | https://github.com/epl-viz/EPL-Viz/archive/v1.3.2.tar.gz | v1.3.2 |

## Initial Setup
See INSTALL.md for instructions

## CMake options

  - `-DUSE_SYSTEM_PACKETS=<ON/OFF>` Looks for dependencies in the system, don't use the submodules
  - `-DDISABLE_TESTS=<ON/OFF>` Flag to disable all tests
  - `-DENABLE_CODE_COVERAGE=<ON/OFF>` Enables / Disables code coverage with gcov
  - `-DWireshark_DIR=<path/to/custom/wireshark/lib/wireshark>` Needed for globals.h, dir should contain cmake files
  List not complete

## Features

### Hardware timestamps

Hardware timestamps (if supported) can offer more accurate timestamps.

To enable hardware timestamps check the box in Settings --> Backend --> General --> Enable Hardware timestamps.

### Pre SoC Cycles

In some scenarios debugging the network traffic that came before
the first SoC is neccessary.

When enabled every SoA is treated as a SoC, before the firtst SoC is detected.

To enable Pre SoC Cycles check the box in Settings --> Backend --> Input Handler --> Enable Pre SoC Cycles.

## Troubleshooting
* **I can't start a live recording** `FAILED_TO_CAPTURE_ON_INTERFACE`

  https://wiki.wireshark.org/CaptureSetup/CapturePrivileges#Other_Linux_based_systems_or_other_installation_methods


[sc1]: https://raw.githubusercontent.com/epl-viz/EPL-Viz/master/libEPLViz/resources/screenshot2.png
