//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolLoadImage
//* .File Name       : $Workfile:   SVToolLoadImage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 08:29:18  $
//******************************************************************************

#pragma once

#include "SVTool.h"
#include "SVOCore/SVImageClass.h"

class SVLoadImageToolClass : public SVToolClass, public AllowResizeToParent
{
	SV_DECLARE_CLASS( SVLoadImageToolClass );

public:
	SVLoadImageToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLOADIMAGETOOL );

	virtual ~SVLoadImageToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage ) override;
	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint ) override;
	virtual bool DoesObjectHaveExtents() const override;
	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent ) override;

	virtual HRESULT SetImageExtentToParent( unsigned long p_ulIndex ) override;
	//************************************
	//! Get the ImageExtents from the Image file 
	//! \param rExtent [out]
	//! \returns HRESULT S_OK if no error occurs
	//************************************
	virtual HRESULT GetParentExtent( SVImageExtentClass& rExtent) const override;

protected:
	void init();

	virtual bool onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

	// Embedded Objects:

	SVImageClass                    m_fileImage;
	SVFileNameValueObjectClass      m_currentPathName;
	SVBoolValueObjectClass          m_continuousReload;
	bool                            m_bResetFileImage;

};

