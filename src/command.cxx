/*
 * Copyright (c) 2013, Alexander Fronkin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iomanip>
#include <node.h>
#include <node_buffer.h>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include "command.h"

namespace node_adabas {

// Adabas buffer record.
struct AdabasBuffer {
	Local<Object> object;
	char *data;
	int length;
};
typedef struct AdabasBuffer AdabasBuffer;

// Private data for command 'exec'.
struct ExecData {
	uv_work_t m_request;
	Command *m_command;
	Persistent<Function> m_callback;

	ExecData(Command *command, Handle<Function> callback)
		: m_command(command)
	{
		m_request.data = this;
		m_callback = Persistent<Function>::New(callback);
	}

	virtual ~ExecData()
	{
		m_callback.Dispose();
	}
};
typedef struct ExecData ExecData;

} // namespace node_adabas

using namespace node_adabas;

Persistent<FunctionTemplate> Command::constructor_template;

/*
 * Set field in Adabac control block.
 */
static bool
SetField(unsigned char &field, Local<Value> &fieldValue)
{
	if (!fieldValue->IsUint32() || fieldValue->Uint32Value() > 0xFF) {
		char *message = "Value must be an unsigned 8-bit integer.";
		ThrowException(Exception::TypeError(String::New(message)));
		return false;
	}

	field = (unsigned char) fieldValue->Uint32Value();
	return true;
}

static bool
SetField(short unsigned int &field, Local<Value> &fieldValue)
{
	if (!fieldValue->IsUint32() || fieldValue->Uint32Value() > 0xFFFF) {
		char *message = "Value must be an unsigned 16-bit integer.";
		ThrowException(Exception::TypeError(String::New(message)));
		return false;
	}

	field = fieldValue->Uint32Value();
	return true;
}

static bool
SetField(unsigned int &field, Local<Value> &fieldValue)
{
	if (!fieldValue->IsUint32()) {
		char *message = "Value must be an unsigned 32-bit integer.";
		ThrowException(Exception::TypeError(String::New(message)));
		return false;
	}

	field = fieldValue->Uint32Value();
	return true;
}

static bool
SetField(unsigned char *field, unsigned int fieldSize,
	Local<Value> &fieldValue)
{
	if (!fieldValue->IsString()) {
		char *message = "Value must be a string.";
		ThrowException(Exception::TypeError(String::New(message)));
		return false;
	}

	String::Value stringValue(fieldValue);
	uint16_t *buffer = *stringValue;
	unsigned int bufferLength = stringValue.length();
	if (bufferLength != fieldSize) {
		char message[80];
		snprintf(message, sizeof(message),
			"Value must be a string of %d characters.", fieldSize);
		ThrowException(Exception::TypeError(String::New(message)));
		return false;
	}

	for (unsigned int i = 0; i < fieldSize; i++) {
		field[i] = (uint8_t) buffer[i];
	}

	return true;
}

static bool
SetFieldArray(unsigned char *field, unsigned int fieldSize,
	Local<Value> &fieldValue)
{
	if (!fieldValue->IsArray()) {
		char *message = "Value must be an array.";
		ThrowException(Exception::TypeError(String::New(message)));
		return false;
	}

	Local<Array> array = Local<Array>::Cast(fieldValue);
	if (array->Length() != fieldSize) {
		char message[80];
		snprintf(message, sizeof(message),
			"Value must be an array with %d elements.", fieldSize);
		ThrowException(Exception::TypeError(String::New(message)));
		return false;
	}

	for (unsigned int i = 0; i < fieldSize; i++) {
		field[i] = array->Get(i)->Uint32Value();
	}

	return true;
}

/*
 * Get field from Adabas control block.
 */
static Local<Integer>
GetField(unsigned char &field)
{
	return Integer::New((int) field);
}

static Local<Integer>
GetField(short unsigned int &field)
{
	return Integer::New(field);
}

