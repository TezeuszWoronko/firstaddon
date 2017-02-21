// *****************************************************************************
// File:			Property_Test.cpp
// Description:		Property_Test add-on test functions
// Project:			APITools/Property_Test
// Namespace:		-
// Contact person:	CSAT
// *****************************************************************************

// --- Includes ----------------------------------------------------------------

#include "Property_Test.hpp"
#include "Property_Test_Helpers.hpp"

// -----------------------------------------------------------------------------
// Test functions
// -----------------------------------------------------------------------------

/*--------------------------------------------------**
** Tests common use-cases involving property groups **
**--------------------------------------------------*/
static GSErrCode SimpleTestPropertyGroups ()
{
	// Define a property group
	API_PropertyGroup group = PropertyTestHelpers::CreateExamplePropertyGroup ();

	// Create it
	ASSERT_NO_ERROR (ACAPI_Property_CreatePropertyGroup (group)); 

	// The create operation generates a guid for the group
	ASSERT (group.guid != APINULLGuid);

	// If we list all groups
	GS::Array<API_PropertyGroup> groups;
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroups (groups));
	// Then the result array will contain the newly created one
	ASSERT (groups.Contains (group));

	API_PropertyGroup group2;
	group2.guid = group.guid;

	// We fill an API_PropertyGroup knowing only its guid
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroup (group2));
	ASSERT (group.name == group2.name);

	// The name of the group can be changed
	group.name = "Renamed Property_Test Add-On Group - " + PropertyTestHelpers::GenearteUniqueName ();
	ASSERT_NO_ERROR (ACAPI_Property_ChangePropertyGroup (group));

	ASSERT (group.name != group2.name);
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroup (group2));
	ASSERT (group.name == group2.name);

	// Property groups can be deleted
	ASSERT_NO_ERROR (ACAPI_Property_DeletePropertyGroup (group.guid));

	// Then, retrieval operation results in an error
	ASSERT (ACAPI_Property_GetPropertyGroup (group) == APIERR_BADID);

	return NoError;
}


/*-------------------------------------------------------------------------**
**		Tests rather unrealistic use-cases involving property groups	   **
** The purpose of this is to check the error handling of the API functions **
**-------------------------------------------------------------------------*/
static GSErrCode ThoroughTestPropertyGroups ()
{
	// Create a property group
	API_PropertyGroup group;
	group.guid = APINULLGuid;
	group.name = "Property_Test Add-On Group - "  + PropertyTestHelpers::GenearteUniqueName ();
	ASSERT_NO_ERROR (ACAPI_Property_CreatePropertyGroup (group));

	// Try to create another group with the same name
	API_PropertyGroup group2;
	group2.guid = APINULLGuid;
	group2.name = group.name;
	ASSERT (ACAPI_Property_CreatePropertyGroup (group2) == APIERR_NAMEALREADYUSED);
	ASSERT (group2.guid == APINULLGuid);

	// The created group can be retrieved
	GS::Array<API_PropertyGroup> groups;
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroups (groups));
	ASSERT (groups.Contains (group));
	API_PropertyGroup temp = group;
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroup (group));
	ASSERT (group == temp);

	// Can't get a group using an APINULLGuid id. 
	ASSERT (ACAPI_Property_GetPropertyGroup (group2) == APIERR_BADID);

	// "Changing" a group without modifying fields does not result in an error
	ASSERT_NO_ERROR (ACAPI_Property_ChangePropertyGroup (group));
	groups.Clear ();
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroups (groups));
	ASSERT (groups.Contains (group));
	temp = group;
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroup (group));
	ASSERT (group == temp);

	// We can rename a group
	group.name += " Renamed";
	ASSERT_NO_ERROR (ACAPI_Property_ChangePropertyGroup (group));
	temp = group;
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroup (group));
	ASSERT (group == temp);
	groups.Clear ();
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroups (groups));
	ASSERT (groups.Contains (group));

	// Now we can create group2, as group was renamed
	ASSERT_NO_ERROR (ACAPI_Property_CreatePropertyGroup (group2));
	temp = group2;
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroup (group2));
	ASSERT (group2 == temp);
	groups.Clear ();
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroups (groups));
	ASSERT (groups.Contains (group2));

	// Deleting group won't remove group2
	ASSERT_NO_ERROR (ACAPI_Property_DeletePropertyGroup (group.guid));
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroup (group2));
	groups.Clear ();
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroups (groups));
	ASSERT (groups.Contains (group2));

	// The deleted group can't be retrieved
	ASSERT (ACAPI_Property_GetPropertyGroup (group) == APIERR_BADID);
	groups.Clear ();
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroups (groups));
	ASSERT (!groups.Contains (group));

	// Deleting the same group twice results in an error
	ASSERT (ACAPI_Property_DeletePropertyGroup (group.guid) == APIERR_BADID);

	// The deleted group still can't be retrieved
	ASSERT (ACAPI_Property_GetPropertyGroup (group) == APIERR_BADID);
	groups.Clear ();
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroups (groups));
	ASSERT (!groups.Contains (group));

	// The not deleted group is still reachable
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroup (group2));
	groups.Clear ();
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroups (groups));
	ASSERT (groups.Contains (group2));

	// Delete group2 just to clean up
	ASSERT_NO_ERROR (ACAPI_Property_DeletePropertyGroup (group2.guid));
	ASSERT (ACAPI_Property_GetPropertyGroup (group2) == APIERR_BADID);
	groups.Clear ();
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyGroups (groups));
	ASSERT (!groups.Contains (group2));

	return NoError;
}


