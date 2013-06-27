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

#include <adabas.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "adamod.h"
#include "messages.h"
#include "modify.h"

#define ISN_BUF_LEN 1000

int db_open(int db_id, const char *db_options);
int db_close(int db_id);

int modify_record(ISN isn);
int delete_record(ISN isn);
int search_records(void);
int scan_file(void);

/*
 * Open Adabas database.
 */
int db_open(int db_id, const char *db_options)
{
	CB_PAR cb;

	/* Prepare Adabas direct call control block. */
	memset(&cb, 0, sizeof(CB_PAR));
	cb.cb_cmd_code[0] = 'O';
	cb.cb_cmd_code[1] = 'P';
	CB_SET_FD(&cb, db_id, 0);
	cb.cb_rec_buf_lng = strlen(db_options);

	/* Execute Adabas direct call command OP. */
	do {
		adabas(&cb, NULL, (char *) db_options, NULL, NULL, NULL);
	} while (cb.cb_return_code == ADA_TABT);

	cb.cb_isn_quantity = 0;
	cb.cb_isn_ll = 0;

	return cb.cb_return_code;
}

/*
 * Close Adabas database.
 */
int db_close(int db_id)
{
	/* Prepare Adabas direct call control block. */
	CB_PAR cb;
	memset(&cb, 0, sizeof(CB_PAR));
	cb.cb_cmd_code[0] = 'C';
	cb.cb_cmd_code[1] = 'L';
	CB_SET_FD(&cb, db_id, 0);

	/* Execute Adabas direct call command CL. */
	adabas(&cb, NULL, NULL, NULL, NULL, NULL);

	return cb.cb_return_code;
}

/*
 * Modify record in Adabas file (specified by ISN).
 */
int modify_record(ISN isn)
{
	CB_PAR cb;

	/*
	 * Get format and record buffers from command line argument
	 * (split argument by delimiter '.').
	 */
	char *format_buf = (char *) options.modify_arg;
	char *record_buf = strchr(options.modify_arg, '.') + 1;
	int format_buf_len = record_buf - format_buf;
	int record_buf_len = strlen(record_buf);

	/* Print ISN of record for high verbose levels. */
	if (options.verbose_level > 2) {
		fprintf(log_file, "%d\n", isn);
	}

	/* In dry run mode skip real record modification. */
	if (options.dry_mode) {
		return ADAMOD_SUCCESS;
	}

	/*
	 * Prepare Adabas direct call control block.
	 * Command A1 (Record Update): modify the value of one or more fields
	 * within a record.
	 */
	memset(&cb, 0, sizeof(CB_PAR));
	cb.cb_cmd_code[0] = 'A';
	cb.cb_cmd_code[1] = '1';
	CB_SET_FD(&cb, options.db_id, options.file_no);
	cb.cb_cop1 = 'H';
	cb.cb_isn = isn;
	cb.cb_fmt_buf_lng = format_buf_len;
	cb.cb_rec_buf_lng = record_buf_len;

	/* Execute Adabas direct call command A1. */
	adabas(&cb, format_buf, record_buf, NULL, NULL, NULL);
	if (cb.cb_return_code != ADA_NORMAL) {
		if (options.verbose_level > 0) {
			dump_adabas_cb(&cb);
		}
		return ADAMOD_E_ADABAS_A1;
	}

	/*
	 * Prepare Adabas direct call control block.
	 * Command ET (End Transaction): end of a logical transaction.
	 */
	memset(&cb, 0, sizeof(CB_PAR));
	cb.cb_cmd_code[0] = 'E';
	cb.cb_cmd_code[1] = 'T';
	CB_SET_FD(&cb, options.db_id, options.file_no);

	/* Execute Adabas direct call command ET. */
	adabas(&cb, NULL, NULL, NULL, NULL, NULL);
	if (cb.cb_return_code != ADA_NORMAL) {
		if (options.verbose_level > 0) {
			dump_adabas_cb(&cb);
		}
		return ADAMOD_E_ADABAS_ET;
	}

	return ADAMOD_SUCCESS;
}

/*
 * Delete record from the Adabas file (specified by ISN).
 */
