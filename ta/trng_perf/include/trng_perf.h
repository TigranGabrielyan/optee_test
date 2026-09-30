/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2026, Renesas Electronics Corporation
 */

#ifndef TRNG_PERF_H
#define TRNG_PERF_H

#include <tee_api.h>

TEE_Result ta_trng_perf_cmd_handler(uint32_t nCommandID,
		uint32_t param_types, TEE_Param params[4]);

#endif /* TRNG_PERF_H */
