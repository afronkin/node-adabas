#include <cstdlib>
#include <node.h>
#include <set>
#include <string>

#include "adabas.h"
#include "v8_helpers.h"

namespace node_adabas {

static uv_mutex_t requestsMutex;
static uv_mutex_t finishedRequestsMutex;
static uv_mutex_t threadsMutex;

/*
 * Initializes and finalizes static module variables.
 */
static struct ModuleInit {
public:
	ModuleInit()
	{
#ifdef _DEBUG
		fprintf(stderr, "[module-enter]\n");
#endif // _DEBUG

		uv_mutex_init(&requestsMutex);
		uv_mutex_init(&finishedRequestsMutex);
		uv_mutex_init(&threadsMutex);
	}

	~ModuleInit()
	{
		uv_mutex_destroy(&requestsMutex);
		uv_mutex_destroy(&finishedRequestsMutex);
		uv_mutex_destroy(&threadsMutex);

#ifdef _DEBUG
		fprintf(stderr, "[module-exit]\n");
#endif // _DEBUG
	}
} moduleInit;

// Flag raises when module is going to exit.
static bool moduleExitFlag = false;

static struct AdabasObjects : public std::set<Adabas*> {
	~AdabasObjects()
	{
		moduleExitFlag = true;
		for (iterator it = begin(); it != end(); it++) {
			(*it)->Finalize();
		}
	}
} adabasObjects;

v8::Persistent<v8::Function> Adabas::constructor;

/*
 * Constructor.
 */
Adabas::Adabas() :
	ObjectWrap()
{
	adabasObjects.insert(this);

	m_threads.resize(1);
	for (size_t threadNo = 0; threadNo < m_threads.size(); threadNo++) {
		Thread& thread = m_threads[threadNo];

		thread.self = this;
		thread.busy = false;

		thread.threadLoop = uv_loop_new();
		uv_sem_init(&thread.threadExitSemaphore, 0);

		thread.exitMessage = (uv_async_t*) malloc(sizeof(uv_async_t));
		uv_async_init(thread.threadLoop, thread.exitMessage,
			ThreadOnExit);
		thread.exitMessage->data = (void*) &thread;

		thread.execMessage = (uv_async_t*) malloc(sizeof(uv_async_t));
		uv_async_init(thread.threadLoop, thread.execMessage,
			ThreadOnExec);
		uv_unref((uv_handle_t*) thread.execMessage);

		thread.execFinishedMessage =
			(uv_async_t*) malloc(sizeof(uv_async_t));
		uv_async_init(uv_default_loop(), thread.execFinishedMessage,
			OnExecFinished);
		uv_unref((uv_handle_t*) thread.execFinishedMessage);

		uv_cond_init(&thread.execEndCond);

		uv_thread_create(&thread.threadId, ThreadEventLoop,
			(void*) &thread);
	}
}

/*
 * Destructor.
 */
Adabas::~Adabas()
{
	adabasObjects.erase(this);
	Finalize();
}

/*
 * Initializes the Node.js class.
 */
void
Adabas::Initialize(v8::Handle<v8::Object> exports)
{
	// Prepare constructor template.
	v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(New);
	t->SetClassName(v8::String::NewSymbol("Adabas"));
	t->InstanceTemplate()->SetInternalFieldCount(1);

	// Prototype.
	V8_METHOD("close", Close);
	V8_METHOD("exec", Exec);

	// Constants for 'Command option 1'.
	V8_CONSTANT("ADA_KEEP_ISN", ADA_KEEP_ISN);
	V8_CONSTANT("ADA_RESTRICTED", ADA_RESTRICTED);
	V8_CONSTANT("ADA_RETURN_OPT", ADA_RETURN_OPT);
	V8_CONSTANT("ADA_READ_OPT", ADA_READ_OPT);
	V8_CONSTANT("ADA_RREAD_OPT", ADA_RREAD_OPT);
	V8_CONSTANT("ADA_MULTI_FETCH", ADA_MULTI_FETCH);
	V8_CONSTANT("ADA_MRSP_FETCH", ADA_MRSP_FETCH);
	V8_CONSTANT("ADA_SORTED_LIST", ADA_SORTED_LIST);
	// Constants for 'Command option 1' or 'Command option 2'.
	V8_CONSTANT("ADA_HOLD_ISN", ADA_HOLD_ISN);
	V8_CONSTANT("ADA_IFBPOOL", ADA_IFBPOOL);
	V8_CONSTANT("ADA_SCTABLE", ADA_SCTABLE);
	V8_CONSTANT("ADA_ISNTAB", ADA_ISNTAB);
	V8_CONSTANT("ADA_GLOBID", ADA_GLOBID);
	// Constants for 'Command option 2'.
	V8_CONSTANT("ADA_USRDSYSF", ADA_USRDSYSF);
	V8_CONSTANT("ADA_GET_NEXT", ADA_GET_NEXT);
	V8_CONSTANT("ADA_ISN_SEQ", ADA_ISN_SEQ);
	V8_CONSTANT("ADA_USRVALUE", ADA_USRVALUE);
	V8_CONSTANT("ADA_ASCEND", ADA_ASCEND);
	V8_CONSTANT("ADA_DESCEND", ADA_DESCEND);
	V8_CONSTANT("ADA_AND_ISN", ADA_AND_ISN);
	V8_CONSTANT("ADA_OR_ISN", ADA_OR_ISN);
	V8_CONSTANT("ADA_NOT_ISN", ADA_NOT_ISN);
	V8_CONSTANT("ADA_LF_SSP", ADA_LF_SSP);
	V8_CONSTANT("ADA_MC_SHORT", ADA_MC_SHORT);

	/*
	 * Constants for Adabas response codes.
	 */
	// Function values.
	V8_CONSTANT("ADA_SUCCESS", ADA_SUCCESS);
	V8_CONSTANT("ADA_ERROR", ADA_ERROR);
	V8_CONSTANT("ADA_DRVERR", ADA_DRVERR);
	// Success.
	V8_CONSTANT("ADA_NORMAL", ADA_NORMAL);
	// Warnings.
	V8_CONSTANT("ADA_ISNNS", ADA_ISNNS);
	V8_CONSTANT("ADA_FNCMP", ADA_FNCMP);
	V8_CONSTANT("ADA_EOF", ADA_EOF);
	// Errors.
	V8_CONSTANT("ADA_TABT", ADA_TABT);
	V8_CONSTANT("ADA_SUBCMD", ADA_SUBCMD);
	V8_CONSTANT("ADA_INFIN", ADA_INFIN);
	V8_CONSTANT("ADA_FICHA", ADA_FICHA);
	V8_CONSTANT("ADA_ACCUS", ADA_ACCUS);
	V8_CONSTANT("ADA_INCID", ADA_INCID);
	V8_CONSTANT("ADA_IUCID", ADA_IUCID);
	V8_CONSTANT("ADA_CMDINV", ADA_CMDINV);
	V8_CONSTANT("ADA_ISNSV", ADA_ISNSV);
	V8_CONSTANT("ADA_BUISN", ADA_BUISN);
	V8_CONSTANT("ADA_ISNLL", ADA_ISNLL);
	V8_CONSTANT("ADA_BLISN", ADA_BLISN);
	V8_CONSTANT("ADA_DUPVAL", ADA_DUPVAL);
	V8_CONSTANT("ADA_IADD1", ADA_IADD1);
	V8_CONSTANT("ADA_MVOPT", ADA_MVOPT);
	V8_CONSTANT("ADA_ENCINV", ADA_ENCINV);
	V8_CONSTANT("ADA_SYNTX", ADA_SYNTX);
	V8_CONSTANT("ADA_ERFBU", ADA_ERFBU);
	V8_CONSTANT("ADA_DESDE", ADA_DESDE);
	V8_CONSTANT("ADA_FBUSU", ADA_FBUSU);
	V8_CONSTANT("ADA_FCOVL", ADA_FCOVL);
	V8_CONSTANT("ADA_MISFB", ADA_MISFB);
	V8_CONSTANT("ADA_NISNOV", ADA_NISNOV);
	V8_CONSTANT("ADA_FUNAV", ADA_FUNAV);
	V8_CONSTANT("ADA_CORTL", ADA_CORTL);
	V8_CONSTANT("ADA_SYRBO", ADA_SYRBO);
	V8_CONSTANT("ADA_INVRB", ADA_INVRB);
	V8_CONSTANT("ADA_DRBVB", ADA_DRBVB);
	V8_CONSTANT("ADA_RBTS", ADA_RBTS);
	V8_CONSTANT("ADA_RBTL", ADA_RBTL);
	V8_CONSTANT("ADA_IFCTE", ADA_IFCTE);
	V8_CONSTANT("ADA_IVTL", ADA_IVTL);
	V8_CONSTANT("ADA_DSPEC", ADA_DSPEC);
	V8_CONSTANT("ADA_SYSBU", ADA_SYSBU);
	V8_CONSTANT("ADA_ERSBU", ADA_ERSBU);
	V8_CONSTANT("ADA_LSPEC", ADA_LSPEC);
	V8_CONSTANT("ADA_UCIDS", ADA_UCIDS);
	V8_CONSTANT("ADA_NSUQU", ADA_NSUQU);
	V8_CONSTANT("ADA_NSWRK", ADA_NSWRK);
	V8_CONSTANT("ADA_NTWRK", ADA_NTWRK);
	V8_CONSTANT("ADA_EXOVFCB", ADA_EXOVFCB);
	V8_CONSTANT("ADA_OVIDX", ADA_OVIDX);
	V8_CONSTANT("ADA_NSAAD", ADA_NSAAD);
	V8_CONSTANT("ADA_OVFST", ADA_OVFST);
	V8_CONSTANT("ADA_HYXNA", ADA_HYXNA);
	V8_CONSTANT("ADA_HYISN", ADA_HYISN);
	V8_CONSTANT("ADA_OVDVT", ADA_OVDVT);
	V8_CONSTANT("ADA_HYPERR", ADA_HYPERR);
	V8_CONSTANT("ADA_COLERR", ADA_COLERR);
	V8_CONSTANT("ADA_INMEM", ADA_INMEM);
	V8_CONSTANT("ADA_UNIQD", ADA_UNIQD);
	V8_CONSTANT("ADA_IOERR", ADA_IOERR);
	V8_CONSTANT("ADA_INVIS", ADA_INVIS);
	V8_CONSTANT("ADA_INVRF", ADA_INVRF);
	V8_CONSTANT("ADA_LOBERR", ADA_LOBERR);
	V8_CONSTANT("ADA_NLOCK", ADA_NLOCK);
	V8_CONSTANT("ADA_ALOCK", ADA_ALOCK);
	V8_CONSTANT("ADA_BSPEC", ADA_BSPEC);
	V8_CONSTANT("ADA_UBNAC", ADA_UBNAC);
	V8_CONSTANT("ADA_ANACT", ADA_ANACT);
	V8_CONSTANT("ADA_SYSCE", ADA_SYSCE);
	V8_CONSTANT("ADA_NUCLI", ADA_NUCLI);
	V8_CONSTANT("ADA_NSACQ", ADA_NSACQ);
	V8_CONSTANT("ADA_IUBSZ", ADA_IUBSZ);
	V8_CONSTANT("ADA_PENDI", ADA_PENDI);
	V8_CONSTANT("ADA_CANCL", ADA_CANCL);
	V8_CONSTANT("ADA_PRE", ADA_PRE);
	V8_CONSTANT("ADA_POST", ADA_POST);
	V8_CONSTANT("ADA_BPMFU", ADA_BPMFU);
	V8_CONSTANT("ADA_NODESC", ADA_NODESC);
	V8_CONSTANT("ADA_NODV", ADA_NODV);
	V8_CONSTANT("ADA_UQDV", ADA_UQDV);
	V8_CONSTANT("ADA_INRAB", ADA_INRAB);
	V8_CONSTANT("ADA_ISNVAL", ADA_ISNVAL);
	V8_CONSTANT("ADA_DARAB", ADA_DARAB);
	V8_CONSTANT("ADA_INVLIST", ADA_INVLIST);
	V8_CONSTANT("ADA_MISAC", ADA_MISAC);
	V8_CONSTANT("ADA_ETDAT", ADA_ETDAT);
	V8_CONSTANT("ADA_SECUR", ADA_SECUR);
	V8_CONSTANT("ADA_INVPWD", ADA_INVPWD);
	V8_CONSTANT("ADA_NFPWD", ADA_NFPWD);
	V8_CONSTANT("ADA_PWDINU", ADA_PWDINU);
	V8_CONSTANT("ADA_BLOST", ADA_BLOST);
	V8_CONSTANT("ADA_RMUTI", ADA_RMUTI);
	V8_CONSTANT("ADA_NOTYET", ADA_NOTYET);
	V8_CONSTANT("ADA_XA_PROTO", ADA_XA_PROTO);
	V8_CONSTANT("ADA_LODUEX", ADA_LODUEX);
	V8_CONSTANT("ADA_ALLOC", ADA_ALLOC);
	V8_CONSTANT("ADA_GCBEX", ADA_GCBEX);
	V8_CONSTANT("ADA_UTUCB", ADA_UTUCB);
	V8_CONSTANT("ADA_OVUCB", ADA_OVUCB);
	V8_CONSTANT("ADA_IDUCB", ADA_IDUCB);
	V8_CONSTANT("ADA_FCTNY", ADA_FCTNY);
	V8_CONSTANT("ADA_IUCAL", ADA_IUCAL);
	V8_CONSTANT("ADA_CALLINV", ADA_CALLINV);
	V8_CONSTANT("ADA_SYLOD", ADA_SYLOD);
	V8_CONSTANT("ADA_BPOLL", ADA_BPOLL);
	// XA userexit XAUEX_0 return codes.
	V8_CONSTANT("XAUEX_OK", XAUEX_OK);
	V8_CONSTANT("XAUEX_IGNORE", XAUEX_IGNORE);

	constructor = v8::Persistent<v8::Function>::New(t->GetFunction());
	exports->Set(v8::String::NewSymbol("Adabas"), constructor);

	// Initialize generator of the random numbers.
	srand((unsigned int) time(NULL));
}

/*
 * Creates new instance of the object.
 */
v8::Handle<v8::Value>
Adabas::New(const v8::Arguments& args)
{
	v8::HandleScope scope;

	if (!args.IsConstructCall()) {
		return scope.Close(constructor->NewInstance());
	}

	Adabas* self = new Adabas();
	self->Wrap(args.This());

	return args.This();
}

/*
 * Frees the libuv handle.
 */
static void
onHandleClosed(uv_handle_t* handle)
{
	if (handle != NULL) {
		free(handle);
	}
}

/*
 * Finalizes the Node.js object.
 */
void
Adabas::Finalize(void)
{
#ifdef _DEBUG
	fprintf(stderr, "[finalize-begin]\n");
#endif // _DEBUG

	for (size_t threadNo = 0; threadNo < m_threads.size(); threadNo++) {
		Thread& thread = m_threads[threadNo];
		if (thread.threadId == 0) {
			continue;
		}

		uv_unref((uv_handle_t*) thread.execMessage);
		uv_close((uv_handle_t*) thread.execMessage, onHandleClosed);

		uv_unref((uv_handle_t*) thread.execFinishedMessage);
		uv_close((uv_handle_t*) thread.execFinishedMessage,
			onHandleClosed);

#ifdef WIN32
		// Windows terminates a threads before the module exits.
		if (!moduleExitFlag) {
			uv_async_send(thread.exitMessage);
			uv_sem_wait(&thread.threadExitSemaphore);
		}
#else
		// In Linux we don't need to worry about threads
		// prematurely closing.
		uv_async_send(thread.exitMessage);
		uv_sem_wait(&thread.threadExitSemaphore);
#endif
		uv_sem_destroy(&thread.threadExitSemaphore);
		uv_cond_destroy(&thread.execEndCond);

		if (thread.threadId != 0) {
			uv_thread_join(&thread.threadId);
			thread.threadId = 0;
		}

		if (thread.threadLoop != NULL) {
			uv_loop_delete(thread.threadLoop);
			thread.threadLoop = NULL;
		}
	}

#ifdef _DEBUG
	fprintf(stderr, "[finalize-end]\n");
#endif // _DEBUG
}

/*
 * Thread event loop.
 */
void
Adabas::ThreadEventLoop(void* data)
{
#ifdef _DEBUG
	fprintf(stderr, "[thread-begin]\n");
#endif // _DEBUG

	Adabas::Thread& thread = *static_cast<Adabas::Thread*>(data);
	uv_run(thread.threadLoop, UV_RUN_DEFAULT);
	uv_sem_post(&thread.threadExitSemaphore);

#ifdef _DEBUG
	fprintf(stderr, "[thread-end]\n");
#endif // _DEBUG
}

/*
 * Processes message 'exit' in the thread.
 */
void
Adabas::ThreadOnExit(uv_async_t* handle, int status)
{
	Adabas::Thread& thread = *static_cast<Adabas::Thread*>(handle->data);

	uv_unref((uv_handle_t*) thread.exitMessage);
	uv_close((uv_handle_t*) thread.exitMessage, onHandleClosed);

#ifdef _DEBUG
	fprintf(stderr, "[thread-exit]\n");
#endif // _DEBUG
}

/*
 * Processes message 'exec' in the thread.
 */
void
Adabas::ThreadOnExec(uv_async_t* handle, int status)
{
	Adabas::Thread& thread = *static_cast<Adabas::Thread*>(handle->data);
	Adabas* self = thread.self;

	while (self->m_requests.size() > 0) {
		uv_mutex_lock(&requestsMutex);
		Request request = self->m_requests.front();
		self->m_requests.pop();
		uv_mutex_unlock(&requestsMutex);

		// Execute Adabas direct call.
		Command *commandPtr = request.commandPtr;
		request.rc = adabas(
			&commandPtr->m_cb,
			commandPtr->m_buffers[0],
			commandPtr->m_buffers[1],
			commandPtr->m_buffers[2],
			commandPtr->m_buffers[3],
			commandPtr->m_buffers[4]);

		uv_mutex_lock(&finishedRequestsMutex);
		self->m_finishedRequests.push_back(request);
		uv_mutex_unlock(&finishedRequestsMutex);

		if (!request.callback.IsEmpty()) {
			thread.execFinishedMessage->data = (void*) &thread;
			uv_async_send(thread.execFinishedMessage);
		} else {
			uv_mutex_lock(&finishedRequestsMutex);
			uv_cond_signal(&thread.execEndCond);
			uv_mutex_unlock(&finishedRequestsMutex);
		}
	}
}

/*
 * Processes message 'exec finished' in main thread.
 */
void
Adabas::OnExecFinished(uv_async_t* handle, int status)
{
	v8::HandleScope scope;
	Thread& thread = *static_cast<Thread*>(handle->data);
	Adabas* self = thread.self;

	while (self->m_finishedRequests.size() > 0) {
		uv_mutex_lock(&finishedRequestsMutex);
		Request request = self->m_finishedRequests.front();
		self->m_finishedRequests.pop_front();
		uv_mutex_unlock(&finishedRequestsMutex);

		v8::Handle<v8::Function> callback;
		if (!request.callback.IsEmpty()) {
			callback = request.callback;
			request.callback.Dispose();
		}

		thread.self->Unref();
		uv_unref((uv_handle_t*) thread.execMessage);
		uv_unref((uv_handle_t*) thread.execFinishedMessage);
		thread.busy = false;

		if (!callback.IsEmpty()) {
			v8::Local<v8::Value> callbackArgs[] = {
				v8::Number::New(int32_t(request.rc))
			};
			v8::TryCatch try_catch;
			callback->Call(thread.self->handle_, 1, callbackArgs);
			if (try_catch.HasCaught()) {
				node::FatalException(try_catch);
			}
		}
	}
}

/*
 * Closes the Adabas instance.
 */
v8::Handle<v8::Value>
Adabas::Close(const v8::Arguments& args)
{
	v8::HandleScope scope;
	Adabas* self = ObjectWrap::Unwrap<Adabas>(args.This());
	self->Finalize();
	return v8::Undefined();
}

/*
 * Executes Adabas request asyncronously (using thread pool).
 */
v8::Handle<v8::Value>
Adabas::Exec(const v8::Arguments& args)
{
        v8::HandleScope scope;
	Adabas* self = ObjectWrap::Unwrap<Adabas>(args.This());

	unsigned int numArgs = args.Length();
        if (numArgs < 1 || numArgs > 2) {
                return V8_ERROR("wrong number of arguments");
 	}

	Command* commandPtr = NULL;
	if (args[0]->IsObject()) {
		v8::Handle<v8::Object> commandObject = args[0]->ToObject();
		std::string constructorName(*v8::String::Utf8Value(
			commandObject->GetConstructorName()));
		if (constructorName == "Command") {
			commandPtr = node::ObjectWrap::Unwrap<Command>(
				commandObject);
		}
	}
	if (commandPtr == NULL) {
		return V8_ERROR(
			"first argument must be an Adabas control block");
	}

	v8::Handle<v8::Function> callback;
        if (numArgs == 2) {
		if (!args[1]->IsFunction()) {
			return V8_ERROR("second argument must be a callback");
		}
		callback = v8::Handle<v8::Function>::Cast(args[1]);
        }

	if (self->m_requests.size() > 20) {
#if _DEBUG
		fprintf(stderr, "[busy]\n");
#endif // _DEBUG

		if (!callback.IsEmpty()) {
			// Execute the application callback if server is busy.
			v8::Local<v8::Value> exception =
				node::ErrnoException(EBUSY, "ExecRequest");
			v8::Local<v8::Value> callbackArgs[] = { exception };
			v8::TryCatch try_catch;
			callback->Call(self->handle_, 1, callbackArgs);
			if (try_catch.HasCaught()) {
				node::FatalException(try_catch);
			}
		}

		return scope.Close(v8::False());
	}

	// Append request to the queue.
	Request request;
	request.commandPtr = commandPtr;
	request.callback = v8::Persistent<v8::Function>::New(callback);

	uv_mutex_lock(&requestsMutex);
	self->m_requests.push(request);
	uv_mutex_unlock(&requestsMutex);

	// Find the available thread.
	uv_mutex_lock(&threadsMutex);
	size_t threadNo = 0;
	size_t numThreads = self->m_threads.size();
	while (threadNo < numThreads && self->m_threads[threadNo].busy) {
		threadNo++;
	}

	if (threadNo == numThreads) {
		// When all threads is busy, send message to a random thread.
		threadNo = rand() % numThreads;
	}
	Thread& thread = self->m_threads[threadNo];
	thread.busy = true;

	// Increase reference counters.
	self->Ref();
	uv_ref((uv_handle_t*) thread.execMessage);
	uv_ref((uv_handle_t*) thread.execFinishedMessage);

	// Send message 'exec' to the thread.
	thread.execMessage->data = (void*) &thread;
	uv_async_send(thread.execMessage);
	uv_mutex_unlock(&threadsMutex);

	// Wait sync execution semaphore if callback is not defined.
	if (callback.IsEmpty()) {
		uv_mutex_lock(&finishedRequestsMutex);
		while (self->m_finishedRequests.size() == 0) {
			uv_cond_wait(&thread.execEndCond, &finishedRequestsMutex);
		}

		Request finishedRequest = self->m_finishedRequests.back();
		self->m_finishedRequests.pop_back();
		uv_mutex_unlock(&finishedRequestsMutex);

		// Return result code.
		return scope.Close(
			v8::Number::New(int32_t(finishedRequest.rc)));
	}

	return scope.Close(v8::True());
}

} // namespace node_adabas
