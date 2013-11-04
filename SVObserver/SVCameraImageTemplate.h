//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraImageTemplate
//* .File Name       : $Workfile:   SVCameraImageTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:49:34  $
//******************************************************************************

#ifndef SVCAMERAIMAGETEMPLATE_H
#define SVCAMERAIMAGETEMPLATE_H

#include <set>

#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVImageClass.h"
#include "SVImageObjectClass.h"

class SVInspectionProcess;
class SVVirtualCamera;

struct SVProductInfoStruct;

class SVCameraImageTemplate : public SVImageClass 
{
public:
	SVCameraImageTemplate( LPCSTR ObjectName );
	SVCameraImageTemplate( SVObjectClass* POwner, int StringResourceID );

	virtual ~SVCameraImageTemplate();

	virtual HRESULT UpdateCameraImage( LPCTSTR p_szCameraName );
	virtual HRESULT UpdateCameraImage( const SVGUID& p_CameraID );

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStruct );
	virtual BOOL SetImageDepth( long lDepth );

	virtual HRESULT ResetObject();

	virtual GUID GetDigitizerID();
	virtual SVVirtualCamera* GetCamera() const;
	virtual SVString GetCameraName() const;

	virtual HRESULT DisconnectBuffers();
	virtual HRESULT ReconnectBuffers();

	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );

#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent = 0 );
#endif
	virtual void Persist(SVObjectWriter& rWriter);

	virtual BOOL ResetImageIndex();

	virtual SVImageIndexStruct GetSourceImageIndex( SVProductInfoStruct* pProduct );
	virtual BOOL GetImageHandleIndex( SVImageIndexStruct& rlIndex ) const;

	virtual HRESULT RestoreMainImage( SVInspectionProcess* p_psvInspection );

protected:
	virtual BOOL CreateBuffers( const SVImageInfoClass& p_rImageInfo, SVImageObjectClassPtr p_ImageArrayPtr ) = 0;

	virtual BOOL DestroyImage();

	virtual HRESULT GetImageIndex( SVDataManagerHandle& p_rHandle, const SVImageIndexStruct& rIndex ) const;

	virtual SVImageObjectClassPtr GetCameraBufferArrayPtr() const;

	virtual HRESULT RegisterAsSubObject();
	virtual HRESULT UnregisterAsSubObject();

	virtual HRESULT RebuildCameraImage();

	SVVirtualCamera* mpCamera;
	SVGUID digitizerObjectID;
	SVImageIndexStruct m_CurrentIndex;

};

typedef std::set< SVCameraImageTemplate* > SVCameraImagePtrSet;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCameraImageTemplate.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:49:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Jan 2013 10:58:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated missing SetObjectValue and GetObjectValue methods based on 6.01 branch changes for the shift tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Sep 2012 14:46:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Sep 2011 12:54:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   13 Apr 2011 14:59:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
