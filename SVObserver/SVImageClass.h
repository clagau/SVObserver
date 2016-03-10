//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageClass
//* .File Name       : $Workfile:   SVImageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   26 Jun 2014 17:41:18  $
//******************************************************************************

#pragma once
#pragma region Includes
#include <set>
#include "ObjectInterfaces/ISVImage.h"
#include "SVContainerLibrary/SVVector.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVMatroxLibrary/SVMatroxEnums.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVRunControlLibrary/SVImageIndexStruct.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVImageChildStruct.h"
#include "SVImageObjectClass.h"
#include "SVObjectAppClass.h"
#pragma endregion Includes

class SVExtentClass;
class SVIPDoc;
class SVImageOverlayClass;
class SVToolClass;
class SVVirtualCamera;

struct SVProductInfoStruct;

typedef SVVector< long > SVImageChildIndexCArray;

class SVImageClass : virtual public SvOi::ISVImage, public SVObjectAppClass
{
	SV_DECLARE_CLASS( SVImageClass );

public:
	typedef std::pair< SVGUID, SVImageExtentClass > SVChildExtentPair;
	typedef std::deque< SVChildExtentPair > SVChildExtentDeque;

	SVImageClass( LPCSTR ObjectName );
	SVImageClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVIMAGE );
	virtual ~SVImageClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStruct );
	virtual BOOL CloseObject();

	virtual SVImageClass* GetParentImage() const;
	virtual const GUID& GetParentImageID() const;
	virtual const SVImageInfoClass& GetImageInfo() const; //@TODO: Change the logic so that this is never needed outside this class

	virtual HRESULT InitializeImage( SVImageTypeEnum p_ImageType );
	virtual HRESULT InitializeImage( SVImageClass* p_pParentImage );
	virtual HRESULT InitializeImage( const GUID& p_rParentID );
	virtual HRESULT InitializeImage( const GUID& p_rParentID, const SVImageInfoClass& p_rImageInfo );

	//@WARNING [Jim][8 July 2015] - Look into and evaluate this behavior.
	//-  This UpdateImage function calls RebuildStorage() which calls 
	//-  UpdateFromParentInformation() which overwrites the extents I just 
	//-  sent in anyhow????
	virtual HRESULT UpdateImage( const SVImageExtentClass& p_rExtent, bool p_ExcludePositionCheck = false );
	virtual HRESULT UpdateImage( const SVImageInfoClass& p_rImageInfo );
	virtual HRESULT UpdateImage( const GUID& p_rParentID );
	virtual HRESULT UpdateImage( const GUID& p_rParentID, const SVImageExtentClass& p_rExtent );
	virtual HRESULT UpdateImage( const GUID& p_rParentID, const SVImageInfoClass& p_rImageInfo );
	virtual HRESULT UpdateImage( SVImageTypeEnum p_ImageType );
	virtual HRESULT UpdateImage( SVImageTypeEnum p_ImageType, const SVImageInfoClass& p_rImageInfo );
	virtual HRESULT UpdateImage( SVImageTypeEnum p_ImageType, const GUID& p_rParentID );
	virtual HRESULT UpdateImage( SVImageTypeEnum p_ImageType, const GUID& p_rParentID, const SVImageInfoClass& p_rImageInfo );

	virtual BOOL SetImageDepth( long lDepth );

	virtual const SVClock::SVTimeStamp& GetLastResetTimeStamp() const;
	
	virtual HRESULT ResetObject();

	virtual SVImageExtentClass GetImageExtents();
	virtual HRESULT GetImageExtentsToFit( SVImageExtentClass p_svInExtent, SVImageExtentClass &p_rsvOutExtent );

	virtual HRESULT ValidateAgainstAllExtents( SVImageExtentClass& p_rsvExtent );
	virtual HRESULT ValidateAgainstParentExtents( SVImageExtentClass& p_rsvExtent );
	virtual HRESULT ValidateAgainstOutputExtents( const SVImageExtentClass& p_rsvExtent );
	virtual HRESULT ValidateAgainstChildrenExtents( SVImageExtentClass& p_rsvExtent );

	virtual HRESULT GetParentImageHandle( SVSmartHandlePointer& p_rsvBufferHandle ); //@TODO:  Change method to const?
	virtual HRESULT GetParentImageHandle( SVImageIndexStruct p_svBufferIndex, SVSmartHandlePointer& p_rsvBufferHandle ); //@TODO:  Change method to const?

	// Need to deal with source and result image buckets
	virtual BOOL GetImageHandleIndex( SVImageIndexStruct& rsvIndex ) const;
	virtual BOOL SetImageHandleIndex( SVImageIndexStruct  svIndex  );

	virtual BOOL CopyImageTo( SVImageIndexStruct svIndex );

	virtual BOOL GetImageHandle( SVSmartHandlePointer& p_rHandlePtr ); //@TODO:  Change method to const?
		virtual BOOL GetImageHandle( SVImageIndexStruct svIndex, SVSmartHandlePointer& rHandle );

	virtual BOOL SafeImageCopyToHandle     ( SVSmartHandlePointer& p_rHandle );
	virtual BOOL SafeImageCopyToHandle     ( SVImageIndexStruct p_svFromIndex, SVSmartHandlePointer& p_rHandle );
	virtual BOOL SafeImageCopyToBSTR       ( SVImageIndexStruct p_svFromIndex, BSTR &p_rbstrData );
	virtual BOOL SafeImageCopyColorToHandle( SVSmartHandlePointer& p_rHandle, long p_lBandNumber );
	virtual BOOL SafeImageCopyColorToHandle( SVImageIndexStruct p_svFromIndex, SVSmartHandlePointer& p_rHandle, long p_lBandNumber );
	virtual BOOL SafeImageCopyClipToHandle ( SVSmartHandlePointer& p_rHandle, long p_lLeftOffset, long p_lTopOffset );
	virtual BOOL SafeImageCopyClipToHandle ( SVImageIndexStruct p_svFromIndex, SVSmartHandlePointer& p_rHandle, long p_lLeftOffset, long p_lTopOffset );
	virtual BOOL SafeImageConvertToHandle  ( SVSmartHandlePointer& p_rHandle, SVImageOperationTypeEnum p_lConversionType );
	virtual BOOL SafeImageConvertToHandle  ( SVImageIndexStruct p_svFromIndex, SVSmartHandlePointer& p_rHandle, SVImageOperationTypeEnum p_lConversionType );

	virtual HRESULT LoadImageFullSize( LPCTSTR p_szFileName, SVImageExtentClass& p_rNewExtent );
	virtual HRESULT LoadImage( LPCTSTR p_szFileName, SVImageIndexStruct p_svToIndex );

	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );

