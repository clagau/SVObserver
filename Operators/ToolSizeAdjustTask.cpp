//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//******************************************************************************
///Set Tool position and Size of a tool during resetObject
/// Set the tool position and size to full Image or in accordance with a formula
/// class is in friendlist  of corresponding tool 
/// the friendlist of this class holds the Svevaluationobjects 
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "EQAdjustSize.h"
#include "ToolSizeAdjustTask.h"
#include "Tools/SVTool.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVStatusLibrary/SvimState.h"
#include "SVStatusLibrary\MessageManager.h"
#include "Definitions/Color.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/TextDefinesSvDef.h"
#include "SVValueObjectLibrary\SVDoubleValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const long ToolSizeAdjustTask::MinToolSize = 1; //< Minimum  allowed WindowSize
const long ToolSizeAdjustTask::MaxToolSize = 100000; //< Maximum allowed WindowSize 
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( ToolSizeAdjustTask, SvPb::ToolSizeAdjustTaskId)

ToolSizeAdjustTask::ToolSizeAdjustTask(bool AllowFullsize , bool AllowAdjustSize, bool AllowAdjustPosition , SVObjectClass* POwner , int StringResourceID )  
: SVTaskObjectClass(POwner, StringResourceID)
, m_AllowFullSize(AllowFullsize)
, m_AllowAdjustSize(AllowAdjustSize)
, m_AllowAdjustPosition(AllowAdjustPosition)
{
	// Identify our output type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolSizeAdjustTaskType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVNotSetSubObjectType;

	RegisterEmbeddedObject( &m_InputModes[SvDef::ToolSizeAdjustEnum::TSWidth], SvPb::ToolSizeAdjustSizeWidthModeEId, IDS_OBJECTNAME_TOOLSIZEADJUSTSIZEWIDTH, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_InputModes[SvDef::ToolSizeAdjustEnum::TSHeight], SvPb::ToolSizeAdjustSizeHeightModeEId, IDS_OBJECTNAME_TOOLSIZEADJUSTSIZEHEIGHT, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_InputModes[SvDef::ToolSizeAdjustEnum::TSPositionX], SvPb::ToolSizeAdjustSizePositionXModeEId, IDS_OBJECTNAME_TOOLSIZEADJUSTPOSITIONX, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_InputModes[SvDef::ToolSizeAdjustEnum::TSPositionY], SvPb::ToolSizeAdjustSizePositionYModeEId, IDS_OBJECTNAME_TOOLSIZEADJUSTPOSITIONY, false, SvOi::SVResetItemNone, true);

	SvDef::NameValueVector EnumVector{ {SvDef::SizeAdjustTextNone, SvDef::ToolSizeModes::TSNone} };
	
	if (m_AllowFullSize == true)
	{
		EnumVector.push_back(SvDef::NameValuePair{ SvDef::SizeAdjustTextAutoFitSize, SvDef::ToolSizeModes::TSAutoFit });
	}
	EnumVector.push_back(SvDef::NameValuePair{ SvDef::SizeAdjustTextFormula, SvDef::ToolSizeModes::TSFormula });

	for( int vType  = SvDef::ToolSizeAdjustEnum::TSPositionX ; vType < SvDef::ToolSizeAdjustEnum::TSValuesCount ; ++vType)
	{
		m_InputModes[vType].SetEnumTypes(EnumVector);
		m_InputModes[vType].SetDefaultValue(SvDef::ToolSizeModes::TSNone, true );
		m_InputModes[vType].SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	}

	//Add Evaluation Objects 
	bool ok(false);
	ok = AddEvaluationObject(&m_resultInput[SvDef::ToolSizeAdjustEnum::TSWidth], SvPb::EQAdjustSizeWidthClassId, _T("ToolSizeAdjustWidth"), SvPb::WidthInputEId);
	Log_Assert(ok);
	ok = AddEvaluationObject(&m_resultInput[SvDef::ToolSizeAdjustEnum::TSHeight], SvPb::EQAdjustSizeHeightClassId, _T("ToolSizeAdjustHeight"), SvPb::HeightInputEId);
	Log_Assert(ok);

	ok = AddEvaluationObject(&m_resultInput[SvDef::ToolSizeAdjustEnum::TSPositionX], SvPb::EQAdjustSizePositionXClassId, _T("ToolSizeAdjustPositionX"), SvPb::PositionXInputEId);
	Log_Assert(ok);
	ok = AddEvaluationObject(&m_resultInput[SvDef::ToolSizeAdjustEnum::TSPositionY], SvPb::EQAdjustSizePositionYClassId,  _T("ToolSizeAdjustPositionY"), SvPb::PositionYInputEId);
	Log_Assert(ok);
}

