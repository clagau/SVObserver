//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file StringTypeDef.h
//*****************************************************************************
// This is the string typedefs required for many projects.
//******************************************************************************

#pragma once
#pragma region Includes
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <utility>
#pragma endregion Includes

namespace SvDef
{
	typedef std::pair<std::string, std::string> StringPair;
	typedef std::set<std::string> StringSet;
	typedef std::vector<std::string> StringVector;
	typedef std::vector<StringPair> StringPairVector;
	typedef std::map<std::string, std::string> TranslateMap;
} //namespace SvDef
