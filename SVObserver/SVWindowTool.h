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
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

class SVWindowToolClass : public SVToolClass, public AllowResizeToParent
{
	SV_DECLARE_CLASS( SVWindowToolClass );

public:
	SVWindowToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVWINDOWTOOL );
	virtual ~SVWindowToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual bool SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent ) override;
	virtual HRESULT SetImageExtentToParent( unsigned long p_ulIndex ) override;
	virtual HRESULT SetImageExtentToFit( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent ) override;

	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint ) override;
	virtual bool DoesObjectHaveExtents() const override;

protected:

private:
	void init();
	void BuildEmbeddedObjectList ();
	void BuildAvailableAnalyzerList ();
	void AddUnaryImageOperatorListAsChild ();

public:
	SVStringValueObjectClass* GetInputImageNames( ) override;

#pragma region Member Variables
private:
	SVStringValueObjectClass m_SourceImageNames;
#pragma endregion Member Variables
};


