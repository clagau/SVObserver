// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBlobInterface
// * .File Name       : $Workfile:   SVMatroxBlobInterface.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   12 Nov 2014 08:24:36  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVMatroxBlobInterface.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxErrorEnum.h"
#include "SVMatroxHelper.h"
#include "SVMatroxResourceMonitor.h"
#include "SVCommandLibrary/SVCommandDataHolder.h"
#pragma endregion Includes

/**
@SVOperationName Default Constructor

@SVOperationDescription

*/
SVMatroxBlobInterface::SVMatroxBlobInterface()
{
}

/**
@SVOperationName Destructor

@SVOperationDescription

*/
SVMatroxBlobInterface::~SVMatroxBlobInterface()
{
}

/**
@SVOperationName Create - Blob Feature List

@SVOperationDescription This function creates a Blob Feature List.

*/
HRESULT SVMatroxBlobInterface::CreateContext(__int64& rContextId)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		__int64 l_NewId = MblobAlloc(M_DEFAULT_HOST, M_DEFAULT, M_DEFAULT, M_NULL);
		l_Code = SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == S_OK)
		{
			SVMatroxResourceMonitor::InsertIdentifier(SVBlobFeatureListID, l_NewId);

			if (M_NULL != rContextId)
			{
				l_Code = DestroyContext(rContextId);
			}
			rContextId = l_NewId;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName Blob Create - Blob Result

@SVOperationDescription This function creates a blob result object.

*/
HRESULT SVMatroxBlobInterface::CreateResult(__int64& rResultId)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{



		__int64 l_NewId = MblobAllocResult(M_DEFAULT_HOST, M_NULL);
		l_Code = SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == S_OK)
		{
			SVMatroxResourceMonitor::InsertIdentifier(SVBlobResultID, l_NewId);

			if (M_NULL != rResultId)
			{
				l_Code = DestroyResult(rResultId);
			}
			rResultId = l_NewId;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName Destroy - Blob Feature List

@SVOperationDescription This function frees the blob feature list.

*/
HRESULT SVMatroxBlobInterface::DestroyContext(__int64& rBlobId)
{
	return DestroyMatroxId(rBlobId, MblobFree, SVMatroxIdentifierEnum::SVBlobFeatureListID);
}

/**
@SVOperationName Destroy - Blob Result

@SVOperationDescription This function frees the blob result resource.

*/
HRESULT SVMatroxBlobInterface::DestroyResult(__int64& rResultId)
{
	return DestroyMatroxId(rResultId, MblobFree, SVMatroxIdentifierEnum::SVBlobResultID);
}


/**
@SVOperationName Execute

@SVOperationDescription This function does the work of finding blobs in the "Blob Identifier Image".

*/
HRESULT SVMatroxBlobInterface::Execute(const __int64& rResultId, const SVMatroxBuffer& rIdentId, const __int64& rBlobId)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!rIdentId.empty() && M_NULL != rBlobId && M_NULL != rResultId)
		{
			MblobCalculate(rBlobId, rIdentId.GetIdentifier(), M_NULL, rResultId);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName GetResult

@SVOperationDescription This function gets an array of doubles for the results.  The number of positions in the array will be the same as the value returned by GetNumber.

*/
HRESULT SVMatroxBlobInterface::GetResult(const __int64& rResultId, SVBlobSelectionEnum lControlType, double* pdResultsArray)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (M_NULL != rResultId)
		{
			MblobGetResult(rResultId, M_DEFAULT, lControlType | M_BINARY, pdResultsArray);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName GetResult

@SVOperationDescription This function gets an array of longs for the results.  The number of positions in the array will be the same as the value returned by GetNumber.

*/
HRESULT SVMatroxBlobInterface::GetResult(const __int64& rResultId, SVBlobSelectionEnum lControlType, long* plResultsArray)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (M_NULL != rResultId)
		{
			MblobGetResult(rResultId, lControlType + M_TYPE_LONG, plResultsArray);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}


/**
@SVOperationName GetNumber

@SVOperationDescription This function gets the number of currently included blobs.

*/
HRESULT SVMatroxBlobInterface::GetNumber(const __int64& rResultId, long& rNumber)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (M_NULL != rResultId)
		{
			MIL_INT lValue;
			MblobGetResult(rResultId, M_DEFAULT, M_NUMBER + M_TYPE_MIL_INT, &lValue);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			if (l_Code == S_OK)
			{
				rNumber = static_cast<long>(lValue);
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}


/**
@SVOperationName Set - double

@SVOperationDescription This function sets the selected control type with the supplied double value

*/
HRESULT SVMatroxBlobInterface::Set(const __int64& rContextId, SVBlobControlEnum lControlType, const double lControlValue)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (M_NULL != rContextId)
		{
			MblobControl(rContextId, lControlType, lControlValue);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;

}

/**
@SVOperationName Set - long

@SVOperationDescription This function sets the selected control type with the supplied long value

*/
HRESULT SVMatroxBlobInterface::Set(const __int64& rContextId, SVBlobControlEnum lControlType, const long lControlValue)
{
	return Set(rContextId, lControlType, static_cast<double>(lControlValue));
}

/**
@SVOperationName GetForeground - bool

@SVOperationDescription This function gets the Foreground control value

*/
HRESULT SVMatroxBlobInterface::GetForeground(const __int64& rContextId, bool& rIsDarkForeground)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (M_NULL != rContextId)
		{
			long value;
			MblobInquire(rContextId, M_FOREGROUND_VALUE, &value);
			rIsDarkForeground = (value) ? true : false;
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName SetForeground - bool

@SVOperationDescription This function sets the Foreground control value

*/
HRESULT SVMatroxBlobInterface::SetForeground(const __int64& rContextId, bool isDarkForeground)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (M_NULL != rContextId)
		{
			MblobControl(rContextId, M_FOREGROUND_VALUE, (isDarkForeground) ? M_ZERO : M_NONZERO);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName GetIdentifier - bool

@SVOperationDescription This function gets the Blob Identifier Type control value

*/
HRESULT SVMatroxBlobInterface::GetIdentifier(const __int64& rContextId, bool& bBinary)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (M_NULL != rContextId)
		{
			long value;
			MblobInquire(rContextId, M_IDENTIFIER_TYPE, &value);
			bBinary = (value) ? true : false;
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName SetIdentifier - bool

@SVOperationDescription This function sets the Blob Identifier Type control value

*/
HRESULT SVMatroxBlobInterface::SetIdentifier(const __int64& rContextId, bool bBinary)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (M_NULL != rContextId)
		{
			MblobControl(rContextId, M_IDENTIFIER_TYPE, (bBinary) ? M_BINARY : M_GRAYSCALE);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName Blob Fill

@SVOperationDescription This function draws the found blobs in an image that meet certian criteria with a specific color.

*/
HRESULT SVMatroxBlobInterface::BlobFill(const __int64& rResultId, const SVMatroxBuffer& rImageID, SVBlobControlEnum eCriterion, const long& lColor)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		// This function replaces MblobFill
		if (M_NULL != rResultId)
		{
			MgraColor(M_DEFAULT, lColor);
			MblobDraw(M_DEFAULT, rResultId, rImageID.GetIdentifier(), M_DRAW_BLOBS, eCriterion, M_DEFAULT);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;

}

/**
@SVOperationName Blob Select

@SVOperationDescription This function selects blobs that meet a specified criterion. These blobs will be included in or excluded from future operations (calculations or result retrieval), or deleted entirely from the result buffer.

*/
HRESULT SVMatroxBlobInterface::BlobSelect(const __int64& rResultId, SVBlobOperationEnum eOperation, SVBlobSelectionEnum eSelection, SVConditionEnum eCondition, const double& dCondLow, const double& dCondHigh)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (M_NULL != rResultId)
		{
			MblobSelect(rResultId,
				eOperation,
				eSelection,
				eCondition,
				dCondLow,
				dCondHigh);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName Blob Select Feature

@SVOperationDescription This function selects the feature(s) to be calculated by Execute() when using the specified feature list.

*/
HRESULT SVMatroxBlobInterface::BlobSelectFeature(const __int64& rBlobId, std::set<SVBlobSelectionEnum> featureSet)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		// This function replaces MblobSelectFeature
		if (M_NULL != rBlobId)
		{
			auto endIter = featureSet.end();
			MIL_DOUBLE controlValue = (featureSet.find(SVEBlobBoxXMin) != endIter || featureSet.find(SVEBlobBoxYMin) != endIter ||
				featureSet.find(SVEBlobBoxXMax) != endIter || featureSet.find(SVEBlobBoxYMax) != endIter ||
				featureSet.find(SVEBlobFeretX) != endIter || featureSet.find(SVEBlobFeretY) != endIter ||
				featureSet.find(SVEBlobFirstPointX) != endIter || featureSet.find(SVEBlobFirstPointY) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_BOX, controlValue);
			controlValue = (featureSet.find(SVEBlobBreadth) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_BREADTH, controlValue);
			controlValue = (featureSet.find(SVEBlobNumberOfChainedPixels) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_CHAINS, controlValue);
			controlValue = (featureSet.find(SVEBlobCompactness) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_COMPACTNESS, controlValue);
			controlValue = (featureSet.find(SVEBlobXMaxAtYMax) != endIter || featureSet.find(SVEBlobXMinAtYMin) != endIter ||
				featureSet.find(SVEBlobYMaxAtXMin) != endIter || featureSet.find(SVEBlobYMinAtXMax) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_CONTACT_POINTS, controlValue);
			controlValue = (featureSet.find(SVEBlobConvexParameter) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_CONVEX_PERIMETER, controlValue);
			controlValue = (featureSet.find(SVEBlobElongation) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_ELONGATION, controlValue);
			controlValue = (featureSet.find(SVEBlobEulerNumber) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_EULER_NUMBER, controlValue);
			controlValue = (featureSet.find(SVEBlobGeneralFeret) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_FERET_GENERAL, controlValue);
			controlValue = (featureSet.find(SVEBlobFeretElongation) != endIter || featureSet.find(SVEBlobFeretMaxAngle) != endIter ||
				featureSet.find(SVEBlobFeretMaxDia) != endIter || featureSet.find(SVEBlobFeretMeanDia) != endIter ||
				featureSet.find(SVEBlobFeretMinAngle) != endIter || featureSet.find(SVEBlobFeretMinDia) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_FERETS, controlValue);
			controlValue = (featureSet.find(SVEBlobIntercept) != endIter || featureSet.find(SVEBlobIntercept45) != endIter ||
				featureSet.find(SVEBlobIntercept90) != endIter || featureSet.find(SVEBlobIntercept135) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_INTERCEPT, controlValue);
			controlValue = (featureSet.find(SVEBlobLength) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_LENGTH, controlValue);
			controlValue = (featureSet.find(SVEBlobNumberOfHoles) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_NUMBER_OF_HOLES, controlValue);
			controlValue = (featureSet.find(SVEBlobPerimeter) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_PERIMETER, controlValue);
			controlValue = (featureSet.find(SVEBlobRoughness) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_ROUGHNESS, controlValue);
			controlValue = (featureSet.find(SVEBlobNumberOfRuns) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_RUNS, controlValue);
			controlValue = (featureSet.find(SVEBlobSumPixel) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_SUM_PIXEL, controlValue);
			controlValue = (featureSet.find(SVEBlobCenterOfGravityX) != endIter || featureSet.find(SVEBlobCenterOfGravityY) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_CENTER_OF_GRAVITY, controlValue);
			controlValue = (featureSet.find(SVEBlobGeneralMoment) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_MOMENT_GENERAL, controlValue);
			controlValue = (featureSet.find(SVEBlobMomentX0Y1) != endIter || featureSet.find(SVEBlobMomentX1Y0) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_MOMENT_FIRST_ORDER, controlValue);
			controlValue = (featureSet.find(SVEBlobMomentX1Y1) != endIter || featureSet.find(SVEBlobMomentX0Y2) != endIter ||
				featureSet.find(SVEBlobMomentX2Y0) != endIter || featureSet.find(SVEBlobMomentCentralX0Y2) != endIter ||
				featureSet.find(SVEBlobMomentCentralX2Y0) != endIter || featureSet.find(SVEBlobMomentCentralX1Y1) != endIter ||
				featureSet.find(SVEBlobAxisPrincipalAngle) != endIter || featureSet.find(SVEBlobAxisSecondaryAngle) != endIter) ? M_ENABLE : M_DISABLE;
			MblobControl(rBlobId, M_MOMENT_SECOND_ORDER, controlValue);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

HRESULT SVMatroxBlobInterface::FillResults(const SVBlobOffsetList& rBlobList, SVCommandDataHolder& rResults)
{
	long l_lNumberBlobs = static_cast<long>(rBlobList.size());

	HRESULT l_Status = rResults.SetValue(_T("Number of Blobs"), l_lNumberBlobs, true);

	if (S_OK == l_Status)
	{
		for (int i = 0; i < l_lNumberBlobs; ++i)
		{
			SVCommandDataHolder l_Blob;
			const POINT& l_rPoint = rBlobList[i];

			l_Status = l_Blob.SetValue(_T("Offset X"), l_rPoint.x, true);

			if (S_OK == l_Status)
			{
				l_Status = l_Blob.SetValue(_T("Offset Y"), l_rPoint.y, true);
			}

			if (S_OK == l_Status)
			{
				std::string l_Name;

				l_Name = SvUl::Format(_T("Blob Result %d"), i);

				l_Status = rResults.SetContainer(_bstr_t(l_Name.c_str()), l_Blob, true);
			}
		}
	}

	return l_Status;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
class SortBlobs : std::binary_function<const SVMatroxBlobInterface::SVBlob&, const SVMatroxBlobInterface::SVBlob&, bool>
{
private:
	int m_type;

public:
	explicit SortBlobs(int sortCriteria) : m_type(sortCriteria) {}

	bool operator()(const SVMatroxBlobInterface::SVBlob& lBlob, const SVMatroxBlobInterface::SVBlob& rBlob)
	{
		return SVMatroxBlobInterface::Compare(m_type, lBlob, rBlob);
	}
};

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
void SVMatroxBlobInterface::Sort(SVBlobList& list, int sortCriteria)
{
	// Sort by specified sort criteria
	std::sort(list.begin(), list.end(), SortBlobs(sortCriteria));
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
bool SVMatroxBlobInterface::Compare(int criteria, const SVBlob& lBlob, const SVBlob& rBlob)
{
	bool bRetVal = true;
	if (criteria & SortAscending)
	{
		if (criteria & SortBy_XY)
		{
			// sort by x(major), y(minor)
			if (lBlob.center.x > rBlob.center.x ||
				(lBlob.center.x == rBlob.center.x &&
				lBlob.center.y >= rBlob.center.y))
				bRetVal = false;
		}
		else if (criteria & SortBy_YX)
		{
			// sort by y(major), x(minor)
			if (lBlob.center.y > rBlob.center.y ||
				(lBlob.center.y == rBlob.center.y &&
				lBlob.center.x >= rBlob.center.x))
				bRetVal = false;
		}
		else if (criteria & SortBy_X)
		{
			if (lBlob.center.x >= rBlob.center.x)
				bRetVal = false;
		}
		else if (criteria & SortBy_Y)
		{
			if (lBlob.center.y >= rBlob.center.y)
				bRetVal = false;
		}
		else if (criteria & SortBy_Area)
		{
			if (lBlob.area >= rBlob.area)
				bRetVal = false;
		}
	}
	else
	{
		bRetVal = false;

		if (criteria & SortBy_XY)
		{
			// sort by x(major), y(minor)
			if (lBlob.center.x > rBlob.center.x ||
				(lBlob.center.x == rBlob.center.x &&
				lBlob.center.y > rBlob.center.y))
				bRetVal = true;
		}
		else if (criteria & SortBy_YX)
		{
			// sort by y(major), x(minor)
			if (lBlob.center.y > rBlob.center.y ||
				(lBlob.center.y == rBlob.center.y &&
				lBlob.center.x > rBlob.center.x))
				bRetVal = true;
		}
		else if (criteria & SortBy_X)
		{
			if (lBlob.center.x > rBlob.center.x)
				bRetVal = true;
		}
		else if (criteria & SortBy_Y)
		{
			if (lBlob.center.y > rBlob.center.y)
				bRetVal = true;
		}
		else if (criteria & SortBy_Area)
		{
			if (lBlob.area > rBlob.area)
				bRetVal = true;
		}
	}
	return bRetVal;
}

long SVMatroxBlobInterface::CalculateOptimalWidth(const SVBlobList& rList, unsigned short padding)
{
	long avgWidth = 0;

	for (SVBlobList::const_iterator it = rList.begin(); it != rList.end(); ++it)
	{
		const SVBlob& blob = (*it);
		int width = blob.boundingRect.right - blob.boundingRect.left;
		if (IsValidDimension(width + padding))
		{
			if (!avgWidth)
			{
				avgWidth = width;
			}
			else
			{
				avgWidth += width;
				avgWidth /= 2;
			}
		}
	}
	return avgWidth;
}

long SVMatroxBlobInterface::CalculateOptimalHeight(const SVBlobList& rList, unsigned short padding)
{
	long avgHeight = 0;

	for (SVBlobList::const_iterator it = rList.begin(); it != rList.end(); ++it)
	{
		const SVBlob& blob = (*it);
		int height = blob.boundingRect.bottom - blob.boundingRect.top;

		if (IsValidDimension(height + padding))
		{
			if (!avgHeight)
			{
				avgHeight = height;
			}
			else
			{
				avgHeight += height;
				avgHeight /= 2;
			}
		}
	}
	return avgHeight;
}

bool SVMatroxBlobInterface::IsValidSize(int width, int height)
{
	bool bRetVal = false;

	// Reject Blobs that are 1 pixel wide or high
	// Reject Blobs that are greater than 255
	if (IsValidDimension(width) && IsValidDimension(height))
	{
		bRetVal = true;
	}
	return bRetVal;
}

bool SVMatroxBlobInterface::IsValidDimension(int value)
{
	bool bRetVal = false;

	// Reject Blobs that are less than 6 pixels wide or high
	// Reject Blobs that are greater than 256 pixel wide or high
	if (value > 5 && value < 257)
	{
		bRetVal = true;
	}
	return bRetVal;
}

bool SVMatroxBlobInterface::IsValidCandidate(const SVBlob& blob, const SIZE& size, const POINT& offset)
{
	bool bRetVal = false;

	RECT rect = blob.boundingRect;
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	// Reject Blobs based on minimum/maximum width and height
	if (IsValidSize(width + offset.x, height + offset.y))
	{
		// Reject Blobs wider or higher than optimal ?
		//if (width <= size.cx && height <= size.cy)
		{
			bRetVal = true;
		}
	}
	return bRetVal;
}

HRESULT SVMatroxBlobInterface::CreateBlobOffsetResultList(const SVBlobList& results, SVBlobOffsetList& offsetList, const SIZE& size, const POINT& offset, const SIZE& imageSize)
{
	HRESULT hr = S_OK;

	//
	offsetList.clear();

	// Get the offset(s) of the blobs
	for (SVBlobList::const_iterator it = results.begin(); it != results.end(); ++it)
	{
		const SVBlob& blob = (*it);
		if (IsValidCandidate(blob, size, offset))
		{
			// Center ROI's on Blobs
			SIZE blobSize = {(blob.boundingRect.right - blob.boundingRect.left) + offset.x, (blob.boundingRect.bottom - blob.boundingRect.top) + offset.y};
			POINT centerPos = {blob.boundingRect.left + (blobSize.cx / 2), blob.boundingRect.top + (blobSize.cy / 2)};
			POINT pos = {centerPos.x - (size.cx / 2), centerPos.y - (size.cy / 2)};

			// Ensure Blob Rects are within the Image Size
			// Check if off the Right Edge
			if (pos.x + size.cx > imageSize.cx)
			{
				// move it
				pos.x -= ((pos.x + size.cx) - imageSize.cx);
			}
			// Check if off the Bottom Edge
			if (pos.y + size.cy > imageSize.cy)
			{
				// move it
				pos.y -= ((pos.y + size.cy) - imageSize.cy);
			}
			// Check if off the Left Edge
			if (pos.x < 0)
			{
				// move it
				pos.x = 0;
			}
			// Check if off the Top Edge
			if (pos.y < 0)
			{
				// move it
				pos.y = 0;
			}

			// 
			offsetList.push_back(pos);
		}
	}
	return hr;
}