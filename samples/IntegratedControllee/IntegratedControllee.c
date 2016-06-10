/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

/**
 * Per-module definition of the current module for debug logging.  Must be defined
 * prior to first inclusion of aj_debug.h.
 * The corresponding flag dbgAJSVCAPP is defined in the containing sample app.
 */
#define AJ_MODULE AJSVCAPP

#include <ajtcl/alljoyn.h>
#include <ajtcl/aj_debug.h>
#include <ajtcl/aj_config.h>
#include <ajtcl/aj_creds.h>
#include <ajtcl/aj_nvram.h>
#include <ajtcl/aj_link_timeout.h>
#include <ajtcl/aj_cert.h>
#include "PropertyStoreOEMProvisioning.h"
#include <ajtcl/services/PropertyStore.h>
#include <ajtcl/services/ServicesCommon.h>
#include <ajtcl/services/ServicesHandlers.h>
#include <ajtcl/services/Common/AllJoynLogo.h>
#include <ajtcl/hae/HaeControllee.h>
#include <ajtcl/hae/interfaces/environment/CurrentAirQuality.h>
#include <ajtcl/hae/interfaces/environment/CurrentAirQualityLevel.h>
#include <ajtcl/hae/interfaces/environment/CurrentTemperature.h>
#include <ajtcl/hae/interfaces/environment/TargetTemperature.h>
#include <ajtcl/hae/interfaces/environment/WaterLevel.h>
#include <ajtcl/hae/interfaces/environment/WindDirection.h>
#include <ajtcl/hae/interfaces/input/Hid.h>
#include <ajtcl/hae/interfaces/operation/AirRecirculationMode.h>
#include <ajtcl/hae/interfaces/operation/AudioVideoInput.h>
#include <ajtcl/hae/interfaces/operation/AudioVolume.h>
#include <ajtcl/hae/interfaces/operation/BatteryStatus.h>
#include <ajtcl/hae/interfaces/operation/Channel.h>
#include <ajtcl/hae/interfaces/operation/ClimateControlMode.h>
#include <ajtcl/hae/interfaces/operation/ClosedStatus.h>
#include <ajtcl/hae/interfaces/operation/CurrentPower.h>
#include <ajtcl/hae/interfaces/operation/CycleControl.h>
#include <ajtcl/hae/interfaces/operation/DishWashingCyclePhase.h>
#include <ajtcl/hae/interfaces/operation/EnergyUsage.h>
#include <ajtcl/hae/interfaces/operation/FanSpeedLevel.h>
#include <ajtcl/hae/interfaces/operation/HeatingZone.h>
#include <ajtcl/hae/interfaces/operation/LaundryCyclePhase.h>
#include <ajtcl/hae/interfaces/operation/MoistureOutputLevel.h>
#include <ajtcl/hae/interfaces/operation/OffControl.h>
#include <ajtcl/hae/interfaces/operation/OnControl.h>
#include <ajtcl/hae/interfaces/operation/OnOffStatus.h>
#include <ajtcl/hae/interfaces/operation/OvenCyclePhase.h>
#include <ajtcl/hae/interfaces/operation/RapidMode.h>
#include <ajtcl/hae/interfaces/operation/RemoteControllability.h>
#include <ajtcl/hae/interfaces/operation/RepeatMode.h>
#include <ajtcl/hae/interfaces/operation/ResourceSaving.h>
#include <ajtcl/hae/interfaces/operation/RobotCleaningCyclePhase.h>
#include <ajtcl/hae/interfaces/operation/SoilLevel.h>
#include <ajtcl/hae/interfaces/operation/SpinSpeedLevel.h>
#include <ajtcl/hae/interfaces/operation/Timer.h>
#include <ajtcl/hae/interfaces/operation/FilterStatus.h>
#include <ajtcl/hae/interfaces/environment/CurrentHumidity.h>
#include <ajtcl/hae/interfaces/environment/TargetHumidity.h>
#include <ajtcl/hae/interfaces/environment/TargetTemperatureLevel.h>
#include <ajtcl/hae/interfaces/operation/HvacFanMode.h>
#include <ajtcl/hae/interfaces/operation/PlugInUnits.h>
#include <ajtcl/hae/interfaces/operation/RapidModeTimed.h>

/*
 * Logger definition
 */

#ifndef NDEBUG
#ifndef ER_DEBUG_AJSVCAPP
#define ER_DEBUG_AJSVCAPP 0
#endif
AJ_EXPORT uint8_t dbgAJSVCAPP = ER_DEBUG_AJSVCAPP;
#endif

/*
 * Define timeout/pause values. Values are in milli seconds.
 * The following may be tuned according to platform requirements such as battery usage.
 */

#define AJAPP_BUS_LINK_TIMEOUT    60
#define AJAPP_CONNECT_TIMEOUT     (1000 * 60 * 10) // Override AJ_CONNECT_TIMEOUT to wait longer for a successful connection to a Routing Node
#define AJAPP_CONNECT_PAUSE       (1000 * 2)       // Override AJ_CONNECT_PAUSE to be more responsive
#define AJAPP_SLEEP_TIME          (1000 * 2)       // A little pause to let things settle
#define AJAPP_UNMARSHAL_TIMEOUT   (1000 * 1)       // Override AJ_UNMARSHAL_TIMEOUT to be more responsive

/**
 * Application wide globals
 */

#define ROUTING_NODE_NAME "org.alljoyn.BusNode"
static uint8_t isBusConnected = FALSE;
#define AJ_ABOUT_SERVICE_PORT 900

#define HAE_OBJECT_PATH_CONTROLLEE "/Hae/IntegratedControllee"

/**
 * Application handlers
 */

typedef enum {
    INIT_START = 0,
    INIT_SERVICES = INIT_START,
    INIT_SERVICES_PORT,
    INIT_ABOUT,
    INIT_CHECK_ANNOUNCE,
    INIT_FINISHED = INIT_CHECK_ANNOUNCE
} enum_init_state_t;

static enum_init_state_t currentServicesInitializationState = INIT_START;
static enum_init_state_t nextServicesInitializationState = INIT_START;
static bool haeInitState = false;

static AJ_Status AJApp_ConnectedHandler(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;

    if (AJ_GetUniqueName(busAttachment)) {
        if (currentServicesInitializationState == nextServicesInitializationState) {
            switch (currentServicesInitializationState) {
            case INIT_SERVICES:
                status = AJSVC_ConnectedHandler(busAttachment);
                if (status != AJ_OK) {
                    goto ErrorExit;
                }
                currentServicesInitializationState = nextServicesInitializationState = INIT_SERVICES_PORT;
                break;

            case INIT_SERVICES_PORT:
                status = AJ_BusBindSessionPort(busAttachment, AJ_ABOUT_SERVICE_PORT, NULL, 0);
                if (status != AJ_OK) {
                    goto ErrorExit;
                }
                nextServicesInitializationState = INIT_ABOUT;
                break;

            case INIT_ABOUT:
                status = AJ_AboutInit(busAttachment, AJ_ABOUT_SERVICE_PORT);
                if (status != AJ_OK) {
                    goto ErrorExit;
                }
                currentServicesInitializationState = nextServicesInitializationState = INIT_CHECK_ANNOUNCE;
                break;

            case INIT_CHECK_ANNOUNCE:
                status = AJ_AboutAnnounce(busAttachment);
                if (status != AJ_OK) {
                    goto ErrorExit;
                }
                break;

            default:
                break;
            }
        }
    }
    return status;

ErrorExit:

    AJ_ErrPrintf(("Application ConnectedHandler returned an error %s\n", (AJ_StatusText(status))));
    return status;
}

static AJSVC_ServiceStatus AJApp_MessageProcessor(AJ_BusAttachment* busAttachment, AJ_Message* msg, AJ_Status* status)
{
    AJSVC_ServiceStatus serviceStatus = AJSVC_SERVICE_STATUS_HANDLED;
    uint16_t port;
    char* joiner;
    uint32_t sessionId = 0;
    uint8_t session_accepted = FALSE;

    if (msg->msgId == AJ_METHOD_ACCEPT_SESSION) {    // Process all incoming request to join a session and pass request for acceptance by all services
        *status = AJ_UnmarshalArgs(msg, "qus", &port, &sessionId, &joiner);
        if (*status != AJ_OK) {
            return serviceStatus;
        }
        session_accepted |= (port == AJ_ABOUT_SERVICE_PORT);
        session_accepted |= AJSVC_CheckSessionAccepted(port, sessionId, joiner);
        *status = AJ_BusReplyAcceptSession(msg, session_accepted);
        AJ_AlwaysPrintf(("%s session session_id=%u joiner=%s for port %u\n", (session_accepted ? "Accepted" : "Rejected"), sessionId, joiner, port));
    } else {
        switch (currentServicesInitializationState) {
        case INIT_SERVICES_PORT:
            if (msg->msgId == AJ_REPLY_ID(AJ_METHOD_BIND_SESSION_PORT)) {
                currentServicesInitializationState = nextServicesInitializationState;
            }
            break;

        default:
            serviceStatus = AJSVC_MessageProcessorAndDispatcher(busAttachment, msg, status);
            break;
        }
    }

    return serviceStatus;
}

static AJ_Status AJApp_DisconnectHandler(AJ_BusAttachment* busAttachment, uint8_t restart)
{
    AJ_Status status = AJ_OK;

    if (restart) {
        AJ_BusUnbindSession(busAttachment, AJ_ABOUT_SERVICE_PORT);
    }

    AJ_AboutSetShouldAnnounce();
    currentServicesInitializationState = nextServicesInitializationState = INIT_START;

    status = AJSVC_DisconnectHandler(busAttachment);
    return status;
}

const char* deviceManufactureName = "COMPANY";
const char* deviceProductName = "GENERIC BOARD";

/**
 * About supported PropertyStore provisioning
 */

static const char DEFAULT_LANGUAGE[] = "en";
static const char* DEFAULT_LANGUAGES[] = { DEFAULT_LANGUAGE };
static const char SUPPORTED_LANG2[] = "de-AT";
static const char* SUPPORTED_LANGUAGES[] = { DEFAULT_LANGUAGE, SUPPORTED_LANG2, NULL };
const char* const* propertyStoreDefaultLanguages = SUPPORTED_LANGUAGES;

/**
 * property array of structure with defaults
 */
static const char DEFAULT_DEVICE_NAME_LANG1[] = { "Company A - \"Generic Board\"" };
static const char DEFAULT_DEVICE_NAME_LANG2[] = { "Firma A - \"Generic Board\"" };
static const char* DEFAULT_DEVICE_NAMES[] = { DEFAULT_DEVICE_NAME_LANG1, DEFAULT_DEVICE_NAME_LANG2 };
static const char* DEFAULT_APP_NAMES[] = { "Controllee" };
static const char DEFAULT_DESCRIPTION_LANG1[] = "My first IOE device";
static const char DEFAULT_DESCRIPTION_LANG2[] = "Mein erstes IOE Geraet";
static const char* DEFAULT_DESCRIPTIONS[] = { DEFAULT_DESCRIPTION_LANG1, DEFAULT_DESCRIPTION_LANG2 };
static const char DEFAULT_MANUFACTURER_LANG1[] = "Company A(EN)";
static const char DEFAULT_MANUFACTURER_LANG2[] = "Firma A(DE-AT)";
static const char* DEFAULT_MANUFACTURERS[] = { DEFAULT_MANUFACTURER_LANG1, DEFAULT_MANUFACTURER_LANG2 };
static const char* DEFAULT_DEVICE_MODELS[] = { "0.0.1" };
static const char* DEFAULT_DATE_OF_MANUFACTURES[] = { "2014-05-01" };
static const char* DEFAULT_SOFTWARE_VERSIONS[] = { "0.0.1" };
static const char* DEFAULT_HARDWARE_VERSIONS[] = { "0.0.1" };
static const char DEFAULT_SUPPORT_URL_LANG1[] = "www.company_a.com";
static const char DEFAULT_SUPPORT_URL_LANG2[] = "www.company_a.com/de-AT";
static const char* DEFAULT_SUPPORT_URLS[] = { DEFAULT_SUPPORT_URL_LANG1, DEFAULT_SUPPORT_URL_LANG2 };

static const char DEFAULT_COUNTRY_OF_PRODUCTION_LANG1[] = "USA";
static const char DEFAULT_COUNTRY_OF_PRODUCTION_LANG2[] = "USA";
static const char* DEFAULT_COUNTRY_OF_PRODUCTION[] = { DEFAULT_COUNTRY_OF_PRODUCTION_LANG1, DEFAULT_COUNTRY_OF_PRODUCTION_LANG2 };
static const char DEFAULT_CORPORATEBRAND_LANG1[] = "CO. BRAND";
static const char DEFAULT_CORPORATEBRAND_LANG2[] = "CO. BRAND";
static const char* DEFAULT_CORPORATEBRAND[] = { DEFAULT_CORPORATEBRAND_LANG1, DEFAULT_CORPORATEBRAND_LANG2 };
static const char DEFAULT_PRODUCTRAND_LANG1[] = "P. BRAND";
static const char DEFAULT_PRODUCTRAND_LANG2[] = "P. BRAND";
static const char* DEFAULT_PRODUCTRAND[] = { DEFAULT_PRODUCTRAND_LANG1, DEFAULT_PRODUCTRAND_LANG2 };
static const char DEFAULT_LOCATION_LANG1[] = "Room1";
static const char DEFAULT_LOCATION_LANG2[] = "Room1";
static const char* DEFAULT_LOCATION[] = { DEFAULT_LOCATION_LANG1, DEFAULT_LOCATION_LANG2 };
static const char* DEFAULT_DEVICETYPEDESC[] = { "TV" };

const char** propertyStoreDefaultValues[AJSVC_PROPERTY_STORE_NUMBER_OF_KEYS+5] =
{
// "Default Values per language",                    "Key Name"
    NULL,                                           /*DeviceId*/
    NULL,                                           /*AppId*/
    DEFAULT_DEVICE_NAMES,                           /*DeviceName*/
    DEFAULT_LANGUAGES,                              /*DefaultLanguage*/
// Add other runtime keys above this line
    DEFAULT_APP_NAMES,                              /*AppName*/
    DEFAULT_DESCRIPTIONS,                           /*Description*/
    DEFAULT_MANUFACTURERS,                          /*Manufacturer*/
    DEFAULT_DEVICE_MODELS,                          /*ModelNumber*/
    DEFAULT_DATE_OF_MANUFACTURES,                   /*DateOfManufacture*/
    DEFAULT_SOFTWARE_VERSIONS,                      /*SoftwareVersion*/
    NULL,                                           /*AJSoftwareVersion*/
// Add other mandatory about keys above this line
    DEFAULT_HARDWARE_VERSIONS,                      /*HardwareVersion*/
    DEFAULT_SUPPORT_URLS,                           /*SupportUrl*/
// Add other optional about keys above this line
    DEFAULT_COUNTRY_OF_PRODUCTION,
    DEFAULT_CORPORATEBRAND,
    DEFAULT_PRODUCTRAND,
    DEFAULT_LOCATION,
    DEFAULT_DEVICETYPEDESC,
};

/**
 * Array of DeviceTypeDescription (DeviceType, ObjectPath)
 */
DeviceTypeDescription deviceTypeDescription[] = {
    {OTHER, HAE_OBJECT_PATH_CONTROLLEE},
};

