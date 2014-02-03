//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectLibrary
//* .File Name       : $Workfile:   SVObjectLibrary.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 10:09:16  $
//******************************************************************************

#ifndef SVOBJECTLIBRARY_H
#define SVOBJECTLIBRARY_H

#include "SVStatusLibrary/SVStatusLibrary.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVClassRegisterListClass.h"

class SVObjectClass;

//******************************************************************************
//* MACRO(S):
//******************************************************************************

// Creates new string using malloc and fills it with the GUID, use free() to deallocate the string memory...
#define SV_GUID_TO_NEW_STRING( XGUID, XLPTSTR )	\
 		{ \
			if( ( XLPTSTR ) = ( LPTSTR ) malloc( sizeof( TCHAR ) * 256 ) )	\
			{	\
				_stprintf( ( XLPTSTR ), "{ 0x%x, 0x%x, 0x%x, { 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x } }", \
				( XGUID ).Data1, ( XGUID ).Data2, ( XGUID ).Data3, \
				( XGUID ).Data4[0], ( XGUID ).Data4[1], ( XGUID ).Data4[2], ( XGUID ).Data4[3], \
				( XGUID ).Data4[4], ( XGUID ).Data4[5], ( XGUID ).Data4[6], ( XGUID ).Data4[7] );	\
			}	\
		}

// GUID String
// XLPSZSTRING must be a LPTSTR !!!
#define SV_GUID_TO_STRING( XGUID, XLPSZSTRING, XLENGTH ) \
		{	\
			if( ( XLPSZSTRING ) && ( XLENGTH ) > 0 ) \
			{ \
				TCHAR szBuf[ 100 ]; \
				_stprintf( szBuf, "{%x-%x-%x-%x %x %x %x %x %x %x %x}", \
						   ( XGUID ).Data1, ( XGUID ).Data2, ( XGUID ).Data3, \
						   ( XGUID ).Data4[0], ( XGUID ).Data4[1], ( XGUID ).Data4[2], ( XGUID ).Data4[3], \
						   ( XGUID ).Data4[4], ( XGUID ).Data4[5], ( XGUID ).Data4[6], ( XGUID ).Data4[7] ); \
				_tcsncpy( XLPSZSTRING, szBuf, min( ( XLENGTH ), 100 ) - 1 ); \
			} \
		}


#define SV_MESSAGE( ID, VALUE, CONTEXT, SUBTEXT )	\
		{   \
			char text[ _MAX_PATH ]; \
			sprintf( text, "Receiver: %s\nMessage ID: %x\nMessage Value: %x\nMessage Context: %x\n%s", GetName(), ( ID ), ( VALUE ), ( CONTEXT ), ( SUBTEXT ) ); \
			::MessageBox( NULL, text, "Message Hint:", MB_ICONINFORMATION ); \
		}

// Use the RTTI 
#define SV_IS_KIND_OF( XP, XCLASSNAME )	\
		( ( XP != NULL ) && ( dynamic_cast< XCLASSNAME* >( XP ) != NULL ) )

// Should be in the class definition...
#define SV_DECLARE_CLASS( XCLASSNAME )					\
		public:	\
			virtual SVGUID GetClassID(); \
			virtual LPCTSTR GetClassName();


