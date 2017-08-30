//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Jim Brown
//*****************************************************************************

/// Classes and functions to ....
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOCore/SVTool.h"  // SVToolClass
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion

class ResizeTool :	public SVToolClass
{
	SV_DECLARE_CLASS (ResizeTool);

#pragma region Public Methods
public:
	ResizeTool(SVObjectClass* pOwner = nullptr, int stringResourceID = IDS_OBJECTNAME_RESIZETOOL);

	virtual ~ResizeTool(void);

	virtual bool CloseObject() override;
	virtual bool CreateObject(SVObjectLevelCreateStruct* pCreateStructure) override;

	virtual HRESULT SetImageExtentToParent() override;
	virtual HRESULT SetImageExtent( const SVImageExtentClass& rImageExtent ) override;
	
	/// GetObjectAtPoint
	///  Tests to see if the passed in point (usually from a mouse location)
	///  is contained within itself (the ROI).  If so it returns itself (this),
	///  otherwise returns a nullptr.
	virtual SVTaskObjectClass *GetObjectAtPoint(const SVExtentPointStruct &rsvPoint) override;
	virtual bool DoesObjectHaveExtents() const override;
	
	/// GetInputImageNames
	///  Retrieves source image name from m_SourceImageNames.
	virtual SVStringValueObjectClass* GetInputImageNames() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	bool ValidateParameters (SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool ValidateOfflineParameters (SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool ValidateScaleFactor(const double value, SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool ValidateInterpolation(const SVInterpolationModeOptions::SVInterpolationModeOptionsEnum interpolationMode, SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool ValidateOverscan(const SVOverscanOptions::SVOverscanOptionsEnum overscan, SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool ValidatePerformance(const SVPerformanceOptions::SVPerformanceOptionsEnum performance, SvStl::MessageContainerVector *pErrorMessages=nullptr);


	HRESULT	BackupInspectionParameters ();
	HRESULT	GetBackupInspectionParameters (	double*	oldHeightScaleFactor,
		double*	oldWidthScaleFactor,
		long*	oldInterpolationMode,
		long*	oldOverscan,
		long*	oldPerformance);
		
	SVImageClass* getInputImage();
	SVImageClass* getLogicalROIImage();
	SVImageClass* getOutputImage(); 

	SVEnumerateValueObjectClass& getInterpolationMode() { return m_ResizeInterpolationMode; };
	SVEnumerateValueObjectClass& getOverscan() { return m_ResizeOverscan; };
	SVEnumerateValueObjectClass& getPerformance() { return m_ResizePerformance; };
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	void LocalInitialize();

	void BuildInputObjectList();
	void BuildEmbeddedObjectList();

	virtual HRESULT IsInputImage(SVImageClass *p_psvImage) override;
	
	virtual bool onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

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
	SVStringValueObjectClass m_SourceImageNames;

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

