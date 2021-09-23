#ifndef _CRYPTO_H_
#define _CRYPTO_H_
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
	#define SAFE_FREE(x) if(x) { free(x); x=NULL; }

	/* RSA2048 */
	typedef struct _RSA2048_KEY_BLOB_
	{
		void*		blob;
		uint32_t	blob_len;
	} RSA2048_KEY_BLOB;

	bool rsa2048_key_generate(RSA2048_KEY_BLOB* pub_key, RSA2048_KEY_BLOB* priv_key);
	bool rsa2048_import_key_from_file(const char* pubkey_file, RSA2048_KEY_BLOB* pub_key, const char* privkey_file, RSA2048_KEY_BLOB* priv_key, uint8_t filemode);
	int rsa2048_encrypt(const void* inbuf, uint32_t buflen, RSA2048_KEY_BLOB* priv_key, void** outbuf);
	int rsa2048_decrypt(const void* inbuf, uint32_t buflen, RSA2048_KEY_BLOB* pub_key, void** outbuf);

	/* base64 */
	int base64_encode(const void* in, int in_len, void** out);
	int base64_decode(const void* in, int in_len, void** out);

	/* blowfish */
	int bf_encode(const void* in, int in_len, const void* key, int keylen, void** out);
	int bf_decode(const void* in, int in_len, const void* key, int keylen, void** out);

	/* crc16 */
	uint16_t CRC16(const void *input_str, int num_bytes);

	/* SHA256 */
	bool SHA256_hash(void* buf, int len, void* hash);

	/* keygen */
	char* crypto_keygen(const void* dev_info, int len, RSA2048_KEY_BLOB* sigkey);
	bool activation_checkout(const void* activate_code, const void* devinfo, int devinfo_len, RSA2048_KEY_BLOB* rsa_key);

#ifdef __cplusplus
}
#endif
#endif