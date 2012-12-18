/*
 * Copyright (c) 2012, Alexander Fronkin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided
 * that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

#include <stdio.h>
#include "adamod.h"
#include "messages.h"

/* Message record. */
struct AdamodMessage {
	AdamodStateCode code;
	const char *message;
};

struct AdamodMessage messages[] = {
	{ ADAMOD_E_INVARG,
	"Error: invalid argument specified" },
	{ ADAMOD_E_INVTARGET,
	"Error: invalid Adabas target specified" },
	{ ADAMOD_E_NOISN,
	"Error: record ISN must be specified" },
	{ ADAMOD_E_INVSEARCH,
	"Error: invalid search or value buffer specified" },
	{ ADAMOD_E_INVMODIFY,
	"Error: invalid format or record buffer specified" },
	{ ADAMOD_E_NODB,
	"Error: Adabas database must be specified" },
	{ ADAMOD_E_NOFILE,
	"Error: Adabas file must be specified" },
	{ ADAMOD_E_NOFBUF,
	"Error: format buffer must be specified" },
	{ ADAMOD_E_NORBUF,
	"Error: record buffer must be specified" },
	{ ADAMOD_E_DBOPEN,
	"Error: can't open Adabas database" },
	{ ADAMOD_E_DBCLOSE,
	"Error: can't close Adabas database" },
	{ ADAMOD_E_RECMODIFY,
	"Error: record modification failed" },

	{ 0, NULL }
};

/*
 * Print message for specified application state code.
 */
void print_message(AdamodStateCode code)
{
	int message_no;
	for (message_no = 0; messages[message_no].code != 0; message_no++) {
		if (messages[message_no].code == code) {
			fprintf(stderr, "%s [%d].\n",
				messages[message_no].message,
				messages[message_no].code);
			return;
		}
	}
}
