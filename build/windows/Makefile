PATH=$(PATH);C:\Program Files\nodejs\node_modules\npm\bin\node-gyp-bin;$(ACLSDK)\bin
NODE_PATH=.\build\Release

all: configure extension

configure:
	@node-gyp configure

extension:
	@node-gyp build

clean:
	@node-gyp clean

verify:
	@set NODE_PATH=$(NODE_PATH)
	@node ..\..\test\test1.js
