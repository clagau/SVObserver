//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for the blob analyzer
//******************************************************************************
#pragma once

#pragma region Includes
#include "IObjectClass.h"
#include "NameValueVector.h"
#pragma endregion Includes

namespace SvOi
{
	enum { SV_MAX_NUMBER_OF_BLOBS = 100 };

	enum SVBlobFeatureEnum
	{
		SV_AREA = 0,            // 0
		SV_BOXX_MAX,            // 1
		SV_BOXX_MIN,            // 2
		SV_BOXY_MAX,            // 3
		SV_BOXY_MIN,            // 4
		SV_BREADTH,             // 5
		SV_CENTEROFGRAVITY_X,   // 6
		SV_CENTEROFGRAVITY_Y,   // 7
		SV_CONVEX_PERIMETER,    // 8
		SV_FERET_ELONGATION,    // 9
		SV_FERETMAX_ANGLE,      // 10
		SV_FERETMAX_DIAMETER,   // 11
		SV_FERETMEAN_DIAMETER,  // 12
		SV_FERETMIN_ANGLE,      // 13
		SV_FERETMIN_DIAMETER,   // 14
		SV_FERET_X,             // 15
		SV_FERET_Y,             // 16
		SV_FIRSTPOINT_X,        // 17
		SV_FIRSTPOINT_Y,        // 18
		SV_LABEL,               // 19
		SV_LENGTH,              // 20
		SV_NBROF_HOLES,         // 21
		SV_PERIMETER,           // 22
		SV_ROUGHNESS,           // 23
		//- SV_SUM_PIXEL - This setting does not work on binary images, and is
		//- therefore being removed from available features at this time.  The 
		//- selection must still be available in the list in order to support
		//- previous configurations, but will not appear in the list of available
		//- features.  This is being enforced within 
		//- SVBlobAnalyzeFeatureListBoxClass::Init ().
		SV_SUM_PIXEL,           // 24  // Will not work with thresholded images i.e.
		// binary images vs. grey scale images.
		//
		// v3.1 Added Blob Features
		//
		SV_COMPACTNESS,         // 25
		SV_NBR_RUNS,            // 26
		SV_XMINAT_YMIN,         // 27
		SV_XMAXAT_YMAX,         // 28
		SV_YMINAT_XMAX,         // 29
		SV_YMAXAT_XMIN,         // 30
		SV_ELONGATION,          // 31
		SV_INTERCEPT_0,         // 32
		SV_INTERCEPT_45,        // 33
		SV_INTERCEPT_90,        // 34
		SV_INTERCEPT_135,       // 35
		SV_MOMENT_X0Y1,         // 36
		SV_MOMENT_X1Y0,         // 37
		SV_MOMENT_X1Y1,         // 38
		SV_MOMENT_X0Y2,         // 39
		SV_MOMENT_X2Y0,         // 40
		SV_CENTRAL_X0Y2,        // 41
		SV_CENTRAL_X2Y0,        // 42
		SV_CENTRAL_X1Y1,        // 43
		SV_AXISPRINCIPAL_ANGLE, // 44
		SV_AXISSECONDARY_ANGLE, // 45
		SV_EULER_NBR,           // 46
		SV_CENTER_X_SOURCE,     // 47
		SV_CENTER_Y_SOURCE,		// 48

		SV_NUMBER_OF_BLOB_FEATURES
	};

	class IBlobAnalyzer
	{
	public:
		virtual ~IBlobAnalyzer() {}

		virtual NameValueVector getFeatureList(bool isSelected) const = 0;
		virtual SvOi::IObjectClass* getResultObject(int Feature) = 0;
		virtual IObjectClass* getResultBlob() = 0;
	};
} //namespace SvOi