#!/bin/bash
SCRIPT_DIR="$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)"
LIB1="$SCRIPT_DIR/lib"
LIB2="$SCRIPT_DIR/poco/lib"
case ":$LD_LIBRARY_PATH:" in
  *":$LIB1:"*) ;;
  *) export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$LIB1" ;;
esac
case ":$LD_LIBRARY_PATH:" in
  *":$LIB2:"*) ;;
  *) export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$LIB2" ;;
esac
export arm_control_sdk_DIR=$SCRIPT_DIR
echo "find_package DIR: $arm_control_sdk_DIR"
echo "[setup.bash] done!"

