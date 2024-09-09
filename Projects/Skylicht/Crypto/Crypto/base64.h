/*********************************************************************
* Filename:   base64.h
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding Base64 implementation.
*********************************************************************/

#ifndef BASE64_H
#define BASE64_H

/*************************** HEADER FILES ***************************/
#include <stddef.h>

#include "crypto_type.h"

/*********************** FUNCTION DECLARATIONS **********************/
#ifdef __cplusplus
extern "C" {
#endif 
	// Returns the size of the output. If called with out = NULL, will just return
	// the size of what the output would have been (without a terminating NULL).
	size_t base64_encode(const BYTE8 in[], BYTE8 out[], size_t len, int newline_flag);

	// Returns the size of the output. If called with out = NULL, will just return
	// the size of what the output would have been (without a terminating NULL).
	size_t base64_decode(const BYTE8 in[], BYTE8 out[], size_t len);
#ifdef __cplusplus
}
#endif 

#endif   // BASE64_H
