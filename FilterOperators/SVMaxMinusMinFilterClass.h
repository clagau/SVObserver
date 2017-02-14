//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVMaxMinusMinFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:28:14  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#pragma endregion Includes

class SVMaxMinusMinFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVMaxMinusMinFilterClass );
public:
	SVMaxMinusMinFilterClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVMAXMINUSMINFILTER );
	virtual ~SVMaxMinusMinFilterClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus ) override;

	SVImageInfoClass mProcBufferInfo;
	SVSmartHandlePointer m_ProcBufferHandlePtr;

private:
	void init();
};