ToolSizeAdjustTask::~ToolSizeAdjustTask()
{
}

bool ToolSizeAdjustTask::AddEvaluationObject(SvOl::InputObject* pInput, SvPb::ClassIdEnum classId, LPCTSTR Name, SvPb::EmbeddedIdEnum embeddedId)
{
	SVTaskObjectClass* pObject(nullptr);
	SVObjectManagerClass::Instance().ConstructObject( classId, pObject );
	if( nullptr != pObject )
	{
		pObject->SetObjectOwner( this );
		AddFriend( pObject );
	}
	else
	{
		return false;
	}
	EQAdjustSize* pAdjustSize  = dynamic_cast<EQAdjustSize*> (pObject);
	if( nullptr == pAdjustSize)
	{
		return false;
	}

	pInput->SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, pAdjustSize->GetResultId());
	pInput->SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
	return registerInputObject(pInput, Name, embeddedId);
}

bool ToolSizeAdjustTask::onRun(SvIe::RunStatus& , SvStl::MessageContainerVector*)
{
	//DoNothing in onRun
	return true;
}

bool  ToolSizeAdjustTask::Run( SvIe::RunStatus&, SvStl::MessageContainerVector* )
{
	//DoNothing in Run
	return true;
}

HRESULT ToolSizeAdjustTask::GetModes(long &rModeWidth,long &rModeHeight,long &rModePosX, long &rModePosY)
{
	rModeWidth = rModeHeight = rModePosX = rModePosY = SvDef::ToolSizeModes::TSModeCount;
	HRESULT hresult =  m_InputModes[SvDef::ToolSizeAdjustEnum::TSWidth].GetValue( rModeWidth );
	if (S_OK == hresult)
	{
		hresult =  m_InputModes[SvDef::ToolSizeAdjustEnum::TSHeight].GetValue( rModeHeight );
	}
	if (S_OK == hresult)
	{
		hresult =  m_InputModes[SvDef::ToolSizeAdjustEnum::TSPositionX].GetValue( rModePosX );
	}
	if (S_OK == hresult)
	{
		hresult =  m_InputModes[SvDef::ToolSizeAdjustEnum::TSPositionY].GetValue( rModePosY );
	}

	if (S_OK == hresult)
	{

		if(( rModeWidth == SvDef::TSAutoFit)  ||  (rModeHeight == SvDef::TSAutoFit)  ||  (rModePosX == SvDef::TSAutoFit)  ||  (rModePosY == SvDef::TSAutoFit))
		{
			if( ( rModeWidth != SvDef::TSAutoFit)  ||   (rModeHeight != SvDef::TSAutoFit)  ||  (rModePosX != SvDef::TSAutoFit)  ||  (rModePosY != SvDef::TSAutoFit))
			{
				hresult = E_FAIL;
			}
			if( false == m_AllowFullSize )
			{
				hresult = E_FAIL;
			}
		}
	}
	if( S_OK == hresult )
	{
		if( ((rModeWidth == SvDef::TSFormula) ||  (rModeHeight == SvDef::TSFormula) ) && false == m_AllowAdjustSize)
		{
			hresult = E_FAIL;
		}
		if( ((rModePosX == SvDef::TSFormula) ||  (rModePosY == SvDef::TSFormula) ) && false== m_AllowAdjustPosition)
		{
			hresult = E_FAIL;
		}
	}
	return hresult;
}