int delete_record(ISN isn)
{
	CB_PAR cb;

	/* Print ISN of record for high verbose levels. */
	if (options.verbose_level > 2) {
		fprintf(log_file, "%d\n", isn);
	}

	/* In dry run mode skip real record modification. */
	if (options.dry_mode) {
		return ADAMOD_SUCCESS;
	}

	/*
	 * Prepare Adabas direct call control block.
	 * Command E1 (Delete Record): delete a record.
	 */
	memset(&cb, 0, sizeof(CB_PAR));
	cb.cb_cmd_code[0] = 'E';
	cb.cb_cmd_code[1] = '1';
	CB_SET_FD(&cb, options.db_id, options.file_no);
	cb.cb_isn = isn;

	/* Execute Adabas direct call command E1. */
	adabas(&cb, NULL, NULL, NULL, NULL, NULL);
	if (cb.cb_return_code != ADA_NORMAL) {
		if (options.verbose_level > 0) {
			dump_adabas_cb(&cb);
		}
		return ADAMOD_E_ADABAS_E1;
	}

	/*
	 * Prepare Adabas direct call control block.
	 * Command ET (End Transaction): end of a logical transaction.
	 */
	memset(&cb, 0, sizeof(CB_PAR));
	cb.cb_cmd_code[0] = 'E';
	cb.cb_cmd_code[1] = 'T';
	CB_SET_FD(&cb, options.db_id, options.file_no);

	/* Execute Adabas direct call command ET. */
	adabas(&cb, NULL, NULL, NULL, NULL, NULL);
	if (cb.cb_return_code != ADA_NORMAL) {
		if (options.verbose_level > 0) {
			dump_adabas_cb(&cb);
		}
		return ADAMOD_E_ADABAS_ET;
	}

	return ADAMOD_SUCCESS;
}

/*
 * Search records according to specified search argument
 * (combination of search and value buffers) and modify
 * found records.
 */
int search_records(void)
{
	int result_code;
	time_t start_time, cur_time, prev_time;
	int rec_no;
	unsigned int isn_no;
	CB_PAR cb;

	/*
	 * Get search and value buffers from command line argument
	 * (split argument by delimiter '.').
	 */
	char *search_buf = (char *) options.search_arg;
	char *value_buf = strchr(options.search_arg, '.') + 1;
	int search_buf_len = value_buf - search_buf;
	int value_buf_len = strlen(value_buf);
	
	/* This ISN buffer will be used in command S1. */
	ISN isn_buf[ISN_BUF_LEN];

	/* Prepare Adabas direct call control block.
	 * Command S1 (Find Records): select a set of records which
	 * satisfy given search criteria.
	 */
	memset(&cb, 0, sizeof(CB_PAR));
	cb.cb_cmd_code[0] = 'S';
	cb.cb_cmd_code[1] = '1';
	/* Specify database identifier and file number. */
	CB_SET_FD(&cb, options.db_id, options.file_no);
	/* Same command identifier will be used for all subsequent commands. */
	cb.cb_cmd_id[0] = 'A';
	cb.cb_cmd_id[1] = 'M';
	cb.cb_cmd_id[2] = 'O';
	cb.cb_cmd_id[3] = 'D';
	/* We don't need to read record fields, so use "." as format buffer. */
	cb.cb_fmt_buf_lng = 1;
	/* Search criteria specified in search and value buffers. */
	cb.cb_sea_buf_lng = search_buf_len;
	cb.cb_val_buf_lng = value_buf_len;
	/* Use small ISN buffer to get next portion of ISNs. */
	cb.cb_isn_buf_lng = sizeof(isn_buf);

	/* Get process start time. */
	time(&start_time);
	prev_time = start_time;

	/*
	 * Search records multiple times (don't want to slow down Adabas by
	 * using 'SAVE LIST' option) and modify every portion of found records.
	 */
	rec_no = 0;
	while (1) {
		/* Execute Adabas direct call command S1. */
		adabas(&cb, (char *) ".", NULL, search_buf, value_buf,
			(char *) isn_buf);
		if (cb.cb_return_code != ADA_NORMAL) {
			if (options.verbose_level > 0) {
				dump_adabas_cb(&cb);
			}
			return ADAMOD_E_ADABAS_S1;
		}

		/* Print number of found records. */
		if (rec_no == 0 && options.verbose_level > 0) {
			fprintf(stderr, "Found records: %d\n",
				cb.cb_isn_quantity);
		}

		/* Get ISN of records from ISN buffer and modify each record. */
		for (isn_no = 0; isn_no < cb.cb_isn_quantity
			&& isn_no < ISN_BUF_LEN; isn_no++)
		{
			/* Increase records counter. */
			rec_no++;

			/* Modify record by ISN. */
			if (options.delete_mode) {
				result_code = delete_record(isn_buf[isn_no]);
			} else {
				result_code = modify_record(isn_buf[isn_no]);
			}
			if (result_code != ADAMOD_SUCCESS) {
				return result_code;
			}

			/* Print process status. */
			time(&cur_time);
			if (cur_time > prev_time) {
				if (options.verbose_level > 1) {
					fprintf(stderr, "\rRecord: %d", rec_no);
				}
	
				prev_time = cur_time;
				fflush(stderr);
			}
		}

		/*
		 * Exit loop if last command S1 returned less ISNs than
		 * fit in ISN buffer.
		 */
		if (cb.cb_isn_quantity < ISN_BUF_LEN) {
			break;
		}
	}

	/* Print used time. */
	if (options.verbose_level > 0) {
		double used_time;
		int used_hours, used_minutes, used_seconds;

		time(&cur_time);
		used_time = (double) (cur_time - start_time);
		used_hours = (int) floor(used_time / 3600);
		used_minutes = (int) floor((used_time
			- (used_hours * 3600)) / 60);
		used_seconds = (int) (used_time - (used_hours * 3600)
			- (used_minutes * 60));

		if (options.verbose_level > 1) {
			fputc('\r', stderr);
		}
		fprintf(stderr, "Processed records: %d\n", rec_no);
		fprintf(stderr, "Done in %d:%02d:%02d.\n",
			used_hours, used_minutes, used_seconds);
	}

	return ADAMOD_SUCCESS;
}

