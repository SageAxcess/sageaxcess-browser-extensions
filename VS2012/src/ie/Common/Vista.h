#pragma once

#include <atlstr.h>
#define _WTL_NO_CSTRING

namespace Vista
{
	const DWORD IL_NOTAPPLICABLE = 0;
	const DWORD IL_LOW = 1;
	const DWORD IL_MEDIUM = 2;
	const DWORD IL_HIGH = 3;

	typedef struct _TOKEN_MANDATORY_LABEL {
		SID_AND_ATTRIBUTES Label;
	} TOKEN_MANDATORY_LABEL, *PTOKEN_MANDATORY_LABEL;

	typedef enum _TOKEN_INFORMATION_CLASS {
		TokenUser = 1,
		TokenGroups,
		TokenPrivileges,
		TokenOwner,
		TokenPrimaryGroup,
		TokenDefaultDacl,
		TokenSource,
		TokenType,
		TokenImpersonationLevel,
		TokenStatistics,
		TokenRestrictedSids,
		TokenSessionId,
		TokenGroupsAndPrivileges,
		TokenSessionReference,
		TokenSandBoxInert,
		TokenAuditPolicy,
		TokenOrigin,
		TokenElevationType,
		TokenLinkedToken,
		TokenElevation,
		TokenHasRestrictions,
		TokenAccessInformation,
		TokenVirtualizationAllowed,
		TokenVirtualizationEnabled,
		TokenIntegrityLevel,
		TokenUIAccess,
		TokenMandatoryPolicy,
		TokenLogonSid,
		MaxTokenInfoClass  // MaxTokenInfoClass should always be the last enum
	} TOKEN_INFORMATION_CLASS, *PTOKEN_INFORMATION_CLASS;

#ifndef SECURITY_MANDATORY_LOW_RID
	const DWORD SECURITY_MANDATORY_LOW_RID = 0x00001000L;
	const DWORD SECURITY_MANDATORY_MEDIUM_RID = 0x00002000L;
	const DWORD SECURITY_MANDATORY_HIGH_RID = 0x00003000L;
#endif	// SECURITY_MANDATORY_LOW_RID

	BOOL IsVista();
	DWORD GetIntegrityLevel();
	PSID GetCurrentUserSid(PTOKEN_USER& usr);
	CString GetCurrentUserSidString();
};


