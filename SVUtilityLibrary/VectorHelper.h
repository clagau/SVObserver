//*****************************************************************************
/// \copyright (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file VectorHelper.h
//*****************************************************************************
// Helper functions for std::vector
//******************************************************************************

#pragma once
namespace SvUl
{
template<typename T>
size_t RemoveDuplicates(std::vector<T>& vec)
{
	std::unordered_set<T> seen;

	auto newEnd = std::remove_if(vec.begin(), vec.end(), [&seen](const T& value)
	{
		if (seen.find(value) != std::end(seen))
			return true;

		seen.insert(value);
		return false;
	});

	vec.erase(newEnd, vec.end());

	return vec.size();
}
}