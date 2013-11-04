//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCustomParameterBuilder
//* .File Name       : $Workfile:   SVCustomParameterBuilder.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 May 2013 11:04:18  $
//******************************************************************************
#ifndef INCL_SVCUSTOMPARAMETERBUILDER_INL
#define INCL_SVCUSTOMPARAMETERBUILDER_INL

#include <memory>
#include "SVCustomParameterBuilder.h"
#include "SVMaterialsLibrary/SVMaterials.h"

template<typename Insertor>
void SVCustomParameterBuilder::BuildCustomDeviceParams(const SVMaterialsTree& rTree, Insertor insertor)
{
	for (SVMaterialsTree::const_iterator it = rTree.begin();it!= rTree.end();++it)
	{
		// Get Option branches as well here
		const SVMaterialsTree::SVTreeContainer* pChildTree = it.GetChildTree();

		std::auto_ptr<SVCustomDeviceParam> pParam(SVCustomParameterBuilder::BuildCustomDeviceParam(it->second, pChildTree));
		if (pParam.get())
		{
			// Add to list (makes a copy)
			insertor = std::make_pair(pParam->Type(), SVDeviceParamWrapper(*pParam));
		}
	}
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraFileLibrary\SVCustomParameterBuilder.inl_v  $
 * 
 *    Rev 1.1   13 May 2013 11:04:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 May 2013 15:54:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to correct memory leak
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 09:41:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Apr 2013 14:05:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added PVCS header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Apr 2013 13:50:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