/**
 * properties array of runtime values' buffers
 */
static char machineIdVar[MACHINE_ID_LENGTH + 1] = { 0 };
static char* machineIdVars[] = { machineIdVar };
static char deviceNameVar[DEVICE_NAME_VALUE_LENGTH + 1] = { 0 };
static char* deviceNameVars[] = { deviceNameVar, deviceNameVar };

PropertyStoreConfigEntry propertyStoreRuntimeValues[AJSVC_PROPERTY_STORE_NUMBER_OF_RUNTIME_KEYS] =
{
//  {"Buffers for Values per language", "Buffer Size"},                  "Key Name"
    { machineIdVars,             MACHINE_ID_LENGTH + 1 },               /*DeviceId*/
    { machineIdVars,             MACHINE_ID_LENGTH + 1 },               /*AppId*/
    { deviceNameVars,            DEVICE_NAME_VALUE_LENGTH + 1 },        /*DeviceName*/
// Add other runtime keys above this line
};

/**
 * AboutIcon Provisioning
 */
const char* aboutIconMimetype = AJ_LogoMimeType;
const uint8_t* aboutIconContent = AJ_LogoData;
const size_t aboutIconContentSize = AJ_LogoSize;
const char* aboutIconUrl = AJ_LogoURL;

/*
 * Default key expiration
 */
static const uint32_t keyexpiration = 0xFFFFFFFF;

static const uint32_t suites[3] = { AUTH_SUITE_ECDHE_ECDSA, AUTH_SUITE_ECDHE_PSK, AUTH_SUITE_ECDHE_NULL };
static const size_t numsuites = 3;

// Copied from ajtcl/samples/secure/SecureServiceECDHE.c
static const char pem_prv[] = {
    "-----BEGIN EC PRIVATE KEY-----"
    "MDECAQEEICCRJMbxSiWUqj4Zs7jFQRXDJdBRPWX6fIVqE1BaXd08oAoGCCqGSM49"
    "AwEH"
    "-----END EC PRIVATE KEY-----"
};

static const char pem_x509[] = {
    "-----BEGIN CERTIFICATE-----"
    "MIIBuDCCAV2gAwIBAgIHMTAxMDEwMTAKBggqhkjOPQQDAjBCMRUwEwYDVQQLDAxv"
    "cmdhbml6YXRpb24xKTAnBgNVBAMMIDgxM2FkZDFmMWNiOTljZTk2ZmY5MTVmNTVk"
    "MzQ4MjA2MB4XDTE1MDcyMjIxMDYxNFoXDTE2MDcyMTIxMDYxNFowQjEVMBMGA1UE"
    "CwwMb3JnYW5pemF0aW9uMSkwJwYDVQQDDCAzOWIxZGNmMjBmZDJlNTNiZGYzMDU3"
    "NzMzMjBlY2RjMzBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABGJ/9F4xHn3Klw7z"
    "6LREmHJgzu8yJ4i09b4EWX6a5MgUpQoGKJcjWgYGWb86bzbciMCFpmKzfZ42Hg+k"
    "BJs2ZWajPjA8MAwGA1UdEwQFMAMBAf8wFQYDVR0lBA4wDAYKKwYBBAGC3nwBATAV"
    "BgNVHSMEDjAMoAoECELxjRK/fVhaMAoGCCqGSM49BAMCA0kAMEYCIQDixoulcO7S"
    "df6Iz6lvt2CDy0sjt/bfuYVW3GeMLNK1LAIhALNklms9SP8ZmTkhCKdpC+/fuwn0"
    "+7RX8CMop11eWCih"
    "-----END CERTIFICATE-----"
};

static const char psk_hint[] = "<anonymous>";
static const char psk_char[] = "faaa0af3dd3f1e0379da046a3ab6ca44";
static X509CertificateChain* chain = NULL;
static AJ_Status AuthListenerCallback(uint32_t authmechanism, uint32_t command, AJ_Credential*cred)
{
    AJ_Status status = AJ_ERR_INVALID;
    X509CertificateChain* node;

    AJ_AlwaysPrintf(("AuthListenerCallback authmechanism %08X command %d\n", authmechanism, command));

    switch (authmechanism) {
    case AUTH_SUITE_ECDHE_NULL:
        cred->expiration = keyexpiration;
        status = AJ_OK;
        break;

    case AUTH_SUITE_ECDHE_PSK:
        switch (command) {
        case AJ_CRED_PUB_KEY:
            cred->data = (uint8_t*) psk_hint;
            cred->len = strlen(psk_hint);
            cred->expiration = keyexpiration;
            status = AJ_OK;
            break;

        case AJ_CRED_PRV_KEY:
            cred->data = (uint8_t*) psk_char;
            cred->len = strlen(psk_char);
            cred->expiration = keyexpiration;
            status = AJ_OK;
            break;
        }
        break;

    case AUTH_SUITE_ECDHE_ECDSA:
        switch (command) {
        case AJ_CRED_PRV_KEY:
            AJ_ASSERT(sizeof (AJ_ECCPrivateKey) == cred->len);
            status = AJ_DecodePrivateKeyPEM((AJ_ECCPrivateKey*) cred->data, pem_prv);
            cred->expiration = keyexpiration;
            break;

        case AJ_CRED_CERT_CHAIN:
            switch (cred->direction) {
            case AJ_CRED_REQUEST:
                // Free previous certificate chain
                AJ_X509FreeDecodedCertificateChain(chain);
                chain = AJ_X509DecodeCertificateChainPEM(pem_x509);
                if (NULL == chain) {
                    return AJ_ERR_INVALID;
                }
                cred->data = (uint8_t*) chain;
                cred->expiration = keyexpiration;
                status = AJ_OK;
                break;

            case AJ_CRED_RESPONSE:
                node = (X509CertificateChain*) cred->data;
                status = AJ_X509VerifyChain(node, NULL, AJ_CERTIFICATE_IDN_X509);
                while (node) {
                    AJ_DumpBytes("CERTIFICATE", node->certificate.der.data, node->certificate.der.size);
                    node = node->next;
                }
                break;
            }
            break;
        }
        break;

    default:
        break;
    }
    return status;
}

// CurrentAirQuality
AJ_Status CurrentAirQuality_OnGetContaminantType(const char* objPath, uint8_t* contaminantType)
{
    printf("CurrentAirQuality_OnGetContaminantType: %s\n", objPath);

    *contaminantType = 2;

    return AJ_OK;
}

AJ_Status CurrentAirQuality_OnGetCurrentValue(const char* objPath, double* currentValue)
{
    printf("CurrentAirQuality_OnGetCurrentValue : %s\n", objPath);

    *currentValue = 10.0;

    return AJ_OK;
}
AJ_Status CurrentAirQuality_OnGetMaxValue(const char* objPath, double* maxValue)
{
    printf("CurrentAirQuality_OnGetMaxValue : %s\n", objPath);

    *maxValue = 30.0;

    return AJ_OK;
}
AJ_Status CurrentAirQuality_OnGetMinValue(const char* objPath, double* minValue)
{
    printf("CurrentAirQuality_OnGetMinValue : %s\n", objPath);

    *minValue = 5.0;

    return AJ_OK;
}
AJ_Status CurrentAirQuality_OnGetPrecision(const char* objPath, double* precision)
{
    printf("CurrentAirQuality_OnGetPrecision : %s\n", objPath);

    *precision = 0.5;

    return AJ_OK;
}
AJ_Status CurrentAirQuality_OnGetUpdateMinTime(const char* objPath, uint16_t* updateMinTime)
{
    printf("CurrentAirQuality_OnGetUpdateMinTime : %s\n", objPath);

    *updateMinTime = 1;

    return AJ_OK;
}

// CurrentAirQualityLevel
AJ_Status CurrentAirQualityLevel_OnGetContaminantType(const char* objPath, uint8_t* contaminantType)
{
    printf("CurrentAirQualityLevel_OnGetContaminantType: %s\n", objPath);

    *contaminantType = 2;

    return AJ_OK;
}
AJ_Status CurrentAirQualityLevel_OnGetCurrentLevel(const char* objPath, uint8_t* currentLevel)
{
    printf("CurrentAirQualityLevel_OnGetCurrentLevel: %s\n", objPath);

    *currentLevel = 10;

    return AJ_OK;
}
AJ_Status CurrentAirQualityLevel_OnGetMaxLevel(const char* objPath, double* maxLevel)
{
    printf("CurrentAirQualityLevel_OnGetMaxLevel: %s\n", objPath);

    *maxLevel = 30;

    return AJ_OK;
}

//CurrentTemperature
AJ_Status OnGetCurrentValue(const char* objPath, double* currentValue)
{
    printf("OnGetCurrentValue : %s\n", objPath);

    *currentValue = 10.0;

    return AJ_OK;
}
AJ_Status OnGetPrecision(const char* objPath, double* precision)
{
    printf("OnGetPrecision : %s\n", objPath);

    *precision = 0.5;

    return AJ_OK;
}
AJ_Status OnGetUpdateMinTime(const char* objPath, uint16_t* updateMinTime)
{
    printf("OnGetUpdateMinTime : %s\n", objPath);

    *updateMinTime = 1;

    return AJ_OK;
}

//TargetTemperature
AJ_Status OnGetTargetValue(const char* objPath, double* targetValue)
{
    printf("OnGetTargetValue : %s\n", objPath);

    *targetValue = 5.0;

    return AJ_OK;
}
AJ_Status OnSetTargetValue(const char* objPath, const double targetValue)
{
    printf("OnSetTargetValue : %s, targetValue: %f\n", objPath, targetValue);

    return AJ_OK;
}
AJ_Status OnGetMinValue(const char* objPath, double* minValue)
{
    printf("OnGetMinValue : %s\n", objPath);

    *minValue = 2.0;

    return AJ_OK;
}
AJ_Status OnGetMaxValue(const char* objPath, double* maxValue)
{
    printf("OnGetMaxValue : %s\n", objPath);

    *maxValue = 8.0;

    return AJ_OK;
}
AJ_Status OnGetStepValue(const char* objPath, double* stepValue)
{
    printf("OnGetStepValue : %s\n", objPath);

    *stepValue = 0.5;

    return AJ_OK;
}

//WindDirection
AJ_Status OnGetHorizontalDirection(const char* objPath, uint16_t* horizontalDirection)
{
    printf("OnGetHorizontalDirection : %s\n", objPath);

    *horizontalDirection = 2;

    return AJ_OK;
}
AJ_Status OnSetHorizontalDirection(const char* objPath, const uint16_t horizontalDirection)
{
    printf("OnSetHorizontalDirection : %s, horizontalDirection: %u\n", objPath, horizontalDirection);

    return AJ_OK;
}
AJ_Status OnGetHorizontalMax(const char* objPath, uint16_t* horizontalMax)
{
    printf("OnGetHorizontalMax : %s\n", objPath);

    *horizontalMax = 5;

    return AJ_OK;
}
AJ_Status OnGetHorizontalAutoMode(const char* objPath, uint8_t* horizontalAutoMode)
{
    printf("OnGetHorizontalAutoMode : %s\n", objPath);

    *horizontalAutoMode = true;

    return AJ_OK;
}
AJ_Status OnSetHorizontalAutoMode(const char* objPath, const uint8_t horizontalAutoMode)
{
    printf("OnSetHorizontalDirection : %s, horizontalAutoMode: %u\n", objPath, horizontalAutoMode);

    return AJ_OK;
}
AJ_Status OnGetVerticalDirection(const char* objPath, uint16_t* verticalDirection)
{
    printf("OnGetVerticalDirection : %s\n", objPath);

    *verticalDirection = 3;

    return AJ_OK;
}
AJ_Status OnSetVerticalDirection(const char* objPath, const uint16_t verticalDirection)
{
    printf("OnSetVerticalDirection : %s, verticalDirection: %u\n", objPath, verticalDirection);

    return AJ_OK;
}
AJ_Status OnGetVerticalMax(const char* objPath, uint16_t* verticalMax)
{
    printf("OnGetVerticalMax : %s\n", objPath);

    *verticalMax = 4;

    return AJ_OK;
}
AJ_Status OnGetVerticalAutoMode(const char* objPath, uint8_t* verticalAutoMode)
{
    printf("OnGetVerticalAutoMode : %s\n", objPath);

    *verticalAutoMode = true;

    return AJ_OK;
}
AJ_Status OnSetVerticalAutoMode(const char* objPath, const uint8_t verticalAutoMode)
{
    printf("OnSetVerticalAutoMode : %s, verticalAutoMode: %u\n", objPath, verticalAutoMode);

    return AJ_OK;
}

//Hid
AJ_Status OnGetSupportedEvents(const char* objPath, SupportedInputEvent* supportedEvents)
{
    SupportedInputEvent event[3] = {
        {1,2,4,10},
        {2,4,3,10},
        {4,5,4,20}
    };
    printf("OnGetSupportedEvents : %s\n", objPath);

    memcpy(supportedEvents, event, sizeof(SupportedInputEvent) * 3);

    return AJ_OK;
}
AJ_Status OnInjectEvents(const char* objPath, const InputEvent* inputEvent, const size_t numRecords)
{
    int i = 0;
    printf("OnInjectEvents : %s, numRecords : %lu\n", objPath, numRecords);

    for (i = 0; i < numRecords; i++) {
        printf("type : %u, code : %u, value : %d\n ", inputEvent[i].type, inputEvent[i].code, inputEvent[i].value);
    }

    return AJ_OK;
}

//AirRecirculationMode
AJ_Status OnGetIsRecirculating(const char* objPath, bool* isRecirculating)
{
    printf("OnGetIsRecirculating : %s\n", objPath);

    *isRecirculating = true;

    return AJ_OK;
}
AJ_Status OnSetIsRecirculating(const char* objPath, const bool isRecirculating)
{
    printf("OnSetIsRecirculating : %s, isRecirculating : %d\n", objPath, isRecirculating);

    return AJ_OK;
}

//AudioVideoInput
AJ_Status OnGetInputSourceId(const char* objPath, uint16_t* inputSourceId)
{
    printf("OnGetInputSourceId : %s\n", objPath);

    *inputSourceId = 2;

    return AJ_OK;
}
AJ_Status OnSetInputSourceId(const char* objPath, const uint16_t inputSourceId)
{
    printf("OnSetInputSourceId : %s, inputSourceId : %u\n", objPath, inputSourceId);

    return AJ_OK;
}
AJ_Status OnGetSupportedInputSources(const char* objPath, InputSource* supportedInputSources)
{
    printf("OnGetSupportedInputSources : %s\n", objPath);

    InputSource testInput[2] = {
        {1, 2, 3, 4, "WUSUCUMA"},
        {2, 3, 6, 4, "WUSUCUMA1"}
    };

    memcpy(supportedInputSources, testInput, sizeof(InputSource) * 2);

    return AJ_OK;
}

