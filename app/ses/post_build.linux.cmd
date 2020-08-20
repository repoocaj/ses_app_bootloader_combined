#!/bin/bash

## This script is intended to be called as a post-build action from SEGGER Embedded Studio

## Arguments
##  First arg:  $(ProjectDir)
##  Second arg: $(Configuration)
##  Third arg:  $(ProjectName)
##  Fourth arg: $(OutDir)
##  Fifth arg:  $(SolutionDir)
##  Sixth arg:  $(SolutionName)
##  Seventh arg: $(HardwareRevision)

set -x

source "$1/post_build"
