//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutObjectInfoStruct
//* .File Name       : $Workfile:   SVOutObjectInfoStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 11:29:20  $
//******************************************************************************

#ifndef SVOUTOBJECTINFOSTRUCT_H
#define SVOUTOBJECTINFOSTRUCT_H

#pragma warning( disable : 4786 )

#include <set>
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVInObjectInfoStruct.h"

class SVObjectClass;

typedef std::pair<SVObjectClass*, SVObjectClass*> SVObjectPair;
typedef std::vector<SVObjectPair> SVObjectPairVector;

struct SVOutObjectInfoStruct : public SVObjectInfoStruct
{
	SVOutObjectInfoStruct();
	SVOutObjectInfoStruct( const SVOutObjectInfoStruct& p_rsvValue );

	virtual ~SVOutObjectInfoStruct();

	const SVOutObjectInfoStruct& operator=( const SVOutObjectInfoStruct& p_rsvValue );

	HRESULT AddInput( SVInObjectInfoStruct& p_rsvInput );
	HRESULT RemoveInput( SVInObjectInfoStruct& p_rsvInput );

	HRESULT DisconnectAllInputs();
	HRESULT GetDependentsList( SVObjectClass* p_psvObject, SVObjectPairVector& rListOfDependents );

	INT_PTR GetInputSize() { return UserInfoList.GetSize(); }
	SVInObjectInfoStruct& GetInputAt( long p_lIndex ) { return UserInfoList[ p_lIndex ]; }

	BOOL Lock( DWORD p_TimeOutMilliseconds = INFINITE ) const;
	BOOL Unlock() const;

private:
	// Who are my users...
	//	If you have to send messages to the user objects ( especially in SETUP MODE ),
	//  you should use their object GUID! In RUN MODE you should normally not send 
	//  messages, but you can use the validated object pointer!
	SVVectorSVInObjectInfoStruct UserInfoList;

	mutable SVCriticalSectionPtr m_CriticalSectionPtr;

};

