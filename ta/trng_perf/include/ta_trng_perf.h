/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2026, Renesas Electronics Corporation
 */

#ifndef TA_TRNG_PERF_H
#define TA_TRNG_PERF_H

#define TA_TRNG_PERF_UUID { 0x3bdfcffa, 0xf6a1, 0x4895, \
	{ 0xa4, 0xc2, 0xfd, 0xa5, 0x97, 0xf7, 0x2d, 0x65 } }

/*
 * Commands implemented by the TA
 *
 * TA_TRNG_PERF_CMD_GENERATE - Call rsipm_trng_generate() (via
 * TEE_RSIPM_TRNG()) in a loop and measure the total elapsed time inside
 * the TEE.
 *
 * params[0].value.a: number of cycles (calls to TEE_RSIPM_TRNG()) to perform
 * params[0].value.b: size in bytes of the random buffer requested per call
 * params[1].value.a: [out] elapsed time in milliseconds for all cycles
 *		       (TEE_GetSystemTime() resolution is 1 ms)
 */
#define TA_TRNG_PERF_CMD_GENERATE	0

/* Keep in sync with TRNG_MAX_WORD * TRNG_BLOCK_SIZE in rcar_trng.h */
#define TA_TRNG_PERF_MAX_BUF_LEN	512

#endif /* TA_TRNG_PERF_H */
