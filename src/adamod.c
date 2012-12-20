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

#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "adamod.h"
#include "messages.h"
#include "modify.h"

/* Application options. */
struct Options options = { 0, 0, NULL, 0, 0, 0, NULL, NULL };

/* Log file. */
FILE *log_file = NULL;

/* Print help information. */
void print_help(void);
/* Parse command line arguments. */
int parse_command_line(int argc, char *argv[]);

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
		"Usage: adamod [-hvd] -t dbid,fileno [-l logfile] [-i isn]\n",
		"       [-s searchbuf.valuebuf] formatbuf.recordbuf\n\n",
		"  -h --help     print this help\n",
		"  -d --dry      dry run (do not modify database)\n",
		"  -i --isn      specify ISN of Adabas record\n",
		"  -l --log      specify log file for utility messages\n",
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
int parse_command_line(int argc, char *argv[])
{
	static const char *short_options = "hvdt:l:i:s:";
	static struct option long_options[] = {
		{ "help", no_argument, 0, 'h' },
		{ "verbose", no_argument, 0, 'v' },
		{ "dry", no_argument, 0, 'd' },
		{ "target", required_argument, 0, 't' },
		{ "log", required_argument, 0, 'l' },
		{ "isn", required_argument, 0, 'i' },
		{ "search", required_argument, 0, 's'},
		{ 0, 0, 0, 0 }
	};
	int option;
	char *target_arg = NULL;

	/* Parse command line arguments with getopt_long(). */
	while ((option = getopt_long(argc, argv, short_options,
		long_options, NULL)) != -1)
	{
		switch (option) {
		case 'h':
			print_help();
			return ADAMOD_E_NOARGS;
		case 'd':
			options.dry_mode = 1;
			break;
		case 'i':
			options.isn = atol(optarg);
			break;
		case 'l':
			options.log_file_name = optarg;
			break;
		case 's':
			options.search_arg = optarg;
			if (strchr(options.search_arg, '.') == NULL) {
				return ADAMOD_E_INVSEARCH;
			}
			break;
		case 't':
			target_arg = optarg;
			if (strchr(target_arg, ',') == NULL) {
				return ADAMOD_E_INVTARGET;
			}

			/*
			 * Get Adabas database identifier and file number
			 * from command line argument.
			 */
			options.db_id = atol(target_arg);
			options.file_no = atol(strchr(target_arg, ',') + 1);
			break;
		case 'v':
			options.verbose_level++;
			break;
		default:
			return ADAMOD_E_INVARG;
		}
	}

	if (optind < argc) {
		options.modify_arg = argv[optind++];
		if (strchr(options.modify_arg, '.') == NULL) {
			return ADAMOD_E_INVMODIFY;
		}
	}

	/* Chech presence of mandatory arguments. */
	if (options.db_id < 1 || options.file_no < 1) {
		return ADAMOD_E_INVTARGET;
	}
	if (options.modify_arg == NULL) {
		return ADAMOD_E_NOMODIFY;
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

	/* Open log file. */
	if (options.log_file_name && options.log_file_name[0] != '-') {
		log_file = fopen(options.log_file_name, "wb");
	}
	if (log_file == NULL) {
		log_file = stdout;
	}

	if (options.dry_mode && options.verbose_level > 1) {
		print_message(ADAMOD_M_DRYMODE);
	}

	/* Search records in specified Adabas file and modify found records. */
	result_code = modify_file_records();
	if (result_code != ADAMOD_SUCCESS) {
		print_message(result_code);
		return 1;
	}

	/* Close log file. */
	if (log_file != stdout) {
		fclose(log_file);
	}

	/* Print final message. */
	if (options.verbose_level > 0) {
		print_message(ADAMOD_M_DONE);
	}

	return 0;
}