/*-------------------------------------------------------**
** Tests common use-cases involving property definitions **
**-------------------------------------------------------*/
static GSErrCode SimpleTestPropertyDefinitions ()
{
	// Define a property group
	API_PropertyGroup group;
	ASSERT_NO_ERROR (PropertyTestHelpers::GetCommonExamplePropertyGroup (group));

	// Define a property definition
	API_PropertyDefinition definition = PropertyTestHelpers::CreateExampleBoolPropertyDefinition (group);

	// Create it
	ASSERT_NO_ERROR (ACAPI_Property_CreatePropertyDefinition (definition));

	// The create operation generates a guid for the definition
	ASSERT (definition.guid != APINULLGuid);

	// If we list all definitions
	GS::Array<API_PropertyDefinition> definitions;
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyDefinitions (/*groupGuid = */ APINULLGuid, definitions));
	// Then the result array will contain the newly created one
	ASSERT (definitions.Contains (definition));

	// We can also list the definitions of only one group
	definitions.Clear ();
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyDefinitions (definition.groupGuid, definitions));
	ASSERT (definitions.Contains (definition));

	API_PropertyDefinition definition2;
	definition2.guid = definition.guid;

	// We can get a definition's data knowing only its guid
	ASSERT (definition != definition2);
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyDefinition (definition2));
	ASSERT (definition == definition2);

	// The parameters of the definition can be changed
	definition.name = "Renamed Property_Test Add-On Definition - " + PropertyTestHelpers::GenearteUniqueName ();
	definition.defaultValue.singleVariant.variant.boolValue = true;
	ASSERT_NO_ERROR (ACAPI_Property_ChangePropertyDefinition (definition));

	ASSERT (definition != definition2);
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyDefinition (definition2));
	ASSERT (definition == definition2);

	// Property definitions can be deleted
	ASSERT_NO_ERROR (ACAPI_Property_DeletePropertyDefinition (definition.guid));

	// Then, retrieval operation results in an error
	ASSERT (ACAPI_Property_GetPropertyDefinition (definition) == APIERR_BADID);

	return NoError;
}