HRESULT ToolSizeAdjustTask::GetResultValue(SvDef::ToolSizeAdjustEnum val, long& rValue) const
{
	HRESULT hresult = SVMSG_INVALID_VALUE_POINTER;
	SvVol::SVDoubleValueObjectClass* pValueObject =GetDResultObjects(val);
	if (nullptr != pValueObject)
	{
		double TempValue( 0.0 );
		hresult =  pValueObject->GetValue(TempValue);
		rValue = static_cast<long> (TempValue);
	}
	if (S_OK == hresult)
	{
		if( val == SvDef::ToolSizeAdjustEnum::TSHeight  || val == SvDef::ToolSizeAdjustEnum::TSWidth )
		{
			if (rValue < MinToolSize)
			{
				hresult = SVMSG_INVALID_SIZE;
			}
			else if (rValue > MaxToolSize)
			{
				hresult = SVMSG_INVALID_SIZE;
			}
		}
		else
		{
			if (rValue < 0)
			{
				hresult = SVMSG_NEGATIVE_POSITION;
			}
			else if (rValue > MaxToolSize)
			{
				hresult = SVMSG_SVO_TO_LARGE_POSITION;
			}
		}
	}
	return hresult;
}

SvTo::SVToolClass* ToolSizeAdjustTask::GetTool() const
{
	return dynamic_cast<SvTo::SVToolClass*> (GetParent());
}

