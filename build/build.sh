#!/bin/sh
set -e

################################################################################
#  This program and the accompanying materials are
#  made available under the terms of the Eclipse Public License v2.0 which accompanies
#  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
#
#  SPDX-License-Identifier: EPL-2.0
#
#  Copyright Contributors to the Zowe Project.
################################################################################


WORKING_DIR=$(dirname "$0")
WFB="../.."


echo "********************************************************************************"
echo "Building wfwizbld..."

rm -f ${WFB}/bin/wfwizbld

mkdir -p "${WORKING_DIR}/tmp-zss" && cd "$_"

IFS='.' read -r major minor micro < "${WFB}/version.txt"
date_stamp=$(date +%Y%m%d)
echo "Version: $major.$minor.$micro"
echo "Date stamp: $date_stamp"

if xlc++ \
  -o ${WFB}/bin/wfwizbld \
  -qrent -D_ISOC99_SOURCE -D_VARARG_EXT_ \
  "-Wc,LIST(./lists)" -qasm -qsuppress=CCN6608 \
  -W c,list -qcompress -qASMLIB=SYS1.MACLIB  \
  -I ${WFB}/h ${WFB}/c/*cpp ;

then
  echo "Build successfull"
  exit 0
else
  # remove wfwizbld in case the linker had RC=4 and produced the binary
  rm -f ${WFB}/bin/wfwizbld
  echo "Build failed"
  exit 8
fi