static Local<Integer>
GetField(unsigned int &field)
{
	return Integer::New(field);
}

static Local<String>
GetField(unsigned char *field, unsigned int fieldSize)
{
	return String::New((char *) field, fieldSize);
}

static Local<Array>
GetFieldArray(unsigned char *field, unsigned int fieldSize)
{
	Local<Array> array = Array::New(fieldSize);
	for (unsigned int i = 0; i < fieldSize; i++) {
		array->Set(i, Integer::New((int) field[i]));
	}
	return array;
}

/*
 * Set pointer to buffer data.
 */
static bool
SetBuffer(void **field, Local<Value> &bufferValue)
{
	if (!Buffer::HasInstance(bufferValue)) {
		char *message = "Value must be a buffer.";
		ThrowException(Exception::TypeError(String::New(message)));
		return false;
	}

	Local<Object> bufferObject = bufferValue->ToObject();
	*field = Buffer::Data(bufferObject);

	return true;
}

/*
 * Convert binary data to ASCII string.
 */
static std::string
BinaryToString(unsigned char *data, unsigned int dataSize)
{
	std::string result;
	for (unsigned int i = 0; i < dataSize; i++) {
		result += (int) data[i] < 32 ? '.' : data[i];
	}
	return result;
}

/*
 * Convert binary data to hexadecimal codes string.
 */
static std::string
BinaryToHex(unsigned char *data, unsigned int dataSize)
{
	std::string result;
	char buf[4];
	for (unsigned int i = 0; i < dataSize; i++) {
		sprintf(buf, i == 0 ? "%02X" : " %02X", data[i]);
		result.append(buf);
	}
	return result;
}

/*
 * Constructor.
 */
Command::Command() : ObjectWrap(), m_ready(true)
{
	Clear();
}

/*
 * Clear object data.
 */
void
Command::Clear(void)
{
	memset(&m_cb, 0, sizeof(CB_PAR));

	for (int i = 0; i < 5; i++) {
		m_buffers[i] = NULL;
	}

	m_callResult = ADA_SUCCESS;
}

/*
 * Create new instance of this object.
 */
Handle<Value>
Command::New(const Arguments &args)
{
	HandleScope scope;

	// Check if this function were called from wrong place.
	if (!args.IsConstructCall()) {
		Local<String> message =
			String::New("Use the new operator to create objects");
		return ThrowException(Exception::Error(message));
	}

	// Create new C++ object and wrap it in JS object.
	Command *self = new Command();
	self->Wrap(args.This());

	return args.This();
}

/*
 * Execute Adabas direct call command asyncronously (using thread pool).
 */
Handle<Value>
Command::Exec(const Arguments &args)
{
	HandleScope scope;
	Command *self = ObjectWrap::Unwrap<Command>(args.This());

	// Check if another command already running.
	if (!self->m_ready) {
		Local<String> message = String::New(
			"Another command already running.");
		return ThrowException(Exception::Error(message));
	}

	// Get Adabas buffers.
	/*
	AdabasBuffer adabasBuffers[5];
	for (int i = 0; i < 5; i++) {
		ArgumentToBuffer(args, 0, adabasBuffers[0]);
	} */

	// Get callback function.
	Local<Function> callback;
	if (args.Length() > 0 && args[0]->IsFunction()) {
		callback = Local<Function>::Cast(args[0]);
	}

	// Set flag to false (prevent parallel execution of commands).
	self->m_ready = false;

	// Create worker (new thread).
	ExecData *execData = new ExecData(self, callback);
	int status = uv_queue_work(uv_default_loop(),
		&execData->m_request, ExecWork, ExecWorkAfter);
	assert(status == 0);
	self->Ref();

	return args.This();
}

/*
 * The "do work" callback for Adabas direct call.
 */