// Should be in the implementation file... ( .cpp )
#define SV_IMPLEMENT_CLASS( XCLASSNAME, XCLASSID )	\
		class XCLASSNAME##RegisterClass : public SVClassRegisterClass \
		{ \
			public: \
				XCLASSNAME##RegisterClass() \
				{ \
				SVClassRegisterListClass::Instance().Add( this ); \
					OutputDebugString( "SV_IMPLEMENT: " #XCLASSNAME ", " ); \
					OutputDebugString( SVGUID( XCLASSID ).ToString().ToString() ); \
					OutputDebugString( _T( "\n" ) ); \
				} \
				virtual SVGUID GetClassID() { return XCLASSID; } \
				virtual SVObjectClass* Construct() { return new ( XCLASSNAME ); } \
				virtual LPCTSTR GetClassName()	{ return #XCLASSNAME; } \
		};	\
		XCLASSNAME##RegisterClass The##XCLASSNAME##Register; \
		SVGUID XCLASSNAME::GetClassID() { return XCLASSID; } \
		LPCTSTR XCLASSNAME::GetClassName() { return #XCLASSNAME; }


//******************************************************************************
//* CONSTANT(S):
//******************************************************************************
struct SVObjectInfoStruct;
struct SVObjectTypeInfoStruct;

extern const GUID  SVInvalidGUID;
extern const SVObjectInfoStruct		SVInvalidObjectInfo;
extern const SVObjectTypeInfoStruct	SVInvalidObjectTypeInfo;

// GUID for the SVObjectImageClass
extern /*const*/ GUID SVObjectImageClassGuid;

enum SVMessageFlags
{
	SVMFUnknown = 0x00000000,
	SVMFSetDefaultInputs = 0x00000001,
	SVMFResetObject = 0x00000002,
	SVMFResetInspection = 0x00000004,
};

////////////////////////////////////////////////////////////////////////////////
//	Global Object Defines
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Message Queueing
const UINT SVM_NOTIFY_ALL                           = 0x00000000;	// Default for all Messages
const UINT SVM_NOTIFY_FIRST_RESPONDING              = 0x01000000;
const UINT SVM_NOTIFY_ONLY_THIS                     = 0x02000000;
const UINT SVM_NOTIFY_ONLY_CHILDREN                 = 0x04000000;
const UINT SVM_NOTIFY_FRIENDS                       = 0x08000000;
const UINT SVM_NOTIFY_ONLY_FRIENDS                  = 0x18000000;

const UINT SVM_NOTIFY_BACKWARDS                     = 0x80000000;

////////////////////////////////////////////////////////////////////////////////
// Message Result Types
const UINT SVM_SVMR_RESULT                          = 0x00100000;
const UINT SVM_POINTER_RESULT                       = ( 0x00200000 | SVM_NOTIFY_FIRST_RESPONDING );

////////////////////////////////////////////////////////////////////////////////
// Message Direction
const UINT SVM_PARENT_TO_CHILD                      = 0x00400000;
const UINT SVM_CHILD_TO_PARENT                      = 0x00800000;

////////////////////////////////////////////////////////////////////////////////
// Message Separator
const UINT SVM_PURE_MESSAGE	                        = 0x0000ffff;	// allow 16 bits for message qualifiers

////////////////////////////////////////////////////////////////////////////////
// Message Response ( Result: SVM_SVMR_RESULT )
const UINT SVMR_NOT_PROCESSED                       = 0x00000000;	// Default for all messages, if not processed! 
															// Also valid for SVM_POINTER_RESULT messages, 
															// if they are not processed or not succesfully!!!
															// NEVER CHANGE THIS VALUE!!!
const UINT SVMR_SUCCESS                             = 0x80000000;
const UINT SVMR_NO_SUCCESS                          = 0xffffffff;

////////////////////////////////////////////////////////////////////////////////
// Message Types
const UINT SVM_GET_MESSAGE                          = 0x00010000;
const UINT SVM_SET_MESSAGE                          = 0x00020000;
const UINT SVM_CREATE_CLOSE_MESSAGE                 = 0x00040000;
const UINT SVM_UPDATE_MESSAGE                       = 0x00080000;


////////////////////////////////////////////////////////////////////////////////
// Note: Message IDs are limited to the lower 16 bits
// and are sequential numbers not bit settings and each message Id must be unique

////////////////////////////////////////////////////////////////////////////////
// Get Messages...
// ...returns SVObjectClass* or SVMR_NOT_PROCESSED

// ...use second message parameter ( DwMessageValue ) to specify the object requesting
// Note: if second parameter is specified - stop when requesting object encountered.
// ...use third message parameter ( DwMessageContext ) to specify the objectTypeInfoStruct! 
// ( SVObjectTypeInfoStruct->ObjectType => SVObjectTypeEnum && SVObjectTypeInfoStruct->SubType => SVObjectSubTypeEnum )
// ...returns SVObjectClass* or SVMR_NOT_PROCESSED
const UINT SVMSGID_GETFIRST_OBJECT                  = 0x00000100;
const UINT SVM_GETFIRST_OBJECT                      = ( SVMSGID_GETFIRST_OBJECT | SVM_POINTER_RESULT | SVM_GET_MESSAGE | SVM_NOTIFY_FIRST_RESPONDING | SVM_NOTIFY_FRIENDS );

/*
// ...use result of SVM_GETFIRST_OBJECT as second message parameter ( DwMessageValue ) to find next specified object!
// ...use third message parameter ( DwMessageContext ) to specify the object!
// ( SVObjectTypeInfoStruct->ObjectType => SVObjectTypeEnum && SVObjectTypeInfoStruct->objectType => SVObjectSubTypeEnum )
// ...returns SVObjectClass* or SVMR_NOT_PROCESSED
const UINT SVMSGID_GETNEXT_OBJECT                   = 0x00000101;
const UINT SVM_GETNEXT_OBJECT                       = ( SVMSGID_GETNEXT_OBJECT | SVM_POINTER_RESULT | SVM_GET_MESSAGE );
*/

// ...use a LPCTSTR as second message parameter ( DwMessageValue ) to specify the name
// ...returns SVObjectClass* or SVMR_NOT_PROCESSED
const UINT SVMSGID_GET_OBJECT_BY_NAME               = 0x00000102;
const UINT SVM_GET_OBJECT_BY_NAME                   = ( SVMSGID_GET_OBJECT_BY_NAME | SVM_POINTER_RESULT | SVM_GET_MESSAGE | SVM_NOTIFY_ONLY_THIS );

// ...returns SVImageInfoClass* or SVMR_NOT_PROCESSED
const UINT SVMSGID_GETFIRST_IMAGE_INFO              = 0x00000103;
const UINT SVM_GETFIRST_IMAGE_INFO                  = ( SVMSGID_GETFIRST_IMAGE_INFO | SVM_POINTER_RESULT | SVM_GET_MESSAGE );

// ... use second message parameter ( DwMessageValue ) to specify a SVClassInfoStructListClass to be filled
// ...use third message parameter ( DwMessageContext ) to specify the objectTypeInfo! 
// ( SVObjectTypeInfoStruct->ObjectType => SVObjectTypeEnum && SVObjectTypeInfoStruct->SubType => SVObjectSubTypeEnum )
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_GETAVAILABLE_OBJECTS             = 0x00000104;
const UINT SVM_GETAVAILABLE_OBJECTS                 = ( SVMSGID_GETAVAILABLE_OBJECTS | SVM_SVMR_RESULT | SVM_GET_MESSAGE );

// ... use second message parameter ( DwMessageValue ) to pass a SVInputInfoListClass Pointer as 
//		the interface list to fill.
// If you only want the input interface of this particular object ( without regarding friend or child interfaces )
//	use message queueing specifier SVM_NOTIFY_ONLY_THIS which can be 'or-ed' to the Message ID.
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_GET_INPUT_INTERFACE              = 0x00000105;
const UINT SVM_GET_INPUT_INTERFACE                  = ( SVMSGID_GET_INPUT_INTERFACE | SVM_SVMR_RESULT | SVM_GET_MESSAGE );

const UINT SVMSGID_GET_IMAGE_BAND0_OBJECT           = 0x00000106;
const UINT SVM_GET_IMAGE_BAND0_OBJECT               = ( SVMSGID_GET_IMAGE_BAND0_OBJECT | SVM_POINTER_RESULT | SVM_GET_MESSAGE);


////////////////////////////////////////////////////////////////////////////////
// Set Messages...
// ...use second message parameter ( DwMessageValue ) as SVObjectClass*
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
//const UINT SVM_SET_OBJECT							= ( 0x00000100 | SVM_SVMR_RESULT | SVM_SET_MESSAGE | SVM_NOTIFY_ONLY_THIS )

// ...use second message parameter ( DwMessageValue ) as objectID ( GUID* )
// ...use third message parameter ( DwMessageContext ) as SVObjectClass*
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_REPLACE_OBJECT                   = 0x00000500;
const UINT SVM_REPLACE_OBJECT                       = ( SVMSGID_REPLACE_OBJECT | SVM_SVMR_RESULT | SVM_SET_MESSAGE | SVM_PARENT_TO_CHILD | SVM_NOTIFY_ONLY_THIS );

// ...use second message parameter ( DwMessageValue ) as objectID ( GUID* )
// ...use third message parameter ( DwMessageContext ) as embeddedID ( GUID* ) 
// ...returns pointer to embedded SVObjectClass
const UINT SVMSGID_OVERWRITE_OBJECT                 = 0x00000501;
const UINT SVM_OVERWRITE_OBJECT                     = ( SVMSGID_OVERWRITE_OBJECT  | SVM_POINTER_RESULT | SVM_SET_MESSAGE | SVM_PARENT_TO_CHILD | SVM_NOTIFY_ONLY_THIS );

// SEJ July 8,1999 - New Message
// ...use second message parameter ( DwMessageValue ) as objectID ( GUID* )
// ...use third message parameter ( DwMessageContext ) as SVObjectScriptDataObjectStruct*
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_SET_OBJECT_VALUE                 = 0x00000600;
const UINT SVM_SET_OBJECT_VALUE                     = ( SVMSGID_SET_OBJECT_VALUE | SVM_SVMR_RESULT | SVM_SET_MESSAGE | SVM_NOTIFY_ONLY_THIS );

////////////////////////////////////////////////////////////////////////////////
// Create, Open, Close Messages...
// ...use second message parameter ( DwMessageValue ) as SVObjectLevelCreateStruct*
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_CREATE_ALL_OBJECTS               = 0x00000700;
//const UINT SVM_CREATE_ALL_OBJECTS                 = ( SVMSGID_CREATE_ALL_OBJECTS | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_PARENT_TO_CHILD );
const UINT SVM_CREATE_ALL_OBJECTS                   = ( SVMSGID_CREATE_ALL_OBJECTS | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_PARENT_TO_CHILD | SVM_NOTIFY_ONLY_CHILDREN );

// Send this message to the object owner to create an object.
// If the owner object is not created yet, it returns SVMR_NOT_PROCESSED.
// Otherwise the owner object sends SVM_CREATE_ALL_OBJECTS to the child object
// and returns the result of this message.
// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_CREATE_CHILD_OBJECT              = 0x00000701;
const UINT SVM_CREATE_CHILD_OBJECT                  = ( SVMSGID_CREATE_CHILD_OBJECT | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_NOTIFY_ONLY_THIS );

// ...use second message parameter ( DwMessageValue ) as SVObjectLevelCreateStruct*
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_CONNECT_ALL_OBJECTS               = 0x00000702;
const UINT SVM_CONNECT_ALL_OBJECTS                   = ( SVMSGID_CONNECT_ALL_OBJECTS | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_PARENT_TO_CHILD | SVM_NOTIFY_ONLY_CHILDREN );

// Send this message to the object owner to connect an object.
// If the owner object is not created yet, it returns SVMR_NOT_PROCESSED.
// Otherwise the owner object sends SVM_CONNECT_ALL_OBJECTS to the child object
// and returns the result of this message.
// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_CONNECT_CHILD_OBJECT              = 0x00000703;
const UINT SVM_CONNECT_CHILD_OBJECT                  = ( SVMSGID_CONNECT_CHILD_OBJECT | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_NOTIFY_ONLY_THIS );

// An object sends this message to the object which must be notfied
//	 that the object is just created
// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of 
//		the object which sends this message.
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_NOTIFY_ABOUT_CREATION            = 0x00000704;
const UINT SVM_NOTIFY_ABOUT_CREATION                = ( SVMSGID_NOTIFY_ABOUT_CREATION | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE );

// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_CONNECT_ALL_INPUTS               = 0x00000800;
const UINT SVM_CONNECT_ALL_INPUTS                   = ( SVMSGID_CONNECT_ALL_INPUTS | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_PARENT_TO_CHILD | SVM_NOTIFY_FRIENDS );

// Send this message to the object which needs to be connected
// ...use second message parameter ( DwMessageValue ) as pointer to InObjectInfo ( SVInObjectInfoStruct* )
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_CONNECT_OBJECT_INPUT             = 0x00000801;
const UINT SVM_CONNECT_OBJECT_INPUT	                = ( SVMSGID_CONNECT_OBJECT_INPUT | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_CHILD_TO_PARENT | SVM_NOTIFY_ONLY_THIS );

// Send this message to the object which needs to be disconnected
// ...use second message parameter ( DwMessageValue ) as pointer to InObjectInfo ( SVInObjectInfoStruct* )
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_DISCONNECT_OBJECT_INPUT          = 0x00000802;
const UINT SVM_DISCONNECT_OBJECT_INPUT              = ( SVMSGID_DISCONNECT_OBJECT_INPUT | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_CHILD_TO_PARENT | SVM_NOTIFY_ONLY_THIS );

// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the object which sends this message!
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_CLOSE_OBJECT                     = 0x00000900;
const UINT SVM_CLOSE_OBJECT                         = ( SVMSGID_CLOSE_OBJECT | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_NOTIFY_ONLY_THIS );

// SEJ Aug 6,1999 - New Message
// Send this message to the parent of the Object to be destroyed
// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
const UINT SVMSGID_DESTROY_CHILD_OBJECT             = 0x00000901;
const UINT SVM_DESTROY_CHILD_OBJECT                 = ( SVMSGID_DESTROY_CHILD_OBJECT | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_NOTIFY_ONLY_THIS);

// SEJ Sep 23,1999 - New Message
// Send this message to the owner of the Friend Object to be destroyed
// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the friend object
const UINT SVMSGID_DESTROY_FRIEND_OBJECT            = 0x00000902;
const UINT SVM_DESTROY_FRIEND_OBJECT                = ( SVMSGID_DESTROY_FRIEND_OBJECT | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_NOTIFY_ONLY_THIS);

// Send this message to the owner of the Analyzer to disconnect extra image(s) from the view(s)
// ...use second message parameter ( DwMessageValue ) as SVAnalyzerClass* owner of the image
const UINT SVMSGID_DISCONNECT_IMAGE_OBJECT          = 0x00000903;

////////////////////////////////////////////////////////////////////////////////
// Update Messages...

// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_RESET_ALL_OBJECTS                = 0x00001004;
const UINT SVM_RESET_ALL_OBJECTS                    = ( SVMSGID_RESET_ALL_OBJECTS | SVM_SVMR_RESULT | SVM_UPDATE_MESSAGE | SVM_NOTIFY_FRIENDS );

// ...A Message sent when system goes 'offline' i.e. from running to not running.
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_GOING_OFFLINE                    = 0x00001005;
const UINT SVM_GOING_OFFLINE                        = ( SVMSGID_GOING_OFFLINE | SVM_SVMR_RESULT | SVM_UPDATE_MESSAGE | SVM_NOTIFY_ALL | SVM_PARENT_TO_CHILD );

// ...A Message sent when Toolset/Tools get renamed.
// ...use second message parameter ( DwMessageValue ) as pointer ot SVObjectClass w/new Name
// ...use third message parameter ( DwMessageContext ) as original Name ( LPCTSTR )
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_OBJECT_RENAMED                   = 0x00001006;
const UINT SVM_OBJECT_RENAMED                       = ( SVMSGID_OBJECT_RENAMED | SVM_SVMR_RESULT | SVM_UPDATE_MESSAGE | SVM_NOTIFY_ALL | SVM_NOTIFY_FRIENDS | SVM_PARENT_TO_CHILD );

// ...A Message sent when system goes 'online' i.e. from not running to running.
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_GOING_ONLINE                     = 0x00001007;
const UINT SVM_GOING_ONLINE                         = ( SVMSGID_GOING_ONLINE | SVM_SVMR_RESULT | SVM_UPDATE_MESSAGE | SVM_NOTIFY_ALL | SVM_PARENT_TO_CHILD );

// ...Is sent as an notification to an tool when the image source changes.
//
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_IMAGE_SOURCE_CHANGED             = 0x0000100B;
const UINT SVM_IMAGE_SOURCE_CHANGED                 = ( SVMSGID_IMAGE_SOURCE_CHANGED | SVM_SVMR_RESULT | SVM_UPDATE_MESSAGE | SVM_NOTIFY_ONLY_THIS );

// ...Is sent before the Toolset runs to copy all the result data forward
// ...This is necessary because of conditionally executing tools
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_COPY_RESULT_DATA_FORWARD         = 0x00001101;
const UINT SVM_COPY_RESULT_DATA_FORWARD             = ( SVMSGID_COPY_RESULT_DATA_FORWARD | SVM_SVMR_RESULT | SVM_SET_MESSAGE | SVM_PARENT_TO_CHILD | SVM_NOTIFY_ALL );

// ...Is sent before the Toolset runs to copy all the result images forward
// ...This is necessary because of conditionally executing tools
// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
const UINT SVMSGID_COPY_RESULT_IMAGES_FORWARD       = 0x00001102;
const UINT SVM_COPY_RESULT_IMAGES_FORWARD           = ( SVMSGID_COPY_RESULT_IMAGES_FORWARD | SVM_SVMR_RESULT | SVM_SET_MESSAGE | SVM_PARENT_TO_CHILD | SVM_NOTIFY_ALL );

/////////////////////////////////////////////////////////////////////////////
//
//
// Output Object attributes - 19 Aug 1999 - frb.
//
const UINT SV_NO_ATTRIBUTES              = 0x00000000;  // default
const UINT SV_VIEWABLE                   = 0x00000001;
const UINT SV_PUBLISHABLE                = 0x00000002;
const UINT SV_ARCHIVABLE                 = 0x00000004;
const UINT SV_SELECTABLE_FOR_EQUATION    = 0x00000008;
const UINT SV_EMBEDABLE                  = 0x00000010;
const UINT SV_SELECTABLE_FOR_STATISTICS  = 0x00000020;
const UINT SV_ARCHIVABLE_IMAGE           = 0x00000040;  // 11 Oct 1999 - frb.
const UINT SV_PRINTABLE                  = 0x00000080;
const UINT SV_REMOTELY_SETABLE           = 0x00000100;  // RY 2002 12 19
const UINT SV_SETABLE_ONLINE             = 0x00000200;
const UINT SV_EXTENT_OBJECT              = 0x00000400;
const UINT SV_HIDDEN                     = 0x00000800;  // EB/TB 2004 05 10
const UINT SV_PUBLISH_RESULT_IMAGE       = 0x00001000;
const UINT SV_CH_CONDITIONAL             = 0x00002000;  // conditional history
const UINT SV_CH_IMAGE                   = 0x00004000;  // conditional history
const UINT SV_CH_VALUE                   = 0x00008000;  // conditional history
const UINT SV_DD_VALUE                   = 0x00010000;  // Selected for DataDefinition Value
const UINT SV_DD_IMAGE                   = 0x00020000;  // Selected for DataDefinition Image

// this was the mis-named SV_ALL_ATTRIBUTES
const UINT SV_DEFAULT_ATTRIBUTES  = (
                                          SV_VIEWABLE
                                        | SV_PUBLISHABLE
                                        | SV_ARCHIVABLE
                                        | SV_SELECTABLE_FOR_EQUATION
                                        | SV_EMBEDABLE
                                        | SV_SELECTABLE_FOR_STATISTICS
                                        | SV_PRINTABLE
                                        | SV_ARCHIVABLE_IMAGE
                                );

const UINT SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES = 
                                (
                                          SV_VIEWABLE
                                        | SV_PUBLISHABLE
                                        | SV_ARCHIVABLE
                                        | SV_SELECTABLE_FOR_EQUATION
                                        | SV_EMBEDABLE
                                        | SV_SELECTABLE_FOR_STATISTICS
                                        | SV_PRINTABLE
										| SV_CH_VALUE
										| SV_CH_CONDITIONAL
										| SV_DD_VALUE
                                );

const UINT SV_DEFAULT_IMAGE_OBJECT_ATTRIBUTES =
                                (
                                          SV_ARCHIVABLE_IMAGE
										| SV_PUBLISH_RESULT_IMAGE
										| SV_CH_IMAGE
										| SV_DD_IMAGE
								);


//******************************************************************************
//* TYPE(S):
//******************************************************************************

/////////////////////////////////////////////////////////////////////////////
// SVHINDEX
typedef UINT SVHINDEX;
	// Not Valid Handle:	NULL
	// First Valid Handle:	0x1

/////////////////////////////////////////////////////////////////////////////
// SVProductCountType
typedef DWORD SVProductCountType;
	// Not Valid Product:	NULL
	// First Valid Product: 0x1



//******************************************************************************
//* FUNCTION DECLARATION(S):
//******************************************************************************

LONG_PTR SVSendMessage( SVObjectClass* PObject, DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext );
LONG_PTR SVSendMessage( const GUID& RUniqueObjectID, DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext );

//******************************************************************************
//* CONSTANT VARIABLE(S):
//******************************************************************************

//******************************************************************************
//* VARIABLE(S):
//******************************************************************************


#endif  // SVOBJECTLIBRARY_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\SVObjectLibrary.h_v  $
 * 
 *    Rev 1.1   01 Feb 2014 10:09:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 16:51:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   15 Jan 2013 10:04:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  807
 * SCR Title:  Allow Shift Tool to use Source Image Extents
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed unused SVString include.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   05 Dec 2012 11:46:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   inluced "SVUtilityLibrary/SVString.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   02 Jul 2012 16:02:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Jun 2012 17:13:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   07 Dec 2010 15:51:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   29 Jun 2010 13:58:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to change object validity test.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Jun 2010 08:29:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definitions and redundunt containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   03 Sep 2009 09:11:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   11 Dec 2008 14:36:26   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  629
 * SCR Title:  Add Histogram analyzer
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added SVMSGID_DISCONNECT_IMAGE_OBJECT const
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   07 Mar 2008 08:19:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Attributes for Image Transfer Definition and Data Transfer Definition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 Jan 2006 15:06:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SV_ALL_ATTRIBUTES was renamed to SV_DEFAULT_ATTRIBUTES
 * added SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES
 * added SV_DEFAULT_IMAGE_OBJECT_ATTRIBUTES
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Jan 2006 09:10:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Conditional History object attributes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Nov 2005 14:43:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SV_PUBLISH_RESULT_IMAGE
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Nov 2005 14:43:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SV_PUBLISH_RESULT_IMAGE
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Feb 2005 11:28:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all extent related messages.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 15:06:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First Iteration of file in SVObjectLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   12 Jul 2004 13:04:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented SV_HIDDEN
 * fixed SV_GET_KIND_OF
 * added copy constructor / operator = to SVInObjectInfoStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   12 Jul 2004 13:01:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  428
 * SCR Title:  Improve Load Configuration time
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented GetCompleteObjectName optimization
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   04 Dec 2003 12:59:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  397
 * SCR Title:  Blob Analyzer - Allow Max number of blobs to be set.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed SVInObjectInfoStruct to have a const copy constructor
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   22 Apr 2003 11:34:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   30 Jan 2003 15:08:22   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added SetImageDepth and GetImageDepth methods to class.  Updated constructor to initialize mlImageDepth attribute.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   22 Jan 2003 13:42:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bugs with SetInputs and SetToolParameterList from the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   10 Jan 2003 17:49:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   forgot the = sign for the const UINT message defines
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   10 Jan 2003 16:54:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   forgot to add ; to constants that are now const UINT instead of #define
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   10 Jan 2003 15:45:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new function VerifyImageForOverlay to determine which object has overlays on which image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   03 Jan 2003 15:28:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new attributes.  SV_SETABLE_ONLINE, SV_SETABLE_REMOTELY, SV_EXTENT_OBJECT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   17 Dec 2002 15:20:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   BOOL GetChildObjectByName( LPCTSTR tszName, SVObjectClass** ppObject );
 * 
 * Changed #define to const <typename> declarations. (So the values can be seen in the debugger)
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   09 Dec 2002 19:44:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added SV_PRINTABLE flag to attribute flages.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   20 Nov 2002 09:11:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226, 272
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed TheClassRegister -> TheSVClassRegister
 * Added constants
 * SVInObjectInfoStruct: Added strName member var
 * SVIPDocLevelCreateStruct: removed references to SVObjectInfoStruct
 * Removed references to  SVPQDataLinkInfo
 * SVObjectClass: Added friend classes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   04 Apr 2002 10:09:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  286
 * SCR Title:  Upgrade SVObserver to Version 3.37 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added debug information variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   02 Oct 2001 15:05:50   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  241
 * SCR Title:  Color SVIM:  Load image tool output not available in window tool as input.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added message definitions for 
 * SVMSGID_GET_IMAGE_BAND0_OBJECT & 
 * SVM_GET_IMAGE_BAND0_OBJECT.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   19 Apr 2001 14:48:08   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Changes due to addition of the SVFileNameObjectClass and new GUID for the class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   06 Sep 2000 16:31:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   14 Jun 2000 16:20:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  159
 * SCR Title:  Incorrect placement of Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added new message definition for SVM_IMAGE_SOURCE_CHANGED.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   19 May 2000 16:18:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  148
 * SCR Title:  Formulas not updated when Tools are renamed.
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised the SVM_OBJECT_RENAMED message so that it
 * will also be routed to friend objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   11 May 2000 08:56:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  139
 * SCR Title:  Math Tool Invalid State
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SVM_CREATE_ALL_CLOSED_OBJECTS to
 * also route this message to friend objects.
 * 
 * Added IsDescendantOfType method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   26 Apr 2000 16:13:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetImageInputContextGuid method.
 * Added new message 
 * SVM_SET_SHADOWED_ABSEXTENT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   23 Mar 2000 14:03:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  117
 * SCR Title:  Bug Fixes for Integration of conditional Toolset/Tool Drawing.
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Introduced new SetDisabled() member function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   03 Mar 2000 16:00:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fixed SVM_RESIZING Message definition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   03 Mar 2000 12:24:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SVM_GET_ABSOLUTE_EXTENT, 
 * SVM_GET_EXTENT Message Comments .
 * Added new Message SVM_SET_SHADOWED_EXTENT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   Mar 01 2000 12:41:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to make checkDrawAllowed(...) public.
 * Added new messages:
 * SVM_MOVE, SVM_LT_SIZE, SVM_RT_SIZE, SVM_LB_SIZE,
 * SVM_RB_SIZE, SVM_L_SIZE, SVM_R_SIZE, SVM_T_SIZE, SVM_B_SIZE
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   25 Feb 2000 16:07:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added new method GetTransform.
 * Added new message SVM_GET_CONTEXT_EXTENT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   21 Feb 2000 15:37:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added comments for Move/Size messages to detail
 * parameters.
 * Added checkDrawAllowed method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   Feb 21 2000 10:45:06   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  109
 * SCR Title:  Freeze on Reject for v3.1
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Versioning to v3.10 Beta 2.
 * Add new Macro for Type Safe Casting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   16 Feb 2000 17:00:38   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Modified Draw functions to use the DrawContext structure
 * and removed unused methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   07 Feb 2000 20:47:56   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Added new Object function GetObjectIcon() which can be overwritten and be used as generic 'get icon func' for any kind of list or tree icon.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:18:40   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   Oct 29 1999 17:59:04   steve
 * 
 *    Rev 1.35   18 Oct 1999 17:36:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Corrected description of SVMSGID_OBJECT_RENAMED
 * parameter usage
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   Oct 18 1999 11:53:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   RO - Bug fix - Added DataLinkID scripting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   14 Oct 1999 17:00:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added HasDependents method to SVOutputInfoListClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   Oct 13 1999 15:14:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Roberts bug fixes for Beta 10.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   Oct 12 1999 08:16:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  44
 * SCR Title:  Version 3.00 Beta 9 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required to change v3.00 Beta 8 to v3.00 Beta 9.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   Oct 07 1999 15:37:50   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Go Online and Go Offline messages.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   06 Oct 1999 15:42:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Corrected problem with SV_ALL_ATRIBUTES
 * ( added parentheses)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   05 Oct 1999 17:20:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Attribute flag for Statistics
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   Oct 04 1999 09:52:02   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fix for lock while going into run mode and Point scripting fix.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   Sep 27 1999 19:37:28   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed RESET_ALL_OBJECTS to support friends.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   Sep 24 1999 09:49:04   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  4
 * SCR Title:  Add "Load image" option
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added Reset_All_Message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   24 Sep 1999 08:50:56   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed space3s from SV_VIEWABLE|SV_PUBLISHABLE|SV_ARCHIVABLE|SV_SELECTABLE_FOR_EQUATION|SV_EMBEDABLE of #define SV_ALL_ATTRIBUTES.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   23 Sep 1999 16:25:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added message SVM_DESTROY_FRIEND_OBJECT.
 * Revised DestroyFriends to send 
 * SVM_DESTROY_FRIEND_OBJECT instead of
 * SVM_DESTROY_CHILD_OBJECTS
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   Sep 23 1999 13:53:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  37
 * SCR Title:  Version 3.00 Beta 3 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   New version specific changes to Serialize methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   22 Sep 1999 18:35:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to fix restoration of friends from script
 * (Added method to adjust friend aliases)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   Sep 20 1999 15:36:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced new broadcast message router.
 * Added new messages: Create_All_Closed_Objects.
 *      Notify_About_Creation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   Sep 16 1999 12:38:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added Product Key Struct and other from SVPPQ.h.
 * Added GetAncestor(ObjectType) and IsDescendantOf( PAncestorObject)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   Sep 13 1999 13:29:14   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added message to get input interface list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   Sep 10 1999 17:37:16   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed SetObjectOwner() to virtual.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   04 Sep 1999 17:24:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to save and restore uObjectAttributesAllowed and
 * uObjectAttributesSet via ObjectScript.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   02 Sep 1999 08:44:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Add Message Qualifiers to better control routing of messages.
 * Add new messsages for drawing/figures.
 * Fixed Bug in CloseObject (Friends recurrsion)
 * Add published attribute and defaults on construction of SVObjectClass..
 * Added GetObjectColor and GetObjectState
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   30 Aug 1999 19:31:42   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Introduced Input Interface Handling.
 * ( GETFIRST - Message Handler )
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   30 Aug 1999 19:15:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SV_IS_KIND_OF not to call SV_VALIDATE_OBJECT.
 * Revised SVM_CREATE_ALL_OBJECTS to route to Children only.
 * Added Scripting support for Freinds Architecure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   27 Aug 1999 12:58:46   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Added SetupDialog() Function to SVObjectClass and Message.
 * This gives every derived class the possibility to support his own Setup Dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   27 Aug 1999 09:06:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Add capability to script freind links.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   Aug 25 1999 22:50:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  1
 * SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised Validate method to be OnValidate method.
 *  ( OnValidate - local scope validation).
 * Added Validate method (Routes to all owned objects)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Aug 25 1999 16:42:04   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added SV_ALL_ATTRIBUTES define.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   25 Aug 1999 11:36:12   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Added SVObjectInfoArrayClass.
 * Added Friend Mechanism.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   Aug 25 1999 10:47:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Correct check in file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Aug 20 1999 19:42:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Added attributes for output (results) picker
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Aug 20 1999 16:54:22   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  14
 * SCR Title:  Integrate luminance analyzer into window tool.
 * Checked in by:  James A. Brown
 * Change Description:  
 *   Added SVErrorClass  msvError to SVObjectClass.
 * 
 * Changed header comment block to new standard which includes Tracker keywords.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
