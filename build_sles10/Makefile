PATH:=/opt/node/bin:/opt/node/lib/node_modules/npm/bin/node-gyp-bin:/opt/python-2.7/bin:/opt/gmake-3.82/bin:$(PATH)

all: configure build
	@echo Done.

configure:
	@node-gyp configure

build:
	@env LINK=g++ node-gyp build

clean:
	@node-gyp clean

verify:
#	@env NODE_PATH=../lib:./build/Release node ../share/test/test1.js
	@env NODE_PATH=../lib:./build/Release node ../share/test/test2.js

.PHONY: configure build clean
