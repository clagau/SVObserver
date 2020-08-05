//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for the blob analyzer 2
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvOi
{
	class IBlobAnalyzer2
	{
	public:
		virtual ~IBlobAnalyzer2() {}

		virtual SvPb::InspectionCmdResponse getFeaturesData() const = 0;
		virtual SvPb::InspectionCmdResponse setFeatures(SvPb::SetFeaturesRequest request) = 0;
	};

	SvPb::InspectionCmdResponse getAvailableFeatures();
} //namespace SvOi