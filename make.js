var path = require("path");

// Making resharper less noisy - These are defined in Generate.js
if (typeof getCompiledTemplate === "undefined") getCompiledTemplate = function () { };
if (typeof templatizeTree === "undefined") templatizeTree = function () { };

exports.makeCombinedAPI = function (apis, sourceDir, apiOutputDir) {
    console.log("Generating Combined api from: " + sourceDir + " to: " + apiOutputDir);

    var libDefines = "ENABLE_PLAYFABADMIN_API;ENABLE_PLAYFABSERVER_API;";
    var clientDefines = "";
    var serverDefines = "ENABLE_PLAYFABADMIN_API;ENABLE_PLAYFABSERVER_API;DISABLE_PLAYFABCLIENT_API;";

    var locals = {
        apis: apis,
        buildIdentifier: sdkGlobals.buildIdentifier,
        clientDefines: clientDefines,
        libDefines: libDefines,
        serverDefines: serverDefines,
        sdkVersion: sdkGlobals.sdkVersion,
        sdkDate: sdkGlobals.sdkVersion.split(".")[2],
        sdkYear: sdkGlobals.sdkVersion.split(".")[2].substr(0, 2),
        vsVer: "v141", // As C++ versions change, we may need to update this
        vsYear: "2017", // As VS versions change, we may need to update this
        getVerticalNameDefault: getVerticalNameDefault
    };

    templatizeTree(locals, path.resolve(sourceDir, "source"), apiOutputDir);
    for (var a = 0; a < apis.length; a++)
        makeApiFiles(apis[a], sourceDir, apiOutputDir);
};

function makeApiFiles(api, sourceDir, apiOutputDir) {
    var locals = {
        api: api,
        enumtypes: getEnumTypes(api.datatypes),
        getApiDefine: getApiDefine,
        getAuthParams: getAuthParams,
        getBaseType: getBaseType,
        getPropertyDefinition: getPropertyDefinition,
        getPropertyFromJson: getPropertyFromJson,
        getPropertyToJson: getPropertyToJson,
        getPropertySafeName: getPropertySafeName,
        getRequestActions: getRequestActions,
        getResultActions: getResultActions,
        hasClientOptions: getAuthMechanisms([api]).includes("SessionTicket"),
        hasAuthParams: hasAuthParams,
        ifHasProps: ifHasProps,
        sdkVersion: sdkGlobals.sdkVersion,
        sortedClasses: getSortedClasses(api.datatypes)
    };

    var apihTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/PlayFab_Api.h.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/include/playfab", "PlayFab" + api.name + "Api.h"), apihTemplate(locals));

    var iapihTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/PlayFab_InstanceApi.h.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/include/playfab", "PlayFab" + api.name + "InstanceApi.h"), iapihTemplate(locals));

    var apiCppTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/PlayFab_Api.cpp.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/source/playfab", "PlayFab" + api.name + "Api.cpp"), apiCppTemplate(locals));

    var iapiCppTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/PlayFab_InstanceApi.cpp.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/source/playfab", "PlayFab" + api.name + "InstanceApi.cpp"), iapiCppTemplate(locals));

    var dataModelTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/PlayFab_DataModels.h.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/include/playfab", "PlayFab" + api.name + "DataModels.h"), dataModelTemplate(locals));
}

// *************************** Internal utility methods ***************************
function getEnumTypes(datatypes) {
    var enumtypes = [];

    for (var typeIdx in datatypes) // Add all types and their dependencies
        if (datatypes[typeIdx].isenum)
            enumtypes.push(datatypes[typeIdx]);
    return enumtypes;
}

