//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMessageWindow
//* .File Name       : $Workfile:   SVMessageWindow.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:36:36  $
//******************************************************************************

#pragma once

class SVMessageWindow : public CDialog
{
public:
	//{{AFX_DATA(SVMessageWindow)
	enum { IDD = IDD_MESSAGE_DIALOG };
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVMessageWindow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(SVMessageWindow)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	
public:
	SVMessageWindow(CWnd* pParent = nullptr);   // Standardkonstruktor
	virtual ~SVMessageWindow();
};