//AudioVolume
AJ_Status OnGetVolume(const char* objPath, uint8_t* volume)
{
    printf("OnGetVolume : %s\n", objPath);

    *volume = 6;

    return AJ_OK;
}
AJ_Status OnSetVolume(const char* objPath, const uint8_t volume)
{
    printf("OnSetVolume : %s %u\n", objPath, volume);

    return AJ_OK;
}
AJ_Status OnGetMaxVolume(const char* objPath, uint8_t* maxVolume)
{
    printf("OnGetMaxVolume : %s\n", objPath);

    *maxVolume = 10;

    return AJ_OK;
}
AJ_Status OnGetMute(const char* objPath, bool* mute)
{
    printf("OnGetMute : %s\n", objPath);

    *mute = true;

    return AJ_OK;
}
AJ_Status OnSetMute(const char* objPath, const bool mute)
{
    printf("OnSetMute : %s, mute : %d\n", objPath, mute);

    return AJ_OK;
}

//BatteryStatus
AJ_Status OnGetCurrentValue2(const char* objPath, uint8_t* currentValue)
{
    printf("OnGetCurrentValue2 : %s\n", objPath);

    *currentValue = 80;

    return AJ_OK;
}
AJ_Status OnGetIsCharging(const char* objPath, bool* isCharging)
{
    printf("OnGetIsCharging : %s\n", objPath);

    *isCharging = true;

    return AJ_OK;
}

//Channel
#define TOTAL_NUM_OF_CHANNEL    5
ChannelInfoRecord channels[TOTAL_NUM_OF_CHANNEL] = {
    {"1-1", "1", "aaa"},
    {"1-2", "2", "bbb"},
    {"1-3", "3", "ccc"},
    {"1-4", "4", "ddd"},
    {"1-5", "5", "eee"}
};

AJ_Status OnGetChannelId(const char* objPath, char* channelId)
{
    printf("OnGetChannelId : %s\n", objPath);

    if (channelId) {
        strncpy(channelId, channels[0].channelId, strlen(channels[0].channelId));
        channelId[strlen(channels[0].channelId)] = '\0';
    }

    return AJ_OK;
}
AJ_Status OnSetChannelId(const char* objPath, const char* channelId)
{
    int i;
    AJ_Status status = AJ_ERR_INVALID;

    printf("OnSetChannelId : %s %s\n", objPath, channelId);

    for (i = 0; i < TOTAL_NUM_OF_CHANNEL; i++) {
        if (strcmp(channels[i].channelId, channelId) == 0) {
            status = AJ_OK;
            break;
        }
    }
    return status;
}
AJ_Status OnGetTotalNumberOfChannels(const char* objPath, uint16_t* totalNumberOfChannels)
{
    printf("OnGetTotalNumberOfChannels : %s\n", objPath);

    *totalNumberOfChannels = TOTAL_NUM_OF_CHANNEL;

    return AJ_OK;
}
AJ_Status OnGetChannelList(const char* objPath, const uint16_t startingRecord, const uint16_t numRecords,
                           ChannelInfoRecord** listOfChannelInfoRecords, uint16_t* numReturnedRecords, ErrorCode* errorCode)
{
    int size = sizeof(channels) / sizeof(ChannelInfoRecord);
    printf("OnGetChannelList : %s, startingRecord : %u, numRecords : %u\n", objPath, startingRecord, numRecords);

    if (startingRecord >= size) {
        if (errorCode) {
            *errorCode = INVALID_VALUE;
        }
        return AJ_ERR_INVALID;
    }

    *listOfChannelInfoRecords = &channels[startingRecord];
    *numReturnedRecords = ((startingRecord + numRecords) <= size) ? numRecords : (size - startingRecord);

    return AJ_OK;
}

//ClimateControlMode
AJ_Status OnGetMode(const char* objPath, uint16_t* mode)
{
    printf("OnGetMode : %s\n", objPath);

    *mode = 1;

    return AJ_OK;
}
AJ_Status OnSetMode(const char* objPath, const uint16_t mode)
{
    printf("OnSetMode : %s, mode : %u\n", objPath, mode);

    return AJ_OK;
}
AJ_Status OnGetSupportedModes(const char* objPath, uint16_t* supportedModes)
{
    int i = 0;
    printf("OnGetSupportedModes : %s\n", objPath);

    for (i = 0 ; i < 3; i++) {
        supportedModes[i] = (uint16_t) i;
    }

    return AJ_OK;
}
AJ_Status OnGetOperationalState(const char* objPath, uint16_t* operationalState)
{
    printf("OnGetOperationalState : %s\n", objPath);

    *operationalState = 2;

    return AJ_OK;
}

//CurrentPower
AJ_Status OnGetCurrentPower(const char* objPath, double* currentPower)
{
    printf("OnGetCurrentPower : %s\n", objPath);

    *currentPower = 10.0;

    return AJ_OK;
}
AJ_Status OnGetPrecision2(const char* objPath, double* precision)
{
    printf("OnGetPrecision2 : %s\n", objPath);

    *precision = 0.5;

    return AJ_OK;
}
AJ_Status OnGetUpdateMinTime2(const char* objPath, uint16_t* updateMinTime)
{
    printf("OnGetUpdateMinTime2 : %s\n", objPath);

    *updateMinTime = 1;

    return AJ_OK;
}

//EnergyUsage
AJ_Status OnGetCumulativeEnergy(const char* objPath, double* cumulativeEnergy)
{
    printf("OnGetCurrentCumulativeEnergy : %s\n", objPath);

    *cumulativeEnergy = 80.0;

    return AJ_OK;
}
AJ_Status OnGetPrecision3(const char* objPath, double* precision)
{
    printf("OnGetPrecision3 : %s\n", objPath);

    *precision = 0.5;

    return AJ_OK;
}
AJ_Status OnGetUpdateMinTime3(const char* objPath, uint16_t* updateMinTime)
{
    printf("OnGetUpdateMinTime3 : %s\n", objPath);

    *updateMinTime = 1;

    return AJ_OK;
}
AJ_Status OnResetCumulativeEnergy(const char* objPath, ErrorCode* errorCode)
{
    printf("OnResetCumulativeEnergy : %s\n", objPath);

    return AJ_OK;
}

AJ_Status OnGetFanSpeedLevel(const char* objPath, uint8_t* fanSpeedLevel)
{
    printf("OnGetFanSpeedLevel : %s\n", objPath);

    *fanSpeedLevel = 3;

    return AJ_OK;
}

AJ_Status OnSetFanSpeedLevel(const char* objPath, const uint8_t fanSpeedLevel)
{
    printf("OnSetSanSpeedLevel : %s, fanSpeedLevel : %u\n", objPath, fanSpeedLevel);

    return AJ_OK;
}

AJ_Status OnGetMaxFanSpeedLevel(const char* objPath, uint8_t* maxFanSpeedLevel)
{
    printf("OnGetMaxFanSpeedLevel : %s\n", objPath);

    *maxFanSpeedLevel = 5;

    return AJ_OK;
}

AJ_Status OnGetAutoMode(const char* objPath, uint8_t* autoMode)
{
    printf("OnGetMaxFanSpeedLevel : %s\n", objPath);

    *autoMode = 1;

    return AJ_OK;
}

AJ_Status OnSetAutoMode(const char* objPath, const uint8_t autoMode)
{
    printf("OnSetAutoMode : %s, autoMode : %u\n", objPath, autoMode);

    return AJ_OK;
}

//OffControl
AJ_Status OnSwitchOff(const char* objPath, ErrorCode* errorCode)
{
    printf("OnSwitchOff : %s\n", objPath);

    return AJ_OK;
}

//OnControl
AJ_Status OnSwitchOn(const char* objPath, ErrorCode* errorCode)
{
    printf("OnSwitchOn : %s\n", objPath);

    return AJ_OK;
}

//OnOffStatus
AJ_Status OnGetOnOff(const char* objPath, bool* onOff)
{
    printf("OnGetOnOff : %s\n", objPath);

    *onOff = true;

    return AJ_OK;
}

//RapidMode
AJ_Status OnGetRapidMode(const char* objPath, bool* rapidMode)
{
    printf("OnGetRapidMode : %s\n", objPath);

    *rapidMode = true;

    return AJ_OK;
}
AJ_Status OnSetRapidMode(const char* objPath, const bool rapidMode)
{
    printf("OnSetRapidMode : %s, rapidMode : %d\n", objPath, rapidMode);

    return AJ_OK;
}

//RepeatMode
AJ_Status OnGetRepeatMode(const char* objPath, bool* repeatMode)
{
    printf("OnGetRepeatMode : %s\n", objPath);

    *repeatMode = true;

    return AJ_OK;
}
AJ_Status OnSetRepeatMode(const char* objPath, const bool repeatMode)
{
    printf("OnSetRepeatMode : %s, repeatMode : %d\n", objPath, repeatMode);

    return AJ_OK;
}

//ResourceSavingMode
AJ_Status OnGetResourceSavingMode(const char* objPath, bool* resourceSavingMode)
{
    printf("OnGetResourceSavingMode : %s\n", objPath);

    *resourceSavingMode = true;

    return AJ_OK;
}
AJ_Status OnSetResourceSavingMode(const char* objPath, const bool resourceSavingMode)
{
    printf("OnSetResourceSavingMode : %s, resourceSavingMode : %d\n", objPath, resourceSavingMode);

    return AJ_OK;
}

//RobotCleaningCyclePhase
CyclePhaseDescriptor description[2] = {
    {0x80, "aa", "aaa"},
    {0x81, "bb", "bbb"}
};

AJ_Status OnGetCyclePhase(const char* objPath, uint8_t* cyclePhase)
{
    printf("OnGetCyclePhase : %s\n", objPath);

    *cyclePhase = 1;

    return AJ_OK;
}

AJ_Status OnGetSupportedCyclePhases(const char* objPath, uint8_t* supportedCyclePhases)
{
    printf("OnGetSupportedCyclePhases : %s\n", objPath);

    supportedCyclePhases[0] = 0x00;
    supportedCyclePhases[1] = 0x01;
    supportedCyclePhases[2] = 0x80;
    supportedCyclePhases[3] = 0x81;

    return AJ_OK;
}

AJ_Status OnGetVendorPhasesDescription(const char* objPath, const char* languageTag, CyclePhaseDescriptor** phasesDescription,
                                       uint16_t* numReturnedRecords, ErrorCode* errorCode)
{
    printf("OnGetVendorPhasesDescription : %s, languageTag : %s\n", objPath, languageTag);

    if(!strncmp(languageTag, "en", strlen(languageTag))) {
        *phasesDescription = description;
        *numReturnedRecords = sizeof(description) / sizeof(CyclePhaseDescriptor);

        return AJ_OK;
    } else {
        if (errorCode) {
            *errorCode = LANGUAGE_NOT_SUPPORTED;
        }

        return AJ_ERR_FAILURE;
    }
}

/* SoilLevel */
AJ_Status OnSetSoilLevelTargetLevel(const char* objPath, const uint8_t targetLevel)
{
    printf("SoilLevel OnSetTargetLevel  : %s, targetLevel : %u\n", objPath, targetLevel);

    return AJ_OK;
}

/* SpinSpeedLevel */
AJ_Status OnSetSpinSpeedLevelTargetLevel(const char* objPath, const uint8_t targetLevel)
{
    printf("SpinSpeedLevel OnSetTargetLevel  : %s, targetLevel : %u\n", objPath, targetLevel);

    return AJ_OK;
}


AJ_Status OnExecuteOperationalCommand(const char* objPath, const uint8_t command, ErrorCode* errorCode)
{
    printf("CycleControl OnExecuteOperationalCommand - command: %u\n", command);

    if (errorCode) {
        *errorCode = NOT_ERROR;
    }

    return AJ_OK;
}

//MoistureOutputLevel
AJ_Status OnGetMoistureOutputLevel(const char* objPath, uint8_t* value)
{
    printf("OnGetMoistureOutputLevel : %s\n", objPath);

    *value = 3;

    return AJ_OK;
}
AJ_Status OnSetMoistureOutputLevel(const char* objPath, const uint8_t value)
{
    printf("OnSetMoistureOutputLevel : %s %u\n", objPath, value);

    return AJ_OK;
}
AJ_Status OnGetMaxMoistureOutputLevel(const char* objPath, uint8_t* value)
{
    printf("OnGetMaxMoistureOutputLevel : %s\n", objPath);

    *value = 8;

    return AJ_OK;
}

//FilterStatus
AJ_Status OnGetExpectedLifeInDays(const char* objPath, uint16_t* value)
{
    printf("OnGetExpectedLifeInDays : %s\n", objPath);

    *value = 15;

    return AJ_OK;
}

AJ_Status OnGetIsCleanable(const char* objPath, bool* isCleanable)
{
    printf("OnGetIsCleanable : %s\n", objPath);

    *isCleanable = false;

    return AJ_OK;
}

AJ_Status OnGetOrderPercentage(const char* objPath, uint8_t* value)
{
    printf("OnGetOrderPercentage : %s\n", objPath);

    *value = 20;

    return AJ_OK;
}

AJ_Status OnGetManufacturer(const char* objPath, char* manufacturer)
{
    const char mf[8] = "AllSeen";
    printf("OnGetManufacturer : %s\n", objPath);

    if (manufacturer) {
        strncpy(manufacturer, mf, strlen(mf));
        manufacturer[strlen(mf)] = '\0';
    }

    return AJ_OK;
}

AJ_Status OnGetPartNumber(const char* objPath, char* partNumber)
{
    const char pn[5] = "AAAA";
    printf("OnGetPartNumber : %s\n", objPath);

    if (partNumber) {
        strncpy(partNumber, pn, strlen(pn));
        partNumber[strlen(pn)] = '\0';
    }

    return AJ_OK;
}

AJ_Status OnGetUrl(const char* objPath, char* url)
{
    const char u[5] = "http";
    printf("OnGetUrl : %s\n", objPath);

    if (url) {
        strncpy(url, u, strlen(u));
        url[strlen(u)] = '\0';
    }

    return AJ_OK;
}

AJ_Status OnGetLifeRemaining(const char* objPath, uint8_t* value)
{
    printf("OnGetLifeRemaining : %s\n", objPath);

    *value = 80;

    return AJ_OK;
}

//RapidModeTimed
AJ_Status OnGetRapidModeMinutesRemaining(const char* objPath, uint16_t* value)
{
    printf("OnGetRapidModeMinutesRemaining : %s\n", objPath);

    *value = 10;

    return AJ_OK;
}

AJ_Status OnSetRapidModeMinutesRemaining(const char* objPath, const uint16_t value)
{
    printf("OnSetRapidModeMinutesRemaining : %s %u\n", objPath, value);

    return AJ_OK;
}

AJ_Status OnGetMaxSetMinutes(const char* objPath, uint16_t* value)
{
    printf("OnGetMaxSetMinutes : %s\n", objPath);

    *value = 90;

    return AJ_OK;
}

//TargetHumidity
AJ_Status TargetHumidity_OnSetTargetValue(const char* objPath, const uint8_t targetValue)
{
    printf("OnSetTargetValue : %s, targetValue: %d\n", objPath, targetValue);

    return AJ_OK;
}

//TargetTemperatureLevel
AJ_Status TargetTemperatureLevel_OnSetTargetLevel(const char* objPath, const uint8_t targetLevel)
{
    printf("OnSetTargetLevel : %s, targetLevel: %d\n", objPath, targetLevel);

    return AJ_OK;
}

AJ_Status InitHaeClosedStatusProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;

    bool isClosed = true;

    bool isClosedRead;

    /*if (status == AJ_OK)*/ {
        status = Hae_ClosedStatusInterfaceSetIsClosed(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, isClosed);
    }

    /*if (status == AJ_OK)*/ {
        status = Hae_ClosedStatusInterfaceGetIsClosed(HAE_OBJECT_PATH_CONTROLLEE, &isClosedRead);
        printf("ClosedStatus IsClosed : %d\n", isClosedRead);
    }

    return status;
}

