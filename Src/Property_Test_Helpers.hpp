// *****************************************************************************
// File:			Property_Test_Helper.hpp
// Description:		Property_Test add-on helper macros and functions
// Project:			APITools/Property_Test
// Namespace:		-
// Contact person:	CSAT
// *****************************************************************************

#if !defined (HELPERS_HPP)
#define	HELPERS_HPP

#include "Property_Test.hpp"
#include "ApiCommon.h"
#include "DGModule.hpp"
#include "StringConversion.hpp"

// -----------------------------------------------------------------------------
// Helper macros
// -----------------------------------------------------------------------------

#if defined (ASSERT)
	#undef ASSERT
#endif
#define ASSERT(expression) PropertyTestHelpers::DebugAssert((expression), #expression, __FILE__, __LINE__, __FUNCTION__)


#if defined (ASSERT_NO_ERROR)
	#undef ASSERT_NO_ERROR
#endif
#define ASSERT_NO_ERROR(expression) PropertyTestHelpers::DebugAssertNoError((expression), #expression, __FILE__, __LINE__, __FUNCTION__)


// -----------------------------------------------------------------------------
// Helper functions
// -----------------------------------------------------------------------------

namespace PropertyTestHelpers 
{

API_Guid				RandomGuid ();

GS::UniString			GenearteUniqueName ();

API_PropertyGroup		CreateExamplePropertyGroup ();

GSErrCode				GetCommonExamplePropertyGroup (API_PropertyGroup& group);

API_PropertyDefinition	CreateExampleBoolPropertyDefinition (API_PropertyGroup group);

API_PropertyDefinition	CreateExampleIntPropertyDefinition (API_PropertyGroup group);

API_PropertyDefinition	CreateExampleStringListPropertyDefinition (API_PropertyGroup group);

API_PropertyDefinition	CreateExampleStringMultiEnumPropertyDefinition (API_PropertyGroup group);

GSErrCode				GetElemCategoryValue (const API_Guid& elemGuid, API_ElemCategoryValue& catValue);

GSErrCode				GetElemCategoryValueDefault (API_ElemTypeID typeId, API_ElemVariationID variationID, API_ElemCategoryValue& catValue);

GS::Array<API_Guid>		GetSelectedElements (bool assertIfNoSel = true);

GSErrCode				CallOnSelectedElem (GSErrCode (*function)(const API_Guid&), bool assertIfNoSel = true);

GS::UniString			ToString (const API_Variant& variant);

GS::UniString			ToString (const API_Property& property);

void					DebugAssert	(bool success, GS::UniString expression, const char* file, UInt32 line, const char* function);

void					DebugAssertNoError  (GSErrCode error, GS::UniString expression, const char* file, UInt32 line, const char* function);

}

bool operator== (const API_Variant& lhs, const API_Variant& rhs);

bool operator== (const API_SingleVariant& lhs, const API_SingleVariant& rhs);

bool operator== (const API_ListVariant& lhs, const API_ListVariant& rhs);

bool operator== (const API_SingleEnumerationVariant& lhs, const API_SingleEnumerationVariant& rhs);

bool operator== (const API_MultipleEnumerationVariant& lhs, const API_MultipleEnumerationVariant& rhs);

bool Equals (const API_PropertyValue& lhs, const API_PropertyValue& rhs, API_PropertyCollectionType collType);

bool operator== (const API_PropertyGroup& lhs, const API_PropertyGroup& rhs);

bool operator== (const API_ElemCategory& lhs, const API_ElemCategory& rhs);

bool operator== (const API_ElemCategoryValue& lhs, const API_ElemCategoryValue& rhs);

bool operator== (const API_PropertyDefinition& lhs, const API_PropertyDefinition& rhs);

bool operator== (const API_Property& lhs, const API_Property& rhs);

template <typename T>
bool operator!= (const T& lhs, const T& rhs)
{
	return !(lhs == rhs);
}

#endif