/*-------------------------------------------------------------------------**
**		Tests rather unrealistic use-cases involving property groups	   **
** The purpose of this is to check the error handling of the API functions **
**-------------------------------------------------------------------------*/
static GSErrCode ThoroughTestPropertyDefinitions ()
{
	// Create a single-choice string property definition
	API_PropertyDefinition definition;
	definition.guid = APINULLGuid;
	definition.groupGuid = APINULLGuid; // this is not valid
	definition.name = "Property_Test Add-On Definition - " + PropertyTestHelpers::GenearteUniqueName ();
	definition.valueType = API_PropertyStringValueType;
	definition.collectionType = API_PropertySingleChoiceEnumerationCollectionType;

	// Set up the possibleEnumValues and the default value
	API_SingleEnumerationVariant variant;
	variant.guid = PropertyTestHelpers::RandomGuid ();
	variant.variant.type = definition.valueType;
	variant.variant.uniStringValue = "One";
	definition.possibleEnumValues.Push (variant);

	definition.defaultValue.singleEnumVariant = variant;

	variant.guid = PropertyTestHelpers::RandomGuid ();
	variant.variant.uniStringValue = "Two";
	definition.possibleEnumValues.Push (variant);

	variant.guid = PropertyTestHelpers::RandomGuid ();
	variant.variant.uniStringValue = "Three";
	definition.possibleEnumValues.Push (variant);

	variant.guid = PropertyTestHelpers::RandomGuid ();
	variant.variant.uniStringValue = "Four";
	definition.possibleEnumValues.Push (variant);

	variant.guid = PropertyTestHelpers::RandomGuid ();
	variant.variant.uniStringValue = "Five";
	definition.possibleEnumValues.Push (variant);

	// Creation results in an error, as the groupGuid is invalid 
	ASSERT (ACAPI_Property_CreatePropertyDefinition (definition) == APIERR_BADID);

	// Create with a valid group
	API_PropertyGroup group;
	ASSERT_NO_ERROR (PropertyTestHelpers::GetCommonExamplePropertyGroup (group));
	definition.groupGuid = group.guid;
	ASSERT_NO_ERROR (ACAPI_Property_CreatePropertyDefinition (definition));

	// Test the direct getter
	API_PropertyDefinition definition2;
	definition2.guid = definition.guid;
	ASSERT (definition != definition2);
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyDefinition (definition2));
	ASSERT (definition == definition2);

	// Test the getter that retrieves all of the definitions
	GS::Array<API_PropertyDefinition> definitions;
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyDefinitions (definition.groupGuid, definitions));
	ASSERT (definitions.Contains (definition));
	
	// We can't create a copy of an existing definition, because of the name collision
	ASSERT (ACAPI_Property_CreatePropertyDefinition (definition2) == APIERR_NAMEALREADYUSED);
	
	// But we can create it with a different name
	definition2.name += PropertyTestHelpers::GenearteUniqueName ();
	ASSERT_NO_ERROR (ACAPI_Property_CreatePropertyDefinition (definition2));

	// Create a new group
	API_PropertyGroup group2 = PropertyTestHelpers::CreateExamplePropertyGroup ();
	ASSERT (group.name != group2.name);
	ASSERT_NO_ERROR (ACAPI_Property_CreatePropertyGroup (group2));

	// Create a copy of an existing definition in the new group
	API_PropertyDefinition definition3 = definition2;
	ASSERT (ACAPI_Property_CreatePropertyDefinition (definition3) == APIERR_NAMEALREADYUSED);
	definition3.groupGuid = group2.guid;
	ASSERT_NO_ERROR (ACAPI_Property_CreatePropertyDefinition (definition3));

	// Move definiton2 to the new group, but with the first definition's name
	ASSERT_NO_ERROR (ACAPI_Property_ChangePropertyDefinition (definition2));
	definition2.name = definition.name;
	definition2.groupGuid = definition3.groupGuid;
	ASSERT_NO_ERROR (ACAPI_Property_ChangePropertyDefinition (definition2));

	// We can't change the name def3's
	definition2.name = definition3.name;
	ASSERT (ACAPI_Property_ChangePropertyDefinition (definition2) == APIERR_NAMEALREADYUSED);
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyDefinition (definition2));

	// We can't move to a group where the name is not unqiue
	definition2.groupGuid = definition.groupGuid;
	ASSERT (ACAPI_Property_ChangePropertyDefinition (definition2) == APIERR_NAMEALREADYUSED);
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyDefinition (definition2));

	// If a possible enum value has null guid, then the API function will generate one
	definition.possibleEnumValues[1].guid = APINULLGuid;
	ASSERT_NO_ERROR (ACAPI_Property_ChangePropertyDefinition (definition));
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyDefinition (definition));
	ASSERT (definition.possibleEnumValues[1].guid != APINULLGuid);

	// If the default value does not refer to a valid possible enum value, we get an error
	definition.possibleEnumValues[0].guid = APINULLGuid;
	ASSERT (ACAPI_Property_ChangePropertyDefinition (definition) == APIERR_BADVALUE);

	// But changing the default value fixes it of course
	definition.defaultValue.singleEnumVariant = definition.possibleEnumValues[2];
	ASSERT_NO_ERROR (ACAPI_Property_ChangePropertyDefinition (definition));
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyDefinition (definition));
	ASSERT (definition.possibleEnumValues[0].guid != APINULLGuid);

	// If more than one possible enum values have the same guid, 
	// then the property definition is considered inconsistent
	for (UInt32 i = 1; i < definition.possibleEnumValues.GetSize (); i++) {
		definition.possibleEnumValues[i].guid = definition.possibleEnumValues[0].guid;
	}
	ASSERT (ACAPI_Property_ChangePropertyDefinition (definition) == APIERR_BADPARS);
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyDefinition (definition));

	// Invalid availability causes APIERR_BADID
	API_ElemCategoryValue categoryValue;
	categoryValue.guid = APINULLGuid;
	definition.availability.Push (categoryValue);
	ASSERT (ACAPI_Property_ChangePropertyDefinition (definition) == APIERR_BADID);
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyDefinition (definition));

	// Deleting one property definition doesn't affect the others
	ASSERT_NO_ERROR (ACAPI_Property_DeletePropertyDefinition (definition3.guid));
	definitions.Clear ();
	ASSERT_NO_ERROR (ACAPI_Property_GetPropertyDefinitions (APINULLGuid, definitions));
	ASSERT (definitions.Contains (definition));
	ASSERT (definitions.Contains (definition2));

	// Double delete results in an error
	ASSERT_NO_ERROR (ACAPI_Property_DeletePropertyDefinition (definition.guid));
	ASSERT (ACAPI_Property_DeletePropertyDefinition (definition.guid) == APIERR_BADID);

	// Deleting a property group removes all the definition inside it too.
	ASSERT_NO_ERROR (ACAPI_Property_DeletePropertyGroup (group2.guid));
	ASSERT (ACAPI_Property_DeletePropertyDefinition (definition2.guid) == APIERR_BADID);

	return NoError;
}


