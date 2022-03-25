#pragma once

#include "SVProtoBuf/InspectionCommands.h"
namespace SvOi
{
	//this class is a interface. It should only have pure virtual public method and no member variables
	//Purpose: access a SVLinearAnalyzer object from InspectionCommands
	class ISVLinearAnalyzer
	{

	public:
		virtual ~ISVLinearAnalyzer() = default;

		virtual SvPb::GetNormalizerValuesResponse getNormalizerValues() = 0;
		virtual SvPb::GetNormalizerValuesResponse setNormalizerRanges(const SvPb::SetNormalizerRangesRequest& ) = 0;
	};
}