void
Command::ExecWork(uv_work_t *req)
{
	ExecData *execData = static_cast<ExecData *>(req->data);
	Command *self = execData->m_command;

	// Execute Adabas direct call.
	self->m_callResult = adabas(&self->m_cb,
		self->m_buffers[0],
		self->m_buffers[1],
		self->m_buffers[2],
		self->m_buffers[3],
		self->m_buffers[4]);

#if 0
	fprintf(stderr, "exec: %.2s %d\n",
		self->m_cb.cb_cmd_code,
		self->m_cb.cb_return_code);
	sleep(2);
#endif // 0
}

/*
 * The "after work" callback for Adabas direct call.
 */
void
Command::ExecWorkAfter(uv_work_t *req)
{
	HandleScope scope;
	ExecData *execData = static_cast<ExecData *>(req->data);
	Command *self = execData->m_command;

#if 0
	for (int i = 0; i < 5; i++) {
		fprintf(stderr, "buf%d:%.*s\n", i, self->m_cb.cb_buf_lng[i],
			(char *) self->m_buffers[i]);
	}
#endif // 0

	self->m_ready = true;

	// Create 'exception' for callback and event functions.
	Local<Value> exception;
	if (self->m_callResult == ADA_SUCCESS
		&& self->m_cb.cb_return_code == ADA_NORMAL)
	{
		exception = Local<Value>::New(Null());
	} else {
		char message[80];
		sprintf(message,
			"Adabas direct call failed (command=%.2s, code=%d).",
			self->m_cb.cb_cmd_code, self->m_cb.cb_return_code);
		exception = Exception::Error(String::New(message));
	}

	// Execute callback function with result code of Adabas direct call.
	if (!execData->m_callback.IsEmpty()) {
		Local<Value> callback_args[1] = { exception };

		TryCatch try_catch;
		execData->m_callback->Call(self->handle_, 1, callback_args);
		if (try_catch.HasCaught()) {
			FatalException(try_catch);
		}
	}

	// Emit event with result code of Adabas direct call.
	// EMIT_EVENT(self->handle_, 2, event_args);
	Local<Function> emitCallback = Local<Function>::Cast(
		self->handle_->Get(String::NewSymbol("emit")));
	if (!emitCallback.IsEmpty()) {
		Local<Value> event_args[] = {
			String::New("exec"),
			exception
		};

		TryCatch try_catch;
		emitCallback->Call(self->handle_, 2, event_args);
		if (try_catch.HasCaught()) {
			FatalException(try_catch);
		}
	}

	// Cleanup.
	delete execData;
	self->Unref();
}

/*
 * Clear field in Adabas control block.
 */
Handle<Value>
Command::Clear(const Arguments &args)
{
	HandleScope scope;
	Command *self = ObjectWrap::Unwrap<Command>(args.This());

	if (!self->m_ready) {
		Local<String> message = String::New(
			"Command already running.");
		return ThrowException(Exception::Error(message));
	}

	self->Clear();
	return True();
}

/*
 * Set value of specified field in Adabas control block.
 */