/*---------------------------------------------------------------**
** Tests common use-cases involving properties on a general elem **
**---------------------------------------------------------------*/
static GSErrCode TestPropertiesOnElem (const API_Guid& elemGuid)
{
	// Create a property group
	API_PropertyGroup group;
	ASSERT_NO_ERROR (PropertyTestHelpers::GetCommonExamplePropertyGroup (group));

	// Create a property definition
	API_Property property;
	property.definition = PropertyTestHelpers::CreateExampleStringListPropertyDefinition (group);
	ASSERT_NO_ERROR (ACAPI_Property_CreatePropertyDefinition (property.definition));

	// Even though we have just created a definition, it is not available for the elem
	// so if we query which definitions apply to the elem, then the result array won't 
	// contain the newly created one
	GS::Array<API_PropertyDefinition> definitions;
	ASSERT_NO_ERROR (ACAPI_Element_GetPropertyDefinitions (elemGuid, definitions));
	ASSERT (!definitions.Contains (property.definition));

	// Make the definition apply for all of the elem's category values
	API_ElemCategoryValue catValue;
	ASSERT_NO_ERROR (PropertyTestHelpers::GetElemCategoryValue (elemGuid, catValue));
	property.definition.availability.Push (catValue);
	ASSERT_NO_ERROR (ACAPI_Property_ChangePropertyDefinition (property.definition));

	// Now the definition apply for the elem, and the query will reflect this
	definitions.Clear ();
	ASSERT_NO_ERROR (ACAPI_Element_GetPropertyDefinitions (elemGuid, definitions));
	ASSERT (definitions.Contains (property.definition));

	// The property's value is now the default value of the definition
	property.isDefault = true;

	// Specific properties can be retrieved using their guid
	API_Property propertyToRetrieve;
	propertyToRetrieve.definition.guid = property.definition.guid;

	GS::Array<API_Property> properties;
	properties.Push (propertyToRetrieve);

	// ACAPI_Element_GetProperties returns the default value
	ASSERT_NO_ERROR (ACAPI_Element_GetProperties (elemGuid, properties));
	ASSERT (properties[0] == property);

	// We can set a custom value to a property
	API_Variant variant;
	variant.type = API_PropertyStringValueType;
	for (int i = 0; i < 5; i++) {
		variant.uniStringValue = GS::ValueToUniString (1 << i);
		property.value.listVariant.variants.Push (variant);
	}
	
	property.isDefault = false;

	properties.Clear ();
	properties.Push (property);
	ASSERT_NO_ERROR (ACAPI_Element_SetProperties (elemGuid, properties));

	// ACAPI_Element_GetProperties returns the custom value now
	properties.Clear ();
	properties.Push (propertyToRetrieve);
	ASSERT_NO_ERROR (ACAPI_Element_GetProperties (elemGuid, properties));
	ASSERT (properties[0] == property);

	// Inconsistent property values cause APIERR_BADPARS
	properties[0].value.listVariant.variants[0].type = API_PropertyBooleanValueType;
	ASSERT (ACAPI_Element_SetProperties (elemGuid, properties) == APIERR_BADPARS);

	// Delete the property definition to clean up after the test
	ASSERT_NO_ERROR (ACAPI_Property_DeletePropertyDefinition (property.definition.guid));

	return NoError;
}


