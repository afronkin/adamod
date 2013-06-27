/*
 * Copyright (c) 2012, Alexander Fronkin
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
	{ ADAMOD_E_INVSEARCH,
	"Error: invalid search or value buffer specified" },
	{ ADAMOD_E_INVMODIFY,
	"Error: invalid format or record buffer specified" },
	{ ADAMOD_E_NOMODIFY,
	"Error: format and record buffers must be specified" },
	{ ADAMOD_E_ADABAS_OP,
	"Error: can't open Adabas database" },
	{ ADAMOD_E_ADABAS_CL,
	"Error: can't close Adabas database" },
	{ ADAMOD_E_ADABAS_S1,
	"Error: search failed" },
	{ ADAMOD_E_ADABAS_L2,
	"Error: record reading failed" },
	{ ADAMOD_E_ADABAS_A1,
	"Error: record modification failed" },
	{ ADAMOD_E_ADABAS_ET,
	"Error: commit transaction failed" },
	{ ADAMOD_E_ADABAS_E1,
	"Error: record deleting failed" },

	{ ADAMOD_M_DRYMODE,
	"Running in dry mode" },
	{ ADAMOD_M_DONE,
	"Done" },

	{ 0, NULL }
};

#define DUMP_MAX_LEN 20

/*
 * Print message for specified application state code.
 */
void print_message(AdamodStateCode code)
{
	int message_no;
	for (message_no = 0; messages[message_no].code != 0; message_no++) {
		if (messages[message_no].code == code) {
			fprintf(stderr, "%s.\n", messages[message_no].message);
			return;
		}
	}
}

/*
 * Print Adabas buffer content.
 */
void dump_adabas_buf(unsigned char *buf, unsigned int buf_len)
{
	unsigned int i;
	unsigned int dump_len = buf_len > DUMP_MAX_LEN ? DUMP_MAX_LEN : buf_len;

	/* Print ASCII representation of buffer content. */
	for (i = 0; i < dump_len; i++) {
		unsigned char c = buf[i];
		if (c >= 0x20 && c <= 0x7F) {
			fputc(c, stderr);
		} else {
			fputc('.', stderr);
		}
	}

	/* Print haxadecimal representation of buffer content. */
	fprintf(stderr, "  [");
	for (i = 0; i < dump_len; i++) {
		fprintf(stderr, " %02X", buf[i]);
	}
	fprintf(stderr, " ]\n");
}

/*
 * Print Adabas control block content.
 */
void dump_adabas_cb(CB_PAR *cb_)
{
	fprintf(stderr, "Call type:        %d\n", (int) cb_->cb_call_type);
	fprintf(stderr, "Command code:     ");
	dump_adabas_buf(cb_->cb_cmd_code, 2);
	fprintf(stderr, "Command ID:       ");
	dump_adabas_buf(cb_->cb_cmd_id, 4);
	fprintf(stderr, "Database number:  %d\n", cb_->cb_un.x_un.x_cb_db_id);
	fprintf(stderr, "File number:      %d\n", cb_->cb_un.x_un.x_cb_file_nr);
	fprintf(stderr, "Return code:      %d\n", cb_->cb_return_code);
	fprintf(stderr, "ISN:              %d\n", cb_->cb_isn);
	fprintf(stderr, "ISN lower limit:  %d\n", cb_->cb_isn_ll);
	fprintf(stderr, "ISN quantity:     %d\n", cb_->cb_isn_quantity);
	fprintf(stderr, "Command option 1: %d\n", (int) cb_->cb_cop1);
	fprintf(stderr, "Command option 2: %d\n", (int) cb_->cb_cop2);
	fprintf(stderr, "Command time:     %d\n", cb_->cb_cmd_time);
	fprintf(stderr, "User area:        ");
	dump_adabas_buf(cb_->cb_user_area, 4);
	fprintf(stderr, "\n");
	fprintf(stderr, "Addition 1 field: ");
	dump_adabas_buf(cb_->cb_add1, CB_L_AD1);
	fprintf(stderr, "Addition 2 field: ");
	dump_adabas_buf(cb_->cb_add2, CB_L_AD2);
	fprintf(stderr, "Addition 3 field: ");
	dump_adabas_buf(cb_->cb_add3, CB_L_AD3);
	fprintf(stderr, "Addition 4 field: ");
	dump_adabas_buf(cb_->cb_add4, CB_L_AD4);
	fprintf(stderr, "Addition 5 field: ");
	dump_adabas_buf(cb_->cb_add5, CB_L_AD5);
	fprintf(stderr, "\n");
	fprintf(stderr, "Format buffer length: %d\n", cb_->cb_fmt_buf_lng);
	fprintf(stderr, "Record buffer length: %d\n", cb_->cb_rec_buf_lng);
	fprintf(stderr, "Search buffer length: %d\n", cb_->cb_sea_buf_lng);
	fprintf(stderr, "Value buffer length:  %d\n", cb_->cb_val_buf_lng);
	fprintf(stderr, "ISN buffer length:    %d\n", cb_->cb_isn_buf_lng);
	fprintf(stderr, "\n");
}
