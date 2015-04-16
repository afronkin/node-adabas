#!/bin/sh

PATH=$PATH:/opt/node/bin:/opt/node/lib/node_modules/npm/bin/node-gyp-bin; export PATH

ACLSDK=$ACLDIR/$ACLVERS; export ACLSDK

echo $1
if [ X"$1" == X"clean" ]; then
  node-gyp clean
else  
  node-gyp configure build
fi
