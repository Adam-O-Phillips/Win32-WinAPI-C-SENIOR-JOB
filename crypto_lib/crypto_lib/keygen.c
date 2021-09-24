#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "crypto.h"

#define KEYGEN_CRC_LEN			2
#define KEYGEN_HASH_LEN			32
#define KEYGEN_SIGNATURE_LEN	256

/*
* Generate activation code from device information
*/
char* crypto_keygen(const void* dev_info, int len, uint32_t dwSeq, uint64_t qwIssuedAt, uint32_t dwPeriod, uint32_t dwFlag, RSA2048_KEY_BLOB* sigkey)
{
	void* base_data = NULL;
	uint8_t hash[KEYGEN_HASH_LEN];
	uint8_t* dptr;
	int dlen = 0;
	int valid_len = 0;
	int padding_len = 0;
	void* encbuf = NULL;
	int enclen = 0;
	char* activate_code = NULL;
	
	// check device information is valid
	if (dev_info == NULL || len == 0 || 
		sigkey == NULL || sigkey->blob == NULL || sigkey->blob_len == 0)
		return NULL;

	// create base container buffer for activation processing
	valid_len = len + KEYGEN_CRC_LEN + sizeof(dwSeq) + sizeof(qwIssuedAt) + sizeof(dwPeriod) + sizeof(dwFlag) + KEYGEN_SIGNATURE_LEN;
	dlen = ((valid_len + 7) / 8) * 8;
	padding_len = dlen - valid_len;
	base_data = malloc(dlen);
	memset(base_data, 0, dlen);
	dptr = base_data;

	// copy device info to container
	memcpy(dptr, dev_info, len);
	dptr += len + padding_len;

	// calculate crc16 of device info and append it
	*(uint16_t*)dptr = crc_16(base_data, (dptr - base_data));
	dptr += KEYGEN_CRC_LEN;

	// dwSequence
	memcpy(dptr, &dwSeq, sizeof(dwSeq));
	dptr += sizeof(dwSeq);

	// qwIssuedAt
	memcpy(dptr, &qwIssuedAt, sizeof(qwIssuedAt));
	dptr += sizeof(qwIssuedAt);

	// dwPeriod
	memcpy(dptr, &dwPeriod, sizeof(dwPeriod));
	dptr += sizeof(dwPeriod);

	// dwFlag
	memcpy(dptr, &dwFlag, sizeof(dwFlag));
	dptr += sizeof(dwFlag);

	// calculate hash and generate signature and then append the signature
	if (!SHA256_hash(base_data, (dptr - base_data), hash))
		goto _err_exit_;
	enclen = rsa2048_encrypt(hash, KEYGEN_HASH_LEN, sigkey, &encbuf);
	if (enclen < 0 || enclen != KEYGEN_SIGNATURE_LEN)
		goto _err_exit_;
	memcpy(dptr, encbuf, enclen);
	SAFE_FREE(encbuf);
	dptr += KEYGEN_SIGNATURE_LEN;

	// encrypt container buffer by blowfish using device info as key
	enclen = bf_encode(base_data, dlen, dev_info, len, &encbuf);
	if (enclen < 0 || enclen != dlen)
		goto _err_exit_;
	memcpy(base_data, encbuf, dlen);
	SAFE_FREE(encbuf);

	// padding ??

	// generate activate code by using base64
	enclen = base64_encode(base_data, dlen, &activate_code);
	if (enclen < 0)
		goto _err_exit_;

	SAFE_FREE(base_data);
	SAFE_FREE(encbuf);
	activate_code[enclen] = 0;
	return activate_code;

_err_exit_:
	SAFE_FREE(base_data);
	SAFE_FREE(encbuf);
	SAFE_FREE(activate_code);
	return NULL;
}

/*
* Check validation of activate code
*/
bool activation_checkout(const void* activate_code, const void* devinfo, int devinfo_len, uint32_t *pdwSeq, uint64_t *pqwIssuedAt, uint32_t *pdwPeriod, uint32_t *pdwFlag, RSA2048_KEY_BLOB* rsa_key)
{
	void* decbuf = NULL;
	int declen = -1;
	uint8_t* info = NULL;
	uint8_t hash[KEYGEN_HASH_LEN];
	uint16_t crc = 0;
	int info_len = -1;
	uint8_t* iptr = NULL;

	// check if parameter is valid
	if (activate_code == NULL || devinfo == NULL || 
		rsa_key == NULL || rsa_key->blob == NULL || rsa_key->blob_len == 0)
		return false;

	// base64 decode
	info_len = base64_decode(activate_code, strlen(activate_code), &info);
	if (info_len < 0 || (info_len % 8) != 0)
		goto _err_exit_;
	
	// blowfish decode
	declen = bf_decode(info, info_len, devinfo, devinfo_len, &decbuf);
	if (declen < 0 || declen != info_len ||
		declen < (KEYGEN_CRC_LEN + KEYGEN_SIGNATURE_LEN))
		goto _err_exit_;
	memcpy(info, decbuf, info_len);
	iptr = info + info_len;
	SAFE_FREE(decbuf);

	// get hash code by decrypting signature block and check it
	iptr -= KEYGEN_SIGNATURE_LEN;
	declen = rsa2048_decrypt(iptr, KEYGEN_SIGNATURE_LEN, rsa_key, &decbuf);
	if (declen < 0)
		goto _err_exit_;
	// check hash of activation info
	SHA256_hash(info, (iptr - info), hash);
	if (memcmp(hash, decbuf, sizeof(hash) != 0))
		goto _err_exit_;
	SAFE_FREE(decbuf);

	// dwFlag
	iptr -= sizeof(*pdwFlag);
	if (pdwFlag)
		memcpy(pdwFlag, iptr, sizeof(*pdwFlag));

	// dwPeriod
	iptr -= sizeof(*pdwPeriod);
	if (pdwPeriod)
		memcpy(pdwPeriod, iptr, sizeof(*pdwPeriod));

	// qwIssuedAt
	iptr -= sizeof(*pqwIssuedAt);
	if (pqwIssuedAt)
		memcpy(pqwIssuedAt, iptr, sizeof(*pqwIssuedAt));

	// dwSeq
	iptr -= sizeof(*pdwSeq);
	if (pdwSeq)
		memcpy(pdwSeq, iptr, sizeof(*pdwSeq));

	// check crc
	iptr -= KEYGEN_CRC_LEN;
	crc = CRC16(info, (iptr - info));
	if (crc != *(uint16_t*)iptr)
		goto _err_exit_;
	
	SAFE_FREE(decbuf);
	SAFE_FREE(info);
	return true;

_err_exit_:
	SAFE_FREE(decbuf);
	SAFE_FREE(info);
	return false;
}
