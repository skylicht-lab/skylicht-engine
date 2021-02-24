/*********************************************************************
* Filename:   sha256.h
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding SHA1 implementation.
*********************************************************************/

#ifndef SHA256_H
#define SHA256_H

/*************************** HEADER FILES ***************************/
#include <stddef.h>

#include "crypto_type.h"

/****************************** MACROS ******************************/
#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

typedef struct {
	BYTE8 data[64];
	WORD32 datalen;
	unsigned long long bitlen;
	WORD32 state[8];
} SHA256_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
#ifdef __cplusplus
extern "C" {
#endif
	void sha256_init(SHA256_CTX* ctx);
	void sha256_update(SHA256_CTX* ctx, const BYTE8 data[], size_t len);
	void sha256_final(SHA256_CTX* ctx, BYTE8 hash[]);
#ifdef __cplusplus
}
#endif

#endif   // SHA256_H
