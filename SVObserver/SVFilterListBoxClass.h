//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVFilterListBoxClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:27:50  $
//******************************************************************************

#ifndef SVFILTERLISTBOXCLASS_H
#define SVFILTERLISTBOXCLASS_H

class SVUnaryImageOperatorListClass;

class SVFilterListBoxClass : public CListBox
{
public:
	SVFilterListBoxClass();
	virtual ~SVFilterListBoxClass();

	void init( SVUnaryImageOperatorListClass* List );

	//{{AFX_VIRTUAL(SVFilterListBoxClass)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(SVFilterListBoxClass)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif	//	SVFILTERLISTBOXCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVFilterListBoxClass.h_v  $
 * 
 *    Rev 1.0   12 Aug 2013 16:27:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