#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent = 0 ); //@TODO:  Change method to const?
#endif
	virtual void Persist( SVObjectWriter& rWriter );
	virtual void PersistImageAttributes( SVObjectWriter& rWriter );

	virtual BOOL OnValidate(); //@TODO:  Change method to const?

	virtual SVImageIndexStruct GetSourceImageIndex( SVProductInfoStruct* pProduct );

	SVImageClass* GetRootImage(); //@TODO:  Change method to const?
	HRESULT TranslateFromOutputSpaceToImage(SVImageClass* p_pImage, SVExtentPointStruct p_InPt, SVExtentPointStruct& p_OutPt); //@TODO:  Change method to const?

	void GetChildExtents( SVChildExtentDeque& p_rChildExtents ) const;

#pragma region virtual method (ISVImage)
	virtual SVImageTypeEnum GetImageType() const override;
	virtual SvOi::ISVImage* GetParentImageInterface() const override;
	virtual SvOi::MatroxImageSmartHandlePtr getImageData() override;
	virtual SvOi::MatroxImageSmartHandlePtr getParentImageData() override;
	virtual SVString getDisplayedName() const override;
	virtual SvOi::IObjectClass* getOwner() const override; // Gets the owner from the ImageInfo
	virtual long getBands() const override;
	virtual long getPixelDepth() const override;
	virtual HRESULT Save(const SVString& rFilename) override; 
	virtual RECT GetOutputRectangle() const override;
#pragma region virtual method (ISVImage)

