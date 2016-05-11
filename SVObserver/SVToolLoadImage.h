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
#include "SVImageClass.h"

class SVLoadImageToolClass : public SVToolClass, public AllowResizeToParent
{
	SV_DECLARE_CLASS( SVLoadImageToolClass );

public:
	SVLoadImageToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLOADIMAGETOOL );

	virtual ~SVLoadImageToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );
	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint );
	virtual bool DoesObjectHaveExtents() const override;
	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );

	virtual BOOL OnValidate();

	virtual HRESULT SetImageExtentToParent( unsigned long p_ulIndex );
	//************************************
	//! Get the ImageExtents from the Image file 
	//! \param rExtent [out]
	//! \returns HRESULT S_OK if no error occurs
	//************************************
	HRESULT GetParentExtent( SVImageExtentClass& rExtent) const;
	
	virtual BOOL IsValid() override;

protected:
	void init();

	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	// Embedded Objects:

	SVImageClass                    m_fileImage;
	SVFileNameValueObjectClass      m_currentPathName;
	SVBoolValueObjectClass          m_continuousReload;
	bool                            m_bResetFileImage;

};

