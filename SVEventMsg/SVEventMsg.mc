MessageIdTypeDef=DWORD

FacilityNames = (None = 0x000:FAC_NONE)

SeverityNames = (Success = 0x0:STATUS_SEVERITY_SUCCESS
                 Informational = 0x1:STATUS_SEVERITY_INFORMATIONAL
                 Warning = 0x2:STATUS_SEVERITY_WARNING
                 Error = 0x3:STATUS_SEVERITY_ERROR)

LanguageNames = (German		= 0x0407:German)
LanguageNames = (USEnglish	= 0x0409:USEnglish)
LanguageNames = (Spanish	= 0x040a:Spanish)
LanguageNames = (French		= 0x040c:French)
LanguageNames = (Italian	= 0x0410:Italian)
LanguageNames = (Portuguese	= 0x0416:Portuguese)
LanguageNames = (UKEnglish	= 0x0809:UKEnglish)

MessageId=0x00
SymbolicName=MSG_INFO_1
Severity=Informational
Facility=None
Language=USEnglish
%1
.

MessageId=0x01
SymbolicName=MSG_WARNING_1
Severity=Warning
Facility=None
Language=USEnglish
%1
.

MessageId=0x02
SymbolicName=MSG_ERROR_1
Severity=Error
Facility=None
Language=USEnglish
%1
.

MessageId=0x03
SymbolicName=MSG_SUCCESS_1
Severity=Success
Facility=None
Language=USEnglish
%1
.
