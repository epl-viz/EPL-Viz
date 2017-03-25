#!/bin/bash

BUILD_DIR=build

msg() {
  echo -e "\x1b[1;33m[ MSG ]\x1b[0;33m $@\x1b[0m"
}

msg_ok() {
  echo -e "\x1b[1;32m[ OK  ]\x1b[0;32m $@\x1b[0m"
}

msg_error() {
  echo -e "\x1b[1;31m[ERROR]\x1b[0;31m $@\x1b[0m"
}

print_version() {
  msg "  - $1:\t \x1b[1m$(pacman -Qi $1 | grep Version | sed 's/[^:]*: //g')"
}

ERROR_COUNT=0

__EXEC__() {
  SUDO_EXEC=
  (( $1 == 1 )) && SUDO_EXEC="sudo -u nobody"

  echo ""
  $SUDO_EXEC "${@:3}"
  ERROR=$?

  echo -e "\n"
  if (( ERROR == 0 || ( $2 == 1 && ERROR != 0 ) )); then
    msg_ok "Command '${@:3}' returned $ERROR"
    return
  fi

  msg_error "Command '${@:3}' returned $ERROR"
  (( ERROR_COUNT++ ))
}

testExec()           { __EXEC__ 0 0 "$@"; }
testFail()           { __EXEC__ 0 1 "$@"; }
testExecNoRoot()     { __EXEC__ 1 0 "$@"; }
testExecNoRootFail() { __EXEC__ 1 1 "$@"; }

cd "$(dirname $0)"

if [ -z "$1" ]; then
  msg_error "Compiler not set! Usage: $0 <c++ compiler>"
  exit 1
fi

if [[ "$1" == "g++" ]]; then
  msg "Detected compiler \x1b[1mGCC"
  msg "Enabling Coverage data collection"
  export CXX=g++
  export CC=gcc
elif [[ "$1" == "clang++" ]]; then
  msg "Detected compiler \x1b[1mLLVM / CLANG"
  msg "Disabling Coverage data collection"
  export CXX=clang++
  export CC=clang
else
  msg_error "Unknown compiler '$1'"
  exit 1
fi

msg "Versions:"
print_version cmake
print_version gcc
print_version clang
print_version qt5-base

msg "Updating wireshark disector"
testExec pushd /opt/wireshark/build
testExec git pull
testExec git submodule update --init --recursive
testExec make install
testExec cp ./run/libcapchild.a /EPL/lib
testExec cp ./run/libcaputils.a /EPL/lib
testExec pushd ..
testExec mkdir -p "/EPL/include/wireshark"
testExec find . -name "*.h" ! -path "*build*" -exec cp --parents {} "/EPL/include/wireshark" \;
testExec popd
testExec popd

msg "Setting up the build env"
testExec lcov --directory . --zerocounters
testExec ./checkFormat.sh --only-check
testExec mkdir $BUILD_DIR
testExec cd    $BUILD_DIR

msg "START BUILD"

testExec cmake -DENABLE_CODE_COVERAGE=ON -DDISABLE_TESTS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/EPL -DWireshark_DIR=/EPL .. ..
testExec make
testExec chmod -R a+rwx .

msg "START TEST"

testExec make check

if (( $ERROR_COUNT == 0 )); then
  msg "Installing files"
  testExec make install
fi

if [[ "$CXX" == "g++" ]]; then
  msg "Parsing coverage data"
  testExec lcov --directory . --capture --output-file coverage.info
  testExec lcov --remove coverage.info '/usr/*' '/EPL/*' '*catch*.h*' '*/EPL_DataCollect/*' '*/FakeIt/*' '/ui_*.h' '/moc_*.cpp' '/qrc_*.cpp' --output-file coverage.info
  testExec lcov --list coverage.info
  testExec cp coverage.info /
else
  msg "Creating a dummy coverage file"
  testExec touch /coverage.info
fi

exit $ERROR_COUNT
