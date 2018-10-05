//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Crypto.cpp
/// All Rights Reserved
//******************************************************************************
/// JWT (JSON Web Token) helper for generating and verfiying tokens.
//******************************************************************************

#include "stdafx.h"

#pragma comment (lib, "crypt32.lib")
#pragma comment (lib, "libcrypto.lib")
#pragma comment (lib, "libssl.lib")

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/md5.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

#include "SVAuthLibrary/Crypto.h"
#include "SVLogLibrary/Logging.h"

namespace
{
struct base64
{
	static const std::array<char, 64>& data()
	{
		static std::array<char, 64> data = {
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
			'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
			'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
			'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
		};
		return data;
	};
	static const std::string& fill()
	{
		static std::string fill = "=";
		return fill;
	}
};
struct base64url
{
	static const std::array<char, 64>& data()
	{
		static std::array<char, 64> data = {
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
			'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
			'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
			'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'
		};
		return data;
	};
	static const std::string& fill()
	{
		static std::string fill = "%3d";
		return fill;
	}
};
}

namespace SvAuth
{

std::string Crypto::md5sum(const std::string& msg)
{
	auto res = std::string(MD5_DIGEST_LENGTH, '\0');
	MD5(
		reinterpret_cast<const unsigned char*>(msg.c_str()),
		msg.size(),
		reinterpret_cast<unsigned char*>(const_cast<char*>(res.data()))
	);
	return res;
}

std::string Crypto::hmac(const std::string& msg, const std::string& key, ALG_ID alg)
{
	unsigned int res_len = SHA256_DIGEST_LENGTH;
	unsigned char result[SHA256_DIGEST_LENGTH];
	unsigned char* data = reinterpret_cast<unsigned char*>(const_cast<char*>(msg.data()));

	auto pCtx = HMAC_CTX_new();
	HMAC_Init_ex(pCtx, key.data(), static_cast<int>(key.size()), EVP_sha256(), NULL);
	HMAC_Update(pCtx, data, msg.size());
	HMAC_Final(pCtx, result, &res_len);
	HMAC_CTX_free(pCtx);

	return std::string(result, result + res_len);
}

std::string Crypto::rsaSign(const std::string& payload, const std::string& privateKey)
{
	std::string result = "";
	BIO* bio = nullptr;
	RSA* rsa = nullptr;
	SHA256_CTX sha256;
	unsigned char hash[SHA256_DIGEST_LENGTH];
	unsigned int sig_len = 0u;
	unsigned char* sig = nullptr;
	int rc;

	bio = BIO_new_mem_buf(privateKey.data(), static_cast<int>(privateKey.size()));
	if (!bio)
	{
		// TODO: fail
		goto cleanup;
	}

	rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);
	if (!rsa)
	{
		goto cleanup;
	}

	rc = SHA256_Init(&sha256);
	if (rc == 0)
	{
		goto cleanup;
	}

	rc = SHA256_Update(&sha256, payload.data(), payload.size());
	if (rc == 0)
	{
		goto cleanup;
	}
	
	rc = SHA256_Final(hash, &sha256);
	if (rc == 0)
	{
		goto cleanup;
	}

	sig_len = 0u;
	sig = static_cast<unsigned char*>(calloc(1, RSA_size(rsa)));

	rc = RSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH, sig, &sig_len, rsa);
	if (rc == 0)
	{
		auto errorTrack = ERR_get_error();
		char *errorChar = new char[120];
		errorChar = ERR_error_string(errorTrack, errorChar);
		SV_LOG_GLOBAL(error) << "RSA signing failed: " << errorChar;
		goto cleanup;
	}

	result = std::string(sig, sig + sig_len);

cleanup:
	if (bio) BIO_free(bio);
	if (rsa) RSA_free(rsa);
	if (sig) free(sig);

	return result;
}

bool Crypto::rsaVerify(
	const std::string& payload,
	const std::string& signature,
	const std::string& publicKey)
{
	bool result = false;
	BIO* bio = nullptr;
	RSA* rsa = nullptr;
	SHA256_CTX sha256;
	unsigned char hash[SHA256_DIGEST_LENGTH];
	unsigned int sig_len = static_cast<unsigned int>(signature.size());
	unsigned char* sig =
		reinterpret_cast<unsigned char*>(const_cast<char*>(signature.data()));
	int rc;

	bio = BIO_new_mem_buf(publicKey.data(), static_cast<int>(publicKey.size()));
	if (!bio)
	{
		//@Todo[][8.10] [05.10.2018] TODO: fail
		goto cleanup;
	}

	rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
	if (!rsa)
	{
		auto errorTrack = ERR_get_error();
		char *errorChar = new char[120];
		errorChar = ERR_error_string(errorTrack, errorChar);
		SV_LOG_GLOBAL(error) << "Parsing public key failed: " << errorChar;
		goto cleanup;
	}

	rc = SHA256_Init(&sha256);
	if (rc == 0)
	{
		goto cleanup;
	}

	rc = SHA256_Update(&sha256, payload.data(), payload.size());
	if (rc == 0)
	{
		goto cleanup;
	}

	rc = SHA256_Final(hash, &sha256);
	if (rc == 0)
	{
		goto cleanup;
	}

	rc = RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH, sig, sig_len, rsa);
	if (rc == 0)
	{
		auto errorTrack = ERR_get_error();
		char *errorChar = new char[120];
		errorChar = ERR_error_string(errorTrack, errorChar);
		SV_LOG_GLOBAL(error) << "RSA verify failed: " << errorChar;
		goto cleanup;
	}

	result = true;

