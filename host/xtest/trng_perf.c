// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2026, Renesas Electronics Corporation
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tee_client_api.h>

#include <ta_trng_perf.h>

#include "trng_perf.h"
#include "xtest_helpers.h"
#include "xtest_test.h"

#define TRNG_PERF_DEF_CYCLES	1000
#define TRNG_PERF_DEF_SIZE	32

static void errx(const char *msg, TEEC_Result res, uint32_t *orig)
{
	fprintf(stderr, "%s: 0x%08x", msg, res);
	if (orig)
		fprintf(stderr, " (orig=%d)", (int)*orig);
	fprintf(stderr, "\n");
	exit(1);
}

static void check_res(TEEC_Result res, const char *errmsg, uint32_t *orig)
{
	if (res != TEEC_SUCCESS)
		errx(errmsg, res, orig);
}

static void run_test(unsigned int cycles, unsigned int size)
{
	TEEC_Context ctx = { };
	TEEC_Session sess = { };
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	TEEC_UUID uuid = TA_TRNG_PERF_UUID;
	TEEC_Result res = TEEC_ERROR_GENERIC;
	uint32_t err_origin = 0;
	uint32_t elapsed_ms = 0;
	double avg_us = 0.0;

	res = TEEC_InitializeContext(NULL, &ctx);
	check_res(res, "TEEC_InitializeContext", NULL);

	res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL,
			       NULL, &err_origin);
	check_res(res, "TEEC_OpenSession", &err_origin);

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_OUTPUT,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = cycles;
	op.params[0].value.b = size;

	printf("Running TRNG perf test: %u cycles, %u bytes/call\n", cycles,
	       size);

	res = TEEC_InvokeCommand(&sess, TA_TRNG_PERF_CMD_GENERATE, &op,
				 &err_origin);
	check_res(res, "TEEC_InvokeCommand", &err_origin);

	elapsed_ms = op.params[1].value.a;
	avg_us = (double)elapsed_ms * 1000.0 / cycles;

	printf("rsipm_trng_generate(): %u cycles, %u bytes/call, "
	       "total=%"PRIu32" ms, avg=%.2f us/call\n",
	       cycles, size, elapsed_ms, avg_us);

	TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);
}

static void usage(const char *applet_optname, unsigned int cycles,
		  unsigned int size)
{
	fprintf(stderr, "Usage: %s %s [-h]\n", xtest_progname,
		applet_optname);
	fprintf(stderr, "Usage: %s %s [-n CYCLES] [-s SIZE]\n",
		xtest_progname, applet_optname);
	fprintf(stderr,
		"TRNG (rsipm_trng_generate) performance testing tool for OP-TEE\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -h|--help  Print this help and exit\n");
	fprintf(stderr,
		"  -n CYCLES  Number of rsipm_trng_generate() calls to perform [%u]\n",
		cycles);
	fprintf(stderr,
		"  -s SIZE    Random buffer size in bytes per call, 1-%u [%u]\n",
		TA_TRNG_PERF_MAX_BUF_LEN, size);
}

#define NEXT_ARG(i) \
	do { \
		if (++i == argc) { \
			fprintf(stderr, "%s %s: %s: missing argument\n", \
				xtest_progname, argv[0], argv[i - 1]); \
			return 1; \
		} \
	} while (0);

int trng_perf_runner_cmd_parser(int argc, char *argv[])
{
	int i = 0;
	unsigned int cycles = TRNG_PERF_DEF_CYCLES;
	unsigned int size = TRNG_PERF_DEF_SIZE;

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			usage(argv[0], cycles, size);
			return 0;
		}
	}

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-n")) {
			NEXT_ARG(i);
			cycles = atoi(argv[i]);
		} else if (!strcmp(argv[i], "-s")) {
			NEXT_ARG(i);
			size = atoi(argv[i]);
		} else {
			fprintf(stderr, "%s %s: invalid argument: %s\n",
				xtest_progname, argv[0], argv[i]);
			usage(argv[0], cycles, size);
			return 1;
		}
	}

	if (cycles == 0) {
		fprintf(stderr, "%s %s: CYCLES must be greater than 0\n",
			xtest_progname, argv[0]);
		return 1;
	}

	if (size == 0 || size > TA_TRNG_PERF_MAX_BUF_LEN) {
		fprintf(stderr, "%s %s: SIZE must be 1-%u\n", xtest_progname,
			argv[0], TA_TRNG_PERF_MAX_BUF_LEN);
		return 1;
	}

	run_test(cycles, size);

	return 0;
}
