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

#include "SVProtobuf/envelope.h"

namespace std
{
static std::string to_string(const SVRPC::Error& err)
{
	std::stringstream ss;
	ss << "Error " << err.error_code() << " " << SVRPC::ErrorCode_Name(err.error_code());
	if (!err.message().empty())
	{
		ss << ": " << err.message();
	}
	return ss.str();
}

} // namespace std

namespace SVRPC
{
static std::runtime_error errorToException(const Error& err)
{
	return std::runtime_error(std::to_string(err));
}

static std::exception_ptr errorToExceptionPtr(const Error& err)
{
	return std::make_exception_ptr(errorToException(err));
}

static Error build_error(ErrorCode ec, const char* msg = nullptr)
{
	Error err;
	err.set_error_code(ec);
	if (msg)
	{
		err.set_message(msg);
	}
	return err;
}

} // namespace SVRPC