/*
 * Scan and modify all records in specified Adabas file.
 */
int scan_file(void)
{
	int result_code;
	time_t start_time, cur_time, prev_time;
	int rec_no;
	CB_PAR cb;

	/* Prepare Adabas direct call control block.
	 * Command L2 (Read Physical Sequence): read a record from a set of
	 * records which are stored in physical sequence in Data Storage.
	 */
	memset(&cb, 0, sizeof(CB_PAR));
	cb.cb_cmd_code[0] = 'L';
	cb.cb_cmd_code[1] = '2';
	/* Specify database identifier and file number. */
	CB_SET_FD(&cb, options.db_id, options.file_no);
	/* Same command identifier will be used for all subsequent commands. */
	cb.cb_cmd_id[0] = 'A';
	cb.cb_cmd_id[1] = 'M';
	cb.cb_cmd_id[2] = 'O';
	cb.cb_cmd_id[3] = 'D';
	/* We don't need to read record fields, so use "." as format buffer. */
	cb.cb_fmt_buf_lng = 1;

	/* Get process start time. */
	time(&start_time);
	prev_time = start_time;

	/*
	 * Fetch all records from file in physical sequence and
	 * modify every record.
	 */
	for (rec_no = 1;; rec_no++) {
		/* Execute Adabas direct call command L2. */
		adabas(&cb, (char *) ".", NULL, NULL, NULL, NULL);
		if (cb.cb_return_code != ADA_NORMAL) {
			/* Exit loop when all records readed. */
			if (cb.cb_return_code == ADA_EOF) {
				break;
			}

			/* Print error message when command failed. */
			if (options.verbose_level > 0) {
				dump_adabas_cb(&cb);
			}

			return ADAMOD_E_ADABAS_L2;
		}

		/* Modify record by ISN. */
		if (options.delete_mode) {
			result_code = delete_record(cb.cb_isn);
		} else {
			result_code = modify_record(cb.cb_isn);
		}
		if (result_code != ADAMOD_SUCCESS) {
			return result_code;
		}

		/* Print process status. */
		time(&cur_time);
		if (cur_time > prev_time) {
			if (options.verbose_level > 1) {
				fprintf(stderr, "\rRecord: %d", rec_no);
			}
	
			prev_time = cur_time;
			fflush(stderr);
		}
	}

	/* Print used time. */
	if (options.verbose_level > 0) {
		double used_time;
		int used_hours, used_minutes, used_seconds;

		time(&cur_time);
		used_time = (double) (cur_time - start_time);
		used_hours = (int) floor(used_time / 3600);
		used_minutes = (int) floor((used_time
			- (used_hours * 3600)) / 60);
		used_seconds = (int) (used_time - (used_hours * 3600)
			- (used_minutes * 60));

		if (options.verbose_level > 1) {
			fputc('\r', stderr);
		}
		fprintf(stderr, "Processed records: %d\n", rec_no);
		fprintf(stderr, "Done in %d:%02d:%02d.\n",
			used_hours, used_minutes, used_seconds);
	}

	return ADAMOD_SUCCESS;
}

/*
 * Search records in specified Adabas file and modify found records.
 */
int modify_file_records(void)
{
	int return_code;
	char db_options[30];

	/* Open Adabas database. */
	sprintf(db_options, "UPD=%d.", options.file_no);
	if (db_open(options.db_id, db_options) != ADA_NORMAL) {
		return ADAMOD_E_ADABAS_OP;
	}

	if (options.isn > 0) {
		/* When ISN specified, modify/delete just one record by ISN. */
		if (options.delete_mode) {
			return_code = delete_record(options.isn);
		} else {
			return_code = modify_record(options.isn);
		}
	} else if (options.search_arg != NULL) {
		/*
		 * When search argument specified -
		 * search and modify records according to this argument.
		 */
		return_code = search_records();
	} else {
		/*
		 * When neither ISN nor search argument specified -
		 * scan and modify all records in file.
		 */
		return_code = scan_file();
	}

	/* Close Adabas database. */
	if (db_close(options.db_id) != ADA_NORMAL) {
		return_code = ADAMOD_E_ADABAS_CL;
	}

	return return_code;
}
