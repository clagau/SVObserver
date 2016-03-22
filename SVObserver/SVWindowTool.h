//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVWindowTool
//* .File Name       : $Workfile:   SVWindowTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   09 Dec 2014 09:47:26  $
//******************************************************************************

#pragma once

#include "SVTool.h"

class SVWindowToolClass : public SVToolClass, public AllowResizeToParent
{
	SV_DECLARE_CLASS( SVWindowToolClass );

public:
	SVWindowToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVWINDOWTOOL );
	virtual ~SVWindowToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual HRESULT ResetObject();
	virtual BOOL IsValid() override;

	virtual BOOL SetDefaultFormulas();

	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );
	virtual HRESULT SetImageExtentToParent( unsigned long p_ulIndex );
	virtual HRESULT SetImageExtentToFit( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );

	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint );
	virtual bool DoesObjectHaveExtents() const override;

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

//	base class should be called from derived class.
	virtual void init();

private:
	void BuildEmbeddedObjectList ();
	void BuildAvailableAnalyzerList ();
	void AddUnaryImageOperatorListAsChild ();

public:
	SVStaticStringValueObjectClass* GetInputImageNames( ) override;

#pragma region Member Variables
private:
	SVStaticStringValueObjectClass m_svSourceImageNames;
#pragma endregion Member Variables
};