/*----------------------------------------------------------------**
** Tests common use-cases involving properties on an elem default **
**----------------------------------------------------------------*/
static GSErrCode TestPropertiesOnElemDefault (API_ElemTypeID typeId = API_ColumnID, API_ElemVariationID variationID = APIVarId_LabelColumn)
{
	// Create a property group
	API_PropertyGroup group;
	ASSERT_NO_ERROR (PropertyTestHelpers::GetCommonExamplePropertyGroup (group));

	// Create the property definition
	API_Property property;
	property.definition = PropertyTestHelpers::CreateExampleStringMultiEnumPropertyDefinition (group);
	property.isDefault = true;
	API_ElemCategoryValue catValue;
	ASSERT_NO_ERROR (PropertyTestHelpers::GetElemCategoryValueDefault (typeId, variationID, catValue));
	property.definition.availability.Push (catValue);
	ASSERT_NO_ERROR (ACAPI_Property_CreatePropertyDefinition (property.definition));

	API_Property propertyToRetrieve;
	propertyToRetrieve.definition.guid = property.definition.guid;

	GS::Array<API_Property> properties;
	properties.Push (propertyToRetrieve);

	// Test the getter
	ASSERT (properties[0] != property);
	ASSERT_NO_ERROR (ACAPI_Element_GetPropertiesDefault (typeId, variationID, properties));
	ASSERT (properties[0] == property);

	// Set a custom value
	property.value.multipleEnumVariant.variants.Push (property.definition.possibleEnumValues[0]);
	property.value.multipleEnumVariant.variants.Push (property.definition.possibleEnumValues[2]);
	property.isDefault = false;
	properties.Clear ();
	properties.Push (property);
	ASSERT_NO_ERROR (ACAPI_Element_SetPropertiesDefault (typeId, variationID, properties));

	// Test that the value has changed
	properties.Clear ();
	properties.Push (propertyToRetrieve);
	ASSERT (properties[0] != property);
	ASSERT_NO_ERROR (ACAPI_Element_GetPropertiesDefault (typeId, variationID, properties));
	ASSERT (properties[0] == property);

	// Inconsistent property values cause APIERR_BADPARS
	properties[0].value.multipleEnumVariant.variants[0].variant.type = API_PropertyBooleanValueType;
	ASSERT (ACAPI_Element_SetPropertiesDefault (typeId, variationID, properties) == APIERR_BADPARS);

	// Delete the property definition to clean up after the test
	ASSERT_NO_ERROR (ACAPI_Property_DeletePropertyDefinition (property.definition.guid)); 

	return NoError;
}


/*----------------------------------------------------------------**
**			Runs all the previously defined test cases.           **
**				Requires an element to be selected.			      **
**----------------------------------------------------------------*/
static GSErrCode RunTestsOnSelectedElem () 
{
	bool wasError = false;
	GSErrCode error = NoError;

	error = SimpleTestPropertyGroups (), wasError = wasError || (error != NoError);
	error = ThoroughTestPropertyGroups (), wasError = wasError || (error != NoError);
	error = SimpleTestPropertyDefinitions (), wasError = wasError || (error != NoError);
	error = ThoroughTestPropertyDefinitions (), wasError = wasError || (error != NoError);
	error = PropertyTestHelpers::CallOnSelectedElem (TestPropertiesOnElem), wasError = wasError || (error != NoError);
	error = TestPropertiesOnElemDefault (), wasError = wasError || (error != NoError);

	if (wasError) {
		return Error;
	} else {
		DGAlert (DG_INFORMATION, "Test success", "Tests executed successfully.", "", "Ok");
		return NoError;
	}
}