Handle<Value>
Command::Set(const Arguments &args)
{
	HandleScope scope;
	Command *self = ObjectWrap::Unwrap<Command>(args.This());

	if (!self->m_ready) {
		Local<String> message = String::New(
			"Command already running.");
		return ThrowException(Exception::Error(message));
	}

	if (args.Length() != 2) {
		Local<String> message = String::New(
			"Invalid number of arguments.");
		return ThrowException(Exception::SyntaxError(message));
	}
	if (!args[0]->IsUint32()) {
		Local<String> message = String::New(
			"[fieldCode] must be an 32-bit unsigned integer.");
		return ThrowException(Exception::TypeError(message));
	}

	int fieldCode = args[0]->Uint32Value();
	Local<Value> fieldValue = args[1];
	CB_PAR *pcb = &self->m_cb;
	short unsigned int dbId, fileNo;

	switch (fieldCode) {
	case COMMAND_CODE:
		SetField(pcb->cb_cmd_code, sizeof(pcb->cb_cmd_code),
			fieldValue);
		break;
	case COMMAND_ID:
		SetField(pcb->cb_cmd_id, L_CID, fieldValue);
		break;
	case DB_ID:
		SetField(dbId, fieldValue);
		fileNo = CB_PHYS_FILE_NR(pcb)
			? pcb->alt_cb_file_nr : pcb->cb_file_nr;
		CB_SET_FD(pcb, dbId, fileNo);
		break;
	case FILE_NO:
		SetField(fileNo, fieldValue);
		dbId = CB_PHYS_FILE_NR(pcb)
			? pcb->alt_cb_db_id : pcb->cb_db_id;
		CB_SET_FD(pcb, dbId, fileNo);
		break;
	case RETURN_CODE:
		if (CB_PHYS_FILE_NR(pcb)) {
			char *message = "This field is used as database ID.";
			return ThrowException(
				Exception::Error(String::New(message)));
		}
		SetField(pcb->cb_return_code, fieldValue);
		break;
	case ISN:
		SetField(pcb->cb_isn, fieldValue);
		break;
	case ISN_LOWER_LIMIT:
		SetField(pcb->cb_isn_ll, fieldValue);
		break;
	case ISN_QUANTITY:
		SetField(pcb->cb_isn_quantity, fieldValue);
		break;
	case FORMAT_BUFFER_LENGTH:
		SetField(pcb->cb_fmt_buf_lng, fieldValue);
		break;
	case RECORD_BUFFER_LENGTH:
		SetField(pcb->cb_rec_buf_lng, fieldValue);
		break;
	case SEARCH_BUFFER_LENGTH:
		SetField(pcb->cb_sea_buf_lng, fieldValue);
		break;
	case VALUE_BUFFER_LENGTH:
		SetField(pcb->cb_val_buf_lng, fieldValue);
		break;
	case ISN_BUFFER_LENGTH:
		SetField(pcb->cb_isn_buf_lng, fieldValue);
		break;
	case COMMAND_OPTION1:
		SetField(pcb->cb_cop1, fieldValue);
		break;
	case COMMAND_OPTION2:
		SetField(pcb->cb_cop2, fieldValue);
		break;
	case ADDITION1:
		SetFieldArray(pcb->cb_add1, CB_L_AD1, fieldValue);
		break;
	case ADDITION2:
		SetFieldArray(pcb->cb_add2, CB_L_AD2, fieldValue);
		break;
	case ADDITION3:
		SetFieldArray(pcb->cb_add3, CB_L_AD3, fieldValue);
		break;
	case ADDITION4:
		SetFieldArray(pcb->cb_add4, CB_L_AD4, fieldValue);
		break;
	case ADDITION5:
		SetFieldArray(pcb->cb_add5, CB_L_AD5, fieldValue);
		break;
	case COMMAND_TIME:
		SetField(pcb->cb_cmd_time, fieldValue);
		break;
	case USER_AREA:
		SetFieldArray(pcb->cb_user_area, sizeof(pcb->cb_user_area),
			fieldValue);
		break;
	case FORMAT_BUFFER:
		SetBuffer(&self->m_buffers[0], fieldValue);
		break;
	case RECORD_BUFFER:
		SetBuffer(&self->m_buffers[1], fieldValue);
		break;
	case SEARCH_BUFFER:
		SetBuffer(&self->m_buffers[2], fieldValue);
		break;
	case VALUE_BUFFER:
		SetBuffer(&self->m_buffers[3], fieldValue);
		break;
	case ISN_BUFFER:
		SetBuffer(&self->m_buffers[4], fieldValue);
		break;
	}

	return True();
}

/*
 * Get value of specified field in Adabas control block.
 */
