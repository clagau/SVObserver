//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionClass 
//* .File Name       : $Workfile:   SVFileAcquisitionClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:35:38  $
//******************************************************************************

#ifndef SVFILEACQUISITIONCLASS_H
#define SVFILEACQUISITIONCLASS_H

#include "SVAcquisitionClass.h"

class SVFileAcquisitionClass : public SVAcquisitionClass  
{
public:
	SVFileAcquisitionClass( const SVAcquisitionConstructParams& p_rParams );
	virtual ~SVFileAcquisitionClass();

	virtual bool IsValid();
	virtual bool IsValidBoard();

	virtual HRESULT Destroy();

	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams );

	virtual HRESULT LoadFiles( SVFileNameArrayClass &rArray);
	virtual HRESULT UnloadFiles();

	virtual HRESULT CreateLightReference( int iBands, int iBrightness, int iContrast );
	virtual HRESULT LoadLightReference( SVLightReference& rArray );
	virtual HRESULT GetLightReference( SVLightReference& rArray );
	virtual HRESULT SetLightReference( SVLightReference& rArray, int iBand = -1 );
	virtual HRESULT ResetLightReference();

	virtual HRESULT CreateLightReferenceBand( int iBand, int iAttributes );
	virtual HRESULT GetMaxLightReferenceValue( unsigned long ulType, long &rlValue );
	virtual HRESULT GetMinLightReferenceValue( unsigned long ulType, long &rlValue );
	virtual HRESULT GetLightReferenceValueStep( unsigned long ulType, unsigned long &rulValue );

protected:
	virtual HRESULT SetLightReferenceImpl( SVLightReference& rLR );

public:
	virtual HRESULT CreateLut( const SVLutInfo& info );
	virtual HRESULT DestroyLut( );
	virtual HRESULT ResetLut( );
	virtual HRESULT GetLut( SVLut& lut );
	virtual HRESULT SetLut( const SVLutBand& lutBand );
	virtual HRESULT SetLut( const SVLut& lut, int iBand = -1 );

	virtual HRESULT GetImageInfo(SVImageInfoClass* pImageInfo);

protected:
	virtual HRESULT GetCameraImageInfo(SVImageInfoClass* pImageInfo);

	virtual HRESULT SetLutImpl( const SVLut& lut );
	virtual HRESULT GetLutImpl( SVLut& lut );

	HRESULT DestroyLocal();


private:
	bool mbIsFilesLoaded;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVFileAcquisitionClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:35:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Aug 2012 16:00:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added missing SetLUT over-ride method in file acquiaition class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Sep 2011 15:55:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with extra consistancy checks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Jan 2011 11:06:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Feb 2010 12:29:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Aug 2009 15:07:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added removed SetDeviceParameters method that was remove in error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 11:36:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Sep 2008 10:23:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

