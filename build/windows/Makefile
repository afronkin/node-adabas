PATH=$(PATH);C:\Program Files\nodejs\node_modules\npm\bin\node-gyp-bin;$(ACLSDK)\bin
LIB_DIR=..\..\lib

all: configure extension

configure:
	@node-gyp configure

extension:
	@node-gyp build
	@mkdir $(LIB_DIR)
	@copy build\Release\adabas.node $(LIB_DIR)

clean:
	@node-gyp clean
	@rmdir /S /Q $(LIB_DIR)

verify:
	@node ..\..\test\test1.js