/*------------------------------------------------------------**
** Creates a new boolean single value property for an element **
**------------------------------------------------------------*/
static GSErrCode DefineNewBoolProperty (const API_Guid& wallGuid)
{
	API_Element         element, mask;
	GSErrCode           err = NoError;

	BNZeroMemory(&element, sizeof(API_Element));
	element.header.guid = wallGuid;
	err = ACAPI_Element_Get(&element);
	if (err != NoError)
		return err;


	if (err == NoError) {
		ACAPI_ELEMENT_MASK_CLEAR(mask);
		ACAPI_ELEMENT_MASK_SET(mask, API_CurtainWallType, nSegments);

		element.curtainWall.nSegments = 10;

		err = ACAPI_Element_Change(&element, &mask, NULL, 0, true);
	}

	return err;
}


/*---------------------------------------------------------**
** Creates a new integer list type property for an element **
**---------------------------------------------------------*/
static GSErrCode DefineNewStringListProperty (const API_Guid& elemGuid)
{
	API_PropertyGroup group;
	ASSERT_NO_ERROR (PropertyTestHelpers::GetCommonExamplePropertyGroup (group));

	API_Property property;
	property.definition = PropertyTestHelpers::CreateExampleStringListPropertyDefinition (group);
	API_ElemCategoryValue catValue;
	ASSERT_NO_ERROR (PropertyTestHelpers::GetElemCategoryValue (elemGuid, catValue));
	property.definition.availability.Push (catValue);
	ASSERT_NO_ERROR (ACAPI_Property_CreatePropertyDefinition (property.definition));

	// Add a custom value
	API_Variant variant;
	variant.type = API_PropertyStringValueType;
	for (int i = 0; i < 5; i++) {
		variant.uniStringValue = GS::ValueToUniString (1 << i);
		property.value.listVariant.variants.Push (variant);
	}
	property.isDefault = false;
	GS::Array<API_Property> properties;
	properties.Push (property);
	ASSERT_NO_ERROR (ACAPI_Element_SetProperties (elemGuid, properties));

	return NoError;
}


/*--------------------------------------------------------------------------**
** Creates a new string multiple choice enumeration property for an element **
**--------------------------------------------------------------------------*/
static GSErrCode DefineStringMultiEnumtProperty (const API_Guid& elemGuid)
{
	API_PropertyGroup group;
	ASSERT_NO_ERROR (PropertyTestHelpers::GetCommonExamplePropertyGroup (group));

	API_Property property;
	property.definition = PropertyTestHelpers::CreateExampleStringMultiEnumPropertyDefinition (group);
	API_ElemCategoryValue catValue;
	ASSERT_NO_ERROR (PropertyTestHelpers::GetElemCategoryValue (elemGuid, catValue));
	property.definition.availability.Push (catValue);
	ASSERT_NO_ERROR (ACAPI_Property_CreatePropertyDefinition (property.definition));

	// Add a custom value
	property.value.multipleEnumVariant.variants.Push (property.definition.possibleEnumValues[1]);
	property.isDefault = false;
	GS::Array<API_Property> properties;
	properties.Push (property);
	ASSERT_NO_ERROR (ACAPI_Element_SetProperties (elemGuid, properties));

	return NoError;
}


/*--------------------------------------------------------------------**
** Lists all the properties (definition name and value) of an element **
**--------------------------------------------------------------------*/
static GSErrCode ListAllProperties (const API_Guid& elemGuid)
{
	GS::Array<API_PropertyDefinition> definitions;
	ASSERT_NO_ERROR (ACAPI_Element_GetPropertyDefinitions (elemGuid, definitions));
	GS::Array<API_Property> properties;
	for (UInt32 i = 0; i < definitions.GetSize (); i++) {
		API_Property property;
		property.definition = definitions[i];
		properties.Push (property);
	}
	ASSERT_NO_ERROR (ACAPI_Element_GetProperties (elemGuid, properties));

	GS::UniString string;
	for (UInt32 i = 0; i < properties.GetSize (); i++) {
		string += PropertyTestHelpers::ToString (properties[i]) + "\n"; 
	}

	if (!string.IsEmpty ()) {
		DGAlert (DG_INFORMATION, "Properties of the selection", "", string, "Ok");
	} else {
		DGAlert (DG_INFORMATION, "Properties of the selection", "No properties found.", "", "Ok");
	}

	return NoError;
}