Handle<Value>
Command::Get(const Arguments &args)
{
	HandleScope scope;
	Command *self = ObjectWrap::Unwrap<Command>(args.This());

	if (args.Length() != 1) {
		Local<String> message = String::New(
			"Invalid number of arguments.");
		return ThrowException(Exception::SyntaxError(message));
	}
	if (!args[0]->IsUint32()) {
		Local<String> message = String::New(
			"[fieldCode] must be an 32-bit unsigned integer.");
		return ThrowException(Exception::TypeError(message));
	}

	int fieldCode = args[0]->Uint32Value();
	CB_PAR *pcb = &self->m_cb;
	Local<Array> arrayValue;

	switch (fieldCode) {
	case COMMAND_CODE:
		return scope.Close(GetField(pcb->cb_cmd_code,
			sizeof(pcb->cb_cmd_code)));
	case COMMAND_ID:
		return scope.Close(GetField(pcb->cb_cmd_id, L_CID));
	case DB_ID:
		if (CB_PHYS_FILE_NR(pcb)) {
			return scope.Close(GetField(pcb->alt_cb_db_id));
		}
		return scope.Close(GetField(pcb->cb_db_id));
	case FILE_NO:
		if (CB_PHYS_FILE_NR(pcb)) {
			return scope.Close(GetField(pcb->alt_cb_file_nr));
		}
		return scope.Close(GetField(pcb->cb_file_nr));
	case RETURN_CODE:
		if (CB_PHYS_FILE_NR(pcb)) {
			char *message = "This field is used as database ID.";
			return ThrowException(
				Exception::Error(String::New(message)));
		}
		return scope.Close(GetField(pcb->cb_return_code));
	case ISN:
		return scope.Close(GetField(pcb->cb_isn));
	case ISN_LOWER_LIMIT:
		return scope.Close(GetField(pcb->cb_isn_ll));
	case ISN_QUANTITY:
		return scope.Close(GetField(pcb->cb_isn_quantity));
	case FORMAT_BUFFER_LENGTH:
		return scope.Close(GetField(pcb->cb_fmt_buf_lng));
	case RECORD_BUFFER_LENGTH:
		return scope.Close(GetField(pcb->cb_rec_buf_lng));
	case SEARCH_BUFFER_LENGTH:
		return scope.Close(GetField(pcb->cb_sea_buf_lng));
	case VALUE_BUFFER_LENGTH:
		return scope.Close(GetField(pcb->cb_val_buf_lng));
	case ISN_BUFFER_LENGTH:
		return scope.Close(GetField(pcb->cb_isn_buf_lng));
	case COMMAND_OPTION1:
		return scope.Close(GetField(pcb->cb_cop1));
	case COMMAND_OPTION2:
		return scope.Close(GetField(pcb->cb_cop2));
	case ADDITION1:
		return scope.Close(GetFieldArray(pcb->cb_add1, CB_L_AD1));
	case ADDITION2:
		return scope.Close(GetFieldArray(pcb->cb_add2, CB_L_AD2));
	case ADDITION3:
		return scope.Close(GetFieldArray(pcb->cb_add3, CB_L_AD3));
	case ADDITION4:
		return scope.Close(GetFieldArray(pcb->cb_add4, CB_L_AD4));
	case ADDITION5:
		return scope.Close(GetFieldArray(pcb->cb_add5, CB_L_AD5));
	case COMMAND_TIME:
		return scope.Close(GetField(pcb->cb_cmd_time));
	case USER_AREA:
		return scope.Close(GetFieldArray(pcb->cb_user_area,
			sizeof(pcb->cb_user_area)));
	case FORMAT_BUFFER:
		break;
	case RECORD_BUFFER:
		break;
	case SEARCH_BUFFER:
		break;
	case VALUE_BUFFER:
		break;
	case ISN_BUFFER:
		break;
	}

	Local<String> message = String::New(
		"Invalid code of field specified.");
	return ThrowException(Exception::RangeError(message));
}

/*
 * Convert Adabas control block to string representation.
 * 'cb' stands for 'control block' (as in Adabas headers).
 */
