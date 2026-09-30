// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2026, Renesas Electronics Corporation
 */

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <trng_perf.h>
#include <ta_trng_perf.h>

#define ASSERT_PARAM_TYPE(pt_in, pt_expect) \
do { \
	if ((pt_in) != (pt_expect)) \
		return TEE_ERROR_BAD_PARAMETERS; \
} while (0)

static inline uint32_t tee_time_to_ms(TEE_Time t)
{
	return t.seconds * 1000 + t.millis;
}

static inline uint32_t get_delta_time_in_ms(TEE_Time start, TEE_Time stop)
{
	return tee_time_to_ms(stop) - tee_time_to_ms(start);
}

static TEE_Result ta_trng_perf_generate(uint32_t param_types,
					TEE_Param params[4])
{
	TEE_Time start_time = { };
	TEE_Time stop_time = { };
	uint32_t cycles = 0;
	uint32_t buf_len = 0;
	uint32_t *elapsed_ms = &params[1].value.a;
	uint8_t *buf = NULL;

	ASSERT_PARAM_TYPE(param_types, TEE_PARAM_TYPES(
					TEE_PARAM_TYPE_VALUE_INPUT,
					TEE_PARAM_TYPE_VALUE_OUTPUT,
					TEE_PARAM_TYPE_NONE,
					TEE_PARAM_TYPE_NONE));

	cycles = params[0].value.a;
	buf_len = params[0].value.b;

	if (cycles == 0)
		return TEE_ERROR_BAD_PARAMETERS;

	if (buf_len == 0 || buf_len > TA_TRNG_PERF_MAX_BUF_LEN)
		return TEE_ERROR_BAD_PARAMETERS;

	buf = TEE_Malloc(buf_len, TEE_MALLOC_FILL_ZERO);
	if (!buf)
		return TEE_ERROR_OUT_OF_MEMORY;

	IMSG("TRNG perf: %"PRIu32" cycles, %"PRIu32" bytes/call", cycles,
	     buf_len);

	TEE_GetSystemTime(&start_time);

	while (cycles--)
		TEE_RSIPM_TRNG(buf, buf_len);

	TEE_GetSystemTime(&stop_time);

	*elapsed_ms = get_delta_time_in_ms(start_time, stop_time);

	IMSG("TRNG perf: start: %"PRIu32".%"PRIu32"(s), stop: %"PRIu32".%"PRIu32"(s), delta: %"PRIu32"(ms)",
	     start_time.seconds, start_time.millis,
	     stop_time.seconds, stop_time.millis, *elapsed_ms);

	TEE_Free(buf);

	return TEE_SUCCESS;
}

TEE_Result ta_trng_perf_cmd_handler(uint32_t nCommandID,
		uint32_t param_types, TEE_Param params[4])
{
	switch (nCommandID) {
	case TA_TRNG_PERF_CMD_GENERATE:
		return ta_trng_perf_generate(param_types, params);
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}
