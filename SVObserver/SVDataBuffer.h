//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataBuffer
//* .File Name       : $Workfile:   SVDataBuffer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:07:26  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#ifndef SVDATABUFFER_H
#define SVDATABUFFER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#include "SVObjectAppClass.h"
#include "SVGlobalHandles.h"

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

class SVToolClass;
class SVIPDoc;

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVDataBufferInfoClass 
// -----------------------------------------------------------------------------
// .Description : 
// -----------------------------------------------------------------------------
// .Export
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :09.09.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVDataBufferInfoClass
{
//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVDataBufferInfoClass();
	SVDataBufferInfoClass( SVDataBufferInfoClass& S2 );

//******************************************************************************
// Destructor(s):
//******************************************************************************

//******************************************************************************
// Operator(s):
//******************************************************************************
	SVDataBufferInfoClass operator=( SVDataBufferInfoClass& S2 );


//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	SVToolClass*				POwnerTool;
	long						Length;
	DWORD						Type;
	SVDataBufferHandleStruct	HBuffer;

	// Data Buffer Types
	enum 
	{
		SVProjectResult		= 0x0040,
		SVEventResult		= 0x0080,
		SVExtremeResult		= 0x0100,
		SVCountResult		= 0x0200,
		SVHistResult		= 0x0400,
		SVContainsFloatData = 0x4000
	};

};



////////////////////////////////////////////////////////////////////////////////
// .Title       : SVDataBufferClass 
// -----------------------------------------------------------------------------
// .Description : This class capsules a Data Buffer like the 
//              : Image Result Buffers in MIL ( refer to MimAllocResult() )
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.yyyy				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
class SVDataBufferClass : public SVObjectAppClass
{
	SV_DECLARE_CLASS( SVDataBufferClass );

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVDataBufferClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVDATABUFFER );

protected:
	void init();

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVDataBufferClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL Resize( int NewLength );

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************
public:
	SVDataBufferInfoClass&		GetDataBufferInfo();
	SVDataBufferHandleStruct&	GetDataBufferHandle();

//******************************************************************************
// Operation(s) Of Representation:
//******************************************************************************

//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************
protected:
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	SVDataBufferInfoClass dataInfo;
};

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	//	SVDATABUFFER_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDataBuffer.h_v  $
 * 
 *    Rev 1.2   15 May 2014 11:07:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 10:32:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:05:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   17 Feb 2005 13:37:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   16 Feb 2005 13:30:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   17 Apr 2003 17:30:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   19 Nov 2002 11:44:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed references to System
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   Apr 17 2001 15:33:56   steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Header file chages due to restructuring of include files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   07 Feb 2000 19:50:46   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Revised init() to be not virtual.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:14:46   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 10 1999 17:34:14   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Replacement for old image result buffer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/