Handle<Value>
Command::ToString(const Arguments &args)
{
	HandleScope scope;
	Command *self = ObjectWrap::Unwrap<Command>(args.This());
	CB_PAR &cb = self->m_cb;
	std::ostringstream buf;

	buf.setf(std::ios_base::left);

	buf << "Command Code     : "
		<< std::setw(10) << BinaryToString(cb.cb_cmd_code, 2)
		<< BinaryToHex(cb.cb_cmd_code, 2) << std::endl;
	buf << "Command Id       : "
		<< std::setw(10) << BinaryToString(cb.cb_cmd_id, L_CID)
		<< BinaryToHex(cb.cb_cmd_id, L_CID) << std::endl;
	buf << "File Number      : "
		<< (CB_PHYS_FILE_NR(&cb) ? cb.alt_cb_file_nr : cb.cb_file_nr)
		<< std::endl;
	buf << "Database Id      : "
		<< (CB_PHYS_FILE_NR(&cb) ? cb.alt_cb_db_id : cb.cb_db_id)
		<< std::endl;
	buf << "Response Code    : " << cb.cb_return_code << std::endl;
	buf << "Isn              : " << cb.cb_isn << std::endl;
	buf << "Isn Lower Limit  : " << cb.cb_isn_ll << std::endl;
	buf << "Isn Quantity     : " << cb.cb_isn_quantity << std::endl;
	buf << "FB Length        : " << cb.cb_fmt_buf_lng << std::endl;
	buf << "RB Length        : " << cb.cb_rec_buf_lng << std::endl;
	buf << "SB Length        : " << cb.cb_sea_buf_lng << std::endl;
	buf << "VB Length        : " << cb.cb_val_buf_lng << std::endl;
	buf << "IB Length        : " << cb.cb_isn_buf_lng << std::endl;
	buf << "Command Option 1 : "
		<< std::setw(10) << BinaryToString(&cb.cb_cop1, 1)
		<< BinaryToHex(&cb.cb_cop1, 1) << std::endl;
	buf << "Command Option 2 : "
		<< std::setw(10) << BinaryToString(&cb.cb_cop2, 1)
		<< BinaryToHex(&cb.cb_cop2, 1) << std::endl;
	buf << "Additions 1      : "
		<< std::setw(10) << BinaryToString(cb.cb_add1, CB_L_AD1)
		<< BinaryToHex(cb.cb_add1, CB_L_AD1) << std::endl;
	buf << "Additions 2      : "
		<< std::setw(10) << BinaryToString(cb.cb_add2, CB_L_AD2)
		<< BinaryToHex(cb.cb_add2, CB_L_AD2) << std::endl;
	buf << "Additions 3      : "
		<< std::setw(10) << BinaryToString(cb.cb_add3, CB_L_AD3)
		<< BinaryToHex(cb.cb_add3, CB_L_AD3) << std::endl;
	buf << "Additions 4      : "
		<< std::setw(10) << BinaryToString(cb.cb_add4, CB_L_AD4)
		<< BinaryToHex(cb.cb_add4, CB_L_AD4) << std::endl;
	buf << "Additions 5      : "
		<< std::setw(10) << BinaryToString(cb.cb_add5, CB_L_AD5)
		<< BinaryToHex(cb.cb_add5, CB_L_AD5) << std::endl;
	buf << "Command Time     : " << cb.cb_cmd_time << std::endl;
	buf << "User Area        : " << std::setw(10)
		<< BinaryToString(cb.cb_user_area, sizeof(cb.cb_user_area))
		<< BinaryToHex(cb.cb_user_area, sizeof(cb.cb_user_area))
		<< std::endl;

	std::string str = buf.str();
	std::string::iterator it = str.begin();
	while (it != str.end()) {
		if (*it != '\n' && (*it < 32 || !isprint(*it))) {
			*it = '.';
		}
		it++;
	}

	return scope.Close(String::New(str.c_str()));
}
