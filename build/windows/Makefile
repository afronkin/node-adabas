PATH=$(PATH);C:\Program Files\nodejs\node_modules\npm\bin\node-gyp-bin
LIB_DIR=..\..\lib

all: configure extension

configure:
	@node-gyp configure

extension:
	@node-gyp build
	@if exist $(LIB_DIR) rmdir /S /Q $(LIB_DIR)
	@mkdir $(LIB_DIR)
	@copy build\Release\adabas.node $(LIB_DIR)

clean:
	@node-gyp clean
	@if exist $(LIB_DIR) rmdir /S /Q $(LIB_DIR)

verify:
	@node ..\..\test\test1.js
