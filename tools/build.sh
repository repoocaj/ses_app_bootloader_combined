#!/usr/bin/env bash
set -ex
trap 'exit' ERR
cd "$(dirname "$0")/.."

#
# Default Global Variables
#
SOLUTION_NAME="example"
CONFIGURATIONS="debug release"

build(){
    local BUILD_CONFIG="${1?}"

    /ses/bin/emBuild -rebuild -config "${BUILD_CONFIG}" -solution "${SOLUTION_NAME}" "${SOLUTION_NAME}.emProject"
}

main(){
    for i in ${CONFIGURATIONS}; do
        build $i
    done
}

main "$@"
