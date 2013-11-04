//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVShiftTool
//* .File Name       : $Workfile:   SVShiftTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:03:22  $
//******************************************************************************

#ifndef SVShiftTool_H
#define SVShiftTool_H

#include "SVBoolValueObjectClass.h"
#include "SVDoubleValueObjectClass.h"
#include "SVEnumerateValueObjectClass.h"
#include "SVImageClass.h"
#include "SVTool.h"


enum SV_SHIFT_ENUM
{
	SV_SHIFT_NONE		= 0,
	SV_SHIFT_ABSOLUTE	= 1,
	SV_SHIFT_REFERENCE	= 2
};


// Add String for SVEnumerateValueObjectClass
const LPCSTR g_strShiftToolEnum = 
                _T( "None=0,Absolute=1,Reference=2" );

class SVShiftTool : public SVToolClass, public OperatorMovable
{
	SV_DECLARE_CLASS( SVShiftTool );

public:
	SVShiftTool( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVSHIFTTOOL );
	virtual ~SVShiftTool();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual HRESULT ResetObject();

	virtual HRESULT CollectInputImageNames( SVRunStatusClass& RRunStatus );
	virtual HRESULT GetInputImageNames( SVStringValueObjectClass*& p_pSourceNames );

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );
	virtual SVTaskObjectClass* GetObjectAtPoint( const SVExtentPointStruct &p_rPoint );
	virtual HRESULT DoesObjectHaveExtents() const;

protected:
	virtual BOOL onRun( SVRunStatusClass &p_rRunStatus );
	virtual BOOL OnValidate();
	virtual HRESULT UpdateImageWithExtent( unsigned long p_Index );

	SVImageClass* GetImageInput() const;
	SVDoubleValueObjectClass* GetTranslationXInput() const;
	SVDoubleValueObjectClass* GetTranslationYInput() const;

	SVInObjectInfoStruct m_ImageInput;
	SVInObjectInfoStruct m_TranslationYInput;
	SVInObjectInfoStruct m_TranslationXInput;



	SVStringValueObjectClass m_SourceImageNames;
//	SVDoubleValueObjectClass m_TranslationX;
//	SVDoubleValueObjectClass m_TranslationY;
	SVLongValueObjectClass m_TranslationX;
	SVLongValueObjectClass m_TranslationY;
	SVDoubleValueObjectClass m_LearnedTranslationX;
	SVDoubleValueObjectClass m_LearnedTranslationY;
	SVDoubleValueObjectClass m_DisplacementX;
	SVDoubleValueObjectClass m_DisplacementY;
	SVDoubleValueObjectClass m_LeftResult;
	SVDoubleValueObjectClass m_TopResult;
	SVImageClass m_OutputImage;

public:
	SVEnumerateValueObjectClass m_evoShiftMode;
	SVBoolValueObjectClass m_EnableSourceImageExtents;

private:
	void LocalInitialize();
	void LocalDestroy();
	void SetAttributeData();

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVShiftTool.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:03:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   11 Jan 2013 15:04:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  807
 * SCR Title:  Allow Shift Tool to use Source Image Extents
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new extent functionality that does not rebuild the image buffers on a positional change.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Jan 2013 16:43:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new value object for Enable Source Image Extents
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Jan 2013 15:00:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added SetAttributeData method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jan 2013 15:12:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated code for the different modes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Dec 2012 11:22:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
