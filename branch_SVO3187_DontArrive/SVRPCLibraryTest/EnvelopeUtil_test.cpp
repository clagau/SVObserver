#include "stdafx.h"

#include <boost/test/included/unit_test.hpp>

#include <SVProtobuf/SVRC.h>
#include <SVRPCLibrary/EnvelopeUtil.h>

using namespace SvPb;
using namespace SvPenv;
using namespace SvRpc;

static SVRCMessages build_message_with_version_payload(const char* version)
{
	SVRCMessages input;
	auto& res = *input.mutable_getversionresponse();
	res.set_version(version);
	return input;
}

BOOST_AUTO_TEST_CASE(should_wrap_and_unwrap_payload)
{
	SVRCMessages input = build_message_with_version_payload("1.2.3");

	Envelope envelope;
	wrap_payload(envelope, input);

	SVRCMessages output;
	auto success = unwrap_payload(envelope, output);
	
	BOOST_REQUIRE_EQUAL(success, true);
	BOOST_REQUIRE_EQUAL(output.message_case(), SVRCMessages::kGetVersionResponse);
	BOOST_REQUIRE_EQUAL(output.getversionresponse().version(), "1.2.3");
}

BOOST_AUTO_TEST_CASE(should_fail_unwrapping_when_payload_is_empty)
{
	Envelope envelope;
	envelope.Clear(); // is clear by default already, but let's make it clear.

	SVRCMessages output;
	auto success = unwrap_payload(envelope, output);

	BOOST_REQUIRE_EQUAL(success, false);
}

BOOST_AUTO_TEST_CASE(should_fail_unwrapping_to_wrong_payload_type)
{
	SVRCMessages input = build_message_with_version_payload("1.2.3");

	Envelope envelope;
	wrap_payload(envelope, input);

	GetVersionResponse output; // wrong type, must be the a message containing a case with all possible messages
	auto success = unwrap_payload(envelope, output);

	BOOST_REQUIRE_EQUAL(success, false);
}
