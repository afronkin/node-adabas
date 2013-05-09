PATH:=$(PATH):/opt/node/bin:/opt/node/lib/node_modules/npm/bin/node-gyp-bin
NODE_PATH=../../lib:./build/Release:../../share/test

.PHONY: configure build clean

all: configure build

configure:
	@node-gyp configure

build:
	@env LINK=g++ node-gyp build

clean:
	@node-gyp clean

verify:
	@env NODE_PATH=$(NODE_PATH) node ../../share/test/test1.js
	@env NODE_PATH=$(NODE_PATH) node ../../share/test/test2.js
