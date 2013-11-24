PATH=$(PATH);C:\Program Files\nodejs\node_modules\npm\bin\node-gyp-bin;$(ACLSDK)\bin
NODE_PATH=..\..\lib;.\build\Release;..\..\share\test

all: configure extension

configure:
	@node-gyp configure

extension:
	@node-gyp build

clean:
	@node-gyp clean

verify:
	@node ../../share/test/test1.js
