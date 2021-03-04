//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLinearImageOperatorList
//* .File Name       : $Workfile:   SVLinearImageOperatorList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:54:06  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVStdImageOperatorListClass.h"
#include "InspectionEngine/SVDataBuffer.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include <array>
#pragma endregion Includes

namespace SvOp
{
	class SVDoubleResult;

class SVLinearImageOperatorList : public SVStdImageOperatorListClass 
{
	SV_DECLARE_CLASS

private:
	static constexpr int m_defaultDoubleResultLowFail = 0;
	static constexpr int m_defaultDoubleResultLowWarn = 0; 
	static constexpr int m_defaultDoubleResultHighFail = 9999;
	static constexpr int m_defaultDoubleResultHighWarn = 9999;

public:
	SVLinearImageOperatorList( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEARIMAGEOPERATORLIST );
	virtual ~SVLinearImageOperatorList();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	virtual bool Run(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;
		
protected:
	HRESULT getUseRotationAngle(BOOL& rUseRotationAngle);
	HRESULT getInputProfileOrientation(long& rProfileOrientation);

	SvOl::InputObject m_inputUseRotationAngle;
	SvOl::InputObject m_inputProfileOrientation;

	SvVol::SVDoubleValueObjectClass m_svLinearData;

	SvVol::SVDoubleValueObjectClass m_MinProfile;
	SvVol::SVDoubleValueObjectClass m_MaxProfile;
	SvVol::SVDoubleValueObjectClass m_ProfileDelta;

	enum ResultIndex
	{
		MinProfileIdx = 0,
		MaxProfileIdx = 1,
		ProfileDeltaIdx = 2
	};

	static constexpr int NumberOfResults = 3;

	std::array< SVDoubleResult*, NumberOfResults> m_Results{ nullptr, nullptr, nullptr };

	SvIe::SVDataBufferClass m_svProfileResultData;

	unsigned long m_ulLineLength;
	std::vector<long> m_aulLineData;

	std::vector<double> m_svArray;

private:
	void init();

	HRESULT UpdateLineExtentData();

	/// If Rotation used, remove ROIImage, if not add ROIImage to the embeddedList.
	void ResetLogicalROIImage();

	bool RunLocalRotation(RunStatus &rRunStatus, SvOi::SVImageBufferHandlePtr input, SvOi::ITRCImagePtr pOutputBuffer, const SVImageExtentClass& rImageExtent);

	/// Allocate SVResult objects for the value objects m_MinProfile, m_MaxProfile, m_ProfileDelta and save the pointers into the 
	/// local array m_Results.
	/// \return returns an error code if the operation was not successful, 0 otherwise.
	DWORD allocateResults();
	/// Helper function to initialize a result object. 
	/// \param pResult [in] pointer to the reult object to be initialized.
	/// \return true if initialization was successful, false otherwise.
	bool initializeResult(SvOp::SVDoubleResult* pResult);
	/// Set default range values for a specified result object.
	/// \param pResult [in] pointer to the reult object to set the range values for.
	/// \return returns an error code if the operation was not successful, 0 otherwise.
	bool setDefaultRange(SvOp::SVDoubleResult* pResult);
	/// Helper function to get the result for the specified double value object. 
	/// \param pSrcObject [in] pointer to an existing double value object.
	/// \return a pointer to the corresponding Result object, if this was already allocated; nullptr if no result was allocated for th specified object.
	SvOp::SVDoubleResult* getResultObject(SvVol::SVDoubleValueObjectClass* pSrcObject);
	/// Helper function to create (i.e. contruct) a result object based on the specified double value object. 
	/// The returned object is NOT a fully initialized result!
	/// \param dValueObject [in] a double value object.
	/// \return a pointer to a constructed SVDoubleResult object. 
	SvOp::SVDoubleResult* constructDoubleResultFromObject(const SvVol::SVDoubleValueObjectClass& dValueObject);
	/// Helper function to map result to the input value, based of the index the internal array m_Results. 
	/// \param i [in] index in the internal array.
	/// \return a pointer to the corresponding value object. 
	SvVol::SVDoubleValueObjectClass* getObjectByResultIndex(int i);
	/// Helper function to check whether all 3 results have been allocated. If the reult has been allocated, it is assigned to the local variable. 
	/// \return true if all result have been allocated, false otherwise. 
	bool areResultsAllocated();
};

} //namespace SvOp