typedef std::set<SVOutObjectInfoStruct*> SVStdSetSVOutObjectInfoStructPtr;
typedef SVVector< SVOutObjectInfoStruct* > SVOutObjectInfoStructPtrArray;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\SVOutObjectInfoStruct.h_v  $
 * 
 *    Rev 1.1   01 Oct 2013 11:29:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 17:20:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   10 Jul 2012 11:54:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added operator= to class/structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Jun 2012 17:13:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   25 Feb 2011 11:59:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Nov 2010 14:26:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Jul 2009 08:35:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Sep 2005 07:35:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects to handle invalid items on the input and output lists.  Methods now verify object validity before use.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Mar 2005 11:54:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  262
 * SCR Title:  Improve performance when loading a configuration in SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added pragma to remove warning message
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:59:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First version of file for SVObjectLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   12 Jul 2004 13:04:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented SV_HIDDEN
 * fixed SV_GET_KIND_OF
 * added copy constructor / operator = to SVInObjectInfoStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   12 Jul 2004 13:01:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  428
 * SCR Title:  Improve Load Configuration time
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented GetCompleteObjectName optimization
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   04 Dec 2003 12:59:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  397
 * SCR Title:  Blob Analyzer - Allow Max number of blobs to be set.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed SVInObjectInfoStruct to have a const copy constructor
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   22 Apr 2003 11:34:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   30 Jan 2003 15:08:22   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added SetImageDepth and GetImageDepth methods to class.  Updated constructor to initialize mlImageDepth attribute.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   22 Jan 2003 13:42:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bugs with SetInputs and SetToolParameterList from the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   10 Jan 2003 17:49:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   forgot the = sign for the const UINT message defines
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   10 Jan 2003 16:54:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   forgot to add ; to constants that are now const UINT instead of #define
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   10 Jan 2003 15:45:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new function VerifyImageForOverlay to determine which object has overlays on which image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   03 Jan 2003 15:28:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new attributes.  SV_SETABLE_ONLINE, SV_SETABLE_REMOTELY, SV_EXTENT_OBJECT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   17 Dec 2002 15:20:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   BOOL GetChildObjectByName( LPCTSTR tszName, SVObjectClass** ppObject );
 * 
 * Changed #define to const <typename> declarations. (So the values can be seen in the debugger)
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   09 Dec 2002 19:44:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added SV_PRINTABLE flag to attribute flages.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   20 Nov 2002 09:11:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226, 272
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed TheClassRegister -> TheSVClassRegister
 * Added constants
 * SVInObjectInfoStruct: Added strName member var
 * SVIPDocLevelCreateStruct: removed references to SVObjectInfoStruct
 * Removed references to  SVPQDataLinkInfo
 * SVObjectClass: Added friend classes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   04 Apr 2002 10:09:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  286
 * SCR Title:  Upgrade SVObserver to Version 3.37 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added debug information variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   02 Oct 2001 15:05:50   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  241
 * SCR Title:  Color SVIM:  Load image tool output not available in window tool as input.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added message definitions for 
 * SVMSGID_GET_IMAGE_BAND0_OBJECT & 
 * SVM_GET_IMAGE_BAND0_OBJECT.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   19 Apr 2001 14:48:08   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Changes due to addition of the SVFileNameObjectClass and new GUID for the class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   06 Sep 2000 16:31:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   14 Jun 2000 16:20:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  159
 * SCR Title:  Incorrect placement of Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added new message definition for SVM_IMAGE_SOURCE_CHANGED.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   19 May 2000 16:18:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  148
 * SCR Title:  Formulas not updated when Tools are renamed.
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised the SVM_OBJECT_RENAMED message so that it
 * will also be routed to friend objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   11 May 2000 08:56:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  139
 * SCR Title:  Math Tool Invalid State
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SVM_CREATE_ALL_CLOSED_OBJECTS to
 * also route this message to friend objects.
 * 
 * Added IsDescendantOfType method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   26 Apr 2000 16:13:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetImageInputContextGuid method.
 * Added new message 
 * SVM_SET_SHADOWED_ABSEXTENT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   23 Mar 2000 14:03:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  117
 * SCR Title:  Bug Fixes for Integration of conditional Toolset/Tool Drawing.
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Introduced new SetDisabled() member function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   03 Mar 2000 16:00:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fixed SVM_RESIZING Message definition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   03 Mar 2000 12:24:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SVM_GET_ABSOLUTE_EXTENT, 
 * SVM_GET_EXTENT Message Comments .
 * Added new Message SVM_SET_SHADOWED_EXTENT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   Mar 01 2000 12:41:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to make checkDrawAllowed(...) public.
 * Added new messages:
 * SVM_MOVE, SVM_LT_SIZE, SVM_RT_SIZE, SVM_LB_SIZE,
 * SVM_RB_SIZE, SVM_L_SIZE, SVM_R_SIZE, SVM_T_SIZE, SVM_B_SIZE
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   25 Feb 2000 16:07:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added new method GetTransform.
 * Added new message SVM_GET_CONTEXT_EXTENT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   21 Feb 2000 15:37:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added comments for Move/Size messages to detail
 * parameters.
 * Added checkDrawAllowed method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   Feb 21 2000 10:45:06   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  109
 * SCR Title:  Freeze on Reject for v3.1
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Versioning to v3.10 Beta 2.
 * Add new Macro for Type Safe Casting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   16 Feb 2000 17:00:38   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Modified Draw functions to use the DrawContext structure
 * and removed unused methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   07 Feb 2000 20:47:56   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Added new Object function GetObjectIcon() which can be overwritten and be used as generic 'get icon func' for any kind of list or tree icon.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:18:40   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   Oct 29 1999 17:59:04   steve
 * 
 *    Rev 1.35   18 Oct 1999 17:36:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Corrected description of SVMSGID_OBJECT_RENAMED
 * parameter usage
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   Oct 18 1999 11:53:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   RO - Bug fix - Added DataLinkID scripting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   14 Oct 1999 17:00:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added HasDependents method to SVOutputInfoListClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   Oct 13 1999 15:14:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Roberts bug fixes for Beta 10.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   Oct 12 1999 08:16:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  44
 * SCR Title:  Version 3.00 Beta 9 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required to change v3.00 Beta 8 to v3.00 Beta 9.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   Oct 07 1999 15:37:50   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Go Online and Go Offline messages.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   06 Oct 1999 15:42:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Corrected problem with SV_ALL_ATRIBUTES
 * ( added parentheses)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   05 Oct 1999 17:20:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Attribute flag for Statistics
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   Oct 04 1999 09:52:02   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fix for lock while going into run mode and Point scripting fix.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   Sep 27 1999 19:37:28   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed RESET_ALL_OBJECTS to support friends.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   Sep 24 1999 09:49:04   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  4
 * SCR Title:  Add "Load image" option
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added Reset_All_Message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   24 Sep 1999 08:50:56   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed space3s from SV_VIEWABLE|SV_PUBLISHABLE|SV_ARCHIVABLE|SV_SELECTABLE_FOR_EQUATION|SV_EMBEDABLE of #define SV_ALL_ATTRIBUTES.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   23 Sep 1999 16:25:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added message SVM_DESTROY_FRIEND_OBJECT.
 * Revised DestroyFriends to send 
 * SVM_DESTROY_FRIEND_OBJECT instead of
 * SVM_DESTROY_CHILD_OBJECTS
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   Sep 23 1999 13:53:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  37
 * SCR Title:  Version 3.00 Beta 3 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   New version specific changes to Serialize methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   22 Sep 1999 18:35:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to fix restoration of friends from script
 * (Added method to adjust friend aliases)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   Sep 20 1999 15:36:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced new broadcast message router.
 * Added new messages: Create_All_Closed_Objects.
 *      Notify_About_Creation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   Sep 16 1999 12:38:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added Product Key Struct and other from SVPPQ.h.
 * Added GetAncestor(ObjectType) and IsDescendantOf( PAncestorObject)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   Sep 13 1999 13:29:14   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added message to get input interface list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   Sep 10 1999 17:37:16   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed SetObjectOwner() to virtual.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   04 Sep 1999 17:24:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to save and restore uObjectAttributesAllowed and
 * uObjectAttributesSet via ObjectScript.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   02 Sep 1999 08:44:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Add Message Qualifiers to better control routing of messages.
 * Add new messsages for drawing/figures.
 * Fixed Bug in CloseObject (Friends recurrsion)
 * Add published attribute and defaults on construction of SVObjectClass..
 * Added GetObjectColor and GetObjectState
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   30 Aug 1999 19:31:42   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Introduced Input Interface Handling.
 * ( GETFIRST - Message Handler )
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   30 Aug 1999 19:15:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SV_IS_KIND_OF not to call SV_VALIDATE_OBJECT.
 * Revised SVM_CREATE_ALL_OBJECTS to route to Children only.
 * Added Scripting support for Freinds Architecure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   27 Aug 1999 12:58:46   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Added SetupDialog() Function to SVObjectClass and Message.
 * This gives every derived class the possibility to support his own Setup Dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   27 Aug 1999 09:06:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Add capability to script freind links.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   Aug 25 1999 22:50:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  1
 * SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised Validate method to be OnValidate method.
 *  ( OnValidate - local scope validation).
 * Added Validate method (Routes to all owned objects)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Aug 25 1999 16:42:04   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added SV_ALL_ATTRIBUTES define.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   25 Aug 1999 11:36:12   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Added SVObjectInfoArrayClass.
 * Added Friend Mechanism.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   Aug 25 1999 10:47:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Correct check in file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Aug 20 1999 19:42:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Added attributes for output (results) picker
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Aug 20 1999 16:54:22   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  14
 * SCR Title:  Integrate luminance analyzer into window tool.
 * Checked in by:  James A. Brown
 * Change Description:  
 *   Added SVErrorClass  msvError to SVObjectClass.
 * 
 * Changed header comment block to new standard which includes Tracker keywords.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/