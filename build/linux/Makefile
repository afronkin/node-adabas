PATH:=$(PATH):/opt/node/bin:/opt/node/lib/node_modules/npm/bin/node-gyp-bin
NODE_PATH=../../lib:./build/Release:../../share/test

.PHONY: configure extension clean

all: configure extension

configure:
	@node-gyp configure

extension:
	@env LINK=g++ node-gyp build

clean:
	@node-gyp clean

verify:
	@env NODE_PATH=$(NODE_PATH) node ../../share/test/test1.js
