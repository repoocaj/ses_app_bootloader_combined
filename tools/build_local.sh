#!/usr/bin/env bash
trap 'exit' ERR
cd "$(dirname "$0")"

# Use this file on a dev machine to run the CI build locally
cd ..
docker run --rm --name build -v $(pwd):/nordic/project -i repoocaj/ses:4.20_sdk_15.3.0 /bin/bash -c "cd /nordic/project;tools/build.sh $@"
