//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVUnaryImageOperatorClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:30:30  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUnaryOperatorClass.h"
#include "SVUnaryImageOperatorList.h"
#pragma endregion Includes

class SVUnaryImageOperatorClass : public SVUnaryOperatorClass
{
	SV_DECLARE_CLASS( SVUnaryImageOperatorClass );
public:
	SVUnaryImageOperatorClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVUNARYIMAGEOPERATOR );
	virtual ~SVUnaryImageOperatorClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual BOOL OnValidate();
	virtual BOOL Run( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
	SVImageClass* getReferenceImage();

	SVUnaryImageOperatorListClass* m_pCurrentUIOPL;
	
private:
	void init();
};

