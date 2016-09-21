//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Jim Brown
//*****************************************************************************

/// Classes and functions to ....
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTool.h"  // SVToolClass
#pragma endregion

class ResizeTool :	public SVToolClass,	public AllowResizeToParent
{
	SV_DECLARE_CLASS (ResizeTool);

#pragma region Public Methods
public:
	ResizeTool(BOOL bCreateDefaultTaskList = false, 
				SVObjectClass* pOwner = nullptr, 
				int stringResourceID = IDS_OBJECTNAME_RESIZETOOL);

	virtual ~ResizeTool(void);

	virtual BOOL CloseObject() override;
	virtual BOOL CreateObject(SVObjectLevelCreateStruct* pCreateStructure) override;

	virtual HRESULT SetImageExtentToParent(unsigned long ulIndex) override;
	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent ) override;
	
	/// GetObjectAtPoint
	///  Tests to see if the passed in point (usually from a mouse location)
	///  is contained within itself (the ROI).  If so it returns itself (this),
	///  otherwise returns a nullptr.
	virtual SVTaskObjectClass *GetObjectAtPoint(const SVExtentPointStruct &rsvPoint) override;
	virtual bool DoesObjectHaveExtents() const override;
	
	/// GetInputImageNames
	///  Retrieves source image name from m_svSourceImageName.
	virtual SVStaticStringValueObjectClass* GetInputImageNames() override;

	virtual HRESULT	ResetObject() override;
	virtual BOOL	IsValid() override;

	virtual bool ValidateRemotelySettableParameters () override;
	virtual bool ValidateOfflineParameters () override;
	HRESULT ValidateScaleFactor(const double value);
	HRESULT ValidateInterpolation(const SVInterpolationModeOptions::SVInterpolationModeOptionsEnum interpolationMode);
	HRESULT ValidateOverscan(const SVOverscanOptions::SVOverscanOptionsEnum overscan);
	HRESULT ValidatePerformance(const SVPerformanceOptions::SVPerformanceOptionsEnum performance);


	HRESULT	BackupInspectionParameters ();
	HRESULT	GetBackupInspectionParameters (	double*	oldHeightScaleFactor,
		double*	oldWidthScaleFactor,
		long*	oldInterpolationMode,
		long*	oldOverscan,
		long*	oldPerformance);
		
	SVImageClass* getInputImage();
	SVImageClass* getLogicalROIImage();
	SVImageClass* getOutputImage(); 

	SVEnumerateValueObjectClass* getInterpolationMode();
	SVEnumerateValueObjectClass* getOverscan();
	SVEnumerateValueObjectClass* getPerformance();
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	void LocalInitialize();

	void BuildInputObjectList();
	void BuildEmbeddedObjectList();

	virtual HRESULT IsInputImage(SVImageClass *p_psvImage) override;
	
	virtual BOOL onRun(SVRunStatusClass& RRunStatus) override;

	// Interpolation Mode - embedded
	HRESULT InitializeInterpolationModeMember();

	// Overscan - embedded
	HRESULT	InitializeOverscanMember();

	// Performance - embedded
	HRESULT InitializePerformanceMember();

#pragma endregion Protected Methods

#pragma region Protected Members
	// Source Image Name - embedded
	// It is desired for the Input Image name to be exposed as a value 
	// object, but it is not desired for it to be buckettized.  It can not 
	// change while in Run Mode and buckettized data is all reset when going 
	// online.  If the Input Image is changed, Output Image buffers will all 
	// be destroyed and recreated anyway.
	SVStaticStringValueObjectClass m_svSourceImageName;

	// Output Image - embedded
	SVImageClass m_OutputImage;
	SVImageClass m_LogicalROIImage;

	SVEnumerateValueObjectClass	m_ResizeInterpolationMode;
	SVEnumerateValueObjectClass	m_ResizeOverscan;
	SVEnumerateValueObjectClass	m_ResizePerformance;

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
	SVInObjectInfoStruct m_InputImageObjectInfo;

#pragma endregion Protected Members
};

