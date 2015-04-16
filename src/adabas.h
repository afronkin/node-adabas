#ifndef NODE_ADABAS_SRC_ADABAS_H
#define NODE_ADABAS_SRC_ADABAS_H

#include <deque>
#include <node.h>
#include <queue>
#include <vector>

#include "command.h"

namespace node_adabas {

/*
 * Wrapper class for the Adabas database.
 */
class Adabas : public node::ObjectWrap {
public:
	struct Request {
		Command* commandPtr;
		int rc;
		v8::Persistent<v8::Function> callback;
	};

	struct Thread {
		Adabas* self;

		// Flag is true when request already running.
		bool busy;

		/* The thread internal variables. */
		uv_thread_t threadId;
		uv_loop_t* threadLoop;
		uv_sem_t threadExitSemaphore;
		uv_async_t* exitMessage;
		uv_async_t* execMessage;
		uv_async_t* execFinishedMessage;
		uv_cond_t execEndCond;
	};

private:
	static v8::Persistent<v8::Function> constructor;

	std::vector<Thread> m_threads;
	std::queue<Request> m_requests;
	std::deque<Request> m_finishedRequests;

private:
	Adabas();
	~Adabas();

	static void ThreadEventLoop(void* data);
	static void ThreadOnExit(uv_async_t* handle, int status);
	static void ThreadOnExec(uv_async_t* handle, int status);
	static void OnExecFinished(uv_async_t* handle, int status);

	static v8::Handle<v8::Value> New(const v8::Arguments& args);
	static v8::Handle<v8::Value> Close(const v8::Arguments& args);
	static v8::Handle<v8::Value> Exec(const v8::Arguments& args);

public:
	static void Initialize(v8::Handle<v8::Object> exports);
	void Finalize(void);
};

} // namespace node_adabas

#endif // NODE_ADABAS_SRC_ADABAS_H