bool ToolSizeAdjustTask::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	///Do nothing if we are in Run Mode 
	if (SvimState::CheckState(SV_STATE_RUNNING))
	{
		return true;
	}

	bool Result = SVTaskObjectClass::ResetObject(pErrorMessages);

	for (auto& rEntry : m_resultInput)
	{
		rEntry.validateInput();
	}


	SvPb::EAutoSize  	 AutoSizeEnable(SvPb::EnableNone);
	SvTo::SVToolClass* pTool = GetTool();
	if (nullptr != pTool)
	{
		AutoSizeEnable = pTool->GetAutoSizeEnabled();
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidOwner, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	bool bAllowNegativePostion = SvPb::EnableNegativePosition == (AutoSizeEnable & SvPb::EnableNegativePosition);

	long ModeWidth(SvDef::TSNone), ModeHeight(SvDef::TSNone), ModeX(SvDef::TSNone), ModeY(SvDef::TSNone);
	bool bDone = (SvPb::EnableNone == AutoSizeEnable);
	HRESULT hresult = S_OK;
	if (false == bDone  && Result)
	{
		hresult = GetModes(ModeWidth, ModeHeight, ModeX, ModeY);
		Result = (S_OK == hresult);
	}
	if( Result && (SvDef::TSNone == ModeWidth) && (SvDef::TSNone == ModeHeight) && (SvDef::TSNone == ModeX) && (SvDef::TSNone == ModeY))
	{
		bDone = true;
	}
	if (!bDone && Result)
	{
		long ParentOutputWidth{0L}, ParentOutputHeight{0L}, oldWidth{0L}, oldHeight{0L};
		long  oldPosX{0L}, oldPosY{0L};
		SVImageExtentClass ImageExtent = pTool->GetImageExtent();
		SVImageExtentClass ParentImageExtent;

		if (S_OK == hresult)
		{
			hresult = pTool->GetParentExtent(ParentImageExtent);
		}

		if (S_OK == hresult)
		{
			hresult =ParentImageExtent.GetExtentProperty(SvPb::SVExtentPropertyOutputWidth, ParentOutputWidth);
		}
		if (S_OK == hresult)
		{
			hresult =ParentImageExtent.GetExtentProperty(SvPb::SVExtentPropertyOutputHeight, ParentOutputHeight);
		}
		if (S_OK == hresult)
		{
			hresult = ImageExtent.GetExtentProperty(SvPb::SVExtentPropertyWidth, oldWidth);
		}

		if (S_OK == hresult)
		{
			hresult = ImageExtent.GetExtentProperty(SvPb::SVExtentPropertyHeight, oldHeight);
		}
		if (S_OK  == hresult)
		{
			hresult = ImageExtent.GetExtentProperty(SvPb::SVExtentPropertyPositionPointX, oldPosX);
		}
		if (S_OK  == hresult)
		{
			hresult = ImageExtent.GetExtentProperty(SvPb::SVExtentPropertyPositionPointY, oldPosY);
		}

		bool bSetImageExtend(false);
		
		if(( S_OK  == hresult) && (ModeWidth == SvDef::TSAutoFit) &&  (AutoSizeEnable == SvPb::EnableSizeAndPosition))
		{
			/////Set To Full size (Image Extent = parent Extent)
			if( ( S_OK  == hresult) && (oldHeight != ParentOutputHeight || oldWidth != ParentOutputWidth || oldPosY != 0 || oldPosX != 0  ))
			{
				hresult = pTool->SetImageExtentToParent(); 
				}
			bDone = true;
		}

		if ((S_OK == hresult) && (false == bDone) && (SvDef::TSFormula == ModeWidth ) && (SvPb::EnableSize == (AutoSizeEnable & SvPb::EnableSize)))
		{
			long newWidth(0);
			hresult = GetResultValue(SvDef::ToolSizeAdjustEnum::TSWidth, newWidth);

			if ((S_OK  == hresult) && (oldWidth != newWidth))
			{
					hresult = ImageExtent.SetExtentProperty(SvPb::SVExtentPropertyWidth, newWidth);
					bSetImageExtend = true;
				}
			}
		if ((S_OK  == hresult) && (false == bDone) &&  (SvDef::TSFormula == ModeHeight) &&  (SvPb::EnableSize == (AutoSizeEnable & SvPb::EnableSize)))
			{
			long   NewHeight(0);
			hresult = GetResultValue(SvDef::ToolSizeAdjustEnum::TSHeight, NewHeight);

			if (S_OK  == hresult)
			{
				hresult = ImageExtent.GetExtentProperty(SvPb::SVExtentPropertyHeight, oldHeight);
			}

			if ((S_OK  == hresult) && (oldHeight != NewHeight))
			{
				hresult = ImageExtent.SetExtentProperty(SvPb::SVExtentPropertyHeight, NewHeight);
				bSetImageExtend = true;
			}
		}
		if ((S_OK == hresult) && (false == bDone) && (SvDef::TSFormula == ModeX) && (SvPb::EnablePosition == (AutoSizeEnable & SvPb::EnablePosition)))
		{
			long  PosX;
			hresult =  GetResultValue(SvDef::ToolSizeAdjustEnum::TSPositionX,PosX);
			
			if (SVMSG_NEGATIVE_POSITION == hresult && bAllowNegativePostion)
			{
				hresult = S_OK;
			}

			if ((S_OK  == hresult) && (oldPosX != PosX))
			{
				hresult = ImageExtent.SetExtentProperty(SvPb::SVExtentPropertyPositionPointX, PosX);
				bSetImageExtend = true;
			}
		}
		if ((S_OK == hresult) && (false == bDone) && (SvDef::TSFormula == ModeY) && (SvPb::EnablePosition == (AutoSizeEnable & SvPb::EnablePosition)))
		{
			long  PosY;
			hresult = GetResultValue(SvDef::ToolSizeAdjustEnum::TSPositionY, PosY);
			
			if (SVMSG_NEGATIVE_POSITION == hresult && bAllowNegativePostion)
			{
				hresult = S_OK;
			}
			
			if ((S_OK == hresult) && (oldPosY != PosY))
			{
				hresult = ImageExtent.SetExtentProperty(SvPb::SVExtentPropertyPositionPointY, PosY);
				bSetImageExtend = true;
			}
		}

		if ((S_OK == hresult) && bSetImageExtend && (false == bDone))
		{
			hresult = pTool->SetImageExtent(ImageExtent);
		}
	
		if (S_OK == hresult)
		{
			pTool->UpdateBottomAndRight(); 
		}
	}

	if ( Result && (S_OK == hresult) )
	{
		hresult = SetExtendPropertyAutoReset();
	}

	if (S_OK != hresult)
	{
		Result = false;

		SvStl::MessageTextEnum messageId = SvStl::Tid_ErrorInReset;
		SvDef::StringVector msgList;
		if(GetTool()->GetName())
		{
			messageId = SvStl::Tid_InValidNewExtents;
			msgList.push_back(GetTool()->GetName());
		}
		SvStl::MessageContainer message;
		message.setMessage( SVMSG_SVO_58_TOOLADJUST_RESET_ERROR, messageId, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage( message.getMessage() );
		if (nullptr != pErrorMessages)
		{
			pErrorMessages->push_back(message);
		}
	}

	return Result;
}

bool ToolSizeAdjustTask::IsFullSizeAllowed() const
{
	return m_AllowFullSize;
}

bool ToolSizeAdjustTask::IsAdjustSizeAllowed() const
{
	return m_AllowAdjustSize;
}

bool ToolSizeAdjustTask::IsAdjustPositionAllowed() const
{
	return m_AllowAdjustPosition;
}

SvOi::ITool* ToolSizeAdjustTask::getTool() const
{
	return GetTool();
}

SvVol::SVDoubleValueObjectClass* ToolSizeAdjustTask::GetDResultObjects(SvDef::ToolSizeAdjustEnum val) const
{
	return m_resultInput[val].getInput<SvVol::SVDoubleValueObjectClass>();
}

HRESULT ToolSizeAdjustTask::SetExtendPropertyAutoReset()
{
	HRESULT hresult = S_OK;
	SvTo::SVToolClass* pTool = GetTool();
	if (nullptr == pTool)
	{
		Log_Assert(false);
		hresult = E_FAIL;
	}

	long ModeWidth(SvDef::TSNone), ModeHeight(SvDef::TSNone), ModeX(SvDef::TSNone), ModeY(SvDef::TSNone);
	bool bWidthSetByReset(true), bHeightSetByReset(true), bPosXSetByReset(true), bPosYSetByReset(true); 

	if (S_OK == hresult)
	{
		hresult = GetModes(ModeWidth, ModeHeight, ModeX, ModeY);
	}

	if (SvDef::TSNone == ModeWidth)
	{	
		bWidthSetByReset = false;
	}
	if (SvDef::TSNone == ModeHeight)
	{	
		bHeightSetByReset = false;
	}
	if (SvDef::TSNone == ModeX)
	{	
		bPosXSetByReset = false;
	}
	if (SvDef::TSNone == ModeY)
	{	
		bPosYSetByReset = false;
	}

	SvIe::SVExtentPropertyInfoStruct info;
	if (S_OK == hresult)
	{
		hresult = pTool->GetPropertyInfo(SvPb::SVExtentPropertyPositionPointX, info);
		info.bSetByReset = bPosXSetByReset;
	}
	if (S_OK == hresult)
	{
		hresult = pTool->SetExtentPropertyInfo( SvPb::SVExtentPropertyPositionPointX, info );
	}

	if (S_OK == hresult)
	{
		hresult = pTool->GetPropertyInfo( SvPb::SVExtentPropertyPositionPointY, info );
		info.bSetByReset = bPosYSetByReset;
	}
	if (S_OK == hresult)
	{
		hresult = pTool->SetExtentPropertyInfo( SvPb::SVExtentPropertyPositionPointY, info );
	}

	if (S_OK == hresult)
	{
		hresult = pTool->GetPropertyInfo( SvPb::SVExtentPropertyWidth, info );
		info.bSetByReset = bWidthSetByReset;
	}
	if (S_OK == hresult)
	{
		hresult = pTool->SetExtentPropertyInfo( SvPb::SVExtentPropertyWidth, info );
	}

	if (S_OK == hresult)
	{
		hresult = pTool->GetPropertyInfo( SvPb::SVExtentPropertyHeight, info );
		info.bSetByReset = bHeightSetByReset;
	}
	if (S_OK == hresult)
	{
		hresult = pTool->SetExtentPropertyInfo( SvPb::SVExtentPropertyHeight, info );
	}
	return hresult;
}

ToolSizeAdjustTask* ToolSizeAdjustTask::AddToFriendlist(SvTo::SVToolClass* pTool, bool AllowFullsize, bool AllowAdjustSize, bool AllowAdjustPosition)
{
	if (nullptr == pTool)
	{
		Log_Assert(false);
		return nullptr;
	}

	//Add ToolSizeAdjustTask to friendlist
	ToolSizeAdjustTask* pToolSizeAdjustTask = new ToolSizeAdjustTask(AllowFullsize, AllowAdjustSize, AllowAdjustPosition, pTool);
	if (nullptr != pToolSizeAdjustTask)
	{
		pToolSizeAdjustTask->SetObjectOwner( pTool );
		pTool->AddFriend( pToolSizeAdjustTask );
	}
	return pToolSizeAdjustTask;
}

HRESULT ToolSizeAdjustTask::EnsureInFriendList(SvTo::SVToolClass* pTool, bool AllowFullsize, bool AllowAdjustSize, bool AllowAdjustPosition)
{
	HRESULT hres(E_FAIL);
	if (nullptr == pTool)
	{
		Log_Assert(false);
		return hres;
	}
	ToolSizeAdjustTask* pToolSizeAdjustTask = GetToolSizeAdjustTask(pTool);
	
	if (nullptr == pToolSizeAdjustTask)
	{
		pToolSizeAdjustTask = AddToFriendlist(pTool, AllowFullsize, AllowAdjustSize, AllowAdjustPosition );
		if (nullptr != pToolSizeAdjustTask)
		{
			pTool->connectAllInputs();
			pTool->CreateChildObject(pToolSizeAdjustTask);
		}
	}
	if (nullptr != pToolSizeAdjustTask)
	{
		pToolSizeAdjustTask->m_AllowAdjustPosition = AllowAdjustPosition;
		pToolSizeAdjustTask->m_AllowAdjustSize = AllowAdjustSize;
		pToolSizeAdjustTask->m_AllowFullSize = AllowFullsize;
		hres = pToolSizeAdjustTask->SetExtendPropertyAutoReset();
	}
	return hres;
}

ToolSizeAdjustTask* ToolSizeAdjustTask::GetToolSizeAdjustTask(const SVTaskObjectClass* pTask)
{
	ToolSizeAdjustTask* pToolSizeAdjustTask = nullptr;
	if (nullptr != pTask)
	{
		SvDef::SVObjectTypeInfoStruct ToolSizeAdjustTaskInfo;
		ToolSizeAdjustTaskInfo.m_ObjectType = SvPb::SVToolSizeAdjustTaskType;
		ToolSizeAdjustTaskInfo.m_SubType = SvPb::SVNotSetSubObjectType;
		pToolSizeAdjustTask = dynamic_cast<ToolSizeAdjustTask*>(pTask->GetFriend(ToolSizeAdjustTaskInfo));
	}
	return pToolSizeAdjustTask;
}

bool ToolSizeAdjustTask::UseSizeAdjust(const SVTaskObjectClass *pObject)
{
	bool ret = false;
	const ToolSizeAdjustTask* pToolSizeAdjustTask = GetToolSizeAdjustTask(pObject);
	if (pToolSizeAdjustTask)
	{
		ret = pToolSizeAdjustTask->UseAutoMode();
	}
	return ret;
}

bool ToolSizeAdjustTask::UseAutoMode() const
{
	long Mode(SvDef::TSModeCount);
	for( int vType  = SvDef::ToolSizeAdjustEnum::TSPositionX ; vType < SvDef::ToolSizeAdjustEnum::TSValuesCount ; ++vType)
	{
		m_InputModes[vType].GetValue( Mode );
		if (SvDef::TSAutoFit == Mode || SvDef::TSFormula == Mode)
		{
			return true;
		}
	}
	return false;
}

DWORD ToolSizeAdjustTask::GetObjectColor() const
{
	return SvDef::DefaultWhiteColor;
}

} //namespace SvOp