function getSortedClasses(datatypes) {
    var sortedClasses = [];
    var addedTypes = new Set();

    var addType = function (datatype) {
        if (addedTypes.has(datatype.name) || datatype.isenum)
            return;
        // In C++, dependent types must be defined first
        if (datatype.properties) {
            for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
                var property = datatype.properties[propIdx];
                if (property.isclass || property.isenum)
                    addType(datatypes[property.actualtype]);
            }
        }
        addedTypes.add(datatype.name);
        sortedClasses.push(datatype);
    };

    for (var typeIdx in datatypes) // Add all types and their dependencies
        addType(datatypes[typeIdx]);
    return sortedClasses;
}

// *************************** ejs-exposed methods ***************************
function getApiDefine(api) {
    if (api.name === "Client")
        return "#ifndef DISABLE_PLAYFABCLIENT_API";
    if (api.name === "Matchmaker")
        return "#ifdef ENABLE_PLAYFABSERVER_API"; // Matchmaker is bound to server, which is just a legacy design decision at this point
    if (api.name === "Admin" || api.name === "Server")
        return "#ifdef ENABLE_PLAYFAB" + api.name.toUpperCase() + "_API";

    // For now, everything else is considered ENTITY
    return "#ifndef DISABLE_PLAYFABENTITY_API";
}

function hasAuthParams(apiCall) {
    try {
        getAuthParams(apiCall, true);
    } catch (err) {
        return false;
    }
    return true;
}

function getAuthParams(apiCall, isInstanceApi) {
    if (apiCall.url === "/Authentication/GetEntityToken")
        return "authKey, authValue";
    switch (apiCall.auth) {
        case "EntityToken": return "\"X-EntityToken\", context->entityToken";
        case "SessionTicket": return "\"X-Authorization\", context->clientSessionTicket";
        case "SecretKey": return "\"X-SecretKey\", settings->developerSecretKey";
    }
    throw Error("getAuthParams: Unknown auth type: " + apiCall.auth + " for " + apiCall.name);
}
function getBaseType(datatype) {
    if (datatype.className.toLowerCase().endsWith("request"))
        return "PlayFabRequestCommon";
    if (datatype.className.toLowerCase().endsWith("loginresult"))
        return "PlayFabLoginResultCommon";
    if (datatype.className.toLowerCase().endsWith("response") || datatype.className.toLowerCase().endsWith("result"))
        return "PlayFabResultCommon";
    return "PlayFabBaseModel";
}

function getPropertyCppType(property, datatype, needOptional) {
    var isOptional = property.optional && needOptional;

    if (property.actualtype === "String")
        return "std::string";
    else if (property.isclass)
        return isOptional ? ("Boxed<" + property.actualtype + ">") : property.actualtype;
    else if (property.jsontype === "Object" && property.actualtype === "object")
        return "Json::Value";
    else if (property.actualtype === "Boolean")
        return isOptional ? "Boxed<bool>" : "bool";
    else if (property.actualtype === "int16")
        return isOptional ? "Boxed<Int16>" : "Int16";
    else if (property.actualtype === "uint16")
        return isOptional ? "Boxed<Uint16>" : "Uint16";
    else if (property.actualtype === "int32")
        return isOptional ? "Boxed<Int32>" : "Int32";
    else if (property.actualtype === "uint32")
        return isOptional ? "Boxed<Uint32>" : "Uint32";
    else if (property.actualtype === "int64")
        return isOptional ? "Boxed<Int64>" : "Int64";
    else if (property.actualtype === "uint64")
        return isOptional ? "Boxed<Uint64>" : "Uint64";
    else if (property.actualtype === "float")
        return isOptional ? "Boxed<float>" : "float";
    else if (property.actualtype === "double")
        return isOptional ? "Boxed<double>" : "double";
    else if (property.actualtype === "DateTime")
        return isOptional ? "Boxed<time_t>" : "time_t";
    else if (property.isenum)
        return isOptional ? ("Boxed<" + property.actualtype + ">") : property.actualtype;
    throw Error("getPropertyCppType: Unknown property type: " + property.actualtype + " for " + property.name + " in " + datatype.name);
}

