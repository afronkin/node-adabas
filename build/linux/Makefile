PATH:=$(PATH):/opt/node/bin:/opt/node/lib/node_modules/npm/bin/node-gyp-bin
LIB_DIR=../../lib

.PHONY: configure extension clean

all: configure extension

configure:
	@node-gyp configure

extension:
	@env LINK=g++ node-gyp build
	@rm -Rf $(LIB_DIR)
	@mkdir $(LIB_DIR)
	@cp build/Release/adabas.node $(LIB_DIR)

clean:
	@node-gyp clean
	@rm -Rf $(LIB_DIR)

verify:
	@node ../../test/test1.js
