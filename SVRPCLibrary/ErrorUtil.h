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

#include "SVProtobuf/Envelope.h"

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

namespace SvRpc
{
static std::runtime_error errorToException(const SvPenv::Error& err)
{
	return std::runtime_error(std::to_string(err));
}

static std::exception_ptr errorToExceptionPtr(const SvPenv::Error& err)
{
	return std::make_exception_ptr(errorToException(err));
}

static SvPenv::Error build_error(SvPenv::ErrorCode ErrorCode, const char* pMsg = nullptr)
{
	SvPenv::Error Result;
	Result.set_errorcode(ErrorCode);
	if (nullptr != pMsg)
	{
		Result.set_message(pMsg);
	}
	return Result;
}

} // namespace SvRpc
