//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineAnalyzer
//* .File Name       : $Workfile:   SVLineAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 13:14:08  $
//******************************************************************************

#ifndef SVLINEANALYZER_H
#define SVLINEANALYZER_H

#include "SVAnalyzer.h"
#include "SVToolExtentClass.h"

class SVLineClass;

class SVLineAnalyzerClass : public SVAnalyzerClass  
{
	SV_DECLARE_CLASS( SVLineAnalyzerClass );

	friend class SVImageClass;

public:
	SVLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEANALYZER );
	virtual ~SVLineAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL DisconnectInput(SVInObjectInfoStruct* pInObjectInfo);

	virtual HRESULT ResetObject();

	virtual bool DoesObjectHaveExtents() const override;
	virtual HRESULT GetImageExtent( SVImageExtentClass &p_rsvImageExtent );
	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );
	virtual HRESULT SetImageExtentToFit( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );

	virtual HRESULT GetRelativePositionPoints( SVExtentPointStruct &p_rsvPoint1, SVExtentPointStruct &p_rsvPoint2 );

	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint );

	virtual BOOL SetObjectOwner( SVObjectClass* PNewOwner );
	virtual BOOL SetObjectOwner( const GUID& RNewOwnerGUID );

	// Min,Max values used as inputs from the SVImageToLineProjectClass
	virtual BOOL GetMinMaxThresholdValues( DWORD& DWMinValue, DWORD& DWMaxValue ){ return FALSE; };

	// *** TB *** 
	//virtual SVDrawObjectListClass* GetDialogFigure();
	SVObjectClass* getImageToLineProject();

	SVObjectClass* getLineROI();

	virtual BOOL OnValidate();

	virtual HRESULT GetThresholdBarsOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine );
	virtual HRESULT GetHistogramOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine );
	virtual HRESULT GetEdgesOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine );
	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine );

	BOOL m_bEnableDirection;
	BOOL m_bEnableEdgeSelect;
	BOOL m_bEnablePolarisation;
	BOOL m_bEnablePosition;
	BOOL m_bEnableThreshold;

	BOOL m_bSetupEdgeA;

protected:
	SVLineClass* getInputLine();

	// New for Profile Tool - Thresholding Options
	virtual BOOL getThresholdValues( DWORD& DWUpperThresholdValue, DWORD& DWLowerThresholdValue ){ return FALSE; };
	virtual BOOL getThresholdValues( DWORD& DWUpperThresholdValue, DWORD& DWLowerThresholdValue, int index ){ return FALSE; };

	virtual BOOL  onRun( SVRunStatusClass& RRunStatus );

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual void removeEmbeddedExtents();

	virtual HRESULT onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray );

	void GetOverlayDrawlingAdjustments( bool& p_rIsGageTool, double& p_rHeight, double& p_rLineCorrect );

	// Input: Line
	SVInObjectInfoStruct		inputLineObjectInfo;

	// Embedded Objects
	SVDoubleValueObjectClass	extentX1;
	SVDoubleValueObjectClass	extentY1;
	SVDoubleValueObjectClass	extentX2;
	SVDoubleValueObjectClass	extentY2;
	SVDoubleValueObjectClass	extentLength;

	SVToolExtentClass           m_svLineExtent;

	// Draw Objects:
	//SVDrawObjectClass   m_svLineFigure;

	BOOL m_bDisplayAnalyzeLine;
	BOOL m_bDisplayAnalyzeEdge;

	//SVDrawObjectListClass m_svDialogFigure;

private:
	void addLineAnalyzerFriend();
	
};

#endif

