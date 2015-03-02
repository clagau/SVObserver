//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUnaryImageOperatorList
//* .File Name       : $Workfile:   SVUnaryImageOperatorList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 15:06:24  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#ifndef SVUNARYIMAGEOPERATORLIST_H
#define SVUNARYIMAGEOPERATORLIST_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////////////////////
// Includes which are necessary to declare or define types, constants and macros
////////////////////////////////////////////////////////////////////////////////

#include "SVTaskObjectList.h"
#include "SVImageClass.h"

#include "SVValueObjectImpl.h"

#include "SVDataBuffer.h"

class SVToolClass;
class SVIPDoc;

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************



////////////////////////////////////////////////////////////////////////////////
// .Title       : SVUnaryImageOperatorListClass 
// -----------------------------------------------------------------------------
// .Description : New base class for 
//				: #1 SVStdImageOperatorListClass		and
//              : #2 SVInPlaceImageOperatorListClass. 
//              :
//				; The difference between these new classes is that #1 has his
//				: own image output and is processing from input to output image.
//				:
//				: #2 is doing an in place processing on the input image, that
//				: means we don't need another output image. This is useful,
//				: if we want to just apply image operators to an existing
//				: image object ( to his owner TaskObject ).
//				:
//				: The #1 replaces the old SVUnaryImageOperatorListClass and 
//				: for compatibility to older versions it inherits the classID
//				: of the old SVUnaryImageOperatorListClass.
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVUnaryImageOperatorListClass : public SVTaskObjectListClass 
{
public:

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

	friend class SVUnaryImageOperatorClass;


//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVUnaryImageOperatorListClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVUNARYIMAGEOPERATORLIST );

private:
	void init();

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVUnaryImageOperatorListClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );

	SVImageClass*				getInputImage();


//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************
public:
	virtual BOOL OnValidate();

protected:
	SVInObjectInfoStruct		inputImageObjectInfo;

};











////////////////////////////////////////////////////////////////////////////////
// .Title       : SVStdImageOperatorListClass  
// -----------------------------------------------------------------------------
// .Description : Provides his own output image.
//				:
//				: First operator in the list is processing
//				: Input Image  ---> Output Image
//				: 
//				: All other succeeding operators are doing in place processing 
//				: on output image
//				: Output Image ---> Output Image
//				:
//				: Replaces the old SVUnaryImageOperatorListClass and 
//				: for compatibility to older versions it inherits the classID
//				: of the old SVUnaryImageOperatorListClass.
// -----------------------------------------------------------------------------
// .Special : Threshold Operator, if he things he is the first one, but 
//			: thresholding is deactivated, he is doing a copy (input to output)
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVStdImageOperatorListClass : public SVUnaryImageOperatorListClass 
{
	SV_DECLARE_CLASS( SVStdImageOperatorListClass )
//******************************************************************************
// Friend Declaration(s):
//******************************************************************************
	friend class SVUnaryImageOperatorClass;

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVStdImageOperatorListClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVSTDIMAGEOPERATORLIST );
	virtual ~SVStdImageOperatorListClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual BOOL Run( SVRunStatusClass& RRunStatus );

	virtual BOOL OnValidate();

protected:
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	SVImageClass *getOutputImage();

	SVImageClass outputImageObject;	// Embedded

private:
	void init();

public:
	HRESULT CollectInputImageNames( SVRunStatusClass& RRunStatus );

};












////////////////////////////////////////////////////////////////////////////////
// .Title       : SVInPlaceImageOperatorListClass 
// -----------------------------------------------------------------------------
// .Description : Makes in place processing on input image. Has NO Output Image!
//				:
//				: All operators are doing in place processing on input image:
//				: Input Image  ---> Input Image
//				:
//				: Input Image must be a physical image type !!!
//				: Validation fails, if input image is not physical.
//				:
//				: Is doing an in place processing on the input image, that
//				: means we don't need another output image. This is useful,
//				: if we want to just apply image operators to an existing
//				: image object ( to his owner TaskObject ).
// -----------------------------------------------------------------------------
// .Special : Threshold Operator, if he things he is the first one, but 
//			: thresholding is deactivated, he is doing a copy (input to input)
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVInPlaceImageOperatorListClass : public SVUnaryImageOperatorListClass 
{
	SV_DECLARE_CLASS( SVInPlaceImageOperatorListClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVInPlaceImageOperatorListClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVINPLACEIMAGEOPERATORLIST );
	virtual ~SVInPlaceImageOperatorListClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );

//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************
public:
	virtual BOOL OnValidate();
	virtual BOOL Run( SVRunStatusClass& RRunStatus );

//******************************************************************************
// Data Element(s):
//******************************************************************************
private:
	void init();
};

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	//	SVUNARYIMAGEOPERATORLIST_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVUnaryImageOperatorList.h_v  $
 * 
 *    Rev 1.2   15 May 2014 15:06:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:26:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:53:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   11 Mar 2008 08:57:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added CollectInputImageNames to Run
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   09 Aug 2005 08:51:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Aug 2005 12:01:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   replaced SVVectorObject with SVValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   19 May 2005 15:01:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   cleaned up file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Feb 2005 14:18:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Made modifications to support new Extents and new Reset Methodology. 
 * Added ResetObject(
 * Added OnValidate(
 * Added GetOutputImage(
 * Removed Unused Code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Jan 2004 10:22:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Update header information to make getInputImage method public.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 17:18:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Nov 2002 14:51:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed GetSystem & GetSystemInfo
 * Removed include SVSystem.h
 * Added SVObserver.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Mar 2000 14:55:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * Provides SVUnaryImageOperatorList base class and
 * SVStdUnaryImageOperatorList (replaces old SVUnaryImageOperatorList)
 * SVInPlaceImageOperatorList (has no image ouput,
 * performs inplace processing on input image(.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/