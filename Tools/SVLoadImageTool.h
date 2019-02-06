//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolLoadImage
//* .File Name       : $Workfile:   SVLoadImageTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 08:29:18  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTool.h"
#include "InspectionEngine/SVImageClass.h"
#pragma endregion Includes

namespace SvTo
{

class SVLoadImageToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVLoadImageToolClass );

public:
	SVLoadImageToolClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLOADIMAGETOOL );

	virtual ~SVLoadImageToolClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual SvIe::SVTaskObjectClass* GetObjectAtPoint(const SVPoint<double>& rPoint) override;
	virtual bool DoesObjectHaveExtents() const override;
	virtual HRESULT SetImageExtent( const SVImageExtentClass& rImageExtent ) override;

	virtual HRESULT SetImageExtentToParent() override;
	//************************************
	//! Get the ImageExtents from the Image file 
	//! \param rExtent [out]
	//! \returns HRESULT S_OK if no error occurs
	//************************************
	virtual HRESULT GetParentExtent( SVImageExtentClass& rExtent) const override;

protected:
	void init();

	virtual bool isInputImage(const SVGUID& rImageGuid) const override;
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

	// Embedded Objects:

	SvIe::SVImageClass m_fileImage;
	SvVol::SVFileNameValueObjectClass m_currentPathName;
	SvVol::SVBoolValueObjectClass m_continuousReload;
	bool m_ReloadFileImage;

};

} //namespace SvTo
