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

#if !defined(ADAMOD_H)
#define ADAMOD_H

#include <stdio.h>
#include <stdint.h>

/* Codes of application states. */
typedef enum {
	ADAMOD_SUCCESS = 0,

	ADAMOD_E_NOARGS,
	ADAMOD_E_INVARG,
	ADAMOD_E_INVTARGET,
	ADAMOD_E_INVSEARCH,
	ADAMOD_E_INVMODIFY,
	ADAMOD_E_NOMODIFY,
	ADAMOD_E_ADABAS_OP,
	ADAMOD_E_ADABAS_CL,
	ADAMOD_E_ADABAS_S1,
	ADAMOD_E_ADABAS_L2,
	ADAMOD_E_ADABAS_A1,
	ADAMOD_E_ADABAS_ET,
	ADAMOD_E_ADABAS_E1,

	ADAMOD_M_DRYMODE,
	ADAMOD_M_DONE
} AdamodStateCode;

/* Application options structure. */
struct Options {
	int verbose_level;
	int dry_mode;
	int delete_mode;
	const char *log_file_name;

	uint16_t db_id;
	uint16_t file_no;

	uint32_t isn;
	const char *search_arg;
	const char *modify_arg;
};

/* Application options variable in module 'adamod'. */
extern struct Options options;

/* Log file. */
extern FILE *log_file;

#endif /* ADAMOD_H */
