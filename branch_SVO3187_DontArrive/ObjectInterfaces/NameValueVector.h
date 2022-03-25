//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Declaration of a Name Value Pair and vector.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <utility>

#pragma endregion Includes

namespace SvOi
{
	typedef std::pair<std::string, long> NameValuePair;
	typedef std::vector<NameValuePair> NameValueVector;
} //namespace SvOi
