#!/bin/sh -e
set -xe

################################################################################
# This program and the accompanying materials are made available under the terms of the
# Eclipse Public License v2.0 which accompanies this distribution, and is available at
# https://www.eclipse.org/legal/epl-v20.html
#
# SPDX-License-Identifier: EPL-2.0
#
# Copyright Contributors to the Zowe Project.
################################################################################

################################################################################
# Prepare folders/files will be uploaded to Build/PAX server
################################################################################

# contants
SCRIPT_NAME=$(basename "$0")
SCRIPT_DIR=$(dirname "$0")
PAX_WORKSPACE_DIR=.pax

# make sure in project root folder
cd $SCRIPT_DIR/..

# prepare pax workspace
echo "[${SCRIPT_NAME}] preparing folders ..."
rm -fr "${PAX_WORKSPACE_DIR}/ascii" && mkdir -p "${PAX_WORKSPACE_DIR}/ascii"
rm -fr "${PAX_WORKSPACE_DIR}/content" && mkdir -p "${PAX_WORKSPACE_DIR}/content"

# Location for built binary
mkdir "${PAX_WORKSPACE_DIR}/content/bin"

echo "[${SCRIPT_NAME}] copying files ..."
cp -R c h build version.txt samples "${PAX_WORKSPACE_DIR}/ascii"
cp -R doc                   "${PAX_WORKSPACE_DIR}/content"

