// *****************************************************************************
// File:			Property_Test_Helper.cpp
// Description:		Property_Test add-on helper macros and functions
// Project:			APITools/Property_Test
// Namespace:		-
// Contact person:	CSAT
// *****************************************************************************

#include "Property_Test_Helpers.hpp"

API_Guid PropertyTestHelpers::RandomGuid () {
	GS::Guid guid;
	guid.Generate ();
	return GSGuid2APIGuid (guid);
}


GS::UniString PropertyTestHelpers::GenearteUniqueName () 
{
	GS::Guid guid;
	guid.Generate ();
	return guid.ToUniString ();
}


API_PropertyGroup PropertyTestHelpers::CreateExamplePropertyGroup () 
{
	API_PropertyGroup group;
	group.guid = APINULLGuid;
	group.name = "Property_Test Add-On Group - " + PropertyTestHelpers::GenearteUniqueName ();
	return group;
}


static API_PropertyGroup CreateCommonExamplePropertyGroup () 
{
	API_PropertyGroup group;
	group.guid = APINULLGuid;
	group.name = "Property_Test Add-On Group";
	return group;
}


GSErrCode PropertyTestHelpers::GetCommonExamplePropertyGroup (API_PropertyGroup& outGroup)
{
	static API_PropertyGroup staticGroup = CreateCommonExamplePropertyGroup ();
	if (ACAPI_Property_GetPropertyGroup (staticGroup) == APIERR_BADID) { // if the group does not exist
		GS::Array<API_PropertyGroup> groups;
		GSErrCode error = ACAPI_Property_GetPropertyGroups (groups);
		if (error != NoError) {
			return error;
		}

		for (UInt32 i = 0; i < groups.GetSize (); ++i) {
			if (groups[i].name == staticGroup.name) {
				outGroup = staticGroup = groups[i];
				return NoError;
			}
		}

		error = ACAPI_Property_CreatePropertyGroup (staticGroup);
		if (error != NoError) {
			return error;
		}
	}

	outGroup = staticGroup;
	return NoError;
}


static API_PropertyDefinition CreateExamplePropertyDefinition (API_PropertyGroup group) 
{
	API_PropertyDefinition definition;
	definition.guid = APINULLGuid;
	definition.groupGuid = group.guid;
	definition.name = "Property_Test Add-On Definition - " + PropertyTestHelpers::GenearteUniqueName ();
	definition.description = "An example property definition.";
	return definition;
}


API_PropertyDefinition PropertyTestHelpers::CreateExampleBoolPropertyDefinition (API_PropertyGroup group) 
{
	API_PropertyDefinition definition = CreateExamplePropertyDefinition (group);
	definition.collectionType = API_PropertySingleCollectionType;
	definition.valueType = API_PropertyBooleanValueType;
	definition.defaultValue.singleVariant.variant.type = definition.valueType;
	definition.defaultValue.singleVariant.variant.boolValue = false;
	return definition;
}

API_PropertyDefinition PropertyTestHelpers::CreateExampleIntPropertyDefinition (API_PropertyGroup group)
{
	API_PropertyDefinition definition = CreateExamplePropertyDefinition (group);
	definition.collectionType = API_PropertySingleCollectionType;
	definition.valueType = API_PropertyIntegerValueType;
	definition.defaultValue.singleVariant.variant.type = definition.valueType;
	definition.defaultValue.singleVariant.variant.intValue = 0;
	return definition;
}


API_PropertyDefinition PropertyTestHelpers::CreateExampleStringListPropertyDefinition (API_PropertyGroup group)
{
	API_PropertyDefinition definition = CreateExamplePropertyDefinition (group);
	definition.collectionType = API_PropertyListCollectionType;
	definition.valueType = API_PropertyStringValueType;
	// default = empty list
	return definition;
}


API_PropertyDefinition PropertyTestHelpers::CreateExampleStringMultiEnumPropertyDefinition (API_PropertyGroup group)
{
	API_PropertyDefinition definition = CreateExamplePropertyDefinition (group);
	definition.collectionType = API_PropertyMultipleChoiceEnumerationCollectionType;
	definition.valueType = API_PropertyStringValueType;

	API_SingleEnumerationVariant variant;
	variant.guid = RandomGuid ();
	variant.variant.type = definition.valueType;
	variant.variant.uniStringValue = "Apple";
	definition.possibleEnumValues.Push (variant);

	// The default value will be apple
	definition.defaultValue.multipleEnumVariant.variants.Push (variant);

	variant.guid = RandomGuid ();
	variant.variant.uniStringValue = "Pear";
	definition.possibleEnumValues.Push (variant);

	variant.guid = RandomGuid ();
	variant.variant.uniStringValue = "Watermelon";
	definition.possibleEnumValues.Push (variant);

	return definition;
}


