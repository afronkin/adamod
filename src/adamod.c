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

#include <adabas.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "adamod.h"
#include "messages.h"

/* Application options structure. */
struct Options {
	int verbose_level;
	uint16_t db_id;
	uint16_t file_no;
	uint32_t isn;
	const char *format_buf;
	const char *record_buf;
};

/* Application options. */
static struct Options options = { 0, 0, 0, 0, NULL, NULL };

/* Modify specified record in ADABAS database. */
AdamodStateCode modify_record(void);
/* Open ADABAS database file. */
int db_open(int db_id, int file_no);
/* Close ADABAS database. */
int db_close(int db_id);
/* Print help information. */
void print_help(void);
/* Parse command line arguments. */
AdamodStateCode parse_command_line(int argc, char *argv[]);

/*
 * Modify specified record in ADABAS database.
 */
AdamodStateCode modify_record(void)
{
	int return_code = ADAMOD_SUCCESS;
	CB_PAR cb;

	/* Open ADABAS database file. */
	if (db_open(options.db_id, options.file_no) != ADA_NORMAL) {
		return_code = ADAMOD_E_DBOPEN;
		goto exit;
	}

	/* Modify specified ADABAS record. */
	memset(&cb, 0, sizeof(CB_PAR));
	cb.cb_cmd_code[0] = 'A';
	cb.cb_cmd_code[1] = '1';
	CB_SET_FD(&cb, options.db_id, options.file_no);
	cb.cb_cop1 = 'H';
	cb.cb_isn = options.isn;
	cb.cb_fmt_buf_lng = strlen(options.format_buf);
	cb.cb_rec_buf_lng = strlen(options.record_buf);
	adabas(&cb, (char *) options.format_buf, (char *) options.record_buf,
		NULL, NULL, NULL);
	if (cb.cb_return_code != ADA_NORMAL) {
		if (options.verbose_level > 0) {
			fprintf(stderr, "Adabas DirectCall failed [%d].\n",
				cb.cb_return_code);
		}
		return_code = ADAMOD_E_RECMODIFY;
		goto exit;
	}

exit:
	/* Close ADABAS database file. */
	if (db_close(options.db_id) != ADA_NORMAL) {
		return_code = ADAMOD_E_DBCLOSE;
	}

	return return_code;
}

/*
 * Open ADABAS database.
 */
int db_open(int db_id, int file_no)
{
	CB_PAR cb;
	char rec_buf[10];

	/* Open ADABAS database file. */
	memset(&cb, 0, sizeof(CB_PAR));
	cb.cb_cmd_code[0] = 'O';
	cb.cb_cmd_code[1] = 'P';
	CB_SET_FD(&cb, db_id, 0);
	sprintf(rec_buf, "UPD=%d.", file_no);
	cb.cb_rec_buf_lng = strlen(rec_buf);
	do {
		adabas(&cb, NULL, rec_buf, NULL, NULL, NULL);
	} while (cb.cb_return_code == ADA_TABT);

	cb.cb_isn_quantity = 0;
	cb.cb_isn_ll = 0;

	return cb.cb_return_code;
}

/*
 * Close ADABAS database.
 */
int db_close(int db_id)
{
	CB_PAR cb;

	memset(&cb, 0, sizeof(CB_PAR));
	cb.cb_cmd_code[0] = 'C';
	cb.cb_cmd_code[1] = 'L';
	CB_SET_FD(&cb, db_id, 0);
	adabas(&cb, NULL, NULL, NULL, NULL, NULL);

	return cb.cb_return_code;
}

/*
 * Print help information.
 */
