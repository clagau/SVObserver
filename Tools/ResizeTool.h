//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Jim Brown
//*****************************************************************************

/// Classes and functions to ....
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTool.h"
#include "SVMatroxLibrary/SVMatroxEnums.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion

namespace SvTo
{

class ResizeTool :	public SVToolClass
{
	SV_DECLARE_CLASS (ResizeTool);

#pragma region Public Methods
public:
	ResizeTool(SVObjectClass* pOwner = nullptr, int stringResourceID = IDS_OBJECTNAME_RESIZETOOL);

	virtual ~ResizeTool(void);

	virtual bool CloseObject() override;
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;

	virtual HRESULT SetImageExtentToParent() override;
	virtual HRESULT SetImageExtent( const SVImageExtentClass& rImageExtent ) override;
	
	/// GetObjectAtPoint
	///  Tests to see if the passed in point (usually from a mouse location)
	///  is contained within itself (the ROI).  If so it returns itself (this),
	///  otherwise returns a nullptr.
	virtual SVToolClass* GetObjectAtPoint(const SVPoint<double>& rPoint) override;
	virtual bool DoesObjectHaveExtents() const override;
	
	/// GetInputImageNames
	///  Retrieves source image name from m_SourceImageNames.
	virtual SvVol::SVStringValueObjectClass* GetInputImageNames() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	bool ValidateParameters (SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool ValidateOfflineParameters (SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool ValidateScaleFactor(const double value, SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool ValidateInterpolation(const SVInterpolationModeOptions::SVInterpolationModeOptionsEnum interpolationMode, SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool ValidateOverscan(const SVOverscanOptions::SVOverscanOptionsEnum overscan, SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool ValidatePerformance(const SVPerformanceOptions::SVPerformanceOptionsEnum performance, SvStl::MessageContainerVector *pErrorMessages=nullptr);


	void BackupInspectionParameters();
	HRESULT	GetBackupInspectionParameters (	double*	oldHeightScaleFactor,
		double*	oldWidthScaleFactor,
		long*	oldInterpolationMode,
		long*	oldOverscan,
		long*	oldPerformance);
		
	SvIe::SVImageClass* getInputImage(bool bRunMode = false) const;

	SvVol::SVEnumerateValueObjectClass& getInterpolationMode() { return m_ResizeInterpolationMode; };
	SvVol::SVEnumerateValueObjectClass& getOverscan() { return m_ResizeOverscan; };
	SvVol::SVEnumerateValueObjectClass& getPerformance() { return m_ResizePerformance; };
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	void LocalInitialize();

	void BuildInputObjectList();
	void BuildEmbeddedObjectList();

	virtual bool isInputImage(uint32_t imageId) const override;

	virtual bool onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	// Interpolation Mode - embedded
	HRESULT InitializeInterpolationModeMember();

	// Overscan - embedded
	HRESULT	InitializeOverscanMember();

	// Performance - embedded
	HRESULT InitializePerformanceMember();

#pragma endregion Protected Methods

#pragma region Protected Members
	SvVol::SVStringValueObjectClass m_SourceImageNames;

	// Output Image - embedded
	SvIe::SVImageClass m_OutputImage;
	SvIe::SVImageClass m_LogicalROIImage;

	SvVol::SVEnumerateValueObjectClass	m_ResizeInterpolationMode;
	SvVol::SVEnumerateValueObjectClass	m_ResizeOverscan;
	SvVol::SVEnumerateValueObjectClass	m_ResizePerformance;

	long		m_ResizeInterpolationMode_Backup;
	long		m_ResizeOverscan_Backup;
	long		m_ResizePerformance_Backup;
	double		m_ResizeWidthSF_Backup;
	double		m_ResizeHeightSF_Backup;

	const static long MinScaleFactorThreshold; // Scale Factor may not 
											   // be less than or equal 
											   // to 0.
	const static long MaxScaleFactor;		   // Maximum allowed Scale Factor. 

	// Source Image - input
	SvOl::SVInObjectInfoStruct m_InputImageObjectInfo;

#pragma endregion Protected Members
};

} //namespace SvTo
