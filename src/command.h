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

#ifndef NODE_ADABAS_SRC_COMMAND_H
#define NODE_ADABAS_SRC_COMMAND_H

extern "C" {
#include <adabasx.h>
}
#include <node.h>
#include <string>

using namespace v8;
using namespace node;

namespace node_adabas {

class Command;

/*
 * Adabas direct call command class.
 */
class Command : public ObjectWrap {
private:
	// Codes of Adabas control block fields.
	enum {
		COMMAND_CODE,
		COMMAND_ID,
		DB_ID,
		FILE_NO,
		RETURN_CODE,
		ISN,
		ISN_LOWER_LIMIT,
		ISN_QUANTITY,
		FORMAT_BUFFER_LENGTH,
		RECORD_BUFFER_LENGTH,
		SEARCH_BUFFER_LENGTH,
		VALUE_BUFFER_LENGTH,
		ISN_BUFFER_LENGTH,
		COMMAND_OPTION1,
		COMMAND_OPTION2,
		ADDITION1,
		ADDITION2,
		ADDITION3,
		ADDITION4,
		ADDITION5,
		COMMAND_TIME,
		USER_AREA,

		FORMAT_BUFFER,
		RECORD_BUFFER,
		SEARCH_BUFFER,
		VALUE_BUFFER,
		ISN_BUFFER
	};

private:
	static Persistent<FunctionTemplate> constructor_template;

	/*
	 * Adabas direct call control block ('cb' is used in Adabas headers).
	 */
	CB_PAR m_cb;

	/*
	 * Adabas direct call buffers.
	 */
	void *m_buffers[5];

	// Adabas thread identifier.
	uv_thread_t m_adabasThreadId;
	// Adabas thread event loop.
	uv_loop_t *m_adabasThreadLoop;
	// Message handles for Adabas thread.
	uv_async_t m_adabasThreadMsgExit;
	uv_async_t m_adabasThreadMsgExec;
	uv_async_t m_adabasThreadMsgExecFinished;

	// Result of Adabas direct call.
	int m_callResult;

	// This flag is false if Adabas direct call command executing.
	bool m_ready;

public:
	// Initialize module class.
	static void Init(Handle<Object> target);

protected:
	Command();
	~Command();

	// Initialize object instance.
	void Open(void);
	// Clear object data.
	void Clear(void);
	// Cleanup.
	void Cleanup(void);

	// Adabas thread event loop.
	static void AdabasThreadEventLoop(void *data);
	// Process message 'exit' in Adabas thread.
	static void AdabasThreadCallbackExit(uv_async_t *handle, int status);
	// Process message 'exec' in Adabas thread.
	static void AdabasThreadCallbackExec(uv_async_t *handle, int status);
	// Process message 'exec finished' in main thread.
	static void CallbackExecFinished(uv_async_t *handle, int status);

	// Create new instance of this object.
	static Handle<Value> New(const Arguments &args);
	// Initialize object instance.
	static Handle<Value> Open(const Arguments &args);
	// Close instance of this object.
	static Handle<Value> Close(const Arguments &args);
	// Execute Adabas direct call command.
	static Handle<Value> Exec(const Arguments &args);

	// Clear field in Adabas control block.
	static Handle<Value> Clear(const Arguments &args);
	// Set value of specified field in Adabas control block.
	static Handle<Value> Set(const Arguments &args);
	// Get value of specified field in Adabas control block.
	static Handle<Value> Get(const Arguments &args);

	// Convert Adabas control block to string representation.
	std::string ToString(void);
	static Handle<Value> ToString(const Arguments &args);
};

} // namespace node_adabas

#endif // NODE_ADABAS_SRC_COMMAND_H
