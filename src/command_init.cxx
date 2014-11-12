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

#include "command.h"

using namespace node_adabas;

/*
 * Initialize module class.
 */
void
Command::Init(Handle<Object> target)
{
	HandleScope scope;

	Local<FunctionTemplate> t = FunctionTemplate::New(New);

	constructor_template = Persistent<FunctionTemplate>::New(t);
	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("Command"));

	NODE_SET_PROTOTYPE_METHOD(constructor_template, "clear", Clear);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "close", Close);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "exec", Exec);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "get", Get);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "open", Open);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "set", Set);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "toString", ToString);

	// Constants for codes of fields.
	NODE_DEFINE_CONSTANT(target, COMMAND_CODE);
	NODE_DEFINE_CONSTANT(target, COMMAND_ID);
	NODE_DEFINE_CONSTANT(target, DB_ID);
	NODE_DEFINE_CONSTANT(target, FILE_NO);
	NODE_DEFINE_CONSTANT(target, RETURN_CODE);
	NODE_DEFINE_CONSTANT(target, ISN);
	NODE_DEFINE_CONSTANT(target, ISN_LOWER_LIMIT);
	NODE_DEFINE_CONSTANT(target, ISN_QUANTITY);
	NODE_DEFINE_CONSTANT(target, FORMAT_BUFFER_LENGTH);
	NODE_DEFINE_CONSTANT(target, RECORD_BUFFER_LENGTH);
	NODE_DEFINE_CONSTANT(target, SEARCH_BUFFER_LENGTH);
	NODE_DEFINE_CONSTANT(target, VALUE_BUFFER_LENGTH);
	NODE_DEFINE_CONSTANT(target, ISN_BUFFER_LENGTH);
	NODE_DEFINE_CONSTANT(target, COMMAND_OPTION1);
	NODE_DEFINE_CONSTANT(target, COMMAND_OPTION2);
	NODE_DEFINE_CONSTANT(target, ADDITION1);
	NODE_DEFINE_CONSTANT(target, ADDITION2);
	NODE_DEFINE_CONSTANT(target, ADDITION3);
	NODE_DEFINE_CONSTANT(target, ADDITION4);
	NODE_DEFINE_CONSTANT(target, ADDITION5);
	NODE_DEFINE_CONSTANT(target, COMMAND_TIME);
	NODE_DEFINE_CONSTANT(target, USER_AREA);
	// Constants for codes of buffers.
	NODE_DEFINE_CONSTANT(target, FORMAT_BUFFER);
	NODE_DEFINE_CONSTANT(target, RECORD_BUFFER);
	NODE_DEFINE_CONSTANT(target, SEARCH_BUFFER);
	NODE_DEFINE_CONSTANT(target, VALUE_BUFFER);
	NODE_DEFINE_CONSTANT(target, ISN_BUFFER);

	// Constants for 'Command option 1'.
	NODE_DEFINE_CONSTANT(target, ADA_KEEP_ISN);
	NODE_DEFINE_CONSTANT(target, ADA_RESTRICTED);
	NODE_DEFINE_CONSTANT(target, ADA_RETURN_OPT);
	NODE_DEFINE_CONSTANT(target, ADA_READ_OPT);
	NODE_DEFINE_CONSTANT(target, ADA_RREAD_OPT);
	NODE_DEFINE_CONSTANT(target, ADA_MULTI_FETCH);
	NODE_DEFINE_CONSTANT(target, ADA_MRSP_FETCH);
	NODE_DEFINE_CONSTANT(target, ADA_SORTED_LIST);
	// Constants for 'Command option 1' or 'Command option 2'.
	NODE_DEFINE_CONSTANT(target, ADA_HOLD_ISN);
	NODE_DEFINE_CONSTANT(target, ADA_IFBPOOL);
	NODE_DEFINE_CONSTANT(target, ADA_SCTABLE);
	NODE_DEFINE_CONSTANT(target, ADA_ISNTAB);
	NODE_DEFINE_CONSTANT(target, ADA_GLOBID);
	// Constants for 'Command option 2'.
	NODE_DEFINE_CONSTANT(target, ADA_USRDSYSF);
	NODE_DEFINE_CONSTANT(target, ADA_GET_NEXT);
	NODE_DEFINE_CONSTANT(target, ADA_ISN_SEQ);
	NODE_DEFINE_CONSTANT(target, ADA_USRVALUE);
	NODE_DEFINE_CONSTANT(target, ADA_ASCEND);
	NODE_DEFINE_CONSTANT(target, ADA_DESCEND);
	NODE_DEFINE_CONSTANT(target, ADA_AND_ISN);
	NODE_DEFINE_CONSTANT(target, ADA_OR_ISN);
	NODE_DEFINE_CONSTANT(target, ADA_NOT_ISN);
	NODE_DEFINE_CONSTANT(target, ADA_LF_SSP);
	NODE_DEFINE_CONSTANT(target, ADA_MC_SHORT);

	/*
	 * Constants for Adabas response codes.
	 */
	// Function values.
	NODE_DEFINE_CONSTANT(target, ADA_SUCCESS);
	NODE_DEFINE_CONSTANT(target, ADA_ERROR);
	NODE_DEFINE_CONSTANT(target, ADA_DRVERR);
	// Success.
	NODE_DEFINE_CONSTANT(target, ADA_NORMAL);
	// Warnings.
	NODE_DEFINE_CONSTANT(target, ADA_ISNNS);
	NODE_DEFINE_CONSTANT(target, ADA_FNCMP);
	NODE_DEFINE_CONSTANT(target, ADA_EOF);
	// Errors.
	NODE_DEFINE_CONSTANT(target, ADA_TABT);
	NODE_DEFINE_CONSTANT(target, ADA_SUBCMD);
	NODE_DEFINE_CONSTANT(target, ADA_INFIN);
	NODE_DEFINE_CONSTANT(target, ADA_FICHA);
	NODE_DEFINE_CONSTANT(target, ADA_ACCUS);
	NODE_DEFINE_CONSTANT(target, ADA_INCID);
	NODE_DEFINE_CONSTANT(target, ADA_IUCID);
	NODE_DEFINE_CONSTANT(target, ADA_CMDINV);
	NODE_DEFINE_CONSTANT(target, ADA_ISNSV);
	NODE_DEFINE_CONSTANT(target, ADA_BUISN);
	NODE_DEFINE_CONSTANT(target, ADA_ISNLL);
	NODE_DEFINE_CONSTANT(target, ADA_BLISN);
	NODE_DEFINE_CONSTANT(target, ADA_DUPVAL);
	NODE_DEFINE_CONSTANT(target, ADA_IADD1);
	NODE_DEFINE_CONSTANT(target, ADA_MVOPT);
	NODE_DEFINE_CONSTANT(target, ADA_ENCINV);
	NODE_DEFINE_CONSTANT(target, ADA_SYNTX);
	NODE_DEFINE_CONSTANT(target, ADA_ERFBU);
	NODE_DEFINE_CONSTANT(target, ADA_DESDE);
	NODE_DEFINE_CONSTANT(target, ADA_FBUSU);
	NODE_DEFINE_CONSTANT(target, ADA_FCOVL);
	NODE_DEFINE_CONSTANT(target, ADA_MISFB);
	NODE_DEFINE_CONSTANT(target, ADA_NISNOV);
	NODE_DEFINE_CONSTANT(target, ADA_FUNAV);
	NODE_DEFINE_CONSTANT(target, ADA_CORTL);
	NODE_DEFINE_CONSTANT(target, ADA_SYRBO);
	NODE_DEFINE_CONSTANT(target, ADA_INVRB);
	NODE_DEFINE_CONSTANT(target, ADA_DRBVB);
	NODE_DEFINE_CONSTANT(target, ADA_RBTS);
	NODE_DEFINE_CONSTANT(target, ADA_RBTL);
	NODE_DEFINE_CONSTANT(target, ADA_IFCTE);
	NODE_DEFINE_CONSTANT(target, ADA_IVTL);
	NODE_DEFINE_CONSTANT(target, ADA_DSPEC);
	NODE_DEFINE_CONSTANT(target, ADA_SYSBU);
	NODE_DEFINE_CONSTANT(target, ADA_ERSBU);
	NODE_DEFINE_CONSTANT(target, ADA_LSPEC);
	NODE_DEFINE_CONSTANT(target, ADA_UCIDS);
	NODE_DEFINE_CONSTANT(target, ADA_NSUQU);
	NODE_DEFINE_CONSTANT(target, ADA_NSWRK);
	NODE_DEFINE_CONSTANT(target, ADA_NTWRK);
	NODE_DEFINE_CONSTANT(target, ADA_EXOVFCB);
	NODE_DEFINE_CONSTANT(target, ADA_OVIDX);
	NODE_DEFINE_CONSTANT(target, ADA_NSAAD);
	NODE_DEFINE_CONSTANT(target, ADA_OVFST);
	NODE_DEFINE_CONSTANT(target, ADA_HYXNA);
	NODE_DEFINE_CONSTANT(target, ADA_HYISN);
	NODE_DEFINE_CONSTANT(target, ADA_OVDVT);
	NODE_DEFINE_CONSTANT(target, ADA_HYPERR);
	NODE_DEFINE_CONSTANT(target, ADA_COLERR);
	NODE_DEFINE_CONSTANT(target, ADA_INMEM);
	NODE_DEFINE_CONSTANT(target, ADA_UNIQD);
	NODE_DEFINE_CONSTANT(target, ADA_IOERR);
	NODE_DEFINE_CONSTANT(target, ADA_INVIS);
	NODE_DEFINE_CONSTANT(target, ADA_INVRF);
	NODE_DEFINE_CONSTANT(target, ADA_LOBERR);
	NODE_DEFINE_CONSTANT(target, ADA_NLOCK);
	NODE_DEFINE_CONSTANT(target, ADA_ALOCK);
	NODE_DEFINE_CONSTANT(target, ADA_BSPEC);
	NODE_DEFINE_CONSTANT(target, ADA_UBNAC);
	NODE_DEFINE_CONSTANT(target, ADA_ANACT);
	NODE_DEFINE_CONSTANT(target, ADA_SYSCE);
	NODE_DEFINE_CONSTANT(target, ADA_NUCLI);
	NODE_DEFINE_CONSTANT(target, ADA_NSACQ);
	NODE_DEFINE_CONSTANT(target, ADA_IUBSZ);
	NODE_DEFINE_CONSTANT(target, ADA_PENDI);
	NODE_DEFINE_CONSTANT(target, ADA_CANCL);
	NODE_DEFINE_CONSTANT(target, ADA_PRE);
	NODE_DEFINE_CONSTANT(target, ADA_POST);
	NODE_DEFINE_CONSTANT(target, ADA_BPMFU);
	NODE_DEFINE_CONSTANT(target, ADA_NODESC);
	NODE_DEFINE_CONSTANT(target, ADA_NODV);
	NODE_DEFINE_CONSTANT(target, ADA_UQDV);
	NODE_DEFINE_CONSTANT(target, ADA_INRAB);
	NODE_DEFINE_CONSTANT(target, ADA_ISNVAL);
	NODE_DEFINE_CONSTANT(target, ADA_DARAB);
	NODE_DEFINE_CONSTANT(target, ADA_INVLIST);
	NODE_DEFINE_CONSTANT(target, ADA_MISAC);
	NODE_DEFINE_CONSTANT(target, ADA_ETDAT);
	NODE_DEFINE_CONSTANT(target, ADA_SECUR);
	NODE_DEFINE_CONSTANT(target, ADA_INVPWD);
	NODE_DEFINE_CONSTANT(target, ADA_NFPWD);
	NODE_DEFINE_CONSTANT(target, ADA_PWDINU);
	NODE_DEFINE_CONSTANT(target, ADA_BLOST);
	NODE_DEFINE_CONSTANT(target, ADA_RMUTI);
	NODE_DEFINE_CONSTANT(target, ADA_NOTYET);
	NODE_DEFINE_CONSTANT(target, ADA_XA_PROTO);
	NODE_DEFINE_CONSTANT(target, ADA_LODUEX);
	NODE_DEFINE_CONSTANT(target, ADA_ALLOC);
	NODE_DEFINE_CONSTANT(target, ADA_GCBEX);
	NODE_DEFINE_CONSTANT(target, ADA_UTUCB);
	NODE_DEFINE_CONSTANT(target, ADA_OVUCB);
	NODE_DEFINE_CONSTANT(target, ADA_IDUCB);
	NODE_DEFINE_CONSTANT(target, ADA_FCTNY);
	NODE_DEFINE_CONSTANT(target, ADA_IUCAL);
	NODE_DEFINE_CONSTANT(target, ADA_CALLINV);
	NODE_DEFINE_CONSTANT(target, ADA_SYLOD);
	NODE_DEFINE_CONSTANT(target, ADA_BPOLL);
	// XA userexit XAUEX_0 return codes.
	NODE_DEFINE_CONSTANT(target, XAUEX_OK);
	NODE_DEFINE_CONSTANT(target, XAUEX_IGNORE);

	target->Set(String::NewSymbol("Command"),
		constructor_template->GetFunction());
}
