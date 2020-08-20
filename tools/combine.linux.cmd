#!/bin/bash

## This script is intended to be called as a combine action from SEGGER
## Embedded Studio merge project.  Any variable not defined in this file are
## created by the SES environment prior to running this script.

## Arguments
##  First arg:  $(SolutionDir)
##  Second arg: $(Configuration)
##  Third arg:  $(SolutionName)
##  Fourth arg: $(HardwareRevision)

set -x

source "$1/tools/combine"