/*----------------------------------------------------------------**
** Removes all the custom values for the properties of an element **
**----------------------------------------------------------------*/
static GSErrCode SetAllPropertiesDefault (const API_Guid& elemGuid)
{
	GS::Array<API_PropertyDefinition> definitions;
	ASSERT_NO_ERROR (ACAPI_Element_GetPropertyDefinitions (elemGuid, definitions));
	GS::Array<API_Property> properties;
	for (UInt32 i = 0; i < definitions.GetSize (); i++) {
		API_Property property;
		property.definition = definitions[i];
		properties.Push (property);
	}
	ASSERT_NO_ERROR (ACAPI_Element_GetProperties (elemGuid, properties));

	for (UInt32 i = 0; i < properties.GetSize (); i++) {
		properties[i].isDefault = true;
	}
	ASSERT_NO_ERROR (ACAPI_Element_SetProperties (elemGuid, properties));

	return NoError;
}


/*-----------------------------------------------------------------------------------------**
** Makes all properties unavailable for an element (affects every element in its category) **
**-----------------------------------------------------------------------------------------*/
static GSErrCode DeleteAllProperties (const API_Guid& elemGuid)
{
	GS::Array<API_PropertyDefinition> definitions;
	ASSERT_NO_ERROR (ACAPI_Element_GetPropertyDefinitions (elemGuid, definitions));
	API_ElemCategoryValue catValue;
	ASSERT_NO_ERROR (PropertyTestHelpers::GetElemCategoryValue (elemGuid, catValue));

	for (UInt32 i = 0; i < definitions.GetSize (); i++) {
		definitions[i].availability.DeleteAll (catValue);
		ASSERT_NO_ERROR (ACAPI_Property_ChangePropertyDefinition (definitions[i]));
	}

	return NoError;
}


namespace SelectionProperties {


/*--------------------------------------------------**
**  Adds a new integer property that is available   **
**		 for all of the selected elements			**
**--------------------------------------------------*/
static GSErrCode DefineNewIntProperty () 
{
	API_PropertyGroup group;
	ASSERT_NO_ERROR (PropertyTestHelpers::GetCommonExamplePropertyGroup (group));

	API_PropertyDefinition definition;
	definition = PropertyTestHelpers::CreateExampleIntPropertyDefinition (group);
	ASSERT_NO_ERROR (ACAPI_ElementList_AddProperty (definition, PropertyTestHelpers::GetSelectedElements()));

	return NoError;
}


/*-------------------------------------------------------------------------**
** Sets all the integer property values of all the selected elements to 42 **
**-------------------------------------------------------------------------*/
static GSErrCode SetAllIntPropertiesTo42 ()
{
	GS::Array<API_PropertyDefinition> definitions;
	ACAPI_Property_GetPropertyDefinitions (APINULLGuid, definitions);
	GS::Array<API_Guid> selectedElements = PropertyTestHelpers::GetSelectedElements();

	for (UIndex i = 0; i < definitions.GetSize (); ++i) {
		if (definitions[i].collectionType == API_PropertySingleCollectionType &&
				 definitions[i].valueType == API_PropertyIntegerValueType) {
			API_Property property;
			property.definition = definitions[i];
			property.value.singleVariant.variant.type = property.definition.valueType;
			property.value.singleVariant.variant.intValue = 42;
			property.isDefault = false;

			// remove the elements from the list, which the property is not available for
			// (if you don't remove them, APIERR_BADPROPERTYFORELEM will be returned)
			GS::Array<API_Guid> filteredSelectedElements;

			for (UIndex i = 0; i < selectedElements.GetSize (); ++i) {
				API_ElemCategoryValue categoryValue;
				ASSERT_NO_ERROR (PropertyTestHelpers::GetElemCategoryValue (selectedElements[i], categoryValue));
				if (property.definition.availability.Contains (categoryValue)) {
					filteredSelectedElements.Push (selectedElements[i]);
				}
			}

			ASSERT_NO_ERROR (ACAPI_ElementList_ModifyPropertyValue (property, filteredSelectedElements));
		}
	}
	return NoError;
}


/*----------------------------------------------------------**
** Makes all integer properties unavailable for all of the  **
** selected element (affects every element in its category) **
**----------------------------------------------------------*/
static GSErrCode DeleteIntegerPropeties ()
{
	GS::Array<API_PropertyDefinition> definitions;
	ACAPI_Property_GetPropertyDefinitions (APINULLGuid, definitions);
	GS::Array<API_Guid> selectedElements = PropertyTestHelpers::GetSelectedElements();

	for (UIndex i = 0; i < definitions.GetSize (); ++i) {
		if (definitions[i].collectionType == API_PropertySingleCollectionType &&
			definitions[i].valueType == API_PropertyIntegerValueType) {
			ASSERT_NO_ERROR (ACAPI_ElementList_DeleteProperty (definitions[i].guid, selectedElements));
		}
	}
	return NoError;
}

} // namespace SelectionProperties

