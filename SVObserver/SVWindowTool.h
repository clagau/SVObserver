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

#pragma region Includes
#include "SVTool.h"
#pragma endregion Includes

class SVWindowToolClass : public SVToolClass, public AllowResizeToParent
{
	SV_DECLARE_CLASS( SVWindowToolClass );

public:
	SVWindowToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVWINDOWTOOL );
	virtual ~SVWindowToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual HRESULT ResetObject() override;
	virtual BOOL IsValid() override;

	virtual BOOL SetDefaultFormulas() override;

	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent ) override;
	virtual HRESULT SetImageExtentToParent( unsigned long p_ulIndex ) override;
	virtual HRESULT SetImageExtentToFit( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent ) override;

	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint ) override;
	virtual bool DoesObjectHaveExtents() const override;

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus ) override;

private:
	void init();
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


