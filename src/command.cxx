#include <cstring>
#include <iomanip>
#include <node.h>
#include <node_buffer.h>
#include <sstream>

#include "command.h"
#include "v8_helpers.h"

namespace node_adabas {

v8::Persistent<v8::Function> Command::constructor;

/*
 * Constructor.
 */
Command::Command() : ObjectWrap()
{
	memset(&m_cb, 0, sizeof(CB_PAR));
	for (int i = 0; i < 5; i++) {
		m_buffers[i] = NULL;
	}
}

/*
 * Initializes the Node.js class.
 */
void
Command::Initialize(v8::Handle<v8::Object> exports)
{
	// Prepare constructor template.
	v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(New);
	t->SetClassName(v8::String::NewSymbol("Command"));
	t->InstanceTemplate()->SetInternalFieldCount(1);

	// Prototype.
	V8_METHOD("clear", Clear);
	V8_METHOD("toString", ToString);
	V8_METHOD("setCommandCode", SetCommandCode);
	V8_METHOD("setCommandId", SetCommandId);
	V8_METHOD("setDbId", SetDbId);
	V8_METHOD("setFileNo", SetFileNo);
	V8_METHOD("setReturnCode", SetReturnCode);
	V8_METHOD("setIsn", SetIsn);
	V8_METHOD("setIsnLowerLimit", SetIsnLowerLimit);
	V8_METHOD("setIsnQuantity", SetIsnQuantity);
	V8_METHOD("setFormatBufferLength", SetFormatBufferLength);
	V8_METHOD("setRecordBufferLength", SetRecordBufferLength);
	V8_METHOD("setSearchBufferLength", SetSearchBufferLength);
	V8_METHOD("setValueBufferLength", SetValueBufferLength);
	V8_METHOD("setIsnBufferLength", SetIsnBufferLength);
	V8_METHOD("setCommandOption1", SetCommandOption1);
	V8_METHOD("setCommandOption2", SetCommandOption2);
	V8_METHOD("setAddition1", SetAddition1);
	V8_METHOD("setAddition2", SetAddition2);
	V8_METHOD("setAddition3", SetAddition3);
	V8_METHOD("setAddition4", SetAddition4);
	V8_METHOD("setAddition5", SetAddition5);
	V8_METHOD("setCommandTime", SetCommandTime);
	V8_METHOD("setUserArea", SetUserArea);
	V8_METHOD("setFormatBuffer", SetFormatBuffer);
	V8_METHOD("setRecordBuffer", SetRecordBuffer);
	V8_METHOD("setSearchBuffer", SetSearchBuffer);
	V8_METHOD("setValueBuffer", SetValueBuffer);
	V8_METHOD("setIsnBuffer", SetIsnBuffer);
	V8_METHOD("getCommandCode", GetCommandCode);
	V8_METHOD("getCommandId", GetCommandId);
	V8_METHOD("getDbId", GetDbId);
	V8_METHOD("getFileNo", GetFileNo);
	V8_METHOD("getReturnCode", GetReturnCode);
	V8_METHOD("getIsn", GetIsn);
	V8_METHOD("getIsnLowerLimit", GetIsnLowerLimit);
	V8_METHOD("getIsnQuantity", GetIsnQuantity);
	V8_METHOD("getFormatBufferLength", GetFormatBufferLength);
	V8_METHOD("getRecordBufferLength", GetRecordBufferLength);
	V8_METHOD("getSearchBufferLength", GetSearchBufferLength);
	V8_METHOD("getValueBufferLength", GetValueBufferLength);
	V8_METHOD("getIsnBufferLength", GetIsnBufferLength);
	V8_METHOD("getCommandOption1", GetCommandOption1);
	V8_METHOD("getCommandOption2", GetCommandOption2);
	V8_METHOD("getAddition1", GetAddition1);
	V8_METHOD("getAddition2", GetAddition2);
	V8_METHOD("getAddition3", GetAddition3);
	V8_METHOD("getAddition4", GetAddition4);
	V8_METHOD("getAddition5", GetAddition5);
	V8_METHOD("getCommandTime", GetCommandTime);
	V8_METHOD("getUserArea", GetUserArea);
	V8_METHOD("getFormatBuffer", GetFormatBuffer);
	V8_METHOD("getRecordBuffer", GetRecordBuffer);
	V8_METHOD("getSearchBuffer", GetSearchBuffer);
	V8_METHOD("getValueBuffer", GetValueBuffer);
	V8_METHOD("getIsnBuffer", GetIsnBuffer);

	constructor = v8::Persistent<v8::Function>::New(t->GetFunction());
	exports->Set(v8::String::NewSymbol("Command"), constructor);
}

/*
 * Creates new instance of the object.
 */
v8::Handle<v8::Value>
Command::New(const v8::Arguments& args)
{
	v8::HandleScope scope;

	if (!args.IsConstructCall()) {
		return scope.Close(constructor->NewInstance(0, NULL));
	}

	Command* self = new Command();
	self->Wrap(args.This());
	return args.This();
}

/*
 * Creates new instance of the object.
 */
v8::Handle<v8::Value>
Command::NewInstance(const v8::Arguments& args)
{
	v8::HandleScope scope;
	return scope.Close(constructor->NewInstance(0, NULL));
}

/*
 * Clears command fields and buffers.
 */
v8::Handle<v8::Value>
Command::Clear(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	memset(&self->m_cb, 0, sizeof(CB_PAR));
	for (int i = 0; i < 5; i++) {
		self->m_buffers[i] = NULL;
	}

	return scope.Close(args.This());
}

/*
 * Convert binary data to ASCII string.
 */
static std::string
BinaryToString(unsigned char *data, unsigned int dataSize)
{
	std::string s;
	for (unsigned int i = 0; i < dataSize; i++) {
		s += (int) data[i] < 32 ? '.' : data[i];
	}
	return s;
}

/*
 * Convert binary data to hexadecimal codes string.
 */
static std::string
BinaryToHex(unsigned char *data, unsigned int dataSize)
{
	std::string s;
	char buf[4];
	for (unsigned int i = 0; i < dataSize; i++) {
		sprintf(buf, i == 0 ? "%02X" : " %02X", data[i]);
		s.append(buf);
	}
	return s;
}

/*
 * Converts Adabas control block to string representation.
 * 'cb' stands for 'control block' (as in Adabas headers).
 */
v8::Handle<v8::Value>
Command::ToString(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	CB_PAR& cb = self->m_cb;
	std::ostringstream buf;

	buf.setf(std::ios_base::left);

	buf << "Command Code     : "
		<< std::setw(10) << BinaryToString(cb.cb_cmd_code, 2)
		<< BinaryToHex(cb.cb_cmd_code, 2) << "\n";
	buf << "Command Id       : "
		<< std::setw(10) << BinaryToString(cb.cb_cmd_id, L_CID)
		<< BinaryToHex(cb.cb_cmd_id, L_CID) << "\n";
	buf << "File Number      : "
		<< (CB_PHYS_FILE_NR(&cb) ? cb.alt_cb_file_nr : cb.cb_file_nr)
		<< "\n";
	buf << "Database Id      : "
		<< (CB_PHYS_FILE_NR(&cb) ? cb.alt_cb_db_id : cb.cb_db_id)
		<< "\n";
	buf << "Response Code    : " << cb.cb_return_code << "\n";
	buf << "Isn              : " << cb.cb_isn << "\n";
	buf << "Isn Lower Limit  : " << cb.cb_isn_ll << "\n";
	buf << "Isn Quantity     : " << cb.cb_isn_quantity << "\n";
	buf << "FB Length        : " << cb.cb_fmt_buf_lng << "\n";
	buf << "RB Length        : " << cb.cb_rec_buf_lng << "\n";
	buf << "SB Length        : " << cb.cb_sea_buf_lng << "\n";
	buf << "VB Length        : " << cb.cb_val_buf_lng << "\n";
	buf << "IB Length        : " << cb.cb_isn_buf_lng << "\n";
	buf << "Command Option 1 : "
		<< std::setw(10) << BinaryToString(&cb.cb_cop1, 1)
		<< BinaryToHex(&cb.cb_cop1, 1) << "\n";
	buf << "Command Option 2 : "
		<< std::setw(10) << BinaryToString(&cb.cb_cop2, 1)
		<< BinaryToHex(&cb.cb_cop2, 1) << "\n";
	buf << "Additions 1      : "
		<< std::setw(10) << BinaryToString(cb.cb_add1, CB_L_AD1)
		<< BinaryToHex(cb.cb_add1, CB_L_AD1) << "\n";
	buf << "Additions 2      : "
		<< std::setw(10) << BinaryToString(cb.cb_add2, CB_L_AD2)
		<< BinaryToHex(cb.cb_add2, CB_L_AD2) << "\n";
	buf << "Additions 3      : "
		<< std::setw(10) << BinaryToString(cb.cb_add3, CB_L_AD3)
		<< BinaryToHex(cb.cb_add3, CB_L_AD3) << "\n";
	buf << "Additions 4      : "
		<< std::setw(10) << BinaryToString(cb.cb_add4, CB_L_AD4)
		<< BinaryToHex(cb.cb_add4, CB_L_AD4) << "\n";
	buf << "Additions 5      : "
		<< std::setw(10) << BinaryToString(cb.cb_add5, CB_L_AD5)
		<< BinaryToHex(cb.cb_add5, CB_L_AD5) << "\n";
	buf << "Command Time     : " << cb.cb_cmd_time << "\n";
	buf << "User Area        : " << std::setw(10)
		<< BinaryToString(cb.cb_user_area, sizeof(cb.cb_user_area))
		<< BinaryToHex(cb.cb_user_area, sizeof(cb.cb_user_area))
		<< "\n";

	std::string s = buf.str();
	std::string::iterator it = s.begin();
	for (; it != s.end(); it++) {
		if (*it != '\n' && (*it < 32 || !isprint(*it))) {
			*it = '.';
		}
	}

	return scope.Close(v8::String::New(s.c_str()));
}

/*
 * Sets field of the Adabas control block.
 */
static const char*
SetField(unsigned char& field, const v8::Local<v8::Value>& value)
{
	if (!value->IsUint32() || value->Uint32Value() > 0xFF) {
		return "argument must be an unsigned 8-bit integer";
	}
	field = (unsigned char)(value->Uint32Value());
	return NULL;
}

static const char*
SetField(short unsigned int& field, const v8::Local<v8::Value>& value)
{
	if (!value->IsUint32() || value->Uint32Value() > 0xFFFF) {
		return "argument must be an unsigned 16-bit integer";
	}
	field = value->Uint32Value();
	return NULL;
}

static const char*
SetField(unsigned int& field, const v8::Local<v8::Value>& value)
{
	if (!value->IsUint32()) {
		return "argument must be an unsigned 32-bit integer";
	}
	field = value->Uint32Value();
	return NULL;
}

static const char*
SetField(unsigned char* field, unsigned int size,
	const v8::Local<v8::Value>& value)
{
	if (!value->IsString()) {
		return "argument must be a string";
	}

	v8::String::Value stringValue(value);
	uint16_t* buffer = *stringValue;
	unsigned int bufferLength = stringValue.length();
	if (bufferLength != size) {
		return "invalid length of the argument";
	}

	for (unsigned int i = 0; i < size; i++) {
		field[i] = uint8_t(buffer[i]);
	}

	return NULL;
}

static const char*
SetFieldArray(unsigned char* field, unsigned int size,
	const v8::Local<v8::Value>& value)
{
	if (!value->IsArray()) {
		return "argument must be an array";
	}

	v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(value);
	if (array->Length() != size) {
		return "invalid size of the argument";
	}

	for (unsigned int i = 0; i < size; i++) {
		field[i] = array->Get(i)->Uint32Value();
	}

	return NULL;
}

/*
 * Sets pointer to buffer data.
 */
static const char*
SetBuffer(void** field, const v8::Local<v8::Value> &value)
{
	if (!node::Buffer::HasInstance(value)) {
		return "argument must be a buffer";
	}
	*field = node::Buffer::Data(value->ToObject());
	return NULL;
}

/*
 * Returns field value of the Adabas control block.
 */
static v8::Local<v8::Integer>
GetField(const unsigned char& field)
{
	return v8::Integer::New(int(field));
}

static v8::Local<v8::Integer>
GetField(const short unsigned int& field)
{
	return v8::Integer::New(field);
}

static v8::Local<v8::Integer>
GetField(const unsigned int& field)
{
	return v8::Integer::New(field);
}

static v8::Local<v8::String>
GetField(const unsigned char* field, unsigned int size)
{
	return v8::String::New((char*)(field), size);
}

static v8::Local<v8::Array>
GetFieldArray(const unsigned char* field, unsigned int size)
{
	v8::Local<v8::Array> array = v8::Array::New(size);
	for (unsigned int i = 0; i < size; i++) {
		array->Set(i, v8::Integer::New(int(field[i])));
	}
	return array;
}

v8::Handle<v8::Value>
Command::SetCommandCode(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetField(self->m_cb.cb_cmd_code,
		sizeof(self->m_cb.cb_cmd_code), args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetCommandId(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetField(self->m_cb.cb_cmd_id, L_CID, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetDbId(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	short unsigned int dbId;
	const char* rc = SetField(dbId, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}
	short unsigned int fileNo = CB_PHYS_FILE_NR(&self->m_cb) ?
		self->m_cb.alt_cb_file_nr : self->m_cb.cb_file_nr;
	CB_SET_FD(&self->m_cb, dbId, fileNo);

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetFileNo(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	short unsigned int fileNo;
	const char* rc = SetField(fileNo, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}
	short unsigned dbId = CB_PHYS_FILE_NR(&self->m_cb) ?
		self->m_cb.alt_cb_db_id : self->m_cb.cb_db_id;
	CB_SET_FD(&self->m_cb, dbId, fileNo);

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetReturnCode(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	if (CB_PHYS_FILE_NR(&self->m_cb)) {
                return V8_ERROR("return code used as database ID");
	}
	const char* rc = SetField(self->m_cb.cb_return_code, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetIsn(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetField(self->m_cb.cb_isn, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetIsnLowerLimit(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetField(self->m_cb.cb_isn_ll, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetIsnQuantity(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetField(self->m_cb.cb_isn_quantity, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetFormatBufferLength(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetField(self->m_cb.cb_fmt_buf_lng, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetRecordBufferLength(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetField(self->m_cb.cb_rec_buf_lng, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetSearchBufferLength(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetField(self->m_cb.cb_sea_buf_lng, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetValueBufferLength(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetField(self->m_cb.cb_val_buf_lng, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetIsnBufferLength(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetField(self->m_cb.cb_isn_buf_lng, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetCommandOption1(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetField(self->m_cb.cb_cop1, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetCommandOption2(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetField(self->m_cb.cb_cop2, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetAddition1(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetFieldArray(self->m_cb.cb_add1, CB_L_AD1, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetAddition2(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetFieldArray(self->m_cb.cb_add2, CB_L_AD2, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetAddition3(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetFieldArray(self->m_cb.cb_add3, CB_L_AD3, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetAddition4(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetFieldArray(self->m_cb.cb_add4, CB_L_AD4, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetAddition5(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetFieldArray(self->m_cb.cb_add5, CB_L_AD5, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetCommandTime(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetField(self->m_cb.cb_cmd_time, args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetUserArea(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetFieldArray(self->m_cb.cb_user_area,
		sizeof(self->m_cb.cb_user_area), args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetFormatBuffer(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetBuffer(&self->m_buffers[0], args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetRecordBuffer(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetBuffer(&self->m_buffers[1], args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetSearchBuffer(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetBuffer(&self->m_buffers[2], args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetValueBuffer(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetBuffer(&self->m_buffers[3], args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::SetIsnBuffer(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
                return V8_ERROR("wrong number of arguments");
	}

	const char* rc = SetBuffer(&self->m_buffers[4], args[0]);
	if (rc) {
                return V8_ERROR(rc);
	}

	return scope.Close(args.This());
}

v8::Handle<v8::Value>
Command::GetCommandCode(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetField(self->m_cb.cb_cmd_code,
		sizeof(self->m_cb.cb_cmd_code)));
}

v8::Handle<v8::Value>
Command::GetCommandId(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetField(self->m_cb.cb_cmd_id, L_CID));
}

v8::Handle<v8::Value>
Command::GetDbId(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	if (CB_PHYS_FILE_NR(&self->m_cb)) {
		return scope.Close(GetField(self->m_cb.alt_cb_db_id));
	}
	return scope.Close(GetField(self->m_cb.cb_db_id));
}

v8::Handle<v8::Value>
Command::GetFileNo(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	if (CB_PHYS_FILE_NR(&self->m_cb)) {
		return scope.Close(GetField(self->m_cb.alt_cb_file_nr));
	}
	return scope.Close(GetField(self->m_cb.cb_file_nr));
}

v8::Handle<v8::Value>
Command::GetReturnCode(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	if (CB_PHYS_FILE_NR(&self->m_cb)) {
                return V8_ERROR("return code used as database ID");
	}
	return scope.Close(GetField(self->m_cb.cb_return_code));
}

v8::Handle<v8::Value>
Command::GetIsn(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetField(self->m_cb.cb_isn));
}

v8::Handle<v8::Value>
Command::GetIsnLowerLimit(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetField(self->m_cb.cb_isn_ll));
}

v8::Handle<v8::Value>
Command::GetIsnQuantity(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetField(self->m_cb.cb_isn_quantity));
}

v8::Handle<v8::Value>
Command::GetFormatBufferLength(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetField(self->m_cb.cb_fmt_buf_lng));
}

v8::Handle<v8::Value>
Command::GetRecordBufferLength(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetField(self->m_cb.cb_rec_buf_lng));
}

v8::Handle<v8::Value>
Command::GetSearchBufferLength(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetField(self->m_cb.cb_sea_buf_lng));
}

v8::Handle<v8::Value>
Command::GetValueBufferLength(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetField(self->m_cb.cb_val_buf_lng));
}

v8::Handle<v8::Value>
Command::GetIsnBufferLength(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetField(self->m_cb.cb_isn_buf_lng));
}

v8::Handle<v8::Value>
Command::GetCommandOption1(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetField(self->m_cb.cb_cop1));
}

v8::Handle<v8::Value>
Command::GetCommandOption2(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetField(self->m_cb.cb_cop2));
}

v8::Handle<v8::Value>
Command::GetAddition1(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetFieldArray(self->m_cb.cb_add1, CB_L_AD1));
}

v8::Handle<v8::Value>
Command::GetAddition2(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetFieldArray(self->m_cb.cb_add2, CB_L_AD2));
}

v8::Handle<v8::Value>
Command::GetAddition3(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetFieldArray(self->m_cb.cb_add3, CB_L_AD3));
}

v8::Handle<v8::Value>
Command::GetAddition4(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetFieldArray(self->m_cb.cb_add4, CB_L_AD4));
}

v8::Handle<v8::Value>
Command::GetAddition5(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetFieldArray(self->m_cb.cb_add5, CB_L_AD5));
}

v8::Handle<v8::Value>
Command::GetCommandTime(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetField(self->m_cb.cb_cmd_time));
}

v8::Handle<v8::Value>
Command::GetUserArea(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(GetFieldArray(self->m_cb.cb_user_area,
		sizeof(self->m_cb.cb_user_area)));
}

v8::Handle<v8::Value>
Command::GetFormatBuffer(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(v8::String::New("not implemented"));
}

v8::Handle<v8::Value>
Command::GetRecordBuffer(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(v8::String::New("not implemented"));
}

v8::Handle<v8::Value>
Command::GetSearchBuffer(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(v8::String::New("not implemented"));
}

v8::Handle<v8::Value>
Command::GetValueBuffer(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(v8::String::New("not implemented"));
}

v8::Handle<v8::Value>
Command::GetIsnBuffer(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Command* self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 0) {
                return V8_ERROR("wrong number of arguments");
	}

	return scope.Close(v8::String::New("not implemented"));
}

} // namespace node_adabas