// -----------------------------------------------------------------------------
// Add-on entry point definition
// -----------------------------------------------------------------------------
GSErrCode __ACENV_CALL APIMenuCommandProc_Main (const API_MenuParams *menuParams)
{
	if (menuParams->menuItemRef.menuResID == 32500) {
		GSErrCode errorCode = ACAPI_CallUndoableCommand ("Property Test API Function",
			[&] () -> GSErrCode {
				switch (menuParams->menuItemRef.itemIndex) {
					case  1: return PropertyTestHelpers::CallOnSelectedElem (DefineNewBoolProperty);
					case  2: return PropertyTestHelpers::CallOnSelectedElem (DefineNewStringListProperty);
					case  3: return PropertyTestHelpers::CallOnSelectedElem (DefineStringMultiEnumtProperty);
					case  4: return NoError; // "-"
					case  5: return PropertyTestHelpers::CallOnSelectedElem (ListAllProperties);
					case  6: return PropertyTestHelpers::CallOnSelectedElem (SetAllPropertiesDefault);
					case  7: return PropertyTestHelpers::CallOnSelectedElem (DeleteAllProperties);
					case  8: return NoError; // "-"
					case  9: return SelectionProperties::DefineNewIntProperty ();
					case 10: return SelectionProperties::SetAllIntPropertiesTo42 ();
					case 11: return SelectionProperties::DeleteIntegerPropeties ();
					case 12: return NoError; // "-"
					case 13: return RunTestsOnSelectedElem ();
					default: return NoError;
			}
		});

		return errorCode;
	}
	return NoError;
}		// APIMenuCommandProc_Main

#ifdef WINDOWS
GSErrCode __ACENV_CALL APIMenuCommandProc_Lister (const API_MenuParams *menuParams)
{
	if (menuParams->menuItemRef.menuResID == 32501) {
		// Lister
	}
	return NoError;
}		// APIMenuCommandProc_Lister
#endif

// -----------------------------------------------------------------------------
// Dependency definitions
// -----------------------------------------------------------------------------

API_AddonType	__ACENV_CALL	CheckEnvironment (API_EnvirParams* envir)
{
	if (envir->serverInfo.serverApplication != APIAppl_ArchiCADID)
		return APIAddon_DontRegister;

	ACAPI_Resource_GetLocStr (envir->addOnInfo.name, 32000, 1);
	ACAPI_Resource_GetLocStr (envir->addOnInfo.description, 32000, 2);

	return APIAddon_Normal;
}


// -----------------------------------------------------------------------------
// Interface definitions
// -----------------------------------------------------------------------------

GSErrCode	__ACENV_CALL	RegisterInterface (void)
{
	GSErrCode err = ACAPI_Register_Menu (32500, 32600, MenuCode_UserDef, MenuFlag_Default);

#ifdef WINDOWS
	if (err == NoError) {
		 err = ACAPI_Register_Menu (32501, 32601, MenuCode_Calculate, MenuFlag_Default);
	}
#endif

	return err;
}


// -----------------------------------------------------------------------------
// Called after the Add-On has been loaded into memory
// -----------------------------------------------------------------------------

GSErrCode	__ACENV_CALL Initialize	(void)
{
	GSErrCode err = ACAPI_Install_MenuHandler (32500, APIMenuCommandProc_Main);

#ifdef WINDOWS
	if (err == NoError) {
		 err = ACAPI_Install_MenuHandler (32501, APIMenuCommandProc_Lister);
	}
#endif

	return err;
}


// -----------------------------------------------------------------------------
// Called when the Add-On is going to be unloaded
// -----------------------------------------------------------------------------

GSErrCode	__ACENV_CALL FreeData	(void)
{
	return NoError;
}
