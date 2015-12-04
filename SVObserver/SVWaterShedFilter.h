// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVWaterShedFilter
// * .File Name       : $Workfile:   SVWaterShedFilter.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   13 Aug 2013 10:32:08  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "ObjectInterfaces\IWatershedFilter.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVFilterClass.h"
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVWatershedFilterClass
// -----------------------------------------------------------------------------
// .Description : Base class for all kinds of Skeleton filters
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
////////////////////////////////////////////////////////////////////////////////
class SVWatershedFilterClass : virtual public SvOi::IWatershedFilter
	,public SVFilterClass
{
	SV_DECLARE_CLASS( SVWatershedFilterClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
// Standard constructor
	SVWatershedFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVWATERSHEDFILTER );
	virtual ~SVWatershedFilterClass();
	
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

#pragma region virtual method (IFilter)
	virtual bool shouldResetInspection() const override { return true; }
#pragma region virtual method (IFilter)

#pragma region virtual method (IWatershedFilter)
public:
			virtual HRESULT addControlFlagRequest(long value) override;
			virtual long getControlFlag() const override;
			virtual HRESULT addMinVariationRequest(long value) override;
			virtual long getMinVariation() const override;
			virtual HRESULT addMarkerUsedRequest(bool value) override;
			virtual bool isMarkerUsed() const override;

			virtual SVString getMarkerImage() const override;
			virtual HRESULT setMarkerImage(const SVString& imageName) override;

			virtual std::vector<SVString> getAvailableMarkerImageNames() override;
#pragma region virtual method (IWatershedFilter)

private:
	void init();

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
	virtual BOOL OnValidate();
	
	//************************************
	//! Return if image a valid pointer and image for a marker for this watershed.
	//! \param pImage [in] Pointer to the image to test. 
	//! \param rToolSet [in] Reference of the tool set.
	//! \returns bool
	//************************************
	bool isValidMarkerImage(const SVImageClass* pImage, const SVToolSetClass &rToolSet) const;
	//************************************
	//! Return a list of available source image for this tool.
	//! \param toolsetGUID [in] GUID for this tool set.
	//! \returns SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque
	//************************************
	static SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque getAvailableSourceImage(const SVGUID toolsetGUID);

//******************************************************************************
// Operator(s):
//******************************************************************************

public:
	SVInObjectInfoStruct m_MarkerImageInfo;

////////////////////////////////////////////////////////////////////////////////
// Create Operator
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************
public:
	SVBoolValueObjectClass m_bvoUseMarker ;
	SVLongValueObjectClass m_lvoMinVariation;
	SVLongValueObjectClass m_lvoControlFlag;

//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************
protected:

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
};

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVWaterShedFilter.h_v  $
 * 
 *    Rev 1.1   13 Aug 2013 10:32:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Included SVFilterClass.h instead of SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 16:30:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Feb 2011 15:17:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Jan 2011 12:04:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Aug 2005 08:53:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Feb 2005 15:06:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new Reset Methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Sep 2004 10:01:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  446
 * SCR Title:  Add new filters to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in of Watershed Filter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
//** EOF **
