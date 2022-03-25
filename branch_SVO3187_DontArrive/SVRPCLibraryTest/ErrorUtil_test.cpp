#include "stdafx.h"

#include <boost/test/unit_test.hpp>

#include <SVRPCLibrary/ErrorUtil.h>

using namespace SvPenv;
using namespace SvRpc;

BOOST_AUTO_TEST_CASE(should_build_error)
{
	auto err = build_error(badRequest, "my error message");

	BOOST_REQUIRE_EQUAL(err.errorcode(), SvPenv::badRequest);
	BOOST_REQUIRE_EQUAL(err.message(), "my error message");
}

BOOST_AUTO_TEST_CASE(should_serialize_error_to_string)
{
	auto err = build_error(badRequest, "my error message");
	auto str = std::to_string(err);

	BOOST_REQUIRE_EQUAL(str, "Error 400 badRequest: my error message");
}

BOOST_AUTO_TEST_CASE(should_create_exception_from_error)
{
	auto err = build_error(badRequest, "my error message");
	auto ex = errorToException(err);

	BOOST_REQUIRE_EQUAL(ex.what(), "Error 400 badRequest: my error message");
}

BOOST_AUTO_TEST_CASE(should_create_exception_ptr_from_error)
{
	auto err = build_error(badRequest, "my error message");
	auto exPtr = errorToExceptionPtr(err);

	try
	{
		std::rethrow_exception(exPtr);
		BOOST_REQUIRE_EQUAL(true, false); // must not reach this point
	}
	catch (const std::runtime_error& ex)
	{
		BOOST_REQUIRE_EQUAL(ex.what(), "Error 400 badRequest: my error message");
	}
}