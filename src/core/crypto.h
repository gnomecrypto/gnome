#ifndef __GNOME_CRYPTO_H__
#define __GNOME_CRYPTO_H__

#include "utils.h"

#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include <array>

namespace gnome
{
	constexpr size_t EDDSA_KEY_LENGTH = 32;
	
	
	struct keypair_t
	{
		const hash_t pub;
		const hash_t prv;
		keypair_t(const hash_t pub, const hash_t prv) : pub(pub), prv(prv) {}
	};
	
	inline byte_t *sha256_raw(const std::string &message)
	{
		const auto data = new byte_t[SHA256_DIGEST_LENGTH];
		SHA256_CTX sha;
		SHA256_Init(&sha);
		SHA256_Update(&sha, message.c_str(), message.size());
		SHA256_Final(data, &sha);
		return data;
	}
	
	inline hash_t sha256(const std::string &message)
	{
		const auto digest = sha256_raw(message);
		hash_t encoded = bytes2hex(digest, SHA256_DIGEST_LENGTH);
		delete[] digest;
		return encoded;
	}

	inline hash_t hash256(const time_t &timestamp, const hash_t &last_hash, const payload_t &payload)
	{
		return sha256(format("%d%s%s", timestamp, last_hash, payload));
	}

	inline byte_t *sha512_raw(const std::string &message)
	{
		const auto data = new byte_t[SHA512_DIGEST_LENGTH];
		SHA512_CTX sha;
		SHA512_Init(&sha);
		SHA512_Update(&sha, message.c_str(), message.size());
		SHA512_Final(data, &sha);
		return data;
	}
	
	inline hash_t sha512(const std::string &message)
	{
		const auto digest = sha512_raw(message);
		hash_t encoded = bytes2hex(digest, SHA512_DIGEST_LENGTH);
		delete[] digest;
		return encoded;
	}

	inline hash_t hash512(const time_t &timestamp, const hash_t &last_hash, const payload_t &payload)
	{
		return sha512(format("%d%s%s", timestamp, last_hash, payload));
	}

	inline keypair_t eddsa_keypair(const payload_t &secret)
	{
		const auto prv = sha256_raw(secret);
		EVP_PKEY *key = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr, prv, EDDSA_KEY_LENGTH);
		const auto ctx = EVP_PKEY_CTX_new(key, nullptr);
		EVP_PKEY_keygen_init(ctx);
		EVP_PKEY_keygen(ctx, &key);
		EVP_PKEY_CTX_free(ctx);

		size_t size = EDDSA_KEY_LENGTH;
		const auto pub = new byte_t[size];
		EVP_PKEY_get_raw_public_key(key, pub, &size);
		EVP_PKEY_free(key);
		auto pair = keypair_t(bytes2hex(pub, size), bytes2hex(prv,size));
		delete[] pub;
		delete[] prv;
		return pair;
	}

	inline hash_t eddsa_sign(const hash_t &private_key, const payload_t &payload)
	{
		const auto prv = hex2bytes(private_key);
		const auto key = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr, prv.data(), EDDSA_KEY_LENGTH);
		const auto ctx = EVP_MD_CTX_new();
		
		const auto digest = sha512_raw(payload);
		size_t sig_len;
		EVP_DigestSignInit(ctx, nullptr, nullptr, nullptr, key);
		EVP_DigestSign(ctx, nullptr, &sig_len, digest, SHA512_DIGEST_LENGTH);
		
		const auto sig = static_cast<byte_t*>(OPENSSL_zalloc(sig_len));
		EVP_DigestSign(ctx, sig, &sig_len, digest, SHA512_DIGEST_LENGTH);
		hash_t sig_hash = bytes2hex(sig, sig_len);
		OPENSSL_free(sig);
		
		EVP_PKEY_free(key);
		EVP_MD_CTX_free(ctx);

		return sig_hash;
	}

	inline bool eddsa_verify(const hash_t &public_key, const hash_t &signature, const payload_t &payload)
	{
		const auto pub = hex2bytes(public_key);
		const auto sig = hex2bytes(signature);
		const auto key = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, nullptr, pub.data(), 32);
		const auto ctx = EVP_PKEY_CTX_new(key, nullptr);

		const auto digest = sha512_raw(payload);
		EVP_PKEY_verify_init(ctx);
		const auto res = EVP_PKEY_verify(ctx, sig.data(), sig.size(), digest, SHA512_DIGEST_LENGTH);

		delete[] digest;
		EVP_PKEY_free(key);
		EVP_PKEY_CTX_free(ctx);

		return res == 1;
	}
}

#endif//__GNOME_CRYPTO_H__
