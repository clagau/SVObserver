//******************************************************************************
//* COPYRIGHT (c) 2014, 2014 by Seidenader
//* All Rights Reserved 
//******************************************************************************
//* .Module Name     : FramedSplitterWnd
//* .File Name       : $Workfile:   FramedSplitterWnd.h  $
//* .Description     : SplitterWnd drawing a blue frame around the frame if it contains an active view  
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 13:23:06  $
//******************************************************************************

#pragma once

class FramedSplitterWnd : public CSplitterWnd
{
#pragma region Constructor
public:
	FramedSplitterWnd();
	virtual ~FramedSplitterWnd();
#pragma endregion Constructor

#pragma region Public Methods
public:
	//************************************
	// Method:    RefreshSplitBars
	// Description:  Force the splitter bar to redraw.
	// Parameter: void
	// Returns:   void
	//************************************
	void RefreshSplitBars();
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	//************************************
	// Method:    OnDrawSplitter
	// Description:  Override the OnDrawSplitter method in CSplitterWnd to mark the pane with the focus with a blue frame.
	// Parameter: CDC* pDC
	// Parameter: ESplitType nType
	// Parameter: const CRect & rectArg
	// Returns:   void
	//************************************
	void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg) override;
#pragma endregion Protected Methods
};