function getPropertyDefinition(tabbing, property, datatype) {
    var cppType = getPropertyCppType(property, datatype, !property.collection);
    var safePropName = getPropertySafeName(property);

    if (!property.collection) {
        return tabbing + cppType + " " + safePropName + ";";
    } else if (property.jsontype === "Object" && property.actualtype === "object") {
        return tabbing + cppType + " " + safePropName + "; // Not truly arbitrary. See documentation for restrictions on format";
    } else if (property.collection === "array") {
        return tabbing + "std::list<" + cppType + "> " + safePropName + ";";
    } else if (property.collection === "map") {
        return tabbing + "std::map<std::string, " + cppType + "> " + safePropName + ";";
    }
    throw Error("getPropertyDefinition: Unknown property type: " + property.actualtype + " for " + property.name + " in " + datatype.name);
}

function getPropertyFromJson(tabbing, property, datatype) {
    var safePropName = getPropertySafeName(property);
    if (property.jsontype === "Object" && property.actualtype === "object")
        return tabbing + safePropName + " = input[\"" + property.name + "\"];";
    if (property.jsontype === "Object")
        return tabbing + "FromJsonUtilO(input[\"" + property.name + "\"], " + safePropName + ");";
    if (property.isenum && (property.collection || property.optional))
        return tabbing + "FromJsonUtilE(input[\"" + property.name + "\"], " + safePropName + ");";
    if (property.isenum)
        return tabbing + "FromJsonEnum(input[\"" + property.name + "\"], " + safePropName + ");";
    if (property.actualtype === "DateTime")
        return tabbing + "FromJsonUtilT(input[\"" + property.name + "\"], " + safePropName + ");";
    if (property.actualtype === "String")
        return tabbing + "FromJsonUtilS(input[\"" + property.name + "\"], " + safePropName + ");";
    var primitives = new Set(["Boolean", "int16", "uint16", "int32", "uint32", "int64", "uint64", "float", "double"]);
    if (primitives.has(property.actualtype))
        return tabbing + "FromJsonUtilP(input[\"" + property.name + "\"], " + safePropName + ");";

    throw Error("getPropertyFromJson: Unknown property type: " + property.actualtype + " for " + property.name + " in " + datatype.name);
}

function getPropertyToJson(tabbing, property, datatype) {
    var safePropName = getPropertySafeName(property);
    if (property.jsontype === "Object" && property.actualtype === "object")
        return tabbing + "output[\"" + property.name + "\"] = " + safePropName + ";";
    if (property.jsontype === "Object")
        return tabbing + "Json::Value each_" + safePropName + "; ToJsonUtilO(" + safePropName + ", each_" + safePropName + "); output[\"" + property.name + "\"] = each_" + safePropName + ";";
    if (property.isenum && (property.collection || property.optional))
        return tabbing + "Json::Value each_" + safePropName + "; ToJsonUtilE(" + safePropName + ", each_" + safePropName + "); output[\"" + property.name + "\"] = each_" + safePropName + ";";
    if (property.isenum)
        return tabbing + "Json::Value each_" + safePropName + "; ToJsonEnum(" + safePropName + ", each_" + safePropName + "); output[\"" + property.name + "\"] = each_" + safePropName + ";";
    if (property.actualtype === "DateTime")
        return tabbing + "Json::Value each_" + safePropName + "; ToJsonUtilT(" + property.name + ", each_" + safePropName + "); output[\"" + property.name + "\"] = each_" + safePropName + ";";
    if (property.actualtype === "String")
        return tabbing + "Json::Value each_" + safePropName + "; ToJsonUtilS(" + safePropName + ", each_" + safePropName + "); output[\"" + property.name + "\"] = each_" + safePropName + ";";
    var primitives = new Set(["Boolean", "int16", "uint16", "int32", "uint32", "int64", "uint64", "float", "double"]);
    if (primitives.has(property.actualtype))
        return tabbing + "Json::Value each_" + safePropName + "; ToJsonUtilP(" + safePropName + ", each_" + safePropName + "); output[\"" + property.name + "\"] = each_" + safePropName + ";";

    throw Error("getPropertyToJson: Unknown property type: " + property.actualtype + " for " + property.name + " in " + datatype.name);
}