void print_help(void)
{
	int i;
	const char *help[] = {
		"adamod 1.1 (18 Dec 2012)\n",
		"Adabas records modification tool.\n",
		"Copyright (c) 2012, Alexander Fronkin\n",
		"\n",
		"Usage: adamod [-hv] -t dbid,fileno [-i isn]\n",
		"       [-s searchbuf.valuebuf] formatbuf.recordbuf\n\n",
		"  -h --help     print this help\n",
		"  -i --isn      specify ISN of Adabas record\n",
		"  -s --search   specify Adabas search and value buffers\n",
		"  -t --target   specify target Adabas database and file\n",
		"  -v --verbose  increase verbosity level (repeatable)\n",
		"  formatbuf     Adabas format buffer\n",
		"  recordbuf     Adabas record buffer\n",
		"\n",
		NULL};

	/*
	 * Print help information from array due to theoretic
	 * limit of string length in c89-compliant compilers.
	 */
	for (i = 0; help[i] != NULL; i++) {
		fputs(help[i], stderr);
	}
}

/*
 * Parse command line arguments.
 */
AdamodStateCode parse_command_line(int argc, char *argv[])
{
	int arg_no;
	char *p;
	char *targetInfo = NULL;

	if (argc <= 1) {
		print_help();
		return ADAMOD_E_NOARGS;
	}

	for (arg_no = 1; arg_no < argc; arg_no++) {
		if (argv[arg_no][0] == '-' && argv[arg_no][1] != '-') {
			if (strchr(argv[arg_no], 'h') != NULL) {
				print_help();
				return ADAMOD_E_NOARGS;
			}

			for (p = argv[arg_no] + 1; *p; p++) {
				switch (*p) {
				case 't':
					if (arg_no + 1 >= argc) {
						return ADAMOD_E_NOTARGET;
					}
					targetInfo = argv[++arg_no];
					p = (char *) "-";
					break;
				case 'v':
					options.verbose_level++;
					break;
				default:
					return ADAMOD_E_INVARG;
				}
			}
		} else if (strcmp(argv[arg_no], "--help") == 0) {
			print_help();
			return 0;
		} else if (strncmp(argv[arg_no], "--target", 8) == 0) {
			if (strchr(argv[arg_no], '=') == NULL) {
				return ADAMOD_E_NOTARGET;
			}
			targetInfo = strchr(argv[arg_no], '=') + 1;
		} else if (strcmp(argv[arg_no], "--verbose") == 0) {
			options.verbose_level++;
		} else if (options.isn == 0) {
			options.isn = atol(argv[arg_no]);
		} else if (options.format_buf == NULL) {
			options.format_buf = argv[arg_no];
		} else if (options.record_buf == NULL) {
			options.record_buf = argv[arg_no];
		} else {
			return ADAMOD_E_INVARG;
		}
	}

	/* Parse input format and encoding. */
	if (targetInfo) {
		if (targetInfo[0] != ',') {
			options.db_id = atol(targetInfo);
		}
		if (strrchr(targetInfo, ',') != NULL) {
			options.file_no = atol(strrchr(targetInfo, ',') + 1);
		}
		if (options.db_id < 1 || options.file_no < 1) {
			return ADAMOD_E_INVTARGET;
		}
	}

	/* Chech presence of mandatory arguments. */
	if (options.db_id == 0) {
		return ADAMOD_E_NODB;
	}
	if (options.file_no == 0) {
		return ADAMOD_E_NOFILE;
	}
	if (options.format_buf == NULL) {
		return ADAMOD_E_NOFBUF;
	}
	if (options.record_buf == NULL) {
		return ADAMOD_E_NORBUF;
	}

	/* Command line parsed successfully. */
	return ADAMOD_SUCCESS;
}

/*
 * Main function.
 */
int main(int argc, char *argv[])
{
	int result_code;

	/* Parse command line arguments. */
	result_code = parse_command_line(argc, argv);
	if (result_code != ADAMOD_SUCCESS) {
		print_message(result_code);
		return 1;
	}

	/* Modify specified record in ADABAS database. */
	result_code = modify_record();
	if (result_code != ADAMOD_SUCCESS) {
		print_message(result_code);
		return 1;
	}

	fprintf(stderr, "Done.\n");

	return 0;
}
