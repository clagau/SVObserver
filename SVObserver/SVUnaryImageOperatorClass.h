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

#ifndef SVUNARYIMAGEOPERATORCLASS_H
#define SVUNARYIMAGEOPERATORCLASS_H

#include "SVUnaryOperatorClass.h"

class SVUnaryImageOperatorClass : public SVUnaryOperatorClass
{
	SV_DECLARE_CLASS( SVUnaryImageOperatorClass );
public:
	SVUnaryImageOperatorClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVUNARYIMAGEOPERATOR );
	
public:

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual BOOL CloseObject();

	virtual BOOL OnValidate();
	virtual BOOL Run( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );

	SVImageClass*                   getReferenceImage();

	SVUnaryImageOperatorListClass*  m_pCurrentUIOPL;
	
private:
	void init();

};

#endif	//	SVUNARYIMAGEOPERATORCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVUnaryImageOperatorClass.h_v  $
 * 
 *    Rev 1.0   12 Aug 2013 16:30:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
