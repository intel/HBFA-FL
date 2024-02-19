/** @file
  The UEFI Library provides functions and macros that simplify the development of
  UEFI Drivers and UEFI Applications.  These functions and macros help manage EFI
  events, build simple locks utilizing EFI Task Priority Levels (TPLs), install
  EFI Driver Model related protocols, manage Unicode string tables for UEFI Drivers,
  and print messages on the console output and standard error devices.

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#include "UefiLibInternal.h"

/**
  Compare whether two names of languages are identical.

  @param  Language1 Name of language 1.
  @param  Language2 Name of language 2.

  @retval TRUE      Language 1 and language 2 are the same.
  @retval FALSE     Language 1 and language 2 are not the same.

**/
BOOLEAN
CompareIso639LanguageCode (
  IN CONST CHAR8  *Language1,
  IN CONST CHAR8  *Language2
  )
{
  UINT32  Name1;
  UINT32  Name2;

  Name1 = ReadUnaligned24 ((CONST UINT32 *) Language1);
  Name2 = ReadUnaligned24 ((CONST UINT32 *) Language2);

  return (BOOLEAN) (Name1 == Name2);
}

/**
  This function looks up a Unicode string in UnicodeStringTable.

  If Language is a member of SupportedLanguages and a Unicode string is found in
  UnicodeStringTable that matches the language code specified by Language, then
  it is returned in UnicodeString.

  @param  Language             A pointer to an ASCII string containing the ISO 639-2 or the
                               RFC 4646 language code for the Unicode string to look up and
                               return. If Iso639Language is TRUE, then this ASCII string is
                               not assumed to be Null-terminated, and only the first three
                               characters are used. If Iso639Language is FALSE, then this ASCII
                               string must be Null-terminated.
  @param  SupportedLanguages   A pointer to a Null-terminated ASCII string that contains a
                               set of ISO 639-2 or RFC 4646 language codes that the Unicode
                               string table supports.  Language must be a member of this set.
                               If Iso639Language is TRUE, then this string contains one or more
                               ISO 639-2 language codes with no separator characters. If Iso639Language
                               is FALSE, then is string contains one or more RFC 4646 language
                               codes separated by ';'.
  @param  UnicodeStringTable   A pointer to the table of Unicode strings. Type EFI_UNICODE_STRING_TABLE
                               is defined in "Related Definitions".
  @param  UnicodeString        A pointer to the Null-terminated Unicode string from UnicodeStringTable
                               that matches the language specified by Language.
  @param  Iso639Language       Specifies the supported language code format. If it is TRUE, then
                               Language and SupportedLanguages follow ISO 639-2 language code format.
                               Otherwise, they follow RFC 4646 language code format.


  @retval  EFI_SUCCESS            The Unicode string that matches the language specified by Language
                                  was found in the table of Unicode strings UnicodeStringTable, and
                                  it was returned in UnicodeString.
  @retval  EFI_INVALID_PARAMETER  Language is NULL.
  @retval  EFI_INVALID_PARAMETER  UnicodeString is NULL.
  @retval  EFI_UNSUPPORTED        SupportedLanguages is NULL.
  @retval  EFI_UNSUPPORTED        UnicodeStringTable is NULL.
  @retval  EFI_UNSUPPORTED        The language specified by Language is not a member of SupportedLanguages.
  @retval  EFI_UNSUPPORTED        The language specified by Language is not supported by UnicodeStringTable.

**/
EFI_STATUS
EFIAPI
LookupUnicodeString2 (
  IN CONST CHAR8                     *Language,
  IN CONST CHAR8                     *SupportedLanguages,
  IN CONST EFI_UNICODE_STRING_TABLE  *UnicodeStringTable,
  OUT CHAR16                         **UnicodeString,
  IN BOOLEAN                         Iso639Language
  )
{
  BOOLEAN   Found;
  UINTN     Index;
  CHAR8     *LanguageString;

  //
  // Make sure the parameters are valid
  //
  if (Language == NULL || UnicodeString == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // If there are no supported languages, or the Unicode String Table is empty, then the
  // Unicode String specified by Language is not supported by this Unicode String Table
  //
  if (SupportedLanguages == NULL || UnicodeStringTable == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Make sure Language is in the set of Supported Languages
  //
  Found = FALSE;
  while (*SupportedLanguages != 0) {
    if (Iso639Language) {
      if (CompareIso639LanguageCode (Language, SupportedLanguages)) {
        Found = TRUE;
        break;
      }
      SupportedLanguages += 3;
    } else {
      for (Index = 0; SupportedLanguages[Index] != 0 && SupportedLanguages[Index] != ';'; Index++);
      if ((AsciiStrnCmp(SupportedLanguages, Language, Index) == 0) && (Language[Index] == 0)) {
        Found = TRUE;
        break;
      }
      SupportedLanguages += Index;
      for (; *SupportedLanguages != 0 && *SupportedLanguages == ';'; SupportedLanguages++);
    }
  }

  //
  // If Language is not a member of SupportedLanguages, then return EFI_UNSUPPORTED
  //
  if (!Found) {
    return EFI_UNSUPPORTED;
  }

  //
  // Search the Unicode String Table for the matching Language specifier
  //
  while (UnicodeStringTable->Language != NULL) {
    LanguageString = UnicodeStringTable->Language;
    while (0 != *LanguageString) {
      for (Index = 0 ;LanguageString[Index] != 0 && LanguageString[Index] != ';'; Index++);
      if (AsciiStrnCmp(LanguageString, Language, Index) == 0) {
        *UnicodeString = UnicodeStringTable->UnicodeString;
        return EFI_SUCCESS;
      }
      LanguageString += Index;
      for (Index = 0 ;LanguageString[Index] != 0 && LanguageString[Index] == ';'; Index++);
    }
    UnicodeStringTable++;
  }

  return EFI_UNSUPPORTED;
}
