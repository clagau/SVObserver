//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

#pragma once

class SVClassInfoStructListClass;

class SVAvailableAnalyzerListComboBoxClass : public CComboBox
{
public:
	SVAvailableAnalyzerListComboBoxClass();
	virtual ~SVAvailableAnalyzerListComboBoxClass();

	int init( SVClassInfoStructListClass* pClassInfoList, int currentSelection = -1 );
	void add (LPCTSTR ClassName);
	void remove (LPCTSTR ClassName);

protected:
	//{{AFX_MSG(SVAvailableAnalyzerListComboBoxClass)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVAvailableAnalyzerListComboBoxClass)
	//}}AFX_VIRTUAL
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

