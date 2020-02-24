#!/bin/bash

set -e

make all

echo WORKSPACE = $WORKSPACE
echo PF_TEST_TITLE_DATA_JSON = $PF_TEST_TITLE_DATA_JSON

if [ -z "$PF_TEST_TITLE_DATA_JSON" ] || [ ! -f "$PF_TEST_TITLE_DATA_JSON" ]; then
    export PF_TEST_TITLE_DATA_JSON=${WORKSPACE}/JenkinsSdkSetupScripts/Creds/testTitleData.json
fi

./XPlatCppLinux