protected:
	typedef std::pair< SVGUID, SVImageClass* > SVParentObjectPair;

	virtual BOOL DestroyImage();

	//- UpdateFromParentInformation () ---------------------------------------
	//- This function will update the Tool Image to itself.
	virtual HRESULT UpdateFromParentInformation();
	virtual HRESULT UpdateFromToolInformation();

	virtual HRESULT ClearParentConnection();

	virtual HRESULT RemoveObjectConnection( const GUID& p_rObjectID );

	virtual SVImageObjectClassPtr GetBufferArrayPtr() const;

	virtual HRESULT IsValidChild( const GUID& p_rChildID, SVImageInfoClass p_svImageInfo ) const;
	virtual HRESULT UpdateChild( const GUID& p_rChildID, SVImageInfoClass p_svImageInfo );
	virtual HRESULT RemoveChild( const GUID& p_rChildID );

	virtual HRESULT UpdatePosition();

	virtual HRESULT UpdateChildren();
	virtual HRESULT RemoveChildren();

	virtual HRESULT UpdateChildBuffers( SVImageObjectClassPtr p_psvChildBuffers, const SVImageInfoClass& p_rImageInfo );

	virtual HRESULT UpdateBufferArrays( bool p_ExcludePositionCheck = false );

	virtual HRESULT GetChildImageInfo( const GUID& p_rChildID, SVImageInfoClass& p_rImageInfo ) const;

	virtual HRESULT GetChildImageHandle( const GUID& p_rChildID, SVSmartHandlePointer& p_rsvBufferHandle ); //@TODO:  Change method to const?
	virtual HRESULT GetChildImageHandle( const GUID& p_rChildID, SVImageIndexStruct p_svBufferIndex, SVSmartHandlePointer& p_rsvBufferHandle ); //@TODO:  Change method to const?

	virtual HRESULT GetImageIndex( SVDataManagerHandle& p_rHandle, const SVImageIndexStruct& rIndex ) const;

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	virtual bool Lock() const;
	virtual bool Unlock() const;

	virtual HRESULT RegisterAsSubObject();
	virtual HRESULT UnregisterAsSubObject();

	HRESULT RebuildStorage( bool p_ExcludePositionCheck );

	mutable SVClock::SVTimeStamp m_LastUpdate;
	mutable SVClock::SVTimeStamp m_LastReset;

	mutable SVParentObjectPair m_ParentImageInfo;
	SVImageTypeEnum m_ImageType;
	SVImageInfoClass m_ImageInfo;
	
	SVImageObjectClassPtr m_BufferArrayPtr;

	SVGuidImageChildMap m_ChildArrays;

private:
	void init();

	mutable bool m_bCriticalSectionCreated;
	mutable CRITICAL_SECTION m_hCriticalSection;
};

typedef SVVector <SVImageClass*, SVImageClass*> SVImageClassArray;

typedef std::set<SVImageClass *> SVImageClassPtrSet;

class SVRGBImageClass : public SVImageClass
{
public:
	virtual ~SVRGBImageClass() {}

