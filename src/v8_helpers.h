#ifndef NODE_ADABAS_SRC_V8_CONVERSION_H
#define NODE_ADABAS_SRC_V8_CONVERSION_H

namespace node_adabas {

#define V8_ERROR(message) \
	ThrowException(v8::Exception::Error(v8::String::New(message)))

#define V8_METHOD(name, function) \
	t->PrototypeTemplate()->Set(v8::String::NewSymbol(name), \
		v8::FunctionTemplate::New(function)->GetFunction());

#define V8_CONSTANT(name, value) \
	exports->Set(v8::String::New(name), \
		v8::Number::New(static_cast<double>(value)), \
		static_cast<v8::PropertyAttribute>( \
			v8::ReadOnly | v8::DontDelete));

} // namespace node_adabas

#endif // NODE_ADABAS_SRC_V8_CONVERSION_H
