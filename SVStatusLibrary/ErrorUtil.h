//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file ErrorUtil.h
/// All Rights Reserved
//******************************************************************************
/// Utils for creating, converting and serializing Error (see envelope.proto).
//******************************************************************************

#pragma once

#include <exception>
#include <sstream>
#include <string>

#include "SVProtoBuf/Envelope.h"

namespace std
{
static std::string to_string(const SvPenv::Error& err)
{
	std::stringstream ss;
	ss << "Error " << err.errorcode() << " " << SvPenv::ErrorCode_Name(err.errorcode());
	if (!err.message().empty())
	{
		ss << ": " << err.message();
	}
	return ss.str();
}

} // namespace std

namespace SvUl
{
inline std::runtime_error errorToException(const SvPenv::Error& err)
{
	return std::runtime_error(std::to_string(err));
}

inline std::exception_ptr errorToExceptionPtr(const SvPenv::Error& err)
{
	return std::make_exception_ptr(errorToException(err));
}

inline SvPenv::Error build_error(SvPenv::ErrorCode ErrorCode, const char* pMsg = nullptr)
{
	SvPenv::Error Result;
	Result.set_errorcode(ErrorCode);
	if (nullptr != pMsg)
	{
		Result.set_message(pMsg);
	}
	return Result;
}

template<class T>
SvPenv::Error build_packed_error(SvPenv::ErrorCode ErrorCode, const char* pMsg, const T& payload)
{
	SvPenv::Error Result;
	Result.set_errorcode(ErrorCode);
	if (nullptr != pMsg)
	{
		Result.set_message(pMsg);
	}
	Result.mutable_payload()->PackFrom(payload);
	return Result;
}

} // namespace SvRpc
