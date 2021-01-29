//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/10/15,2020/10/15 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file SHA256.cpp
/// DESCRIPTION
//******************************************************************************

#pragma region Includes 
#include "stdafx.h"
#include <fstream>
#include <sstream>
#include <iomanip>

// OpenSSL Library
#pragma comment (lib, "crypt32.lib")
#pragma comment (lib, "libcrypto.lib")
#pragma comment (lib, "libssl.lib")
#include <openssl/sha.h>
#include "sha256.h"

#pragma endregion Includes





namespace SvUl
{
	std::string SHA256(const char* const path)
	{
#ifdef TRACE_HASH_TIME
		double starttime = SvUl::GetTimeStamp();
#endif 				
		
		std::ifstream fp(path, std::ios::in | std::ios::binary);

		if (false == fp.good())
		{
			std::ostringstream os;
			os << "Cannot open \"" << path << "\": " << std::strerror(errno) << ".";
			throw std::runtime_error(os.str());
		}

		constexpr const std::size_t buffer_size{ 1 << 12 };
		char buffer[buffer_size];

		unsigned char hash[SHA256_DIGEST_LENGTH] = { 0 };

		SHA256_CTX ctx;
		SHA256_Init(&ctx);

		while (fp.good())
		{
			fp.read(buffer, buffer_size);
			SHA256_Update(&ctx, buffer, static_cast<size_t>(fp.gcount()));
		}

		SHA256_Final(hash, &ctx);
		fp.close();

		std::ostringstream os;
		os << std::hex << std::setfill('0');

		for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
		{
			os << std::setw(2) << static_cast<unsigned int>(hash[i]);
		}
#ifdef TRACE_HASH_TIME
		double time = SvUl::GetTimeStamp() - starttime;
		TRACE1("hash: %s\n ", hash.c_str());
		TRACE1("Time for hash: %lf\n ", time);
#endif
		return os.str();
	}
}