AJ_Status InitHaeCycleControlProperties(AJ_BusAttachment* busAttachment)
{
    #define SUPPORTED_OPER_STATES_NUM   6
    #define SUPPORTED_OPER_COMMANDS_NUM 4

    AJ_Status status = AJ_OK;
    int i = 0;

    uint8_t operationalState = 0;
    const uint8_t supportedOperationalStates[SUPPORTED_OPER_STATES_NUM] = {0, 1, 2, 3, 4, 5};
    const uint8_t supportedOperationalCommands[SUPPORTED_OPER_COMMANDS_NUM] = {0, 1, 2, 3};

    uint8_t operationalStateRead;
    uint8_t supportedOperationalStatesRead[SUPPORTED_OPER_STATES_NUM];
    uint8_t supportedOperationalCommandsRead[SUPPORTED_OPER_COMMANDS_NUM];

    /* write in this order (operational state as last) to pass the validation */
    /*if (status == AJ_OK)*/ {
        status = Hae_CycleControlInterfaceSetOperationalState(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, operationalState);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_CycleControlInterfaceSetSupportedOperationalStates(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, supportedOperationalStates, SUPPORTED_OPER_STATES_NUM);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_CycleControlInterfaceSetSupportedOperationalCommands(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, supportedOperationalCommands, SUPPORTED_OPER_COMMANDS_NUM);
    }

    /*if (status == AJ_OK)*/ {
        status = Hae_CycleControlInterfaceGetOperationalState(HAE_OBJECT_PATH_CONTROLLEE, &operationalStateRead);
        printf("CycleControl OperationalState: %u\n", operationalStateRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_CycleControlInterfaceGetSupportedOperationalStates(HAE_OBJECT_PATH_CONTROLLEE, supportedOperationalStatesRead);
        printf("CycleControl SupportedOperationalStates : \n");
        for (i = 0; i < SUPPORTED_OPER_STATES_NUM; i++) {
            printf("%u\t", supportedOperationalStatesRead[i]);
        }
        printf("\n");
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_CycleControlInterfaceGetSupportedOperationalCommands(HAE_OBJECT_PATH_CONTROLLEE, supportedOperationalCommandsRead);
        printf("CycleControl SupportedOperationalCommands : \n");
        for (i = 0; i < SUPPORTED_OPER_COMMANDS_NUM; i++) {
            printf("%u\t", supportedOperationalCommandsRead[i]);
        }
        printf("\n");
    }

    #undef SUPPORTED_OPER_STATES_NUM
    #undef SUPPORTED_OPER_COMMANDS_NUM

    return status;
}

AJ_Status InitHaeDishWashingCyclePhaseProperties(AJ_BusAttachment* busAttachment)
{
    #define DISH_WASHING_SUPPORTED_CYCLEPHASE_NUM   5

    AJ_Status status = AJ_OK;
    int i = 0;

    uint8_t cyclePhase = 0;
    const uint8_t supportedCyclePhases[DISH_WASHING_SUPPORTED_CYCLEPHASE_NUM] = {0x00, 0x01, 0x02, 0x03, 0x04};

    uint8_t cyclePhaseRead;
    uint8_t supportedCyclePhasesRead[DISH_WASHING_SUPPORTED_CYCLEPHASE_NUM];

    /* write in this order (cycle phase as last) to pass the validation */
    /*if (status == AJ_OK)*/ {
        status = Hae_DishWashingCyclePhaseInterfaceSetSupportedCyclePhases(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, supportedCyclePhases, DISH_WASHING_SUPPORTED_CYCLEPHASE_NUM);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_DishWashingCyclePhaseInterfaceSetCyclePhase(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, cyclePhase);
    }

    /*if (status == AJ_OK)*/ {
        status = Hae_DishWashingCyclePhaseInterfaceGetCyclePhase(HAE_OBJECT_PATH_CONTROLLEE, &cyclePhaseRead);
        printf("DishWashingCyclePhase CyclePhase: %u\n", cyclePhaseRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_DishWashingCyclePhaseInterfaceGetSupportedCyclePhases(HAE_OBJECT_PATH_CONTROLLEE, supportedCyclePhasesRead);
        printf("DishWashingCyclePhase SupportedCyclePhases : \n");
        for (i = 0; i < DISH_WASHING_SUPPORTED_CYCLEPHASE_NUM; i++) {
            printf("%u\t", supportedCyclePhasesRead[i]);
        }
        printf("\n");
    }

    #undef DISH_WASHING_SUPPORTED_CYCLEPHASE_NUM

    return status;
}

AJ_Status InitHaeLaundryCyclePhaseProperties(AJ_BusAttachment* busAttachment)
{
    #define LAUNDRY_SUPPORTED_CYCLEPHASE_NUM   9

    AJ_Status status = AJ_OK;
    int i = 0;

    uint8_t cyclePhase = 0;
    const uint8_t supportedCyclePhases[LAUNDRY_SUPPORTED_CYCLEPHASE_NUM] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

    uint8_t cyclePhaseRead;
    uint8_t supportedCyclePhasesRead[LAUNDRY_SUPPORTED_CYCLEPHASE_NUM];

    /* write in this order (cycle phase as last) to pass the validation */
    /*if (status == AJ_OK)*/ {
        status = Hae_LaundryCyclePhaseInterfaceSetSupportedCyclePhases(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, supportedCyclePhases, LAUNDRY_SUPPORTED_CYCLEPHASE_NUM);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_LaundryCyclePhaseInterfaceSetCyclePhase(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, cyclePhase);
    }

    /*if (status == AJ_OK)*/ {
        status = Hae_LaundryCyclePhaseInterfaceGetCyclePhase(HAE_OBJECT_PATH_CONTROLLEE, &cyclePhaseRead);
        printf("LaundryCyclePhase CyclePhase: %u\n", cyclePhaseRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_LaundryCyclePhaseInterfaceGetSupportedCyclePhases(HAE_OBJECT_PATH_CONTROLLEE, supportedCyclePhasesRead);
        printf("LaundryCyclePhase SupportedCyclePhases : \n");
        for (i = 0; i < LAUNDRY_SUPPORTED_CYCLEPHASE_NUM; i++) {
            printf("%u\t", supportedCyclePhasesRead[i]);
        }
        printf("\n");
    }

    #undef LAUNDRY_SUPPORTED_CYCLEPHASE_NUM

    return status;
}

AJ_Status InitHaeOvenCyclePhaseProperties(AJ_BusAttachment* busAttachment)
{
    #define OVEN_SUPPORTED_CYCLEPHASE_NUM   4

    AJ_Status status = AJ_OK;
    int i = 0;

    uint8_t cyclePhase = 0;
    const uint8_t supportedCyclePhases[OVEN_SUPPORTED_CYCLEPHASE_NUM] = {0x00, 0x01, 0x02, 0x03};

    uint8_t cyclePhaseRead;
    uint8_t supportedCyclePhasesRead[OVEN_SUPPORTED_CYCLEPHASE_NUM];

    /* write in this order (cycle phase as last) to pass the validation */
    /*if (status == AJ_OK)*/ {
        status = Hae_OvenCyclePhaseInterfaceSetSupportedCyclePhases(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, supportedCyclePhases, OVEN_SUPPORTED_CYCLEPHASE_NUM);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_OvenCyclePhaseInterfaceSetCyclePhase(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, cyclePhase);
    }

    /*if (status == AJ_OK)*/ {
        status = Hae_OvenCyclePhaseInterfaceGetCyclePhase(HAE_OBJECT_PATH_CONTROLLEE, &cyclePhaseRead);
        printf("OvenCyclePhase CyclePhase: %u\n", cyclePhaseRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_OvenCyclePhaseInterfaceGetSupportedCyclePhases(HAE_OBJECT_PATH_CONTROLLEE, supportedCyclePhasesRead);
        printf("OvenCyclePhase SupportedCyclePhases : \n");
        for (i = 0; i < OVEN_SUPPORTED_CYCLEPHASE_NUM; i++) {
            printf("%u\t", supportedCyclePhasesRead[i]);
        }
        printf("\n");
    }

    #undef OVEN_SUPPORTED_CYCLEPHASE_NUM

    return status;
}

AJ_Status InitHaeHeatingZoneProperties(AJ_BusAttachment* busAttachment)
{
    #define HEATING_ZONES_NUM 5

    AJ_Status status = AJ_OK;
    int i = 0;

    uint8_t numberOfHeatingZones = HEATING_ZONES_NUM;
    const uint8_t maxHeatingLevels[HEATING_ZONES_NUM] = {10, 10, 10, 10, 10};
    const uint8_t heatingLevels[HEATING_ZONES_NUM] = {0, 1, 2, 3, 4};

    uint8_t numberOfHeatingZonesRead;
    uint8_t maxHeatingLevelsRead[HEATING_ZONES_NUM];
    uint8_t heatingLevelsRead[HEATING_ZONES_NUM];


    /* write in this order (heating levels as last) to pass the validation */
    /*if (status == AJ_OK)*/ {
        status = Hae_HeatingZoneInterfaceSetNumberOfHeatingZones(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, numberOfHeatingZones);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_HeatingZoneInterfaceSetMaxHeatingLevels(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, maxHeatingLevels, HEATING_ZONES_NUM);
    }

    /*if (status == AJ_OK)*/ {
        status = Hae_HeatingZoneInterfaceSetHeatingLevels(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, heatingLevels, HEATING_ZONES_NUM);
    }

    /*if (status == AJ_OK)*/ {
        status = Hae_HeatingZoneInterfaceGetNumberOfHeatingZones(HAE_OBJECT_PATH_CONTROLLEE, &numberOfHeatingZonesRead);
        printf("HeatingZone NumberOfHeatingZonesRead: %u\n", numberOfHeatingZonesRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_HeatingZoneInterfaceGetMaxHeatingLevels(HAE_OBJECT_PATH_CONTROLLEE, maxHeatingLevelsRead);
        printf("HeatingZone MaxHeatingLevels : \n");
        for (i = 0; i < HEATING_ZONES_NUM; i++) {
            printf("%u\t", maxHeatingLevelsRead[i]);
        }
        printf("\n");
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_HeatingZoneInterfaceGetHeatingLevels(HAE_OBJECT_PATH_CONTROLLEE, heatingLevelsRead);
        printf("HeatingZone HeatingLevels : \n");
        for (i = 0; i < HEATING_ZONES_NUM; i++) {
            printf("%u\t", heatingLevelsRead[i]);
        }
        printf("\n");
    }

    #undef HEATING_ZONES_NUM

    return status;
}

AJ_Status InitHaeRapidModeProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;

    bool rapidMode = true;

    bool rapidModeRead;

    /*if (status == AJ_OK)*/ {
        status = Hae_RapidModeInterfaceSetRapidMode(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, rapidMode);
    }

    /*if (status == AJ_OK)*/ {
        status = Hae_RapidModeInterfaceGetRapidMode(HAE_OBJECT_PATH_CONTROLLEE, &rapidModeRead);
        printf("RapidMode : %d\n", rapidModeRead);
    }

    return status;
}

AJ_Status InitHaeRemoteControllabilityProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;

    bool isControllable = true;

    bool isControllableRead;

    /*if (status == AJ_OK)*/ {
        status = Hae_RemoteControllabilityInterfaceSetIsControllable(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, isControllable);
    }

    /*if (status == AJ_OK)*/ {
        status = Hae_RemoteControllabilityInterfaceGetIsControllable(HAE_OBJECT_PATH_CONTROLLEE, &isControllableRead);
        printf("RemoteControllability IsClosed : %d\n", isControllableRead);
    }

    return status;
}

AJ_Status InitHaeSoilLevelProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    int i = 0;

    uint8_t maxLevel = 5;
    uint8_t targetLevel = 0;
    const uint8_t selectableLevels[] = { 0, 1, 2 };

    uint8_t maxLevelRead;
    uint8_t targetLevelRead;
    uint8_t selectableLevelsRead[3];

    /* write in this order (target level as last) to pass the validation */
    /*if (status == AJ_OK)*/ {
        status = Hae_SoilLevelInterfaceSetMaxLevel(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, maxLevel);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_SoilLevelInterfaceSetSelectableLevels(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, selectableLevels, 3);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_SoilLevelInterfaceSetTargetLevel(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, targetLevel);
    }

    /*if (status == AJ_OK)*/ {
        status = Hae_SoilLevelInterfaceGetMaxLevel(HAE_OBJECT_PATH_CONTROLLEE, &maxLevelRead);
        printf("SoilLevel MaxLevel: %u\n", maxLevelRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_SoilLevelInterfaceGetTargetLevel(HAE_OBJECT_PATH_CONTROLLEE, &targetLevelRead);
        printf("SoilLevel TargetLevel : %u\n", targetLevelRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_SoilLevelInterfaceGetSelectableLevels(HAE_OBJECT_PATH_CONTROLLEE, selectableLevelsRead);
        printf("SoilLevel SelectableLevels : \n");
        for (i = 0; i < 3; i++) {
            printf("%u\t", selectableLevelsRead[i]);
        }
        printf("\n");
    }

    return status;
}

AJ_Status InitHaeSpinSpeedLevelProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    int i = 0;

    uint8_t maxLevel = 5;
    uint8_t targetLevel = 0;
    const uint8_t selectableLevels[] = { 0, 1, 2 };

    uint8_t maxLevelRead;
    uint8_t targetLevelRead;
    uint8_t selectableLevelsRead[3];

    /* write in this order (target level as last) to pass the validation */
    /*if (status == AJ_OK)*/ {
        status = Hae_SpinSpeedLevelInterfaceSetMaxLevel(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, maxLevel);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_SpinSpeedLevelInterfaceSetSelectableLevels(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, selectableLevels, 3);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_SpinSpeedLevelInterfaceSetTargetLevel(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, targetLevel);
    }

    /*if (status == AJ_OK)*/ {
        status = Hae_SpinSpeedLevelInterfaceGetMaxLevel(HAE_OBJECT_PATH_CONTROLLEE, &maxLevelRead);
        printf("SpinSpeedLevel MaxLevel: %u\n", maxLevelRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_SpinSpeedLevelInterfaceGetTargetLevel(HAE_OBJECT_PATH_CONTROLLEE, &targetLevelRead);
        printf("SpinSpeedLevel TargetLevel : %u\n", targetLevelRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_SpinSpeedLevelInterfaceGetSelectableLevels(HAE_OBJECT_PATH_CONTROLLEE, selectableLevelsRead);
        printf("SpinSpeedLevel SelectableLevels : \n");
        for (i = 0; i < 3; i++) {
            printf("%u\t", selectableLevelsRead[i]);
        }
        printf("\n");
    }

    return status;
}

AJ_Status InitHaeWaterLevelProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;

    uint8_t supplySource = 0;
    uint8_t currentLevel = 5;
    uint8_t maxLevel = 10;

    uint8_t supplySourceRead;
    uint8_t currentLevelRead;
    uint8_t maxLevelRead;

    /* write in this order (current level as last) to pass the validation */
    /*if (status == AJ_OK)*/ {
        status = Hae_WaterLevelInterfaceSetSupplySource(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, supplySource);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_WaterLevelInterfaceSetMaxLevel(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, maxLevel);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_WaterLevelInterfaceSetCurrentLevel(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, currentLevel);
    }

    /*if (status == AJ_OK)*/ {
        status = Hae_WaterLevelInterfaceGetSupplySource(HAE_OBJECT_PATH_CONTROLLEE, &supplySourceRead);
        printf("Water Supply Source: %u\n", supplySourceRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_WaterLevelInterfaceGetCurrentLevel(HAE_OBJECT_PATH_CONTROLLEE, &currentLevelRead);
        printf("Water Current Level: %u\n", currentLevelRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_WaterLevelInterfaceGetMaxLevel(HAE_OBJECT_PATH_CONTROLLEE, &maxLevelRead);
        printf("Water Max Level : %u\n", maxLevelRead);
    }

    return status;
}

AJ_Status InitHaeTimerProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;

    int32_t referenceTimer = 0;
    int32_t targetTimeToStart = 0;
    int32_t targetTimeToStop = 0;
    int32_t estimatedTimeToEnd = 0;
    int32_t runningTime = 0;
    int32_t targetDuration = 0;

    int32_t referenceTimerRead;
    int32_t targetTimeToStartRead;
    int32_t targetTimeToStopRead;
    int32_t estimatedTimeToEndRead;
    int32_t runningTimeRead;
    int32_t targetDurationRead;

    /*if (status == AJ_OK)*/ {
        status = Hae_TimerInterfaceSetReferenceTimer(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, referenceTimer);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_TimerInterfaceSetTargetTimeToStart(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, targetTimeToStart);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_TimerInterfaceSetTargetTimeToStop(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, targetTimeToStop);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_TimerInterfaceSetEstimatedTimeToEnd(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, estimatedTimeToEnd);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_TimerInterfaceSetRunningTime(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, runningTime);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_TimerInterfaceSetTargetDuration(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, targetDuration);
    }

    /*if (status == AJ_OK)*/ {
        status = Hae_TimerInterfaceGetReferenceTimer(HAE_OBJECT_PATH_CONTROLLEE, &referenceTimerRead);
        printf("Timer ReferenceTimer : %i\n", referenceTimerRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_TimerInterfaceGetTargetTimeToStart(HAE_OBJECT_PATH_CONTROLLEE, &targetTimeToStartRead);
        printf("Timer TargetTimeToStart : %i\n", targetTimeToStartRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_TimerInterfaceGetTargetTimeToStop(HAE_OBJECT_PATH_CONTROLLEE, &targetTimeToStopRead);
        printf("Timer TargetTimeToStop : %i\n", targetTimeToStopRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_TimerInterfaceGetEstimatedTimeToEnd(HAE_OBJECT_PATH_CONTROLLEE, &estimatedTimeToEndRead);
        printf("Timer EstimatedTimeToEnd : %i\n", estimatedTimeToEndRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_TimerInterfaceGetRunningTime(HAE_OBJECT_PATH_CONTROLLEE, &runningTimeRead);
        printf("Timer RunningTime : %i\n", runningTimeRead);
    }
    /*if (status == AJ_OK)*/ {
        status = Hae_TimerInterfaceGetTargetDuration(HAE_OBJECT_PATH_CONTROLLEE, &targetDurationRead);
        printf("Timer TargetDuration : %i\n", targetDurationRead);
    }

    return status;
}


AJ_Status InitHaeProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    int i = 0;
    double precision = 0.5;
    double precisionRead = 0.0;
    uint16_t updateMinTime = 1;
    uint16_t updateMinTimeRead = 0;
    double temperature = 3.0;
    double temperatureMin = 1.0;
    double temperatureMax = 7.0;
    double temperatureStep = 0.5;
    double temperatureRead = 0.0;
    uint16_t horizontalMax = 3;
    uint16_t horizontalDirection = 2;
    uint16_t verticalMax = 5;
    uint16_t verticalDirection = 1;
    uint16_t directionRead = 0;
    uint8_t autoMode = 1;
    uint8_t autoModeRead = 0;
    const SupportedInputEvent events[3] = {
        {1,2,4,10},
        {2,4,3,10},
        {4,5,4,20}
    };
    SupportedInputEvent eventsRead[3];
    bool boolRead = false;
    const InputSource inputs[2] = {
        {1, 2, 3, 4, "WUSUCUMA"},
        {2, 3, 6, 4, "WUSUCUMA1"}
    };
    InputSource inputsRead[2];
    uint16_t inputSourceId = 1;
    uint16_t inputSourceIdRead = 0;
    uint8_t volMax = 8;
    uint8_t vol = 3;
    uint8_t volRead = 0;
    uint8_t battery = 70;
    uint8_t batteryRead = 0;
    char channelId[81] = "1-1";
    char channelIdBuf[128];
    uint16_t numOfChannels = TOTAL_NUM_OF_CHANNEL;
    uint16_t numOfChannelsRead = 0;
    const uint16_t supportedModes[3] = { 0, 1, 2 };
    uint16_t supportedModesRead[3];
    uint16_t mode = 2;
    uint16_t modeRead = 0;
    uint16_t operState = 1;
    uint16_t operStateRead = 0;
    double currentPower = 8.0;
    double currentPowerRead = 0.0;
    double cumulativeEnergy = 70.0;
    double cumulativeEnergyRead = 0.0;
    uint8_t maxFanSpeedLevel = 10;
    uint8_t fanSpeedLevel = 3;
    uint8_t levelRead = 0;
    uint8_t supportedCyclePhases[4] = { 0x00, 0x01, 0x80, 0x81 };
    uint8_t supportedCyclePhasesRead[4];
    uint8_t cyclePhase = 1;
    uint8_t cyclePhaseRead = 0;
    uint8_t moisture = 1;
    uint8_t maxMoisture = 10;
    uint8_t moistureRead = 0;
    uint16_t expectedLife = 30;
    uint16_t expectedLifeRead = 0;
    bool isCleanable = true;
    uint8_t orderPercentage = 10;
    uint8_t orderPercentageRead = 0;
    char manufacturer[10] = "HAE";
    char partNumber[10] = "A1";
    char url[10] = "http://";
    char strBuf[10];
    uint8_t lifeRemaining = 100;
    uint8_t lifeRemainingRead = 0;
    uint8_t aq_contaminantType = 1;
    uint8_t aq_contaminantTypeRead = 0;
    double aq_currentValue = 10.0;
    double aq_currentValueRead = 0.0;
    double aq_maxValue = 30.0;
    double aq_maxValueRead = 0.0;
    double aq_minValue = 1.0;
    double aq_minValueRead = 0.0;
    double aq_precision = 1.0;
    double aq_precisionRead = 0.0;
    uint16_t aq_updateMinTime = 1;
    uint16_t aq_updateMinTimeRead = 0;
    uint8_t aql_currentLevel = 10;
    uint8_t aql_currentLevelRead = 0;
    uint8_t aql_maxLevel = 30;
    uint8_t aql_maxLevelRead = 0;
    uint16_t rapidModeMinutesRemaining = 15;
    uint16_t maxSetMinutes = 60;
    uint16_t minutesRead = 0;

    status = Hae_CurrentAirQualityInterfaceSetContaminantType(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, aq_contaminantType);
    status = Hae_CurrentAirQualityInterfaceGetContaminantType(HAE_OBJECT_PATH_CONTROLLEE, &aq_contaminantTypeRead);
    printf("Current Air Quality Contaminant Type Read : %d\n", (int)aq_contaminantTypeRead);

    status = Hae_CurrentAirQualityInterfaceSetCurrentValue(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, aq_currentValue);
    status = Hae_CurrentAirQualityInterfaceGetCurrentValue(HAE_OBJECT_PATH_CONTROLLEE, &aq_currentValueRead);
    printf("Current Air Quality Current Value Read : %f\n", aq_currentValueRead);

    status = Hae_CurrentAirQualityInterfaceSetMaxValue(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, aq_maxValue);
    status = Hae_CurrentAirQualityInterfaceGetMaxValue(HAE_OBJECT_PATH_CONTROLLEE, &aq_maxValueRead);
    printf("Current Air Quality Max Value Read : %f\n", aq_maxValueRead);

    status = Hae_CurrentAirQualityInterfaceSetMinValue(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, aq_minValue);
    status = Hae_CurrentAirQualityInterfaceGetMinValue(HAE_OBJECT_PATH_CONTROLLEE, &aq_minValueRead);
    printf("Current Air Quality Min Value Read : %f\n", aq_minValueRead);

    status = Hae_CurrentAirQualityInterfaceSetPrecision(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, aq_precision);
    status = Hae_CurrentAirQualityInterfaceGetPrecision(HAE_OBJECT_PATH_CONTROLLEE, &aq_precisionRead);
    printf("Precision Read : %f\n", aq_precisionRead);

    status = Hae_CurrentAirQualityInterfaceSetUpdateMinTime(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, aq_updateMinTime);
    status = Hae_CurrentAirQualityInterfaceGetUpdateMinTime(HAE_OBJECT_PATH_CONTROLLEE, &aq_updateMinTimeRead);
    printf("UpdateMinTime Read : %u\n", aq_updateMinTimeRead);

    status = Hae_CurrentAirQualityLevelInterfaceSetContaminantType(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, aq_contaminantType);
    status = Hae_CurrentAirQualityLevelInterfaceGetContaminantType(HAE_OBJECT_PATH_CONTROLLEE, &aq_contaminantTypeRead);
    printf("Current Air Quality Level Contaminant Type Read : %d\n", (int)aq_contaminantTypeRead);

    status = Hae_CurrentAirQualityLevelInterfaceSetCurrentLevel(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, aql_currentLevel);
    status = Hae_CurrentAirQualityLevelInterfaceGetCurrentLevel(HAE_OBJECT_PATH_CONTROLLEE, &aql_currentLevelRead);
    printf("Current Air Quality Level Current Value Read : %d\n", (int)aql_currentLevelRead);

    status = Hae_CurrentAirQualityLevelInterfaceSetMaxLevel(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, aql_maxLevel);
    status = Hae_CurrentAirQualityLevelInterfaceGetMaxLevel(HAE_OBJECT_PATH_CONTROLLEE, &aql_maxLevelRead);
    printf("Current Air Quality Level Max Value Read : %d\n", (int)aql_maxLevelRead);

    status = Hae_CurrentTemperatureInterfaceSetCurrentValue(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, temperature);
    status = Hae_CurrentTemperatureInterfaceGetCurrentValue(HAE_OBJECT_PATH_CONTROLLEE, &temperatureRead);
    printf("Current Temperature Read : %f\n", temperatureRead);

    status = Hae_CurrentTemperatureInterfaceSetPrecision(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, precision);
    status = Hae_CurrentTemperatureInterfaceGetPrecision(HAE_OBJECT_PATH_CONTROLLEE, &precisionRead);
    printf("Precision Read : %f\n", precisionRead);

    status = Hae_CurrentTemperatureInterfaceSetUpdateMinTime(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, updateMinTime);
    status = Hae_CurrentTemperatureInterfaceGetUpdateMinTime(HAE_OBJECT_PATH_CONTROLLEE, &updateMinTimeRead);
    printf("UpdateMinTime Read : %u\n", updateMinTimeRead);

    status = Hae_TargetTemperatureInterfaceSetMinValue(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, temperatureMin);
    status = Hae_TargetTemperatureInterfaceGetMinValue(HAE_OBJECT_PATH_CONTROLLEE, &temperatureRead);
    printf("Target Temperature Min Read : %f\n", temperatureRead);

    status = Hae_TargetTemperatureInterfaceSetMaxValue(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, temperatureMax);
    status = Hae_TargetTemperatureInterfaceGetMaxValue(HAE_OBJECT_PATH_CONTROLLEE, &temperatureRead);
    printf("Target Temperature Max Read : %f\n", temperatureRead);

    status = Hae_TargetTemperatureInterfaceSetStepValue(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, temperatureStep);
    status = Hae_TargetTemperatureInterfaceGetStepValue(HAE_OBJECT_PATH_CONTROLLEE, &temperatureRead);
    printf("Target Temperature Step Read : %f\n", temperatureRead);

    status = Hae_TargetTemperatureInterfaceSetTargetValue(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, temperature+1.0);
    status = Hae_TargetTemperatureInterfaceGetTargetValue(HAE_OBJECT_PATH_CONTROLLEE, &temperatureRead);
    printf("Target Temperature Read : %f\n", temperatureRead);

    status = Hae_WindDirectionInterfaceSetHorizontalMax(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, horizontalMax);
    status = Hae_WindDirectionInterfaceGetHorizontalMax(HAE_OBJECT_PATH_CONTROLLEE, &directionRead);
    printf("Horizontal Max Read : %u\n", directionRead);

    status = Hae_WindDirectionInterfaceSetHorizontalDirection(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, horizontalDirection);
    status = Hae_WindDirectionInterfaceGetHorizontalDirection(HAE_OBJECT_PATH_CONTROLLEE, &directionRead);
    printf("Horizontal Direction Read : %u\n", directionRead);

    status = Hae_WindDirectionInterfaceSetHorizontalAutoMode(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, autoMode);
    status = Hae_WindDirectionInterfaceGetHorizontalAutoMode(HAE_OBJECT_PATH_CONTROLLEE, &autoModeRead);
    printf("Horizontal AutoMode Read : %u\n", autoModeRead);

    status = Hae_WindDirectionInterfaceSetVerticalMax(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, verticalMax);
    status = Hae_WindDirectionInterfaceGetVerticalMax(HAE_OBJECT_PATH_CONTROLLEE, &directionRead);
    printf("Vertical Max Read : %u\n", directionRead);

    status = Hae_WindDirectionInterfaceSetVerticalDirection(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, verticalDirection);
    status = Hae_WindDirectionInterfaceGetVerticalDirection(HAE_OBJECT_PATH_CONTROLLEE, &directionRead);
    printf("Vertical Direction Read : %u\n", directionRead);

    status = Hae_WindDirectionInterfaceSetVerticalAutoMode(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, autoMode);
    status = Hae_WindDirectionInterfaceGetVerticalAutoMode(HAE_OBJECT_PATH_CONTROLLEE, &autoModeRead);
    printf("Vertical AutoMode Read : %u\n", autoModeRead);

    status = Hae_HidInterfaceSetSupportedEvents(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, events, 3);
    status = Hae_HidInterfaceGetSupportedEvents(HAE_OBJECT_PATH_CONTROLLEE, eventsRead);
    printf("Hid SupportedEvents Read\n");
    for (i = 0; i < 3; i++) {
        printf("%u %u %i %i\n", eventsRead[i].type, eventsRead[i].code, eventsRead[i].min, eventsRead[i].max);
    }

    status = Hae_AirRecirculationModeInterfaceSetIsRecirculating(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, true);
    status = Hae_AirRecirculationModeInterfaceGetIsRecirculating(HAE_OBJECT_PATH_CONTROLLEE, &boolRead);
    printf("IsRecirculatingRead : %d\n", boolRead);

    status = Hae_AudioVideoInputInterfaceSetSupportedInputSources(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, inputs, 2);
    status = Hae_AudioVideoInputInterfaceGetSupportedInputSources(HAE_OBJECT_PATH_CONTROLLEE, inputsRead);
    printf("SupportedInputs Read\n");
    for (i = 0; i < 2; i++) {
        printf("%u %u %u %u %s\n", inputsRead[i].id, inputsRead[i].sourceType, inputsRead[i].detectedState,
                                   inputsRead[i].portNumber, inputsRead[i].friendlyName);
    }

    status = Hae_AudioVideoInputInterfaceSetInputSourceId(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, inputSourceId);
    status = Hae_AudioVideoInputInterfaceGetInputSourceId(HAE_OBJECT_PATH_CONTROLLEE, &inputSourceIdRead);
    printf("inputSourceId Read : %u\n", inputSourceIdRead);

    status = Hae_AudioVolumeInterfaceSetMaxVolume(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, volMax);
    status = Hae_AudioVolumeInterfaceGetMaxVolume(HAE_OBJECT_PATH_CONTROLLEE, &volRead);
    printf("VolumeMax Read: %u\n", volRead);

    status = Hae_AudioVolumeInterfaceSetVolume(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, vol);
    status = Hae_AudioVolumeInterfaceGetVolume(HAE_OBJECT_PATH_CONTROLLEE, &volRead);
    printf("Volume Read: %u\n", volRead);

    status = Hae_AudioVolumeInterfaceSetMute(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, true);
    status = Hae_AudioVolumeInterfaceGetMute(HAE_OBJECT_PATH_CONTROLLEE, &boolRead);
    printf("Mute Read: %d\n", boolRead);

    status = Hae_BatteryStatusInterfaceSetCurrentValue(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, battery);
    status = Hae_BatteryStatusInterfaceGetCurrentValue(HAE_OBJECT_PATH_CONTROLLEE, &batteryRead);
    printf("Battery CurrentValue Read: %u\n", batteryRead);

    status = Hae_BatteryStatusInterfaceSetIsCharging(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, true);
    status = Hae_BatteryStatusInterfaceGetIsCharging(HAE_OBJECT_PATH_CONTROLLEE, &boolRead);
    printf("IsCharing Read: %d\n", boolRead);

    status = Hae_ChannelInterfaceSetChannelId(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, channelId);
    status = Hae_ChannelInterfaceGetChannelId(HAE_OBJECT_PATH_CONTROLLEE, channelIdBuf);
    printf("ChannelId Read : %s\n", channelIdBuf);

    status = Hae_ChannelInterfaceSetTotalNumberOfChannels(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, numOfChannels);
    status = Hae_ChannelInterfaceGetTotalNumberOfChannels(HAE_OBJECT_PATH_CONTROLLEE, &numOfChannelsRead);
    printf("NumOfChannels Read : %u\n", numOfChannelsRead);

    status = Hae_ClimateControlModeInterfaceSetSupportedModes(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, supportedModes, 3);
    status = Hae_ClimateControlModeInterfaceGetSupportedModes(HAE_OBJECT_PATH_CONTROLLEE, supportedModesRead);
    printf("ClimateControlMode SupportedModes Read\n");
    for (i = 0; i < 3; i++) {
        printf("%u\t", supportedModesRead[i]);
    }
    printf("\n");

    status = Hae_ClimateControlModeInterfaceSetMode(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, mode);
    status = Hae_ClimateControlModeInterfaceGetMode(HAE_OBJECT_PATH_CONTROLLEE, &modeRead);
    printf("ClimateControlMode Mode Read : %u\n", modeRead);

    status = Hae_ClimateControlModeInterfaceSetOperationalState(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, operState);
    status = Hae_ClimateControlModeInterfaceGetOperationalState(HAE_OBJECT_PATH_CONTROLLEE, &operStateRead);
    printf("ClimateControlMode OperationalState Read : %u\n", operStateRead);

    status = Hae_CurrentPowerInterfaceSetCurrentPower(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, currentPower);
    status = Hae_CurrentPowerInterfaceGetCurrentPower(HAE_OBJECT_PATH_CONTROLLEE, &currentPowerRead);
    printf("CurrentPower Read : %f\n", currentPowerRead);

    status = Hae_CurrentPowerInterfaceSetPrecision(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, precision);
    status = Hae_CurrentPowerInterfaceGetPrecision(HAE_OBJECT_PATH_CONTROLLEE, &precisionRead);
    printf("Precision Read : %f\n", precisionRead);

    status = Hae_CurrentPowerInterfaceSetUpdateMinTime(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, updateMinTime);
    status = Hae_CurrentPowerInterfaceGetUpdateMinTime(HAE_OBJECT_PATH_CONTROLLEE, &updateMinTimeRead);
    printf("UpdateMinTime Read : %u\n", updateMinTimeRead);

    status = Hae_EnergyUsageInterfaceSetCumulativeEnergy(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, cumulativeEnergy);
    status = Hae_EnergyUsageInterfaceGetCumulativeEnergy(HAE_OBJECT_PATH_CONTROLLEE, &cumulativeEnergyRead);
    printf("CumulativeEnergy Read : %f\n", cumulativeEnergyRead);

    status = Hae_EnergyUsageInterfaceSetPrecision(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, precision);
    status = Hae_EnergyUsageInterfaceGetPrecision(HAE_OBJECT_PATH_CONTROLLEE, &precisionRead);
    printf("Precision Read : %f\n", precisionRead);

    status = Hae_EnergyUsageInterfaceSetUpdateMinTime(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, updateMinTime);
    status = Hae_EnergyUsageInterfaceGetUpdateMinTime(HAE_OBJECT_PATH_CONTROLLEE, &updateMinTimeRead);
    printf("UpdateMinTime Read : %u\n", updateMinTimeRead);

    status = Hae_FanSpeedLevelInterfaceSetMaxFanSpeedLevel(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, maxFanSpeedLevel);
    status = Hae_FanSpeedLevelInterfaceGetMaxFanSpeedLevel(HAE_OBJECT_PATH_CONTROLLEE, &levelRead);
    printf("MaxFanSpeedLevel Read : %u\n", levelRead);

    status = Hae_FanSpeedLevelInterfaceSetFanSpeedLevel(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, fanSpeedLevel);
    status = Hae_FanSpeedLevelInterfaceGetFanSpeedLevel(HAE_OBJECT_PATH_CONTROLLEE, &levelRead);
    printf("FanSpeedLevel Read : %u\n", levelRead);

    status = Hae_FanSpeedLevelInterfaceSetAutoMode(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, autoMode);
    status = Hae_FanSpeedLevelInterfaceGetAutoMode(HAE_OBJECT_PATH_CONTROLLEE, &autoModeRead);
    printf("AutoMode Read : %u\n", autoModeRead);

    status = Hae_OnOffStatusInterfaceSetOnOff(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, true);
    status = Hae_OnOffStatusInterfaceGetOnOff(HAE_OBJECT_PATH_CONTROLLEE, &boolRead);
    printf("OnOff Read : %d\n", boolRead);

    status = Hae_RepeatModeInterfaceSetRepeatMode(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, true);
    status = Hae_RepeatModeInterfaceGetRepeatMode(HAE_OBJECT_PATH_CONTROLLEE, &boolRead);
    printf("RepeatMode Read : %d\n", boolRead);

    status = Hae_ResourceSavingInterfaceSetResourceSavingMode(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, true);
    status = Hae_ResourceSavingInterfaceGetResourceSavingMode(HAE_OBJECT_PATH_CONTROLLEE, &boolRead);
    printf("ResourceSavingMode Read : %d\n", boolRead);

    status = Hae_RobotCleaningCyclePhaseInterfaceSetSupportedCyclePhases(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, supportedCyclePhases, 4);
    status = Hae_RobotCleaningCyclePhaseInterfaceGetSupportedCyclePhases(HAE_OBJECT_PATH_CONTROLLEE, supportedCyclePhasesRead);
    printf("RobotCleaningCyclePhase SupportedCyclePhases Read\n");
    for (i = 0; i < 4; i++) {
        printf("%u\t", supportedCyclePhasesRead[i]);
    }
    printf("\n");

    status = Hae_RobotCleaningCyclePhaseInterfaceSetCyclePhase(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, cyclePhase);
    status = Hae_RobotCleaningCyclePhaseInterfaceGetCyclePhase(HAE_OBJECT_PATH_CONTROLLEE, &cyclePhaseRead);
    printf("RobotCleaningCyclePhase CyclePhase Read : %u\n", cyclePhaseRead);

    status = Hae_MoistureOutputLevelInterfaceSetMaxMoistureOutputLevel(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, maxMoisture);
    status = Hae_MoistureOutputLevelInterfaceGetMaxMoistureOutputLevel(HAE_OBJECT_PATH_CONTROLLEE, &moistureRead);
    printf("MaxMoistureOutputLevel Read: %u\n", moistureRead);

    status = Hae_MoistureOutputLevelInterfaceSetMoistureOutputLevel(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, moisture);
    status = Hae_MoistureOutputLevelInterfaceGetMoistureOutputLevel(HAE_OBJECT_PATH_CONTROLLEE, &moistureRead);
    printf("MoistureOutputLevel Read: %u\n", moistureRead);

    status = Hae_MoistureOutputLevelInterfaceSetAutoMode(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, true);
    status = Hae_MoistureOutputLevelInterfaceGetAutoMode(HAE_OBJECT_PATH_CONTROLLEE, &autoModeRead);
    printf("AutoMode Read: %u\n", autoModeRead);

    status = Hae_FilterStatusInterfaceSetExpectedLifeInDays(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, expectedLife);
    status = Hae_FilterStatusInterfaceGetExpectedLifeInDays(HAE_OBJECT_PATH_CONTROLLEE, &expectedLifeRead);
    printf("ExpectedLifeInDays Read: %u\n", expectedLifeRead);

    status = Hae_FilterStatusInterfaceSetIsCleanable(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, isCleanable);
    status = Hae_FilterStatusInterfaceGetIsCleanable(HAE_OBJECT_PATH_CONTROLLEE, &boolRead);
    printf("IsCleanable Read: %d\n", boolRead);

    status = Hae_FilterStatusInterfaceSetOrderPercentage(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, orderPercentage);
    status = Hae_FilterStatusInterfaceGetOrderPercentage(HAE_OBJECT_PATH_CONTROLLEE, &orderPercentageRead);
    printf("OrderPercentage Read : %u\n", orderPercentageRead);

    status = Hae_FilterStatusInterfaceSetManufacturer(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, manufacturer);
    status = Hae_FilterStatusInterfaceGetManufacturer(HAE_OBJECT_PATH_CONTROLLEE, strBuf);
    printf("Manufacturer Read : %s\n", strBuf);

    status = Hae_FilterStatusInterfaceSetPartNumber(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, partNumber);
    status = Hae_FilterStatusInterfaceGetPartNumber(HAE_OBJECT_PATH_CONTROLLEE, strBuf);
    printf("PartNumber Read : %s\n", strBuf);

    status = Hae_FilterStatusInterfaceSetUrl(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, url);
    status = Hae_FilterStatusInterfaceGetUrl(HAE_OBJECT_PATH_CONTROLLEE, strBuf);
    printf("Url Read : %s\n", strBuf);

    status = Hae_FilterStatusInterfaceSetLifeRemaining(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, lifeRemaining);
    status = Hae_FilterStatusInterfaceGetLifeRemaining(HAE_OBJECT_PATH_CONTROLLEE, &lifeRemainingRead);
    printf("LifeRemaining Read : %u\n", lifeRemainingRead);

    /* if (status == AJ_OK) */ {
        status = InitHaeClosedStatusProperties(busAttachment);
    }

    /* if (status == AJ_OK) */ {
        status = InitHaeCycleControlProperties(busAttachment);
    }

    /* if (status == AJ_OK) */ {
        status = InitHaeDishWashingCyclePhaseProperties(busAttachment);
    }

    /* if (status == AJ_OK) */ {
        status = InitHaeLaundryCyclePhaseProperties(busAttachment);
    }

    /* if (status == AJ_OK) */ {
        status = InitHaeOvenCyclePhaseProperties(busAttachment);
    }

    /* if (status == AJ_OK) */ {
        status = InitHaeHeatingZoneProperties(busAttachment);
    }

    /* if (status == AJ_OK) */ {
        status = InitHaeRapidModeProperties(busAttachment);
    }

    /* if (status == AJ_OK) */ {
        status = InitHaeRemoteControllabilityProperties(busAttachment);
    }

    /* if (status == AJ_OK) */ {
        status = InitHaeSoilLevelProperties(busAttachment);
    }

    /* if (status == AJ_OK) */ {
        status = InitHaeSpinSpeedLevelProperties(busAttachment);
    }

    /* if (status == AJ_OK) */ {
        status = InitHaeWaterLevelProperties(busAttachment);
    }

    /* if (status == AJ_OK) */ {
        status = InitHaeTimerProperties(busAttachment);
    }

    // Init CurrentHumidity
    {
        uint8_t humidity = 10;
        uint8_t maxValue = 90;
        uint8_t humidityRead = 0;

        status = Hae_CurrentHumidityInterfaceSetCurrentValue(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, humidity);
        status = Hae_CurrentHumidityInterfaceGetCurrentValue(HAE_OBJECT_PATH_CONTROLLEE, &humidityRead);
        printf("Current Humidity Read : %d\n", (int)humidityRead);

        status = Hae_CurrentHumidityInterfaceSetMaxValue(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, maxValue);
        status = Hae_CurrentHumidityInterfaceGetMaxValue(HAE_OBJECT_PATH_CONTROLLEE, &humidityRead);
        printf("MaxValue Read : %d\n", (int)humidityRead);
    }

    // Init TargetHumidity
    {
        uint8_t humidity = 30;
        uint8_t humidityMin = 10;
        uint8_t humidityMax = 70;
        uint8_t humidityStep = 5;
        uint8_t humidityRead = 0;
        int i;
        const uint8_t selectableHumidityLevels[] = { 10, 20, 30, 40, 50, 60, 70 };
        uint8_t selectableHumidityLevelsRead[7];

        status = Hae_TargetHumidityInterfaceSetMinValue(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, humidityMin);
        status = Hae_TargetHumidityInterfaceGetMinValue(HAE_OBJECT_PATH_CONTROLLEE, &humidityRead);
        printf("Target Humidity Min Read : %d\n", (int)humidityRead);

        status = Hae_TargetHumidityInterfaceSetMaxValue(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, humidityMax);
        status = Hae_TargetHumidityInterfaceGetMaxValue(HAE_OBJECT_PATH_CONTROLLEE, &humidityRead);
        printf("Target Humidity Max Read : %d\n", (int)humidityRead);

        status = Hae_TargetHumidityInterfaceSetStepValue(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, humidityStep);
        status = Hae_TargetHumidityInterfaceGetStepValue(HAE_OBJECT_PATH_CONTROLLEE, &humidityRead);
        printf("Target Humidity Step Read : %d\n", (int)humidityRead);

        status = Hae_TargetHumidityInterfaceSetTargetValue(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, humidity);
        status = Hae_TargetHumidityInterfaceGetTargetValue(HAE_OBJECT_PATH_CONTROLLEE, &humidityRead);
        printf("Target Humidity Read : %d\n", (int)humidityRead);

        status = Hae_TargetHumidityInterfaceSetSelectableHumidityLevels(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, selectableHumidityLevels, sizeof(selectableHumidityLevels) / sizeof(selectableHumidityLevels[0]));
        status = Hae_TargetHumidityInterfaceGetSelectableHumidityLevels(HAE_OBJECT_PATH_CONTROLLEE, selectableHumidityLevelsRead);
        printf("Target Humidity Read : { ");
        for (i = 0; i < 7; i++) {
            printf("%d ", (int)selectableHumidityLevelsRead[i]);
        }
        printf("}\n");
    }

    // Init TargetTemperatureLevel
    {
        uint8_t temperatureLevel = 30;
        uint8_t temperatureLevelMax = 70;
        uint8_t temperatureLevelRead = 0;
        int i;
        const uint8_t selectableTemperatureLevels[] = { 10, 20, 30, 40, 50, 60, 70 };
        uint8_t selectableTemperatureLevelsRead[7];

        status = Hae_TargetTemperatureLevelInterfaceSetMaxLevel(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, temperatureLevelMax);
        status = Hae_TargetTemperatureLevelInterfaceGetMaxLevel(HAE_OBJECT_PATH_CONTROLLEE, &temperatureLevelRead);
        printf("Target TemperatureLevel Max Read : %d\n", (int)temperatureLevelRead);

        status = Hae_TargetTemperatureLevelInterfaceSetTargetLevel(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, temperatureLevel);
        status = Hae_TargetTemperatureLevelInterfaceGetTargetLevel(HAE_OBJECT_PATH_CONTROLLEE, &temperatureLevelRead);
        printf("Target TemperatureLevel Read : %d\n", (int)temperatureLevelRead);

        status = Hae_TargetTemperatureLevelInterfaceSetSelectableTemperatureLevels(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, selectableTemperatureLevels, sizeof(selectableTemperatureLevels) / sizeof(selectableTemperatureLevels[0]));
        status = Hae_TargetTemperatureLevelInterfaceGetSelectableTemperatureLevels(HAE_OBJECT_PATH_CONTROLLEE, selectableTemperatureLevelsRead);
        printf("Target TemperatureLevel Read : { ");
        for (i = 0; i < 7; i++) {
            printf("%d ", (int)selectableTemperatureLevelsRead[i]);
        }
        printf("}\n");
    }

    {
        uint16_t mode = 2;
        uint16_t modeRead = 0;

        const uint16_t supportedModes[3] = { 0, 1, 2 };
        uint16_t supportedModesRead[3];

        status = Hae_HvacFanModeInterfaceSetMode(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, mode);
        status = Hae_HvacFanModeInterfaceGetMode(HAE_OBJECT_PATH_CONTROLLEE, &modeRead);
        printf("HvacFanMode Mode Read : %u\n", modeRead);

        status = Hae_HvacFanModeInterfaceSetSupportedModes(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, supportedModes, 3);
        status = Hae_HvacFanModeInterfaceGetSupportedModes(HAE_OBJECT_PATH_CONTROLLEE, supportedModesRead);
        printf("HvacFanMode SupportedModes Read\n");
        for (i = 0; i < 3; i++) {
            printf("%u\t", supportedModesRead[i]);
        }
        printf("\n");
    }

    {
        PlugInInfo units[2] = { { HAE_OBJECT_PATH_CONTROLLEE, 1, false }, { HAE_OBJECT_PATH_CONTROLLEE, 2, true } };
        PlugInInfo unitsRead[2];

        status = Hae_PlugInUnitsInterfaceSetPlugInUnits(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, units, 2);
        status = Hae_PlugInUnitsInterfaceGetPlugInUnits(HAE_OBJECT_PATH_CONTROLLEE, unitsRead);
        printf("PlugInUnits Read \n");
        for (i = 0; i < 2; i++) {
            printf("%s, %d, %s\n", unitsRead[i].objectPath, unitsRead[i].deviceId, unitsRead[i].pluggedIn ? "true" : "false");
        }
    }

    status = Hae_RapidModeTimedInterfaceSetMaxSetMinutes(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, maxSetMinutes);
    status = Hae_RapidModeTimedInterfaceGetMaxSetMinutes(HAE_OBJECT_PATH_CONTROLLEE, &minutesRead);
    printf("MaxSetMinutes Read: %u\n", minutesRead);

    status = Hae_RapidModeTimedInterfaceSetRapidModeMinutesRemaining(busAttachment, HAE_OBJECT_PATH_CONTROLLEE, rapidModeMinutesRemaining);
    status = Hae_RapidModeTimedInterfaceGetRapidModeMinutesRemaining(HAE_OBJECT_PATH_CONTROLLEE, &minutesRead);
    printf("RapidModeMinutesRemaining Read: %u\n", minutesRead);

    return status;
}

/**
 * The AllJoyn Message Loop
 */

int AJ_Main(void)
{
    AJ_Status status = AJ_OK;
    uint8_t isUnmarshalingSuccessful = FALSE;
    AJSVC_ServiceStatus serviceStatus;
    AJ_Message msg;
    uint8_t forcedDisconnnect = FALSE;
    uint8_t rebootRequired = FALSE;
    AJ_BusAttachment busAttachment;
    CurrentAirQualityListener currentAirQualityListener;
    CurrentAirQualityLevelListener currentAirQualityLevelListener;
    CurrentTemperatureListener currentTemperatureListener;
    TargetTemperatureListener targetTemperatureListener;
    WindDirectionListener windDirectionListener;
    HidListener hidListener;
    AirRecirculationModeListener airRecirculationModeListener;
    AudioVideoInputListener audioVideoInputListener;
    AudioVolumeListener audioVolumeListener;
    BatteryStatusListener batteryStatusListener;
    ChannelListener channelListener;
    ClimateControlModeListener climateControlModeListener;
    ClosedStatusListener closedStatusListener;
    CurrentPowerListener currentPowerListener;
    CycleControlListener cycleControlListener;
    DishWashingCyclePhaseListener dishWashingCyclePhaseListener;
    EnergyUsageListener energyUsageListener;
    FanSpeedLevelListener fanSpeedLevelListener;
    FilterStatusListener filterStatusListener;
    HeatingZoneListener heatingZoneListener;
    LaundryCyclePhaseListener laundryCyclePhaseListener;
    MoistureOutputLevelListener moistureOutputLevelListener;
    OffControlListener offControlListener;
    OnControlListener onControlListener;
    OnOffStatusListener onOffStatusListener;
    OvenCyclePhaseListener ovenCyclePhaseListener;
    RapidModeListener rapidModeListener;
    RemoteControllabilityListener remoteControllabilityListener;
    RepeatModeListener repeatModeListener;
    ResourceSavingListener resourceSavingListener;
    RobotCleaningCyclePhaseListener robotCleaningCyclePhaseListener;
    SoilLevelListener soilLevelListener;
    SpinSpeedLevelListener spinSpeedLevelListener;
    WaterLevelListener waterLevelListener;
    TimerListener timerListener;
    CurrentHumidityListener currentHumidityListener;
    TargetHumidityListener targetHumidityListener;
    TargetTemperatureLevelListener targetTemperatureLevelListener;
    HvacFanModeListener hvacFanModeListener;
    PlugInUnitsListener plugInUnitsListener;
    RapidModeTimedListener rapidModeTimedListener;

    AJ_Initialize();

    AJ_AboutSetIcon(aboutIconContent, aboutIconContentSize, aboutIconMimetype, aboutIconUrl);

    status = PropertyStore_Init();
    if (status != AJ_OK) {
        goto Exit;
    }

    status = Hae_Init();


    currentAirQualityListener.OnGetContaminantType = NULL;
    //currentAirQualityListener.OnGetContaminantType = CurrentAirQuality_OnGetContaminantType;
    currentAirQualityListener.OnGetCurrentValue = NULL;
    //currentAirQualityListener.OnGetCurrentValue = CurrentAirQuality_OnGetCurrentValue;
    currentAirQualityListener.OnGetMaxValue = NULL;
    //currentAirQualityListener.OnGetMaxValue = CurrentAirQuality_OnGetMaxValue;
    currentAirQualityListener.OnGetMinValue = NULL;
    //currentAirQualityListener.OnGetMinValue = CurrentAirQuality_OnGetMinValue;
    currentAirQualityListener.OnGetPrecision = NULL;
    //currentAirQualityListener.OnGetPrecision = CurrentAirQuality_OnGetPrecision;
    currentAirQualityListener.OnGetUpdateMinTime = NULL;
    //currentAirQualityListener.OnGetUpdateMinTime = CurrentAirQuality_OnGetUpdateMinTime;
    status = Hae_CreateInterface(CURRENT_AIR_QUALITY_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &currentAirQualityListener);

    currentAirQualityLevelListener.OnGetContaminantType = NULL;
    //currentAirQualityLevelListener.OnGetContaminantType = CurrentAirQualityLevel_OnGetContaminantType;
    currentAirQualityLevelListener.OnGetCurrentLevel = NULL;
    //currentAirQualityLevelListener.OnGetCurrentLevel = CurrentAirQualityLevel_OnGetCurrentLevel;
    currentAirQualityLevelListener.OnGetMaxLevel = NULL;
    //currentAirQualityLevelListener.OnGetMaxLevel = CurrentAirQualityLevel_OnGetMaxLevel;
    status = Hae_CreateInterface(CURRENT_AIR_QUALITY_LEVEL_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &currentAirQualityLevelListener);

    currentTemperatureListener.OnGetCurrentValue = NULL;
    //currentTemperatureListener.OnGetCurrentValue = OnGetCurrentValue;
    currentTemperatureListener.OnGetPrecision = NULL;
    //currentTemperatureListener.OnGetPrecision = OnGetPrecision;
    currentTemperatureListener.OnGetUpdateMinTime = NULL;
    //currentTemperatureListener.OnGetUpdateMinTime = OnGetUpdateMinTime;
    status = Hae_CreateInterface(CURRENT_TEMPERATURE_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &currentTemperatureListener);

    targetTemperatureListener.OnGetTargetValue = NULL;
    //targetTemperatureListener.OnGetTargetValue = OnGetTargetValue;
    targetTemperatureListener.OnSetTargetValue = OnSetTargetValue;
    targetTemperatureListener.OnGetMinValue = NULL;
    //targetTemperatureListener.OnGetMinValue = OnGetMinValue;
    targetTemperatureListener.OnGetMaxValue = NULL;
    //targetTemperatureListener.OnGetMaxValue = OnGetMaxValue;
    targetTemperatureListener.OnGetStepValue = NULL;
    //targetTemperatureListener.OnGetStepValue = OnGetStepValue;
    status = Hae_CreateInterface(TARGET_TEMPERATURE_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &targetTemperatureListener);

    windDirectionListener.OnGetHorizontalDirection = NULL;
    //windDirectionListener.OnGetHorizontalDirection = OnGetHorizontalDirection;
    windDirectionListener.OnSetHorizontalDirection = OnSetHorizontalDirection;
    windDirectionListener.OnGetHorizontalMax = NULL;
    //windDirectionListener.OnGetHorizontalMax = OnGetHorizontalMax;
    windDirectionListener.OnGetHorizontalAutoMode = NULL;
    //windDirectionListener.OnGetHorizontalAutoMode = OnGetHorizontalAutoMode;
    windDirectionListener.OnSetHorizontalAutoMode = OnSetHorizontalAutoMode;
    windDirectionListener.OnGetVerticalDirection = NULL;
    //windDirectionListener.OnGetVerticalDirection = OnGetVerticalDirection;
    windDirectionListener.OnSetVerticalDirection = OnSetVerticalDirection;
    windDirectionListener.OnGetVerticalMax = NULL;
    //windDirectionListener.OnGetVerticalMax = OnGetVerticalMax;
    windDirectionListener.OnGetVerticalAutoMode = NULL;
    //windDirectionListener.OnGetVerticalAutoMode = OnGetVerticalAutoMode;
    windDirectionListener.OnSetVerticalAutoMode = OnSetVerticalAutoMode;
    status = Hae_CreateInterface(WIND_DIRECTION_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &windDirectionListener);

    hidListener.OnGetSupportedEvents = NULL;
    //hidListener.OnGetSupportedEvents = OnGetSupportedEvents;
    hidListener.OnInjectEvents = OnInjectEvents;
    status = Hae_CreateInterface(HID_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &hidListener);

    airRecirculationModeListener.OnGetIsRecirculating = NULL;
    //airRecirculationModeListener.OnGetIsRecirculating = OnGetIsRecirculating;
    airRecirculationModeListener.OnSetIsRecirculating = OnSetIsRecirculating;
    status = Hae_CreateInterface(AIR_RECIRCULATION_MODE_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &airRecirculationModeListener);

    audioVideoInputListener.OnGetInputSourceId = NULL;
    //audioVideoInputListener.OnGetInputSourceId = OnGetInputSourceId;
    audioVideoInputListener.OnGetSupportedInputSources = NULL;
    //audioVideoInputListener.OnGetSupportedInputSources = OnGetSupportedInputSources;
    audioVideoInputListener.OnSetInputSourceId = OnSetInputSourceId;
    status = Hae_CreateInterface(AUDIO_VIDEO_INPUT_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &audioVideoInputListener);

    audioVolumeListener.OnGetVolume = NULL;
    //audioVolumeListener.OnGetVolume = OnGetVolume;
    audioVolumeListener.OnSetVolume = OnSetVolume;
    audioVolumeListener.OnGetMaxVolume = NULL;
    //audioVolumeListener.OnGetMaxVolume = OnGetMaxVolume;
    audioVolumeListener.OnGetMute = NULL;
    //audioVolumeListener.OnGetMute = OnGetMute;
    audioVolumeListener.OnSetMute = OnSetMute;
    status = Hae_CreateInterface(AUDIO_VOLUME_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &audioVolumeListener);

    batteryStatusListener.OnGetCurrentValue = NULL;
    //batteryStatusListener.OnGetCurrentValue = OnGetCurrentValue2;
    batteryStatusListener.OnGetIsCharging = NULL;
    //batteryStatusListener.OnGetIsCharging = OnGetIsCharging;
    status = Hae_CreateInterface(BATTERY_STATUS_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &batteryStatusListener);

    channelListener.OnGetChannelId = NULL;
    //channelListener.OnGetChannelId = OnGetChannelId;
    channelListener.OnSetChannelId = OnSetChannelId;
    channelListener.OnGetTotalNumberOfChannels = NULL;
    //channelListener.OnGetTotalNumberOfChannels = OnGetTotalNumberOfChannels;
    channelListener.OnGetChannelList = OnGetChannelList;
    status = Hae_CreateInterface(CHANNEL_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &channelListener);

    climateControlModeListener.OnGetMode = NULL;
    //climateControlModeListener.OnGetMode = OnGetMode;
    climateControlModeListener.OnSetMode = OnSetMode;
    climateControlModeListener.OnGetSupportedModes = NULL;
    //climateControlModeListener.OnGetSupportedModes = OnGetSupportedModes;
    climateControlModeListener.OnGetOperationalState = NULL;
    //climateControlModeListener.OnGetOperationalState = OnGetOperationalState;
    status = Hae_CreateInterface(CLIMATE_CONTROL_MODE_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &climateControlModeListener);

    closedStatusListener.OnGetIsClosed = NULL;
    status = Hae_CreateInterface(CLOSED_STATUS_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &closedStatusListener);

    currentPowerListener.OnGetCurrentPower = NULL;
    //currentPowerListener.OnGetCurrentPower = OnGetCurrentPower;
    currentPowerListener.OnGetPrecision = NULL;
    //currentPowerListener.OnGetPrecision = OnGetPrecision2;
    currentPowerListener.OnGetUpdateMinTime = NULL;
    //currentPowerListener.OnGetUpdateMinTime = OnGetUpdateMinTime2;
    status = Hae_CreateInterface(CURRENT_POWER_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &currentPowerListener);

    cycleControlListener.OnGetOperationalState = NULL;
    cycleControlListener.OnGetSupportedOperationalStates = NULL;
    cycleControlListener.OnGetSupportedOperationalCommands = NULL;
    //cycleControlListener.OnExecuteOperationalCommand = NULL;
    cycleControlListener.OnExecuteOperationalCommand = OnExecuteOperationalCommand;
    status = Hae_CreateInterface(CYCLE_CONTROL_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &cycleControlListener);

    dishWashingCyclePhaseListener.OnGetCyclePhase = NULL;
    dishWashingCyclePhaseListener.OnGetSupportedCyclePhases = NULL;
    dishWashingCyclePhaseListener.OnGetVendorPhasesDescription = NULL;
    status = Hae_CreateInterface(DISH_WASHING_CYCLE_PHASE_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &dishWashingCyclePhaseListener);

    laundryCyclePhaseListener.OnGetCyclePhase = NULL;
    laundryCyclePhaseListener.OnGetSupportedCyclePhases = NULL;
    laundryCyclePhaseListener.OnGetVendorPhasesDescription = NULL;
    status = Hae_CreateInterface(LAUNDRY_CYCLE_PHASE_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &laundryCyclePhaseListener);

    ovenCyclePhaseListener.OnGetCyclePhase = NULL;
    ovenCyclePhaseListener.OnGetSupportedCyclePhases = NULL;
    ovenCyclePhaseListener.OnGetVendorPhasesDescription = NULL;
    status = Hae_CreateInterface(OVEN_CYCLE_PHASE_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &ovenCyclePhaseListener);

    energyUsageListener.OnGetCumulativeEnergy = NULL;
    //energyUsageListener.OnGetCumulativeEnergy = OnGetCumulativeEnergy;
    energyUsageListener.OnGetPrecision = NULL;
    //energyUsageListener.OnGetPrecision = OnGetPrecision3;
    energyUsageListener.OnGetUpdateMinTime = NULL;
    //energyUsageListener.OnGetUpdateMinTime = OnGetUpdateMinTime3;
    energyUsageListener.OnResetCumulativeEnergy = OnResetCumulativeEnergy;
    status = Hae_CreateInterface(ENERGY_USAGE_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &energyUsageListener);

    fanSpeedLevelListener.OnGetFanSpeedLevel = NULL;
    //fanSpeedLevelListener.OnGetFanSpeedLevel = OnGetFanSpeedLevel;
    fanSpeedLevelListener.OnSetFanSpeedLevel = OnSetFanSpeedLevel;
    fanSpeedLevelListener.OnGetMaxFanSpeedLevel = NULL;
    //fanSpeedLevelListener.OnGetMaxFanSpeedLevel = OnGetMaxFanSpeedLevel;
    fanSpeedLevelListener.OnGetAutoMode = NULL;
    //fanSpeedLevelListener.OnGetAutoMode = OnGetAutoMode;
    fanSpeedLevelListener.OnSetAutoMode = OnSetAutoMode;
    status = Hae_CreateInterface(FAN_SPEED_LEVEL_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &fanSpeedLevelListener);

    offControlListener.OnSwitchOff = OnSwitchOff;
    status = Hae_CreateInterface(OFF_CONTROL_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &offControlListener);

    onControlListener.OnSwitchOn = OnSwitchOn;
    status = Hae_CreateInterface(ON_CONTROL_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &onControlListener);

    onOffStatusListener.OnGetOnOff = NULL;
    //onOffStatusListener.OnGetOnOff = OnGetOnOff;
    status = Hae_CreateInterface(ON_OFF_STATUS_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &onOffStatusListener);

    rapidModeListener.OnGetRapidMode = NULL;
    //rapidModeListener.OnGetRapidMode = OnGetRapidMode;
    rapidModeListener.OnSetRapidMode = OnSetRapidMode;
    status = Hae_CreateInterface(RAPID_MODE_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &rapidModeListener);

    remoteControllabilityListener.OnGetIsControllable = NULL;
    status = Hae_CreateInterface(REMOTE_CONTROLLABILITY_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &remoteControllabilityListener);

    repeatModeListener.OnGetRepeatMode = NULL;
    //repeatModeListener.OnGetRepeatMode = OnGetRepeatMode;
    repeatModeListener.OnSetRepeatMode = OnSetRepeatMode;
    status = Hae_CreateInterface(REPEAT_MODE_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &repeatModeListener);

    resourceSavingListener.OnGetResourceSavingMode = NULL;
    //resourceSavingListener.OnGetResourceSavingMode = OnGetResourceSavingMode;
    resourceSavingListener.OnSetResourceSavingMode = OnSetResourceSavingMode;
    status = Hae_CreateInterface(RESOURCE_SAVING_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &resourceSavingListener);

    robotCleaningCyclePhaseListener.OnGetCyclePhase = NULL;
    //robotCleaningCyclePhaseListener.OnGetCyclePhase = OnGetCyclePhase;
    robotCleaningCyclePhaseListener.OnGetSupportedCyclePhases = NULL;
    //robotCleaningCyclePhaseListener.OnGetSupportedCyclePhases = OnGetSupportedCyclePhases;
    robotCleaningCyclePhaseListener.OnGetVendorPhasesDescription = OnGetVendorPhasesDescription;
    status = Hae_CreateInterface(ROBOT_CLEANING_CYCLE_PHASE_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &robotCleaningCyclePhaseListener);

    heatingZoneListener.OnGetNumberOfHeatingZones = NULL;
    heatingZoneListener.OnGetMaxHeatingLevels = NULL;
    heatingZoneListener.OnGetHeatingLevels = NULL;
    status = Hae_CreateInterface(HEATING_ZONE_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &heatingZoneListener);

    soilLevelListener.OnGetMaxLevel = NULL;
    soilLevelListener.OnGetTargetLevel = NULL;
    soilLevelListener.OnSetTargetLevel = OnSetSoilLevelTargetLevel;
    soilLevelListener.OnGetSelectableLevels = NULL;
    status = Hae_CreateInterface(SOIL_LEVEL_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &soilLevelListener);

    spinSpeedLevelListener.OnGetMaxLevel = NULL;
    spinSpeedLevelListener.OnGetTargetLevel = NULL;
    spinSpeedLevelListener.OnSetTargetLevel = OnSetSpinSpeedLevelTargetLevel;
    spinSpeedLevelListener.OnGetSelectableLevels = NULL;
    status = Hae_CreateInterface(SPIN_SPEED_LEVEL_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &spinSpeedLevelListener);

    waterLevelListener.OnGetCurrentLevel = NULL;
    waterLevelListener.OnGetSupplySource = NULL;
    waterLevelListener.OnGetMaxLevel = NULL;
    status = Hae_CreateInterface(WATER_LEVEL_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &waterLevelListener);

    timerListener.OnGetReferenceTimer = NULL;
    timerListener.OnGetTargetTimeToStart = NULL;
    timerListener.OnGetTargetTimeToStop = NULL;
    timerListener.OnGetEstimatedTimeToEnd = NULL;
    timerListener.OnGetRunningTime = NULL;
    timerListener.OnGetTargetDuration = NULL;
    timerListener.OnSetTargetTimeToStart = NULL;
    timerListener.OnSetTargetTimeToStop = NULL;
    status = Hae_CreateInterface(TIMER_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &timerListener);

    moistureOutputLevelListener.OnGetMoistureOutputLevel = NULL;
    //moistureOutputLevelListener.OnGetMoistureOutputLevel = OnGetMoistureOutputLevel;
    moistureOutputLevelListener.OnSetMoistureOutputLevel = OnSetMoistureOutputLevel;
    moistureOutputLevelListener.OnGetMaxMoistureOutputLevel = NULL;
    //moistureOutputLevelListener.OnGetMaxMoistureOutputLevel = OnGetMaxMoistureOutputLevel;
    moistureOutputLevelListener.OnGetAutoMode = NULL;
    //moistureOutputLevelListener.OnGetAutoMode = OnGetAutoMode;
    moistureOutputLevelListener.OnSetAutoMode = OnSetAutoMode;
    status = Hae_CreateInterface(MOISTURE_OUTPUT_LEVEL_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &moistureOutputLevelListener);

    filterStatusListener.OnGetExpectedLifeInDays = NULL;
    //filterStatusListener.OnGetExpectedLifeInDays = OnGetExpectedLifeInDays;
    filterStatusListener.OnGetIsCleanable = NULL;
    //filterStatusListener.OnGetIsCleanable = OnGetIsCleanable;
    filterStatusListener.OnGetOrderPercentage = NULL;
    //filterStatusListener.OnGetOrderPercentage = OnGetOrderPercentage;
    filterStatusListener.OnGetManufacturer = NULL;
    //filterStatusListener.OnGetManufacturer = OnGetManufacturer;
    filterStatusListener.OnGetPartNumber = NULL;
    //filterStatusListener.OnGetPartNumber = OnGetPartNumber;
    filterStatusListener.OnGetUrl = NULL;
    //filterStatusListener.OnGetUrl = OnGetUrl;
    filterStatusListener.OnGetLifeRemaining = NULL;
    //filterStatusListener.OnGetLifeRemaining = OnGetLifeRemaining;
    status = Hae_CreateInterface(FILTER_STATUS_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &filterStatusListener);

    currentHumidityListener.OnGetCurrentValue = NULL;
    currentHumidityListener.OnGetMaxValue = NULL;
    status = Hae_CreateInterface(CURRENT_HUMIDITY_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &currentHumidityListener);

    targetHumidityListener.OnGetTargetValue = NULL;
    targetHumidityListener.OnSetTargetValue = TargetHumidity_OnSetTargetValue;
    targetHumidityListener.OnGetMinValue = NULL;
    targetHumidityListener.OnGetMaxValue = NULL;
    targetHumidityListener.OnGetStepValue = NULL;
    targetHumidityListener.OnGetSelectableHumidityLevels = NULL;
    status = Hae_CreateInterface(TARGET_HUMIDITY_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &targetHumidityListener);

    targetTemperatureLevelListener.OnGetTargetLevel = NULL;
    targetTemperatureLevelListener.OnSetTargetLevel = TargetTemperatureLevel_OnSetTargetLevel;
    targetTemperatureLevelListener.OnGetMaxLevel = NULL;
    targetTemperatureLevelListener.OnGetStepValue = NULL;
    targetTemperatureLevelListener.OnGetSelectableTemperatureLevels = NULL;
    status = Hae_CreateInterface(TARGET_TEMPERATURE_LEVEL_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &targetTemperatureLevelListener);

    hvacFanModeListener.OnGetMode = NULL;
    hvacFanModeListener.OnSetMode = OnSetMode;
    hvacFanModeListener.OnGetSupportedModes = NULL;
    status = Hae_CreateInterface(HVAC_FAN_MODE_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &hvacFanModeListener);

    plugInUnitsListener.OnGetPlugInUnits = NULL;
    status = Hae_CreateInterface(PLUG_IN_UNITS_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &plugInUnitsListener);

    rapidModeTimedListener.OnGetMaxSetMinutes = NULL;
    //rapidModeTimedListener.OnGetMaxSetMinutes = OnGetMaxSetMinutes;
    rapidModeTimedListener.OnGetRapidModeMinutesRemaining = NULL;
    //rapidModeTimedListener.OnGetRapidModeMinutesRemaining = OnGetRapidModeMinutesRemaining;
    rapidModeTimedListener.OnSetRapidModeMinutesRemaining = OnSetRapidModeMinutesRemaining;
    status = Hae_CreateInterface(RAPID_MODE_TIMED_INTERFACE, HAE_OBJECT_PATH_CONTROLLEE, &rapidModeTimedListener);

    status = Hae_Start();

    while (TRUE) {
        status = AJ_OK;
        serviceStatus = AJSVC_SERVICE_STATUS_NOT_HANDLED;

        if (!isBusConnected) {
            status = AJSVC_RoutingNodeConnect(&busAttachment, ROUTING_NODE_NAME, AJAPP_CONNECT_TIMEOUT, AJAPP_CONNECT_PAUSE, AJAPP_BUS_LINK_TIMEOUT, &isBusConnected);
            if (!isBusConnected) { // Failed to connect to Routing Node?
                continue; // Retry establishing connection to Routing Node.
            }

            status = Hae_EnableSecurity(&busAttachment, suites, numsuites, AuthListenerCallback);
            if (status != AJ_OK) {
                AJSVC_RoutingNodeDisconnect(&busAttachment, 1, AJAPP_SLEEP_TIME, AJAPP_SLEEP_TIME, &isBusConnected);
                break;
            }
        }

        status = AJApp_ConnectedHandler(&busAttachment);
        if (!haeInitState) {
            status = InitHaeProperties(&busAttachment);
            if (status == AJ_OK) {
                haeInitState = true;
            }
        }

        if (status == AJ_OK) {
            status = AJ_UnmarshalMsg(&busAttachment, &msg, AJAPP_UNMARSHAL_TIMEOUT);
            isUnmarshalingSuccessful = (status == AJ_OK);

            if (status == AJ_ERR_TIMEOUT) {
                if (AJ_ERR_LINK_TIMEOUT == AJ_BusLinkStateProc(&busAttachment)) {
                    status = AJ_ERR_READ;             // something's not right. force disconnect
                }
            }

            if (isUnmarshalingSuccessful) {
                if (serviceStatus == AJSVC_SERVICE_STATUS_NOT_HANDLED) {
                    serviceStatus = Hae_MessageProcessor(&busAttachment, &msg, &status);
                }
                if (serviceStatus == AJSVC_SERVICE_STATUS_NOT_HANDLED) {
                    serviceStatus = AJApp_MessageProcessor(&busAttachment, &msg, &status);
                }
                if (serviceStatus == AJSVC_SERVICE_STATUS_NOT_HANDLED) {
                    //Pass to the built-in bus message handlers
                    status = AJ_BusHandleBusMessage(&msg);
                }
                AJ_NotifyLinkActive();
            }

            //Unmarshaled messages must be closed to free resources
            AJ_CloseMsg(&msg);
        }

        if (status == AJ_ERR_READ || status == AJ_ERR_WRITE || status == AJ_ERR_RESTART || status == AJ_ERR_RESTART_APP) {
            if (isBusConnected) {
                forcedDisconnnect = (status != AJ_ERR_READ);
                rebootRequired = (status == AJ_ERR_RESTART_APP);
                AJApp_DisconnectHandler(&busAttachment, forcedDisconnnect);
                AJSVC_RoutingNodeDisconnect(&busAttachment, forcedDisconnnect, AJAPP_SLEEP_TIME, AJAPP_SLEEP_TIME, &isBusConnected);
                if (rebootRequired) {
                    AJ_Reboot();
                }
            }
        }
    }     // while (TRUE)

    Hae_Deinit();

    return 0;

Exit:

    return (1);
}

#ifdef AJ_MAIN
int main()
{
    return AJ_Main();
}
#endif

