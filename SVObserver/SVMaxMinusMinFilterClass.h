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

#ifndef SVMAXMINUSMINFILTERCLASS_H
#define SVMAXMINUSMINFILTERCLASS_H

#include "SVFilterClass.h"

class SVMaxMinusMinFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVMaxMinusMinFilterClass );
public:
	SVMaxMinusMinFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVMAXMINUSMINFILTER );
	~SVMaxMinusMinFilterClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual HRESULT ResetObject();

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );

	SVImageInfoClass mProcBufferInfo;
	SVSmartHandlePointer m_ProcBufferHandlePtr;

private:
	void init();
};
		
#endif	//	SVMAXMINUSMINFILTERCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMaxMinusMinFilterClass.h_v  $
 * 
 *    Rev 1.0   12 Aug 2013 16:28:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
