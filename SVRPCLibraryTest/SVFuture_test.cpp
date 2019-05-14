#include "stdafx.h"

#include <boost/test/unit_test.hpp>

#include <SVRPCLibrary/ErrorUtil.h>
#include <SVSystemLibrary/SVFuture.h>

using namespace SvRpc;
using namespace SvSyl;

BOOST_AUTO_TEST_CASE(void_future_should_not_block_when_value_already_set_before_get_future)
{
	SVPromise<void> promise;

	promise.set_value();
	auto future = promise.get_future();

	future.get();
}

BOOST_AUTO_TEST_CASE(void_future_should_not_block_when_value_set_after_get_future)
{
	SVPromise<void> promise;

	auto future = promise.get_future();
	promise.set_value();

	future.get();
}

BOOST_AUTO_TEST_CASE(int_future_should_not_block_when_value_already_set_before_get_future)
{
	SVPromise<int> promise;

	promise.set_value(23);
	auto future = promise.get_future();

	auto value = future.get();
	BOOST_REQUIRE_EQUAL(23, value);
}

BOOST_AUTO_TEST_CASE(int_future_should_not_block_when_value_set_after_get_future)
{
	SVPromise<int> promise;
	
	auto future = promise.get_future();
	promise.set_value(23);

	auto value = future.get();
	BOOST_REQUIRE_EQUAL(23, value);
}

BOOST_AUTO_TEST_CASE(int_future_should_rethrow_exception)
{
	SVPromise<int> promise;

	auto future = promise.get_future();

	SvPenv::Error err;
	promise.set_exception(errorToExceptionPtr(err));

	BOOST_CHECK_THROW(future.get(), std::runtime_error);
}
