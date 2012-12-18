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

#if !defined(ADAMOD_H)
#define ADAMOD_H

/* Codes of application states. */
typedef enum {
	ADAMOD_SUCCESS = 0,
	ADAMOD_E_NOARGS = 1,
	ADAMOD_E_INVARG = 2,
	ADAMOD_E_INVTARGET = 3,
	ADAMOD_E_NOISN = 4,
	ADAMOD_E_INVSEARCH = 5,
	ADAMOD_E_INVMODIFY = 6,
	ADAMOD_E_NODB = 7,
	ADAMOD_E_NOFILE = 8,
	ADAMOD_E_NOFBUF = 9,
	ADAMOD_E_NORBUF = 10,
	ADAMOD_E_DBOPEN = 11,
	ADAMOD_E_DBCLOSE = 12,
	ADAMOD_E_RECMODIFY = 13
} AdamodStateCode;

#endif /* ADAMOD_H */