function getPropertySafeName(property) {
    return (property.actualtype === property.name) ? "pf" + property.name : property.name;
}

function getRequestActions(tabbing, apiCall, isInstanceApi) {
    //TODO Bug 6594: add to this titleId check. 
    // If this titleId does not exist we should be throwing an error informing the user MUST have a titleId.
    if (apiCall.result === "LoginResult" || apiCall.result === "RegisterPlayFabUserResult")
        return tabbing + "if (request.TitleId.empty())\n"
            + tabbing + "{\n"
            + tabbing + "    request.TitleId = settings->titleId;\n"
            + tabbing + "}\n";

    if (apiCall.url === "/Authentication/GetEntityToken")
        return tabbing + "std::string authKey, authValue;\n" +
            tabbing + "if (context->entityToken.length() > 0)\n" +
            tabbing + "{\n" +
            tabbing + "    authKey = \"X-EntityToken\"; authValue = context->entityToken;\n" +
            tabbing + "}\n" +
            tabbing + "else if (context->clientSessionTicket.length() > 0)\n" +
            tabbing + "{\n" +
            tabbing + "    authKey = \"X-Authorization\"; authValue = context->clientSessionTicket;\n" +
            tabbing + "}\n" +
            "#if defined(ENABLE_PLAYFABSERVER_API) || defined(ENABLE_PLAYFABADMIN_API)\n" +
            tabbing + "else if (settings->developerSecretKey.length() > 0)\n" +
            tabbing + "{\n" +
            tabbing + "    authKey = \"X-SecretKey\"; authValue = settings->developerSecretKey;\n" +
            tabbing + "}\n" +
            "#endif\n";

    return "";
}

function getResultActions(tabbing, apiCall, isInstanceApi) {
    if (apiCall.url === "/Authentication/GetEntityToken")
        return tabbing + "context->HandlePlayFabLogin(\"\", \"\", outResult.Entity->Id, outResult.Entity->Type, outResult.EntityToken);\n";
    if (apiCall.result === "LoginResult")
        return tabbing + "outResult.authenticationContext = std::make_shared<PlayFabAuthenticationContext>();\n" +
            tabbing + "outResult.authenticationContext->HandlePlayFabLogin(outResult.PlayFabId, outResult.SessionTicket, outResult.EntityToken->Entity->Id, outResult.EntityToken->Entity->Type, outResult.EntityToken->EntityToken);\n" +
            tabbing + "context->HandlePlayFabLogin(outResult.PlayFabId, outResult.SessionTicket, outResult.EntityToken->Entity->Id, outResult.EntityToken->Entity->Type, outResult.EntityToken->EntityToken);\n" +
            tabbing + "MultiStepClientLogin(context, outResult.SettingsForUser->NeedsAttribution);\n";
    if (apiCall.result === "RegisterPlayFabUserResult")
        return tabbing + "context->HandlePlayFabLogin(outResult.PlayFabId, outResult.SessionTicket, outResult.EntityToken->Entity->Id, outResult.EntityToken->Entity->Type, outResult.EntityToken->EntityToken);\n"
            + tabbing + "MultiStepClientLogin(context, outResult.SettingsForUser->NeedsAttribution);\n";
    if (apiCall.result === "AttributeInstallResult")
        return tabbing + "context->advertisingIdType += \"_Successful\";\n";

    return "";
}

function ifHasProps(datatype, displayText) {
    if (datatype.properties.length === 0)
        return "";
    return displayText;
}

function getVerticalNameDefault() {
    if (sdkGlobals.verticalName) {
        return sdkGlobals.verticalName;
    }
    return "";
}
