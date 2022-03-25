#include "stdafx.h"

#include <boost/test/unit_test.hpp>

#include <SVProtobuf/SVRC.h>
#include <SVRPCLibrary/ObserverWrapper.h>

using namespace SvPb;
using namespace SvPenv;
using namespace SvRpc;

BOOST_AUTO_TEST_CASE(should_wrap_the_observer)
{
	bool onNextCalled = false;
	auto onNext = [&](Envelope&&) -> SvSyl::SVFuture<void> {
		onNextCalled = true;
		return SvSyl::SVFuture<void>::make_ready();
	};
	bool onFinishCalled = false;
	auto onFinish = [&]() { onFinishCalled = true; };
	bool onErrorCalled = false;
	auto onError = [&](const SvPenv::Error&) { onErrorCalled = true; };

	auto observer = Observer<Envelope>(onNext, onFinish, onError);
	auto handler = [](
		GetSVObserverVersionRequest&& req,
		Observer<GetVersionResponse> observer,
		ServerStreamContext::Ptr ctx)
	{
		GetVersionResponse res;
		res.set_version("1.2.3");
		observer.onNext(std::move(res));
		observer.finish();
	};
	auto wrapper = ObserverWrapper<SVRCMessages, GetSVObserverVersionRequest, GetVersionResponse>(handler);

	Envelope emptyEnvelope;
	wrapper(std::move(emptyEnvelope), observer, nullptr);

	BOOST_REQUIRE_EQUAL(onNextCalled, false);
	BOOST_REQUIRE_EQUAL(onFinishCalled, false);
	BOOST_REQUIRE_EQUAL(onErrorCalled, true);
}