GSErrCode PropertyTestHelpers::GetElemCategoryValue (const API_Guid& elemGuid, API_ElemCategoryValue& catValue) 
{
	GS::Array<API_ElemCategory> categoryList;
	GSErrCode error = ACAPI_Database (APIDb_GetElementCategoriesID, &categoryList);
	if (error != NoError) {
		return error;
	}

	categoryList.Enumerate ([&] (const API_ElemCategory& category) {
		if (category.categoryID == API_ElemCategory_ElementClassification) {
			error = ACAPI_Element_GetCategoryValue (elemGuid, category, &catValue);
		}
	});

	return error;
}


GSErrCode PropertyTestHelpers::GetElemCategoryValueDefault (API_ElemTypeID typeId, API_ElemVariationID variationID, API_ElemCategoryValue& catValue) 
{
	GS::Array<API_ElemCategory> categoryList;
	GSErrCode error = ACAPI_Database (APIDb_GetElementCategoriesID, &categoryList);
	if (error != NoError) {
		return error;
	}

	categoryList.Enumerate ([&] (const API_ElemCategory& category) {
		if (category.categoryID == API_ElemCategory_ElementClassification) {
			error = ACAPI_Element_GetCategoryValueDefault (typeId, variationID, category, &catValue);
		}
	});

	return error;
}


GS::Array<API_Guid>	PropertyTestHelpers::GetSelectedElements (bool assertIfNoSel /* = true*/) 
{
	GSErrCode            err;
	API_SelectionInfo    selectionInfo;
	API_Neig             **selNeigs;

	err = ACAPI_Selection_Get (&selectionInfo, &selNeigs, true);
	BMKillHandle ((GSHandle *)&selectionInfo.marquee.coords);
	if (err == APIERR_NOSEL || selectionInfo.typeID == API_SelEmpty) {
		if (assertIfNoSel) {
			DGAlert (DG_ERROR, "Error", "Please select an element!", "", "Ok");
		}
	}

	if (err != NoError) {
		BMKillHandle ((GSHandle *)&selNeigs);
		return GS::Array<API_Guid>();
	}

	UInt32 nSel = BMGetHandleSize ((GSHandle)selNeigs) / sizeof (API_Neig);

	GS::Array<API_Guid> guidArray;
	for (UInt32 i = 0; i < nSel; ++i) {
		guidArray.Push ((*selNeigs)[i].guid);
	}

	BMKillHandle ((GSHandle *)&selNeigs);

	return guidArray;
}


GSErrCode PropertyTestHelpers::CallOnSelectedElem (GSErrCode (*function)(const API_Guid&), bool assertIfNoSel /* = true*/)
{
	GS::Array<API_Guid> guidArray = GetSelectedElements(assertIfNoSel);
	if (guidArray.GetSize () > 0) {
		return function (guidArray[0]);
	}

	return APIERR_NOSEL;
}


GS::UniString PropertyTestHelpers::ToString (const API_Variant& variant) 
{
	switch (variant.type) {
		case API_PropertyIntegerValueType: return GS::ValueToUniString (variant.intValue);
		case API_PropertyRealValueType: return GS::ValueToUniString (variant.doubleValue);
		case API_PropertyStringValueType: return variant.uniStringValue;
		case API_PropertyBooleanValueType: return GS::ValueToUniString (variant.boolValue);
		default: DBBREAK(); return "Invalid Value";
	}
}


GS::UniString PropertyTestHelpers::ToString (const API_Property& property) 
{
	GS::UniString string;
	string += property.definition.name;
	string += ": ";
	const API_PropertyValue* value;
	if (property.isDefault) {
		value = &property.definition.defaultValue;
	} else {
		value = &property.value;
	}
	switch (property.definition.collectionType) {
		case API_PropertySingleCollectionType: {
			string += ToString (value->singleVariant.variant);
		} break;
		case API_PropertyListCollectionType: {
			string += '[';
			for (UInt32 i = 0; i < value->listVariant.variants.GetSize (); i++) {
				string += ToString (value->listVariant.variants[i]);
				if (i != value->listVariant.variants.GetSize () - 1) {
					string += ", ";
				}
			}
			string += ']';
		} break;
		case API_PropertySingleChoiceEnumerationCollectionType: {
			string += ToString (value->singleEnumVariant.variant);
		} break;
		case API_PropertyMultipleChoiceEnumerationCollectionType: {
			string += '[';
			for (UInt32 i = 0; i < value->multipleEnumVariant.variants.GetSize (); i++) {
				string += ToString (value->multipleEnumVariant.variants[i].variant);
				if (i != value->multipleEnumVariant.variants.GetSize () - 1) {
					string += ", ";
				}
			}
			string += ']';
		} break;
		default: {
			DBBREAK();
			string = "Invalid value";
		}
	}
	return string;
}


