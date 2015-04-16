#ifndef NODE_ADABAS_SRC_COMMAND_H
#define NODE_ADABAS_SRC_COMMAND_H

#ifndef CE_VOID
#define CE_VOID void
#endif // CE_VOID

extern "C" {
#include <adabasx.h>
}

#include <node.h>
#include <vector>

namespace node_adabas {

/*
 * Wrapper class for the Adabas command.
 */
class Command : public node::ObjectWrap {
protected:
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
	static v8::Persistent<v8::Function> constructor;

public:
	/*
	 * Adabas direct call control block ('cb' is used in Adabas headers).
	 */
	CB_PAR m_cb;

	/*
	 * Adabas direct call buffers.
	 */
	void *m_buffers[5];

private:
	Command();

	static v8::Handle<v8::Value> New(const v8::Arguments& args);
	static v8::Handle<v8::Value> Clear(const v8::Arguments& args);
	static v8::Handle<v8::Value> ToString(const v8::Arguments& args);

	static v8::Handle<v8::Value>
		SetCommandCode(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetCommandId(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetDbId(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetFileNo(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetReturnCode(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetIsn(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetIsnLowerLimit(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetIsnQuantity(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetFormatBufferLength(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetRecordBufferLength(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetSearchBufferLength(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetValueBufferLength(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetIsnBufferLength(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetCommandOption1(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetCommandOption2(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetAddition1(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetAddition2(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetAddition3(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetAddition4(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetAddition5(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetCommandTime(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetUserArea(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetFormatBuffer(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetRecordBuffer(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetSearchBuffer(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetValueBuffer(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		SetIsnBuffer(const v8::Arguments& args);

	static v8::Handle<v8::Value>
		GetCommandCode(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetCommandId(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetDbId(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetFileNo(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetReturnCode(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetIsn(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetIsnLowerLimit(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetIsnQuantity(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetFormatBufferLength(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetRecordBufferLength(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetSearchBufferLength(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetValueBufferLength(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetIsnBufferLength(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetCommandOption1(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetCommandOption2(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetAddition1(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetAddition2(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetAddition3(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetAddition4(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetAddition5(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetCommandTime(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetUserArea(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetFormatBuffer(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetRecordBuffer(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetSearchBuffer(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetValueBuffer(const v8::Arguments& args);
	static v8::Handle<v8::Value>
		GetIsnBuffer(const v8::Arguments& args);

public:
	static void Initialize(v8::Handle<v8::Object> exports);
	static v8::Handle<v8::Value> NewInstance(const v8::Arguments& args);
};

} // namespace node_adabas

#endif // NODE_ADABAS_SRC_COMMAND_H