cleanup:
	if (bio) BIO_free(bio);
	if (rsa) RSA_free(rsa);

	return result;
}

std::string Crypto::encodeBase64(const std::string& str)
{
	return encode_base64(str, base64::data(), base64::fill());
}

std::string Crypto::decodeBase64(const std::string& str)
{
	return decode_base64(str, base64::data(), base64::fill());
}

std::string Crypto::encodeBase64Url(const std::string& str)
{
	return encode_base64(str, base64url::data(), base64url::fill());
}

std::string Crypto::decodeBase64Url(const std::string& str)
{
	return decode_base64(str, base64url::data(), base64url::fill());
}

std::string Crypto::encodeBase64Jwt(const std::string& str)
{
	return encode_base64(str, base64url::data(), "");
}

std::string Crypto::decodeBase64Jwt(const std::string& str)
{
	auto copy = str;
	switch (str.size() % 4)
	{
		case 1:
			copy += base64url::fill();
		case 2:
			copy += base64url::fill();
		case 3:
			copy += base64url::fill();
		default:
			break;
	}
	return decode_base64(copy, base64url::data(), base64url::fill());
}

std::string Crypto::encode_base64(const std::string& str, const std::array<char, 64>& alphabet, const std::string& fill)
{
	auto size = str.size();
	std::string res;

	// clear incomplete bytes
	auto fast_size = size - size % 3;
	for (auto i = 0u; i < fast_size;)
	{
		uint32_t octet_a = (unsigned char)str[i++];
		uint32_t octet_b = (unsigned char)str[i++];
		uint32_t octet_c = (unsigned char)str[i++];

		uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		res += alphabet[(triple >> 3 * 6) & 0x3F];
		res += alphabet[(triple >> 2 * 6) & 0x3F];
		res += alphabet[(triple >> 1 * 6) & 0x3F];
		res += alphabet[(triple >> 0 * 6) & 0x3F];
	}

	if (fast_size == size)
		return res;

	auto mod = size % 3;

	uint32_t octet_a = fast_size < size ? (unsigned char)str[fast_size++] : 0;
	uint32_t octet_b = fast_size < size ? (unsigned char)str[fast_size++] : 0;
	uint32_t octet_c = fast_size < size ? (unsigned char)str[fast_size++] : 0;

	auto triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

	switch (mod)
	{
		case 1:
			res += alphabet[(triple >> 3 * 6) & 0x3F];
			res += alphabet[(triple >> 2 * 6) & 0x3F];
			res += fill;
			res += fill;
			break;
		case 2:
			res += alphabet[(triple >> 3 * 6) & 0x3F];
			res += alphabet[(triple >> 2 * 6) & 0x3F];
			res += alphabet[(triple >> 1 * 6) & 0x3F];
			res += fill;
			break;
		default:
			break;
	}

	return res;
}

std::string Crypto::decode_base64(const std::string& str, const std::array<char, 64>& alphabet, const std::string& fill)
{
	size_t size = str.size();

	size_t fill_cnt = 0;
	while (size > fill.size())
	{
		if (str.substr(size - fill.size(), fill.size()) == fill)
		{
			fill_cnt++;
			size -= fill.size();
			if (fill_cnt > 2)
				throw std::runtime_error("Invalid input");
		}
		else break;
	}

	if ((size + fill_cnt) % 4 != 0)
		throw std::runtime_error("Invalid input");

	size_t out_size = size / 4 * 3;
	std::string res;
	res.reserve(out_size);

	auto get_sextet = [&](size_t offset) -> uint32_t
	{
		for (size_t i = 0u; i < alphabet.size(); i++)
		{
			if (alphabet[i] == str[offset])
				return static_cast<uint32_t>(i);
		}
		throw std::runtime_error("Invalid input");
	};

	auto fast_size = size - size % 4;
	for (auto i = 0u; i < fast_size;)
	{
		uint32_t sextet_a = get_sextet(i++);
		uint32_t sextet_b = get_sextet(i++);
		uint32_t sextet_c = get_sextet(i++);
		uint32_t sextet_d = get_sextet(i++);

		auto triple = (sextet_a << 3 * 6)
			+ (sextet_b << 2 * 6)
			+ (sextet_c << 1 * 6)
			+ (sextet_d << 0 * 6);

		res += (triple >> 2 * 8) & 0xFF;
		res += (triple >> 1 * 8) & 0xFF;
		res += (triple >> 0 * 8) & 0xFF;
	}

	if (fill_cnt == 0)
		return res;

	uint32_t triple = (get_sextet(fast_size) << 3 * 6)
		+ (get_sextet(fast_size + 1) << 2 * 6);

	switch (fill_cnt)
	{
		case 1:
			triple |= (get_sextet(fast_size + 2) << 1 * 6);
			res += (triple >> 2 * 8) & 0xFF;
			res += (triple >> 1 * 8) & 0xFF;
			break;
		case 2:
			res += (triple >> 2 * 8) & 0xFF;
			break;
		default:
			break;
	}

	return res;
}

} // namespace SvAuth