void PropertyTestHelpers::DebugAssert (bool success, GS::UniString expression, const char* file, UInt32 line, const char* function)
{
	if (success) {
		return;
	}

	expression += " is false.";

#if defined (DEBUVERS)
	DBBreak (file, line, expression.ToCStr ().Get (), nullptr, function, nullptr);
#else
	UNUSED_PARAMETER (function);
	DGAlert (DG_ERROR, "Assertion",	expression,
			 "At: " + GS::UniString (file) + ":" + GS::ValueToUniString (line), "Ok");
#endif

	throw GS::Exception(expression);
}


void PropertyTestHelpers::DebugAssertNoError (GSErrCode error, GS::UniString expression, const char* file, UInt32 line, const char* function)
{
	if (error == NoError) {
		return;
	}

	expression += " returned with an error.";

#if defined (DEBUVERS)
	DBBreak (file, line, expression.ToCStr ().Get (), nullptr, function, nullptr);
#else
	UNUSED_PARAMETER (function);
	DGAlert (DG_ERROR, "Assertion",	expression,
			 "ErrorCode: " + GS::ValueToUniString (error) +
			 "\nAt: " + GS::UniString (file) + ":" + GS::ValueToUniString (line), "Ok");
#endif

	throw GS::Exception(expression);
}


bool operator== (const API_Variant& lhs, const API_Variant& rhs)
{
	if (lhs.type != rhs.type) {
		return false;
	}

	switch (lhs.type) {
		case API_PropertyIntegerValueType:
			return lhs.intValue == rhs.intValue;
		case API_PropertyRealValueType:
			return lhs.doubleValue == rhs.doubleValue;
		case API_PropertyStringValueType:
			return lhs.uniStringValue == rhs.uniStringValue;
		case API_PropertyBooleanValueType:
			return lhs.boolValue == rhs.boolValue;
		default:
			return false;
	}
}


bool operator== (const API_SingleVariant& lhs, const API_SingleVariant& rhs)
{
	return lhs.variant == rhs.variant;
}


bool operator== (const API_ListVariant& lhs, const API_ListVariant& rhs)
{
	return lhs.variants == rhs.variants;
}


bool operator== (const API_SingleEnumerationVariant& lhs, const API_SingleEnumerationVariant& rhs)
{
	return lhs.guid == rhs.guid && lhs.variant == rhs.variant;
}


bool operator== (const API_MultipleEnumerationVariant& lhs, const API_MultipleEnumerationVariant& rhs)
{
	return lhs.variants == rhs.variants;
}


bool Equals (const API_PropertyValue& lhs, const API_PropertyValue& rhs, API_PropertyCollectionType collType)
{
	switch (collType) {
		case API_PropertySingleCollectionType:
			return lhs.singleVariant == rhs.singleVariant;
		case API_PropertyListCollectionType:
			return lhs.listVariant == rhs.listVariant;
		case API_PropertySingleChoiceEnumerationCollectionType:
			return lhs.singleEnumVariant == rhs.singleEnumVariant;
		case API_PropertyMultipleChoiceEnumerationCollectionType:
			return lhs.multipleEnumVariant == rhs.multipleEnumVariant;
		default:
			return false;
	}
}


bool operator== (const API_PropertyGroup& lhs, const API_PropertyGroup& rhs)
{
	return lhs.guid == rhs.guid && 
		   lhs.name == rhs.name;
}


bool operator== (const API_ElemCategory& lhs, const API_ElemCategory& rhs)
{
	return lhs.guid == rhs.guid && 
		   lhs.categoryID == rhs.categoryID &&
		   GS::UniString (lhs.name) == GS::UniString (rhs.name);
}


bool operator== (const API_ElemCategoryValue& lhs, const API_ElemCategoryValue& rhs)
{
	return lhs.guid == rhs.guid && 
		   lhs.category == rhs.category &&
		   GS::UniString (lhs.name) == GS::UniString (rhs.name) &&
		   lhs.isVisibleOnUI == rhs.isVisibleOnUI &&
		   lhs.isAdditional == rhs.isAdditional &&
		   lhs.usableAsCriteria == rhs.usableAsCriteria;
}


bool operator== (const API_PropertyDefinition& lhs, const API_PropertyDefinition& rhs)
{
	return lhs.guid == rhs.guid &&
		   lhs.groupGuid == rhs.groupGuid && 
		   lhs.name == rhs.name &&
		   lhs.description == rhs.description &&
		   lhs.collectionType == rhs.collectionType &&
		   lhs.valueType == rhs.valueType &&
		   Equals (lhs.defaultValue, rhs.defaultValue, lhs.collectionType) &&
		   lhs.availability == rhs.availability &&
		   lhs.possibleEnumValues == rhs.possibleEnumValues;
}


bool operator== (const API_Property& lhs, const API_Property& rhs)
{
	if (lhs.definition != rhs.definition || lhs.isDefault != rhs.isDefault) {
		return false;
	}
	if (!lhs.isDefault) {
		return Equals (lhs.value, rhs.value, lhs.definition.collectionType);
	} else {
		return true;
	}
}
