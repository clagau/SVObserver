//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVAvailableFilterListComboBoxClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:26:42  $
//******************************************************************************

#ifndef SVAVAILABLEFILTERLISTCOMBOBOXCLASS_H
#define SVAVAILABLEFILTERLISTCOMBOBOXCLASS_H

class SvOi::IClassInfoStructList;

namespace Seidenader
{
	namespace SVOGui
	{
		class SVAvailableFilterListComboBoxClass : public CComboBox
		{
		public:
			SVAvailableFilterListComboBoxClass();
			virtual ~SVAvailableFilterListComboBoxClass();

			void init( SvOi::IClassInfoStructList* L );

			//{{AFX_VIRTUAL(SVAvailableFilterListComboBoxClass)
			//}}AFX_VIRTUAL

		protected:
			//{{AFX_MSG(SVAvailableFilterListComboBoxClass)
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()
		};
	}  //end namespace SVOGUI
}  //end namespace Seidenader

namespace SvOg = Seidenader::SVOGui;

#endif	//	SVAVAILABLEFILTERLISTCOMBOBOXCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVAvailableFilterListComboBoxClass.h_v  $
 * 
 *    Rev 1.0   12 Aug 2013 16:26:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