	//Child Buffers on Color Images do not support DIB
	void setDibBufferFlag(bool flag=false) { m_ImageInfo.setDibBufferFlag(flag); }
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVImageClass.h_v  $
 * 
 *    Rev 1.5   26 Jun 2014 17:41:18   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add method getDisplayedName
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 May 2014 12:44:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Feb 2014 10:42:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 May 2013 16:16:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   29 Apr 2013 14:15:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 08:20:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   19 Apr 2013 08:56:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added LoadImageFullSize to the SVImageClass.  Also removed unused class LoadImage( filename).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:52:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   21 Jan 2013 10:59:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated missing SetObjectValue and GetObjectValue methods based on 6.01 branch changes for the shift tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   11 Jan 2013 15:00:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  807
 * SCR Title:  Allow Shift Tool to use Source Image Extents
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated buffer checking and upating functionality to allow the extent data to be updated without rebuilding all of the image buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   04 Sep 2012 15:00:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   28 Sep 2011 12:56:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   24 May 2011 15:27:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image class to use Owner Object Id insted of pointer and updated Image View with additional checks to improve performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   13 Apr 2011 15:19:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   07 Apr 2011 16:45:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Image class to fix a problem with image properties and Parent ID collection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   30 Mar 2011 16:20:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   21 Mar 2011 11:45:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Image Info to contain the logic to automaticly move and resize image extents that was contained within the image class.  Updated Image Object and Image classes to handle auto resize events.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   18 Mar 2011 07:20:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix performance issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   16 Mar 2011 13:35:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   11 Mar 2011 14:22:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   25 Feb 2011 12:11:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   10 Feb 2011 14:45:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   01 Feb 2011 11:21:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix a casting issues with the buffer management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   09 Nov 2010 16:21:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   08 Nov 2010 14:39:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   04 Nov 2010 13:39:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   28 Oct 2010 13:54:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   16 Dec 2009 11:00:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   03 Sep 2009 10:10:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   30 Jul 2009 11:56:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   09 Apr 2008 09:31:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  624
 * SCR Title:  Fix problems with Color Tool when it gets deleted
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced Get Parent operator with Get Parent Image operator and Set Parent operator with Set Parent Image operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   24 Jul 2007 13:06:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade Matrox Imaging Library to 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15.2.0   14 Jul 2009 13:41:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image class to use the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15.1.0   07 Apr 2008 09:53:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated the GetParent method to GetParentImage method to allow visibility of the impacted areas of the program.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   17 Jan 2006 09:58:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Conditional History attributes
 * SVImageBufferClass was renamed to SVImageOverlayClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   02 Nov 2005 09:07:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented published result images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   08 Sep 2005 12:25:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new method to copy an image to a BSTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   26 Aug 2005 15:00:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  509
 * SCR Title:  Fix incorrect image problem with Window Tool on a Load Image Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed CopyImageTo and LoadImage child index problem
 * removed dead code
 * cleaned up code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   10 Aug 2005 12:31:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   22 Jun 2005 09:25:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SetObjectValue now returns SVHRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   19 May 2005 15:00:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed HRESULT pass-through
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   22 Feb 2005 10:44:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image object with new load file method to fix buffer leaks and disconnection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   18 Feb 2005 11:48:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  382
 * SCR Title:  Add locking mechanism to internal image objects to eliminate access problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added safe copy functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   17 Feb 2005 14:25:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Feb 2005 14:29:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Aug 2003 12:00:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  380
 * SCR Title:  Add Image Mask option
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new methods for the new mask image
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 10:07:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Apr 2003 17:53:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  347
 * SCR Title:  RGB Color system has out of sync displays and improper results
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed color stuff so that the image view panes are all in sync.  Added new class so that color banded images will be displayed at source image indexes instead of result image indexes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Mar 2003 10:14:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated clear method to virtual.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Jan 2003 15:03:06   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved source code to new file.  All methods in class has changed to handle new buffer array stucture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   16 Jan 2003 18:37:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added code to support image overlays.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   10 Jan 2003 18:28:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new functionality to determine which object has overlays on which image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   10 Jan 2003 17:26:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  309
 * SCR Title:  Implement the ability to lock acquired or processed image from the SIAC.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add Get CircleBuffer and Index fuctions for images so they can be locked correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   18 Dec 2002 08:15:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   17 Dec 2002 15:19:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added
 * struct SVProductInfoStruct; 
 * typedef SVArrayClass <SVImageClass*, SVImageClass*> SVImageClassArray;
 * 
 * SVImageClass:
 * virtual long GetSourceImageIndex( SVProductInfoStruct* pProduct );
 * SVMainImageClass:
 * virtual long GetSourceImageIndex( SVProductInfoStruct* pProduct );
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   19 Nov 2002 13:18:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed references to System to SVImageProcessingClass and SVVirtualCamera
 *       Added SVImageFormatEnum
 * SVImageClass:
 *             Added muiFormat member variable
 *             SVMainImageClass
 *             Added TransformImage member function
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   02 Oct 2001 15:04:30   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  241
 * SCR Title:  Color SVIM:  Load image tool output not available in window tool as input.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added Member, 
 * SVImageClass::pSecondaryDisplayContextImage.
 * Added functions, 
 * SVImageClass::GetSecondaryImageInputContextGuid, 
 * SVImageClass::SetSecondaryDisplayContextImage.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   20 Sep 2001 11:44:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  195
 * SCR Title:  Fix Random NAKs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed ReleaseCurrentBuffer to a virtual method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   18 Sep 2001 15:49:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  195
 * SCR Title:  Fix Random NAKs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added ReleaseCurrentBuffer to SVMainImageClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   19 Apr 2001 14:22:26   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   06 Sep 2000 16:30:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   08 Mar 2000 22:03:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised MoveAbsoluteOrigins to use translation x and y
 * parameters.
 * Added NotiftyUsersOfImageExtentChanged method,
 * primarily to keep SVLineClass object's absolute extents 
 * updated when the input image changes (SVLineROIClass).
 * Revised SetExtent to update the width and height in the
 * AbsoluteExtent variable as well.
 * Revised SetAbsoluteExtent to update the width and height in the Extent variable as well.
 * Revised Move to call NotifyUsersOfImageExtentsChanged.
 * Revised updateChildrenAbsoluteExtents to call
 * NotifyUsersOfImageExtentsChanged.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   04 Mar 2000 11:41:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added updateChildrenAbsoluteExtents method to synchronize absolute extents between Physical images of
 * ImageType 0 and their children when the Physical Images of
 * Image Type 0  are moved.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   03 Mar 2000 16:09:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Renamed SVImageClass::SetExtent() to SetImageExtent().
 * Renamed SVImageClass::SetAbsoluteExtent() to SetImageAbsoluteExtent().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   03 Mar 2000 12:27:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Extent and AbsoluteExtent to be private.
 * Added access Methods to get/set extents.
 * Revised to use access methods to get/set extents.
 * Revised GetObjectScript to write double value types for extents.
 * Revised SetObjectValue to accept double value types for extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   26 Feb 2000 20:01:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Move method to add translation parameters.
 * Revised GetTransform to correct a problem with the
 * ImageTool and the LoadImageTool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   25 Feb 2000 17:36:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added variable Translation (POINT) to SVImageInfoClass
 * Revised SVImageClass getObjectScript and SetObjectValue
 * to save/restore Translation variable.
 * Added GetTransform method to SVImageClass to handle
 * determination of transformation needed for drawing context.
 * Revised isMoveable, isLResizeable, isRResizeable,
 * isTResizeable, isBResizeable, isLTResizeable,
 * isLBResizeable, isRTResizeable, isRBResizeable to
 * call GetTransform and apply the InverseTransform to the
 * mouse point.
 * Added message handler for SVM_GET_CONTEXT_EXTENT
 * Remove message handler for SVM_SET_EXTENT.
 * Revised MoveAbsoluteOrigins to set/reset translation values.
 * Revised SetExtent to document setting of relative extent
 * Left and Top to zero for certain image types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   21 Feb 2000 14:34:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised isMoveable() ,isLResizable(), ,isRResizable(),
 * isTResizable(), isBResizable(), isLTResizable(),
 * isLTResizable(), isLBResizable(), isRTResizable(), and
 * isRBResizable() to use SVDrawContext* and to use relative extents.
 * Removed SetChildListDisplacement (never called).
 * Added GetRootImage(), GetImageInputContextGuid(),
 * and SetDisplayContextImage().
 * Removed dead code in ResetImage().
 * Removed calls to MoveObjectsBase().
 * Revised SetExtent to not hard code Left and 
 * Top coordinates to zero for non main physical images.
 * Added handler in processMessage for SVMSGID_SET_EXTENT.
 * Added pDisplayImageContext (SVImageClass *) variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   Feb 21 2000 11:05:36   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  109
 * SCR Title:  Freeze on Reject for v3.1
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added new global func. SVFillImageInfo...
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   07 Feb 2000 20:18:20   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Changed to work with Main Image Manager. Changed Scripting ( only additions ) to save new necessary information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:46:00   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   28 Oct 1999 18:37:52   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  53
 * SCR Title:  Implement Regression Test Capability in SVObserver
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Implemented Regression Test
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Oct 13 1999 14:59:36   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added input image and Output image.
 * Introduced Logical image types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Sep 10 1999 17:21:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed old image result buffer info.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Sep 1999 17:29:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to save/restore extents, absolute extents,
 * rotationAngle, rotationCenter, and pixelDepth
 * via ObjectScript.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Aug 23 1999 10:10:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  22
 * SCR Title:  Port Multiple Position Queue code from program version 2.4.2
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised to use new Digitizer Classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Feb 1999 10:49:28   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  5
 * SCR Title:  Fix acquisition call back "dismiss bug", make correlated functions more robust
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Stabilzed acquisition circle buffer blocking system. ( Image Occupation )
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
