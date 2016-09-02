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
#include <ajtcl/cdm/CdmControllee.h>
#include <ajtcl/cdm/interfaces/environment/CurrentAirQuality.h>
#include <ajtcl/cdm/interfaces/environment/CurrentAirQualityLevel.h>
#include <ajtcl/cdm/interfaces/environment/CurrentTemperature.h>
#include <ajtcl/cdm/interfaces/environment/TargetTemperature.h>
#include <ajtcl/cdm/interfaces/environment/WaterLevel.h>
#include <ajtcl/cdm/interfaces/environment/WindDirection.h>
#include <ajtcl/cdm/interfaces/input/Hid.h>
#include <ajtcl/cdm/interfaces/operation/AirRecirculationMode.h>
#include <ajtcl/cdm/interfaces/operation/Alerts.h>
#include <ajtcl/cdm/interfaces/operation/AudioVideoInput.h>
#include <ajtcl/cdm/interfaces/operation/AudioVolume.h>
#include <ajtcl/cdm/interfaces/operation/BatteryStatus.h>
#include <ajtcl/cdm/interfaces/operation/Channel.h>
#include <ajtcl/cdm/interfaces/operation/ClimateControlMode.h>
#include <ajtcl/cdm/interfaces/operation/ClosedStatus.h>
#include <ajtcl/cdm/interfaces/operation/CurrentPower.h>
#include <ajtcl/cdm/interfaces/operation/CycleControl.h>
#include <ajtcl/cdm/interfaces/operation/DishWashingCyclePhase.h>
#include <ajtcl/cdm/interfaces/operation/EnergyUsage.h>
#include <ajtcl/cdm/interfaces/operation/FanSpeedLevel.h>
#include <ajtcl/cdm/interfaces/operation/FilterStatus.h>
#include <ajtcl/cdm/interfaces/operation/HeatingZone.h>
#include <ajtcl/cdm/interfaces/operation/LaundryCyclePhase.h>
#include <ajtcl/cdm/interfaces/operation/MoistureOutputLevel.h>
#include <ajtcl/cdm/interfaces/operation/OffControl.h>
#include <ajtcl/cdm/interfaces/operation/OnControl.h>
#include <ajtcl/cdm/interfaces/operation/OnOffStatus.h>
#include <ajtcl/cdm/interfaces/operation/OvenCyclePhase.h>
#include <ajtcl/cdm/interfaces/operation/RapidMode.h>
#include <ajtcl/cdm/interfaces/operation/RapidModeTimed.h>
#include <ajtcl/cdm/interfaces/operation/RemoteControllability.h>
#include <ajtcl/cdm/interfaces/operation/RepeatMode.h>
#include <ajtcl/cdm/interfaces/operation/ResourceSaving.h>
#include <ajtcl/cdm/interfaces/operation/RobotCleaningCyclePhase.h>
#include <ajtcl/cdm/interfaces/operation/SoilLevel.h>
#include <ajtcl/cdm/interfaces/operation/SpinSpeedLevel.h>
#include <ajtcl/cdm/interfaces/operation/Timer.h>
#include <ajtcl/cdm/interfaces/environment/CurrentHumidity.h>
#include <ajtcl/cdm/interfaces/environment/TargetHumidity.h>
#include <ajtcl/cdm/interfaces/environment/TargetTemperatureLevel.h>
#include <ajtcl/cdm/interfaces/operation/HvacFanMode.h>
#include <ajtcl/cdm/interfaces/operation/PlugInUnits.h>

/* to enable/disable a specific interface, uncomment/comment the related row */
#define ENABLE_ENVIRONMENT_CURRENT_AIR_QUALITY_IF
#define ENABLE_ENVIRONMENT_CURRENT_AIR_QUALITY_LEVEL_IF
#define ENABLE_ENVIRONMENT_CURRENT_TEMPERATURE_IF
#define ENABLE_ENVIRONMENT_TARGET_TEMPERATURE_IF
#define ENABLE_ENVIRONMENT_WATER_LEVEL_IF
#define ENABLE_ENVIRONMENT_WIND_DIRECTION_IF
#define ENABLE_ENVIRONMENT_CURRENT_HUMIDITY_IF
#define ENABLE_ENVIRONMENT_TARGET_HUMIDITY_IF
#define ENABLE_ENVIRONMENT_TARGET_TEMPERATURE_LEVEL_IF
#define ENABLE_INPUT_HID_IF
#define ENABLE_OPERATION_AIR_RECIRCULATION_MODE_IF
#define ENABLE_OPERATION_ALERTS_IF
#define ENABLE_OPERATION_AUDIO_VIDEO_INPUT_IF
#define ENABLE_OPERATION_AUDIO_VOLUME_IF
#define ENABLE_OPERATION_BATTERY_STATUS_IF
#define ENABLE_OPERATION_CHANNEL_IF
#define ENABLE_OPERATION_CLIMATE_CONTROL_MODE_IF
#define ENABLE_OPERATION_CLOSED_STATUS_IF
#define ENABLE_OPERATION_CURRENT_POWER_IF
#define ENABLE_OPERATION_CYCLE_CONTROL_IF
#define ENABLE_OPERATION_DISH_WASHING_CYCLE_PHASE_IF
#define ENABLE_OPERATION_ENERGY_USAGE_IF
#define ENABLE_OPERATION_FAN_SPEED_LEVEL_IF
#define ENABLE_OPERATION_FILTER_STATUS_IF
#define ENABLE_OPERATION_HEATING_ZONE_IF
#define ENABLE_OPERATION_HVAC_FAN_MODE_IF
#define ENABLE_OPERATION_LAUNDRY_CYCLE_PHASE_IF
#define ENABLE_OPERATION_MOISTURE_OUTPUT_LEVEL_IF
#define ENABLE_OPERATION_OFF_CONTROL_IF
#define ENABLE_OPERATION_ON_CONTROL_IF
#define ENABLE_OPERATION_ON_OFF_STATUS_IF
#define ENABLE_OPERATION_OVEN_CYCLE_PHASE_IF
#define ENABLE_OPERATION_RAPID_MODE_IF
#define ENABLE_OPERATION_RAPID_MODE_TIMED_IF
#define ENABLE_OPERATION_REMOTE_CONTROLLABILITY_IF
#define ENABLE_OPERATION_REPEAT_MODE_IF
#define ENABLE_OPERATION_RESOURCE_SAVING_IF
#define ENABLE_OPERATION_ROBOT_CLEANING_CYCLE_PHASE_IF
#define ENABLE_OPERATION_SOIL_LEVEL_IF
#define ENABLE_OPERATION_SPIN_SPEED_LEVEL_IF
#define ENABLE_OPERATION_TIMER_IF
#define ENABLE_OPERATION_PLUG_IN_UNITS_IF

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

#define CDM_OBJECT_PATH_CONTROLLEE "/Cdm/ConfigurableControllee"

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
static bool cdmInitState = false;

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

const char* deviceManufactureName = "THE COMPANY";
const char* deviceProductName = "CONFIG. DEVICE";

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
static const char DEFAULT_DEVICE_NAME_LANG1[] = { "Company X - \"Config. Device\"" };
static const char DEFAULT_DEVICE_NAME_LANG2[] = { "Firma X - \"Config. Device\"" };
static const char* DEFAULT_DEVICE_NAMES[] = { DEFAULT_DEVICE_NAME_LANG1, DEFAULT_DEVICE_NAME_LANG2 };
static const char* DEFAULT_APP_NAMES[] = { "Config. Controllee" };
static const char DEFAULT_DESCRIPTION_LANG1[] = "IOE Multipurpose Device";
static const char DEFAULT_DESCRIPTION_LANG2[] = "IOE Vielzweck-Geraet";
static const char* DEFAULT_DESCRIPTIONS[] = { DEFAULT_DESCRIPTION_LANG1, DEFAULT_DESCRIPTION_LANG2 };
static const char DEFAULT_MANUFACTURER_LANG1[] = "Company X(EN)";
static const char DEFAULT_MANUFACTURER_LANG2[] = "Firma X(DE-AT)";
static const char* DEFAULT_MANUFACTURERS[] = { DEFAULT_MANUFACTURER_LANG1, DEFAULT_MANUFACTURER_LANG2 };
static const char* DEFAULT_DEVICE_MODELS[] = { "0.0.1" };
static const char* DEFAULT_DATE_OF_MANUFACTURES[] = { "2016-01-01" };
static const char* DEFAULT_SOFTWARE_VERSIONS[] = { "0.0.1" };
static const char* DEFAULT_HARDWARE_VERSIONS[] = { "0.0.1" };
static const char DEFAULT_SUPPORT_URL_LANG1[] = "www.company_x.com";
static const char DEFAULT_SUPPORT_URL_LANG2[] = "www.company_x.com/de";
static const char* DEFAULT_SUPPORT_URLS[] = { DEFAULT_SUPPORT_URL_LANG1, DEFAULT_SUPPORT_URL_LANG2 };

static const char DEFAULT_COUNTRY_OF_PRODUCTION_LANG1[] = "USA";
static const char DEFAULT_COUNTRY_OF_PRODUCTION_LANG2[] = "USA";
static const char* DEFAULT_COUNTRY_OF_PRODUCTION[] = { DEFAULT_COUNTRY_OF_PRODUCTION_LANG1, DEFAULT_COUNTRY_OF_PRODUCTION_LANG2 };
static const char DEFAULT_CORPORATEBRAND_LANG1[] = "CO. BRAND";
static const char DEFAULT_CORPORATEBRAND_LANG2[] = "CO. MARKE";
static const char* DEFAULT_CORPORATEBRAND[] = { DEFAULT_CORPORATEBRAND_LANG1, DEFAULT_CORPORATEBRAND_LANG2 };
static const char DEFAULT_PRODUCTRAND_LANG1[] = "P. BRAND";
static const char DEFAULT_PRODUCTRAND_LANG2[] = "P. MARKE";
static const char* DEFAULT_PRODUCTRAND[] = { DEFAULT_PRODUCTRAND_LANG1, DEFAULT_PRODUCTRAND_LANG2 };
static const char DEFAULT_LOCATION_LANG1[] = "Room1";
static const char DEFAULT_LOCATION_LANG2[] = "Zimmer1";
static const char* DEFAULT_LOCATION[] = { DEFAULT_LOCATION_LANG1, DEFAULT_LOCATION_LANG2 };
static const char* DEFAULT_DEVICETYPEDESC[] = { "Config. Device" };

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
    {OTHER, CDM_OBJECT_PATH_CONTROLLEE},
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
const char* aboutIconMimetype = (const char*)AJ_LogoMimeType;
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

/* properties initialization functions for each specific interface  */

typedef struct CDM_ProprietyDescriptor {
    char name[30];
    char isArray;
    char dataType;
    void* valuePtr;
    void* readPtr;
    size_t size;
} CDM_ProprietyDescriptor;

void InitCdmPropertiesPrintResult(AJ_Status setStatus, AJ_Status getStatus, const char* interfaceName, const CDM_ProprietyDescriptor* proprietyDescriptor)
{
    uint8_t i;

    const char* const  BooleanLabels[] = {
        "false",
        "true",
    };

    if (setStatus != AJ_OK) {
        printf("!!!%s %s set error : %u !!!\n", interfaceName, proprietyDescriptor->name, setStatus);
    }
    if (getStatus != AJ_OK) {
        printf("!!! %s %s get error : %u !!!\n", interfaceName, proprietyDescriptor->name, getStatus);
    } else {
        if ( proprietyDescriptor->isArray == 'a')
        {
            printf("%s %s get ok :", interfaceName, proprietyDescriptor->name);
            switch (proprietyDescriptor->dataType) {
                case 'y':
                    for (i = 0; i < proprietyDescriptor->size; i++)
                    {
                        printf("\t%u", ((uint8_t*)proprietyDescriptor->readPtr)[i]);
                    }
                    break;
                case 'q':
                    for (i = 0; i < proprietyDescriptor->size; i++)
                    {
                        printf("\t%u", ((uint16_t*)proprietyDescriptor->readPtr)[i]);
                    }
                    break;
                default:
                    printf("InitCdmPropertiesPrintResult: undefined type a%c\n", proprietyDescriptor->dataType);
                    break;
            } /* switch (proprietyDescriptor->dataType) */
            printf("\n");
        } else {
            switch (proprietyDescriptor->dataType) {
                case 'b':
                    printf("%s %s get ok : %s\n", interfaceName, proprietyDescriptor->name, BooleanLabels[*(uint8_t*)proprietyDescriptor->readPtr]);
                    break;
                case 'y':
                    printf("%s %s get ok : %u\n", interfaceName, proprietyDescriptor->name, *(uint8_t*)proprietyDescriptor->readPtr);
                    break;
                case 'q':
                    printf("%s %s get ok : %u\n", interfaceName, proprietyDescriptor->name, *(uint16_t*)proprietyDescriptor->readPtr);
                    break;
                case 'i':
                    printf("%s %s get ok : %i\n", interfaceName, proprietyDescriptor->name, *(int32_t*)proprietyDescriptor->readPtr);
                    break;
                case 'd':
                    printf("%s %s get ok : %f\n", interfaceName, proprietyDescriptor->name, *(double*)proprietyDescriptor->readPtr);
                    break;
                case 's':
                    printf("%s %s get ok : %s\n", interfaceName, proprietyDescriptor->name, (char*)proprietyDescriptor->readPtr);
                    break;
                default:
                    printf("InitCdmPropertiesPrintResult: undefined type %c\n", proprietyDescriptor->dataType);
                    break;
            } /* switch (proprietyDescriptor->dataType) */
        }
    }
    return;
}

AJ_Status CdmInterfaceSetProperty(AJ_BusAttachment* busAttachment, const char* objPath, const char* interfaceName, const CDM_ProprietyDescriptor* proprietyDescriptor)
{
    AJ_Status status = AJ_OK;

    bool bVar;
    uint8_t yVar;
    uint16_t qVar;
    int32_t iVar;
    double dVar;
    char* sVar;
    uint8_t* ayVar;
    uint16_t* aqVar;

    if ( proprietyDescriptor->isArray == 'a')
    {
        switch (proprietyDescriptor->dataType) {
            case 'y':
                ayVar = (uint8_t*)proprietyDescriptor->valuePtr;
                break;
            case 'q':
                aqVar = (uint16_t*)proprietyDescriptor->valuePtr;
                break;
            default:
                printf("CdmInterfaceSetProperty: undefined type a%c\n", proprietyDescriptor->dataType);
                break;
        }
    } else {
        switch (proprietyDescriptor->dataType) {
            case 'b':
                bVar = *(bool*)proprietyDescriptor->valuePtr;
                break;
            case 'y':
                yVar = *(uint8_t*)proprietyDescriptor->valuePtr;
                break;
            case 'q':
                qVar = *(uint16_t*)proprietyDescriptor->valuePtr;
                break;
            case 'i':
                iVar = *(int32_t*)proprietyDescriptor->valuePtr;
                break;
            case 'd':
                dVar = *(double*)proprietyDescriptor->valuePtr;
                break;
            case 's':
                sVar = (char*)proprietyDescriptor->valuePtr;
                break;
            default:
                printf("CdmInterfaceSetProperty: undefined type %c\n", proprietyDescriptor->dataType);
                break;
        } /* switch (proprietyDescriptor->dataType) */
    }

//    /* debug printf (commented) */
//    if ( proprietyDescriptor->isArray == 'a')
//    {    
//        uint8_t i;
//        switch (proprietyDescriptor->dataType) {
//            case 'y':
//                printf("set %s", "ay");
//                for (i = 0; i < proprietyDescriptor->size; i++)
//                {
//                    printf("\t%u", ayVar[i]);
//                }
//                break;
//            case 'q':
//                printf("set %s", "aq");
//                for (i = 0; i < proprietyDescriptor->size; i++)
//                {
//                    printf("\t%u", aqVar[i]);
//                }
//                break;
//        }
//        printf("\n");
//    } else {
//        switch (proprietyDescriptor->dataType) {
//            case 'b':
//                printf("set %s %u\n", "b", bVar);
//                break;
//            case 'y':
//                printf("set %s %u\n", "y", yVar);
//                break;
//            case 'q':
//                printf("set %s %u\n", "q", qVar);
//                break;
//            case 'd':
//                printf("set %s %f\n", "d", dVar);
//                break;
//            case 's':
//                printf("set %s %s\n", "s", sVar);
//                break;
//            default:
//                break;
//        } /* switch (proprietyDescriptor->dataType) */
//    }
    if (strcmp(interfaceName, "CurrentAirQuality") == 0) {
        if (strcmp(proprietyDescriptor->name, "ContaminantType") == 0) {
            status = Cdm_CurrentAirQualityInterfaceSetContaminantType(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "CurrentValue") == 0) {
            status = Cdm_CurrentAirQualityInterfaceSetCurrentValue(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, dVar);
        } else if (strcmp(proprietyDescriptor->name, "MaxValue") == 0) {
            status = Cdm_CurrentAirQualityInterfaceSetMaxValue(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, dVar);
        } else if (strcmp(proprietyDescriptor->name, "MinValue") == 0) {
            status = Cdm_CurrentAirQualityInterfaceSetMinValue(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, dVar);
        } else if (strcmp(proprietyDescriptor->name, "Precision") == 0) {
            status = Cdm_CurrentAirQualityInterfaceSetPrecision(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, dVar);
        } else if (strcmp(proprietyDescriptor->name, "UpdateMinTime") == 0) {
            status = Cdm_CurrentAirQualityInterfaceSetUpdateMinTime(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, qVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    }
    else if (strcmp(interfaceName, "CurrentAirQualityLevel") == 0) {
        if (strcmp(proprietyDescriptor->name, "ContaminantType") == 0) {
            status = Cdm_CurrentAirQualityLevelInterfaceSetContaminantType(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "CurrentLevel") == 0) {
            status = Cdm_CurrentAirQualityLevelInterfaceSetCurrentLevel(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "MaxLevel") == 0) {
            status = Cdm_CurrentAirQualityLevelInterfaceSetMaxLevel(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    }
    else if (strcmp(interfaceName, "CurrentTemperature") == 0) {
        if (strcmp(proprietyDescriptor->name, "CurrentValue") == 0) {
            status = Cdm_CurrentTemperatureInterfaceSetCurrentValue(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, dVar);
        } else if (strcmp(proprietyDescriptor->name, "Precision") == 0) {
            status = Cdm_CurrentTemperatureInterfaceSetPrecision(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, dVar);
        } else if (strcmp(proprietyDescriptor->name, "UpdateMinTime") == 0) {
            status = Cdm_CurrentTemperatureInterfaceSetUpdateMinTime(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, qVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "TargetTemperature") == 0) {
        if (strcmp(proprietyDescriptor->name, "MinValue") == 0) {
            status = Cdm_TargetTemperatureInterfaceSetMinValue( busAttachment, CDM_OBJECT_PATH_CONTROLLEE, dVar);
        } else if (strcmp(proprietyDescriptor->name, "MaxValue") == 0) {
            status = Cdm_TargetTemperatureInterfaceSetMaxValue( busAttachment, CDM_OBJECT_PATH_CONTROLLEE, dVar);
        } else if (strcmp(proprietyDescriptor->name, "StepValue") == 0) {
            status = Cdm_TargetTemperatureInterfaceSetStepValue( busAttachment, CDM_OBJECT_PATH_CONTROLLEE, dVar);
        } else if (strcmp(proprietyDescriptor->name, "TargetValue") == 0) {
            status = Cdm_TargetTemperatureInterfaceSetTargetValue( busAttachment, CDM_OBJECT_PATH_CONTROLLEE, dVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "WaterLevel") == 0) {
        if (strcmp(proprietyDescriptor->name, "SupplySource") == 0) {
            status = Cdm_WaterLevelInterfaceSetSupplySource( busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "MaxLevel") == 0) {
            Cdm_WaterLevelInterfaceSetMaxLevel( busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "CurrentLevel") == 0) {
            Cdm_WaterLevelInterfaceSetCurrentLevel( busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "WindDirection") == 0) {
        if (strcmp(proprietyDescriptor->name, "HorizontalMax") == 0) {
            status = Cdm_WindDirectionInterfaceSetHorizontalMax(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, qVar);
        } else if (strcmp(proprietyDescriptor->name, "HorizontalDirection") == 0) {
            status = Cdm_WindDirectionInterfaceSetHorizontalDirection(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, qVar);
        } else if (strcmp(proprietyDescriptor->name, "HorizontalAutoMode") == 0) {
            status = Cdm_WindDirectionInterfaceSetHorizontalAutoMode(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "VerticalMax") == 0) {
            status = Cdm_WindDirectionInterfaceSetVerticalMax(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, qVar);
        } else if (strcmp(proprietyDescriptor->name, "VerticalDirection") == 0) {
            status = Cdm_WindDirectionInterfaceSetVerticalDirection(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, qVar);
        } else if (strcmp(proprietyDescriptor->name, "VerticalAutoMode") == 0) {
            status = Cdm_WindDirectionInterfaceSetVerticalAutoMode(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "AirRecirculationMode") == 0) {
        if (strcmp(proprietyDescriptor->name, "IsRecirculating") == 0) {
            status = Cdm_AirRecirculationModeInterfaceSetIsRecirculating(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, bVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "AudioVolume") == 0) {
        if (strcmp(proprietyDescriptor->name, "Volume") == 0) {
            status = Cdm_AudioVolumeInterfaceSetVolume(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "MaxVolume") == 0) {
            status = Cdm_AudioVolumeInterfaceSetMaxVolume(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "Mute") == 0) {
            status = Cdm_AudioVolumeInterfaceSetMute(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, bVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "BatteryStatus") == 0) {
        if (strcmp(proprietyDescriptor->name, "CurrentValue") == 0) {
            status = Cdm_BatteryStatusInterfaceSetCurrentValue(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "IsCharging") == 0) {
            status = Cdm_BatteryStatusInterfaceSetIsCharging(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, bVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "Channel") == 0) {
        if (strcmp(proprietyDescriptor->name, "ChannelId") == 0) {
            status = Cdm_ChannelInterfaceSetChannelId(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, sVar);
        } else
        if (strcmp(proprietyDescriptor->name, "TotalNumberOfChannels") == 0) {
            status = Cdm_ChannelInterfaceSetTotalNumberOfChannels(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, qVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "ClimateControlMode") == 0) {
        if (strcmp(proprietyDescriptor->name, "SupportedModes") == 0) {
            status = Cdm_ClimateControlModeInterfaceSetSupportedModes(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, aqVar, proprietyDescriptor->size);
        } else if (strcmp(proprietyDescriptor->name, "Mode") == 0) {
            status = Cdm_ClimateControlModeInterfaceSetMode(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, qVar);
        } else if (strcmp(proprietyDescriptor->name, "OperationalState") == 0) {
            status = Cdm_ClimateControlModeInterfaceSetOperationalState(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, qVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "ClosedStatus") == 0) {
        if (strcmp(proprietyDescriptor->name, "IsClosed") == 0) {
            status = Cdm_ClosedStatusInterfaceSetIsClosed(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, bVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "CurrentPower") == 0) {
        if (strcmp(proprietyDescriptor->name, "CurrentPower") == 0) {
            status = Cdm_CurrentPowerInterfaceSetCurrentPower(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, dVar);
        } else if (strcmp(proprietyDescriptor->name, "Precision") == 0) {
            status = Cdm_CurrentPowerInterfaceSetPrecision(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, dVar);
        } else if (strcmp(proprietyDescriptor->name, "UpdateMinTime") == 0) {
            status = Cdm_CurrentPowerInterfaceSetUpdateMinTime(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, qVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "CycleControl") == 0) {
        if (strcmp(proprietyDescriptor->name, "SupportedOperationalStates") == 0) {
            status = Cdm_CycleControlInterfaceSetSupportedOperationalStates(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, ayVar, proprietyDescriptor->size);
        } else if (strcmp(proprietyDescriptor->name, "SupportedOperationalCommands") == 0) {
            status = Cdm_CycleControlInterfaceSetSupportedOperationalCommands(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, ayVar, proprietyDescriptor->size);
        } else if (strcmp(proprietyDescriptor->name, "OperationalState") == 0) {
            status = Cdm_CycleControlInterfaceSetOperationalState(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "DishWashingCyclePhase") == 0) {
        if (strcmp(proprietyDescriptor->name, "SupportedCyclePhases") == 0) {
            status = Cdm_DishWashingCyclePhaseInterfaceSetSupportedCyclePhases(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, ayVar, proprietyDescriptor->size);
        } else if (strcmp(proprietyDescriptor->name, "CyclePhase") == 0) {
            status = Cdm_DishWashingCyclePhaseInterfaceSetCyclePhase(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "EnergyUsage") == 0) {
        if (strcmp(proprietyDescriptor->name, "CumulativeEnergy") == 0) {
            status = Cdm_EnergyUsageInterfaceSetCumulativeEnergy(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, dVar);
        } else if (strcmp(proprietyDescriptor->name, "Precision") == 0) {
            status = Cdm_EnergyUsageInterfaceSetPrecision(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, dVar);
        } else if (strcmp(proprietyDescriptor->name, "UpdateMinTime") == 0) {
            status = Cdm_EnergyUsageInterfaceSetUpdateMinTime(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, qVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "FanSpeedLevel") == 0) {
        if (strcmp(proprietyDescriptor->name, "MaxFanSpeedLevel") == 0) {
            status = Cdm_FanSpeedLevelInterfaceSetMaxFanSpeedLevel(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "AutoMode") == 0) {
            status = Cdm_FanSpeedLevelInterfaceSetAutoMode(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "FanSpeedLevel") == 0) {
            status = Cdm_FanSpeedLevelInterfaceSetFanSpeedLevel(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "FilterStatus") == 0) {
        if (strcmp(proprietyDescriptor->name, "ExpectedLifeInDays") == 0) {
            status = Cdm_FilterStatusInterfaceSetExpectedLifeInDays(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, qVar);
        } else if (strcmp(proprietyDescriptor->name, "IsCleanable") == 0) {
            status = Cdm_FilterStatusInterfaceSetIsCleanable(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, bVar);
        } else if (strcmp(proprietyDescriptor->name, "OrderPercentage") == 0) {
            status = Cdm_FilterStatusInterfaceSetOrderPercentage(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "Manufacturer") == 0) {
            status = Cdm_FilterStatusInterfaceSetManufacturer(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, sVar);
        } else if (strcmp(proprietyDescriptor->name, "PartNumber") == 0) {
            status = Cdm_FilterStatusInterfaceSetPartNumber(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, sVar);
        } else if (strcmp(proprietyDescriptor->name, "Url") == 0) {
            status = Cdm_FilterStatusInterfaceSetUrl(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, sVar);
        } else if (strcmp(proprietyDescriptor->name, "LifeRemaining") == 0) {
            status = Cdm_FilterStatusInterfaceSetLifeRemaining(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "HeatingZone") == 0) {
        if (strcmp(proprietyDescriptor->name, "NumberOfHeatingZones") == 0) {
            status = Cdm_HeatingZoneInterfaceSetNumberOfHeatingZones(busAttachment, CDM_OBJECT_PATH_CONTROLLEE , yVar);
        } else if  (strcmp(proprietyDescriptor->name, "MaxHeatingLevels") == 0) {
            status = Cdm_HeatingZoneInterfaceSetMaxHeatingLevels(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, ayVar , proprietyDescriptor->size);
        } else if (strcmp(proprietyDescriptor->name, "HeatingLevels") == 0) {
            status = Cdm_HeatingZoneInterfaceSetHeatingLevels(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, ayVar , proprietyDescriptor->size);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "HvacFanMode") == 0) {
        if (strcmp(proprietyDescriptor->name, "SupportedModes") == 0) {
            status = Cdm_HvacFanModeInterfaceSetSupportedModes(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, aqVar , proprietyDescriptor->size);
        } else if (strcmp(proprietyDescriptor->name, "Mode") == 0) {
            status = Cdm_HvacFanModeInterfaceSetMode(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, qVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "LaundryCyclePhase") == 0) {
        if (strcmp(proprietyDescriptor->name, "SupportedCyclePhases") == 0) {
            status = Cdm_LaundryCyclePhaseInterfaceSetSupportedCyclePhases(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, ayVar , proprietyDescriptor->size);
        } else if (strcmp(proprietyDescriptor->name, "CyclePhase") == 0) {
            status = Cdm_LaundryCyclePhaseInterfaceSetCyclePhase(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "MoistureOutputLevel") == 0) {
        if (strcmp(proprietyDescriptor->name, "MoistureOutputLevel") == 0) {
            status = Cdm_MoistureOutputLevelInterfaceSetMoistureOutputLevel(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "MaxMoistureOutputLevel") == 0) {
            status = Cdm_MoistureOutputLevelInterfaceSetMaxMoistureOutputLevel(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "AutoMode") == 0) {
            status = Cdm_MoistureOutputLevelInterfaceSetAutoMode(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "OnOffStatus") == 0) {
        if (strcmp(proprietyDescriptor->name, "IsOn") == 0) {
            status = Cdm_OnOffStatusInterfaceSetIsOn(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, bVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "OvenCyclePhase") == 0) {
        if (strcmp(proprietyDescriptor->name, "SupportedCyclePhases") == 0) {
            status = Cdm_OvenCyclePhaseInterfaceSetSupportedCyclePhases(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, ayVar , proprietyDescriptor->size);
        } else if (strcmp(proprietyDescriptor->name, "CyclePhase") == 0) {
            status = Cdm_OvenCyclePhaseInterfaceSetCyclePhase(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "RapidMode") == 0) {
        if (strcmp(proprietyDescriptor->name, "RapidMode") == 0) {
            status = Cdm_RapidModeInterfaceSetRapidMode(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, bVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "RapidModeTimed") == 0) {
        if (strcmp(proprietyDescriptor->name, "RapidModeMinutesRemaining") == 0) {
            status = Cdm_RapidModeTimedInterfaceSetRapidModeMinutesRemaining(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, qVar);
        } else if (strcmp(proprietyDescriptor->name, "MaxSetMinutes") == 0) {
            status = Cdm_RapidModeTimedInterfaceSetMaxSetMinutes(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, qVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "RemoteControllability") == 0) {
        if (strcmp(proprietyDescriptor->name, "IsControllable") == 0) {
            status = Cdm_RemoteControllabilityInterfaceSetIsControllable(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, bVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "RepeatMode") == 0) {
        if (strcmp(proprietyDescriptor->name, "RepeatMode") == 0) {
            status = Cdm_RepeatModeInterfaceSetRepeatMode(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, bVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "ResourceSaving") == 0) {
        if (strcmp(proprietyDescriptor->name, "ResourceSavingMode") == 0) {
            status = Cdm_ResourceSavingInterfaceSetResourceSavingMode(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, bVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "RobotCleaningCyclePhase") == 0) {
        if (strcmp(proprietyDescriptor->name, "SupportedCyclePhases") == 0) {
            status = Cdm_RobotCleaningCyclePhaseInterfaceSetSupportedCyclePhases(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, ayVar , proprietyDescriptor->size);
        } else if (strcmp(proprietyDescriptor->name, "CyclePhase") == 0) {
            status = Cdm_RobotCleaningCyclePhaseInterfaceSetCyclePhase(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "SoilLevel") == 0) {
        if (strcmp(proprietyDescriptor->name, "MaxLevel") == 0) {
            status = Cdm_SoilLevelInterfaceSetMaxLevel(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "SelectableLevels") == 0) {
            status = Cdm_SoilLevelInterfaceSetSelectableLevels(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, ayVar , proprietyDescriptor->size);
        } else if (strcmp(proprietyDescriptor->name, "TargetLevel") == 0) {
            status = Cdm_SoilLevelInterfaceSetTargetLevel(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "SpinSpeedLevel") == 0) {
        if (strcmp(proprietyDescriptor->name, "MaxLevel") == 0) {
            status = Cdm_SpinSpeedLevelInterfaceSetMaxLevel(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "SelectableLevels") == 0) {
            status = Cdm_SpinSpeedLevelInterfaceSetSelectableLevels(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, ayVar , proprietyDescriptor->size);
        } else if (strcmp(proprietyDescriptor->name, "TargetLevel") == 0) {
            status = Cdm_SpinSpeedLevelInterfaceSetTargetLevel(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "Timer") == 0) {
        if (strcmp(proprietyDescriptor->name, "ReferenceTimer") == 0) {
            status = Cdm_TimerInterfaceSetReferenceTimer(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, iVar);
        } else if (strcmp(proprietyDescriptor->name, "TargetTimeToStart") == 0) {
            status = Cdm_TimerInterfaceSetTargetTimeToStart(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, iVar);
        } else if (strcmp(proprietyDescriptor->name, "TargetTimeToStop") == 0) {
            status = Cdm_TimerInterfaceSetTargetTimeToStop(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, iVar);
        } else if (strcmp(proprietyDescriptor->name, "EstimatedTimeToEnd") == 0) {
            status = Cdm_TimerInterfaceSetEstimatedTimeToEnd(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, iVar);
        } else if (strcmp(proprietyDescriptor->name, "RunningTime") == 0) {
            status = Cdm_TimerInterfaceSetRunningTime(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, iVar);
        } else if (strcmp(proprietyDescriptor->name, "TargetDuration") == 0) {
            status = Cdm_TimerInterfaceSetTargetDuration(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, iVar);
        }
    } else if (strcmp(interfaceName, "CurrentHumidity") == 0) {
        if (strcmp(proprietyDescriptor->name, "CurrentValue") == 0) {
            status = Cdm_CurrentHumidityInterfaceSetCurrentValue(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "MaxValue") == 0) {
            status = Cdm_CurrentHumidityInterfaceSetMaxValue(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "TargetHumidity") == 0) {
        if (strcmp(proprietyDescriptor->name, "MinValue") == 0) {
            status = Cdm_TargetHumidityInterfaceSetMinValue( busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "MaxValue") == 0) {
            status = Cdm_TargetHumidityInterfaceSetMaxValue( busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "StepValue") == 0) {
            status = Cdm_TargetHumidityInterfaceSetStepValue( busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "TargetValue") == 0) {
            status = Cdm_TargetHumidityInterfaceSetTargetValue( busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "SelectableHumidityLevels") == 0) {
            status = Cdm_TargetHumidityInterfaceSetSelectableHumidityLevels( busAttachment, CDM_OBJECT_PATH_CONTROLLEE, ayVar, proprietyDescriptor->size);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "TargetTemperatureLevel") == 0) {
        if (strcmp(proprietyDescriptor->name, "MaxLevel") == 0) {
            status = Cdm_TargetTemperatureLevelInterfaceSetMaxLevel( busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "TargetLevel") == 0) {
            status = Cdm_TargetTemperatureLevelInterfaceSetTargetLevel( busAttachment, CDM_OBJECT_PATH_CONTROLLEE, yVar);
        } else if (strcmp(proprietyDescriptor->name, "SelectableTemperatureLevels") == 0) {
            status = Cdm_TargetTemperatureLevelInterfaceSetSelectableTemperatureLevels( busAttachment, CDM_OBJECT_PATH_CONTROLLEE, ayVar, proprietyDescriptor->size);
        } else {
            printf("CdmInterfaceSetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else {
        printf("CdmInterfaceSetProperty: undefined interface name %s\n", interfaceName);
    }
    return status;
}

AJ_Status CdmInterfaceGetProperty(const char* objPath, const char* interfaceName, const CDM_ProprietyDescriptor* proprietyDescriptor)
{
    AJ_Status status = AJ_OK;

    bool* bVarPtr;
    uint8_t* yVarPtr;
    uint16_t* qVarPtr;
    int32_t* iVarPtr;
    double* dVarPtr;
    char** sVarPtr;

    /* void cast to prevent warning */
    (void)bVarPtr;
    (void)yVarPtr;
    (void)qVarPtr;
    (void)iVarPtr;
    (void)dVarPtr;
    (void)sVarPtr;

    switch (proprietyDescriptor->dataType) {
        case 'b':
            bVarPtr = (bool*)proprietyDescriptor->readPtr;
            break;
        case 'y':
            yVarPtr = (uint8_t*)proprietyDescriptor->readPtr;
            break;
        case 'q':
            qVarPtr = (uint16_t*)proprietyDescriptor->readPtr;
            break;
        case 'i':
            iVarPtr = (int32_t*)proprietyDescriptor->readPtr;
            break;
        case 'd':
            dVarPtr = (double*)proprietyDescriptor->readPtr;
            break;
        case 's':
            sVarPtr = (char**)proprietyDescriptor->readPtr;
            break;
        default:
            printf("CdmInterfaceGetProperty: undefined type %c\n", proprietyDescriptor->dataType);
            break;
    } /* switch (proprietyDescriptor->dataType) */

    if (strcmp(interfaceName, "CurrentAirQuality") == 0) {
        if (strcmp(proprietyDescriptor->name, "ContaminantType") == 0) {
            status = Cdm_CurrentAirQualityInterfaceGetContaminantType(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "CurrentValue") == 0) {
            status = Cdm_CurrentAirQualityInterfaceGetCurrentValue(CDM_OBJECT_PATH_CONTROLLEE, dVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "MaxValue") == 0) {
            status = Cdm_CurrentAirQualityInterfaceGetMaxValue(CDM_OBJECT_PATH_CONTROLLEE, dVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "MinValue") == 0) {
            status = Cdm_CurrentAirQualityInterfaceGetMinValue(CDM_OBJECT_PATH_CONTROLLEE, dVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "Precision") == 0) {
            status = Cdm_CurrentAirQualityInterfaceGetPrecision(CDM_OBJECT_PATH_CONTROLLEE, dVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "UpdateMinTime") == 0) {
            status = Cdm_CurrentAirQualityInterfaceGetUpdateMinTime(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    }
    else if (strcmp(interfaceName, "CurrentAirQualityLevel") == 0) {
        if (strcmp(proprietyDescriptor->name, "ContaminantType") == 0) {
            status = Cdm_CurrentAirQualityLevelInterfaceGetContaminantType(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "CurrentLevel") == 0) {
            status = Cdm_CurrentAirQualityLevelInterfaceGetCurrentLevel(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "MaxLevel") == 0) {
            status = Cdm_CurrentAirQualityLevelInterfaceGetMaxLevel(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    }
     else if (strcmp(interfaceName, "CurrentTemperature") == 0) {
        if (strcmp(proprietyDescriptor->name, "CurrentValue") == 0) {
            status = Cdm_CurrentTemperatureInterfaceGetCurrentValue(CDM_OBJECT_PATH_CONTROLLEE, dVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "Precision") == 0) {
            status = Cdm_CurrentTemperatureInterfaceGetPrecision(CDM_OBJECT_PATH_CONTROLLEE, dVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "UpdateMinTime") == 0) {
            status = Cdm_CurrentTemperatureInterfaceGetUpdateMinTime(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "TargetTemperature") == 0) {
        if (strcmp(proprietyDescriptor->name, "MinValue") == 0) {
            status = Cdm_TargetTemperatureInterfaceGetMinValue(CDM_OBJECT_PATH_CONTROLLEE, dVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "MaxValue") == 0) {
            status = Cdm_TargetTemperatureInterfaceGetMaxValue(CDM_OBJECT_PATH_CONTROLLEE, dVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "StepValue") == 0) {
           status = Cdm_TargetTemperatureInterfaceGetStepValue(CDM_OBJECT_PATH_CONTROLLEE, dVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "TargetValue") == 0) {
            status = Cdm_TargetTemperatureInterfaceGetTargetValue(CDM_OBJECT_PATH_CONTROLLEE, dVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "WaterLevel") == 0) {
        if (strcmp(proprietyDescriptor->name, "SupplySource") == 0) {
            status = Cdm_WaterLevelInterfaceGetSupplySource(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "MaxLevel") == 0) {
            status = Cdm_WaterLevelInterfaceGetMaxLevel(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "CurrentLevel") == 0) {
            status = Cdm_WaterLevelInterfaceGetCurrentLevel(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "WindDirection") == 0) {
        if (strcmp(proprietyDescriptor->name, "HorizontalMax") == 0) {
            status = Cdm_WindDirectionInterfaceGetHorizontalMax(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "HorizontalDirection") == 0) {
            status = Cdm_WindDirectionInterfaceGetHorizontalDirection(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "HorizontalAutoMode") == 0) {
            status = Cdm_WindDirectionInterfaceGetHorizontalAutoMode(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "VerticalMax") == 0) {
            status = Cdm_WindDirectionInterfaceGetVerticalMax(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "VerticalDirection") == 0) {
            status = Cdm_WindDirectionInterfaceGetVerticalDirection(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "VerticalAutoMode") == 0) {
            status = Cdm_WindDirectionInterfaceGetVerticalAutoMode(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "AirRecirculationMode") == 0) {
        if (strcmp(proprietyDescriptor->name, "IsRecirculating") == 0) {
            status = Cdm_AirRecirculationModeInterfaceGetIsRecirculating(CDM_OBJECT_PATH_CONTROLLEE, bVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "AudioVolume") == 0) {
        if (strcmp(proprietyDescriptor->name, "Volume") == 0 ) {
            status = Cdm_AudioVolumeInterfaceGetVolume(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "MaxVolume") == 0 ) {
            status = Cdm_AudioVolumeInterfaceGetMaxVolume(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "Mute") == 0 ) {
            status = Cdm_AudioVolumeInterfaceGetMute(CDM_OBJECT_PATH_CONTROLLEE, bVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "BatteryStatus") == 0) {
        if (strcmp(proprietyDescriptor->name, "CurrentValue") == 0 ) {
            status = Cdm_BatteryStatusInterfaceGetCurrentValue(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "IsCharging") == 0 ) {
            status = Cdm_BatteryStatusInterfaceGetIsCharging(CDM_OBJECT_PATH_CONTROLLEE, bVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "Channel") == 0) {
        if (strcmp(proprietyDescriptor->name, "ChannelId") == 0 ) {
            status = Cdm_ChannelInterfaceGetChannelId(CDM_OBJECT_PATH_CONTROLLEE, (char*)sVarPtr);
        } else
        if (strcmp(proprietyDescriptor->name, "TotalNumberOfChannels") == 0 ) {
            status = Cdm_ChannelInterfaceGetTotalNumberOfChannels(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "ClimateControlMode") == 0) {
       if (strcmp(proprietyDescriptor->name, "SupportedModes") == 0 ) {
            status = Cdm_ClimateControlModeInterfaceGetSupportedModes(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "Mode") == 0 ) {
            status = Cdm_ClimateControlModeInterfaceGetMode(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "OperationalState") == 0 ) {
            status = Cdm_ClimateControlModeInterfaceGetOperationalState(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "ClosedStatus") == 0) {
        if (strcmp(proprietyDescriptor->name, "IsClosed") == 0) {
            status = Cdm_ClosedStatusInterfaceGetIsClosed(CDM_OBJECT_PATH_CONTROLLEE, bVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "CurrentPower") == 0) {
        if (strcmp(proprietyDescriptor->name, "CurrentPower") == 0) {
            status = Cdm_CurrentPowerInterfaceGetCurrentPower(CDM_OBJECT_PATH_CONTROLLEE, dVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "Precision") == 0) {
            status = Cdm_CurrentPowerInterfaceGetPrecision(CDM_OBJECT_PATH_CONTROLLEE, dVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "UpdateMinTime") == 0) {
            status = Cdm_CurrentPowerInterfaceGetUpdateMinTime(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "CycleControl") == 0) {
        if (strcmp(proprietyDescriptor->name, "SupportedOperationalStates") == 0) {
            status = Cdm_CycleControlInterfaceGetSupportedOperationalStates(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "SupportedOperationalCommands") == 0) {
            status = Cdm_CycleControlInterfaceGetSupportedOperationalCommands(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "OperationalState") == 0) {
            status = Cdm_CycleControlInterfaceGetOperationalState(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "DishWashingCyclePhase") == 0) {
        if (strcmp(proprietyDescriptor->name, "SupportedCyclePhases") == 0) {
            status = Cdm_DishWashingCyclePhaseInterfaceGetSupportedCyclePhases(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "CyclePhase") == 0) {
            status = Cdm_DishWashingCyclePhaseInterfaceGetCyclePhase(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "EnergyUsage") == 0) {
        if (strcmp(proprietyDescriptor->name, "CumulativeEnergy") == 0) {
            status = Cdm_EnergyUsageInterfaceGetCumulativeEnergy(CDM_OBJECT_PATH_CONTROLLEE, dVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "Precision") == 0) {
            status = Cdm_EnergyUsageInterfaceGetPrecision(CDM_OBJECT_PATH_CONTROLLEE, dVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "UpdateMinTime") == 0) {
            status = Cdm_EnergyUsageInterfaceGetUpdateMinTime(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "FanSpeedLevel") == 0) {
        if (strcmp(proprietyDescriptor->name, "MaxFanSpeedLevel") == 0) {
            status = Cdm_FanSpeedLevelInterfaceGetMaxFanSpeedLevel(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "AutoMode") == 0) {
            status = Cdm_FanSpeedLevelInterfaceGetAutoMode(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "FanSpeedLevel") == 0) {
            status = Cdm_FanSpeedLevelInterfaceGetFanSpeedLevel(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "FilterStatus") == 0) {
        if (strcmp(proprietyDescriptor->name, "ExpectedLifeInDays") == 0) {
            status = Cdm_FilterStatusInterfaceGetExpectedLifeInDays(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "IsCleanable") == 0) {
            status = Cdm_FilterStatusInterfaceGetIsCleanable(CDM_OBJECT_PATH_CONTROLLEE, bVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "OrderPercentage") == 0) {
            status = Cdm_FilterStatusInterfaceGetOrderPercentage(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "Manufacturer") == 0) {
            status = Cdm_FilterStatusInterfaceGetManufacturer(CDM_OBJECT_PATH_CONTROLLEE, (char*)sVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "PartNumber") == 0) {
            status = Cdm_FilterStatusInterfaceGetPartNumber(CDM_OBJECT_PATH_CONTROLLEE, (char*)sVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "Url") == 0) {
            status = Cdm_FilterStatusInterfaceGetUrl(CDM_OBJECT_PATH_CONTROLLEE, (char*)sVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "LifeRemaining") == 0) {
            status = Cdm_FilterStatusInterfaceGetLifeRemaining(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "HeatingZone") == 0) {
        if (strcmp(proprietyDescriptor->name, "NumberOfHeatingZones") == 0) {
            status = Cdm_HeatingZoneInterfaceGetNumberOfHeatingZones(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "MaxHeatingLevels") == 0) {
            status = Cdm_HeatingZoneInterfaceGetMaxHeatingLevels(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "HeatingLevels") == 0) {
            status = Cdm_HeatingZoneInterfaceGetHeatingLevels(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else  {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "HvacFanMode") == 0) {
        if (strcmp(proprietyDescriptor->name, "SupportedModes") == 0) {
            status = Cdm_HvacFanModeInterfaceGetSupportedModes(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "Mode") == 0) {
            status = Cdm_HvacFanModeInterfaceGetMode(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "LaundryCyclePhase") == 0) {
        if (strcmp(proprietyDescriptor->name, "SupportedCyclePhases") == 0) {
            status = Cdm_LaundryCyclePhaseInterfaceGetSupportedCyclePhases(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "CyclePhase") == 0) {
            status = Cdm_LaundryCyclePhaseInterfaceGetCyclePhase(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "MoistureOutputLevel") == 0) {
        if (strcmp(proprietyDescriptor->name, "MoistureOutputLevel") == 0) {
            status = Cdm_MoistureOutputLevelInterfaceGetMoistureOutputLevel(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "MaxMoistureOutputLevel") == 0) {
            status = Cdm_MoistureOutputLevelInterfaceGetMaxMoistureOutputLevel(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "AutoMode") == 0) {
            status = Cdm_MoistureOutputLevelInterfaceGetAutoMode(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "OnOffStatus") == 0) {
        if (strcmp(proprietyDescriptor->name, "IsOn") == 0) {
            status = Cdm_OnOffStatusInterfaceGetIsOn(CDM_OBJECT_PATH_CONTROLLEE, bVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "OvenCyclePhase") == 0) {
        if (strcmp(proprietyDescriptor->name, "SupportedCyclePhases") == 0) {
            status = Cdm_OvenCyclePhaseInterfaceGetSupportedCyclePhases(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "CyclePhase") == 0) {
            status = Cdm_OvenCyclePhaseInterfaceGetCyclePhase(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "RapidMode") == 0) {
        if (strcmp(proprietyDescriptor->name, "RapidMode") == 0) {
            status = Cdm_RapidModeInterfaceGetRapidMode(CDM_OBJECT_PATH_CONTROLLEE, bVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "RapidModeTimed") == 0) {
        if (strcmp(proprietyDescriptor->name, "RapidModeMinutesRemaining") == 0) {
            status = Cdm_RapidModeTimedInterfaceGetRapidModeMinutesRemaining(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "MaxSetMinutes") == 0) {
            status = Cdm_RapidModeTimedInterfaceGetMaxSetMinutes(CDM_OBJECT_PATH_CONTROLLEE, qVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "RemoteControllability") == 0) {
        if (strcmp(proprietyDescriptor->name, "IsControllable") == 0) {
            status = Cdm_RemoteControllabilityInterfaceGetIsControllable(CDM_OBJECT_PATH_CONTROLLEE, bVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "RepeatMode") == 0) {
        if (strcmp(proprietyDescriptor->name, "RepeatMode") == 0) {
            status = Cdm_RepeatModeInterfaceGetRepeatMode(CDM_OBJECT_PATH_CONTROLLEE, bVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "ResourceSaving") == 0) {
        if (strcmp(proprietyDescriptor->name, "ResourceSavingMode") == 0) {
            status = Cdm_ResourceSavingInterfaceGetResourceSavingMode(CDM_OBJECT_PATH_CONTROLLEE, bVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "RobotCleaningCyclePhase") == 0) {
        if (strcmp(proprietyDescriptor->name, "SupportedCyclePhases") == 0) {
            status = Cdm_RobotCleaningCyclePhaseInterfaceGetSupportedCyclePhases(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "CyclePhase") == 0) {
            status = Cdm_RobotCleaningCyclePhaseInterfaceGetCyclePhase(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "SoilLevel") == 0) {
        if (strcmp(proprietyDescriptor->name, "MaxLevel") == 0) {
            status = Cdm_SoilLevelInterfaceGetMaxLevel(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "SelectableLevels") == 0) {
            status = Cdm_SoilLevelInterfaceGetSelectableLevels(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "TargetLevel") == 0) {
            status = Cdm_SoilLevelInterfaceGetTargetLevel(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "SpinSpeedLevel") == 0) {
        if (strcmp(proprietyDescriptor->name, "MaxLevel") == 0) {
            status = Cdm_SpinSpeedLevelInterfaceGetMaxLevel(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "SelectableLevels") == 0) {
            status = Cdm_SpinSpeedLevelInterfaceGetSelectableLevels(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "TargetLevel") == 0) {
            status = Cdm_SpinSpeedLevelInterfaceGetTargetLevel(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "Timer") == 0) {
        if (strcmp(proprietyDescriptor->name, "ReferenceTimer") == 0 ) {
            status = Cdm_TimerInterfaceGetReferenceTimer(CDM_OBJECT_PATH_CONTROLLEE, iVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "TargetTimeToStart") == 0 ) {
            status = Cdm_TimerInterfaceGetTargetTimeToStart(CDM_OBJECT_PATH_CONTROLLEE, iVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "TargetTimeToStop") == 0 ) {
            status = Cdm_TimerInterfaceGetTargetTimeToStop(CDM_OBJECT_PATH_CONTROLLEE, iVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "EstimatedTimeToEnd") == 0 ) {
            status = Cdm_TimerInterfaceGetEstimatedTimeToEnd(CDM_OBJECT_PATH_CONTROLLEE, iVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "RunningTime") == 0 ) {
            status = Cdm_TimerInterfaceGetRunningTime(CDM_OBJECT_PATH_CONTROLLEE, iVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "TargetDuration") == 0 ) {
            status = Cdm_TimerInterfaceGetTargetDuration(CDM_OBJECT_PATH_CONTROLLEE, iVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "CurrentHumidity") == 0) {
        if (strcmp(proprietyDescriptor->name, "CurrentValue") == 0) {
            status = Cdm_CurrentHumidityInterfaceGetCurrentValue(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "MaxValue") == 0) {
            status = Cdm_CurrentHumidityInterfaceGetMaxValue(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "TargetHumidity") == 0) {
        if (strcmp(proprietyDescriptor->name, "MinValue") == 0) {
            status = Cdm_TargetHumidityInterfaceGetMinValue(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "MaxValue") == 0) {
            status = Cdm_TargetHumidityInterfaceGetMaxValue(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "StepValue") == 0) {
           status = Cdm_TargetHumidityInterfaceGetStepValue(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "TargetValue") == 0) {
            status = Cdm_TargetHumidityInterfaceGetTargetValue(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "SelectableHumidityLevels") == 0) {
            status = Cdm_TargetHumidityInterfaceGetSelectableHumidityLevels(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else if (strcmp(interfaceName, "TargetTemperatureLevel") == 0) {
        if (strcmp(proprietyDescriptor->name, "MaxLevel") == 0) {
            status = Cdm_TargetTemperatureLevelInterfaceGetMaxLevel(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "TargetLevel") == 0) {
            status = Cdm_TargetTemperatureLevelInterfaceGetTargetLevel(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else if (strcmp(proprietyDescriptor->name, "SelectableTemperatureLevels") == 0) {
            status = Cdm_TargetTemperatureLevelInterfaceGetSelectableTemperatureLevels(CDM_OBJECT_PATH_CONTROLLEE, yVarPtr);
        } else {
            printf("CdmInterfaceGetProperty: undefined property name %s\n", proprietyDescriptor->name);
        }
    } else {
        printf("CdmInterfaceGetProperty: undefined interface name %s\n", interfaceName);
    }

//    /* debug printf (commented) */
//    if ( proprietyDescriptor->isArray == 'a')
//    {
//        uint8_t i;
//        switch (proprietyDescriptor->dataType) {
//            case 'y':
//                printf("set %s", "ay");
//                for (i = 0; i < proprietyDescriptor->size; i++)
//                {
//                    printf("\t%u", yVarPtr[i]);
//                }
//                break;
//            case 'q':
//                printf("get %s", "aq");
//                for (i = 0; i < proprietyDescriptor->size; i++)
//                {
//                    printf("\t%u", qVarPtr[i]);
//                }
//                break;
//        }
//        printf("\n");
//    } else {
//        switch (proprietyDescriptor->dataType) {
//            case 'b':
//                printf("get %s %u\n", "b", *bVarPtr);
//                break;
//            case 'y':
//                printf("get %s %u\n", "y", *yVarPtr);
//                break;
//            case 'q':
//                printf("get %s %u\n", "q", *qVarPtr);
//                break;
//            case 'i':
//                printf("get %s %f\n", "i", *dVarPtr);
//                break;
//            case 'd':
//                printf("get %s %f\n", "d", *dVarPtr);
//                break;
//            case 's':
//                printf("get %s %s\n", "s", (char*)sVarPtr);
//                break;
//            default:
//                break;
//        } /* switch (proprietyDescriptor->dataType) */
//    }

    return status;
}

/* InitCdm<If>Properties definition for each interface */

#if defined(ENABLE_ENVIRONMENT_CURRENT_AIR_QUALITY_IF)
/* Environment.CurrentAirQuality */

AJ_Status InitCdmCurrentAirQualityProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t contaminantType = 0;
    double currentValue = 3.0;
    double maxValue = 10.0;
    double minValue = 1.0;
    double precision = 0.5;
    uint16_t updateMinTime = 1;
    uint8_t contaminantTypeRead;
    double currentValueRead;
    double maxValueRead;
    double minValueRead;
    double precisionRead;
    uint16_t updateMinTimeRead;

    const char* const interfaceName = "CurrentAirQuality";

    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "ContaminantType"  , 0 , 'y' , &contaminantType,  &contaminantTypeRead ,  1 },
        { "CurrentValue"     , 0 , 'd' , &currentValue  ,   &currentValueRead  , 1 },
        { "MaxValue"         , 0 , 'd' , &maxValue      ,   &maxValueRead  , 1 },
        { "MinValue"         , 0 , 'd' , &minValue      ,   &minValueRead  , 1 },
        { "Precision"        , 0 , 'd' , &precision     ,   &precisionRead     , 1 },
        { "UpdateMinTime"    , 0 , 'q' , &updateMinTime ,   &updateMinTimeRead , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}


#endif /* defined(ENABLE_ENVIRONMENT_CURRENT_AIR_QUALITY_IF) */

#if defined(ENABLE_ENVIRONMENT_CURRENT_AIR_QUALITY_LEVEL_IF)
/* Environment.CurrentAirQualityLevel */

AJ_Status InitCdmCurrentAirQualityLevelProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t contaminantType = 0;
    uint8_t currentLevel = 3;
    uint8_t maxLevel = 10;
    uint8_t contaminantTypeRead;
    uint8_t currentLevelRead;
    uint8_t maxLevelRead;

    const char* const interfaceName = "CurrentAirQualityLevel";

    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "ContaminantType"  , 0 , 'y' , &contaminantType,  &contaminantTypeRead ,  1 },
        { "CurrentLevel"     , 0 , 'y' , &currentLevel  ,   &currentLevelRead  , 1 },
        { "MaxLevel"         , 0 , 'y' , &maxLevel      ,   &maxLevelRead  , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}


#endif /* defined(ENABLE_ENVIRONMENT_CURRENT_AIR_QUALITY_LEVEL_IF) */


#if defined(ENABLE_ENVIRONMENT_CURRENT_TEMPERATURE_IF)
/* Environment.CurrentTemperature */

AJ_Status InitCdmCurrentTemperatureProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    double currentValue = 3.0;
    double precision = 0.5;
    uint16_t updateMinTime = 1;
    double currentValueRead;
    double precisionRead;
    uint16_t updateMinTimeRead;

    const char* const interfaceName = "CurrentTemperature";

    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "CurrentValue"  , 0 , 'd' , &currentValue  , &currentValueRead  , 1 },
        { "Precision"     , 0 , 'd' , &precision     , &precisionRead     , 1 }, 
        { "UpdateMinTime" , 0 , 'q' , &updateMinTime , &updateMinTimeRead , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

#endif /* defined(ENABLE_ENVIRONMENT_CURRENT_TEMPERATURE_IF) */

#if defined(ENABLE_ENVIRONMENT_TARGET_TEMPERATURE_IF)
/* Environment.TargetTemperature */

AJ_Status InitCdmTargetTemperatureProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    double targetValue = 4.0;
    double minValue = 1.0;
    double maxValue = 7.0;
    double stepValue = 0.5;
    double targetValueRead;
    double minValueRead;
    double maxValueRead;
    double stepValueRead;

    const char* const interfaceName = "TargetTemperature";
    /* be carefull to the order */
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "MinValue"    , 0 , 'd' , &minValue    , &minValueRead    , 1 },
        { "MaxValue"    , 0 , 'd' , &maxValue    , &maxValueRead    , 1 },
        { "StepValue"   , 0 , 'd' , &stepValue   , &stepValueRead   , 1 },
        { "TargetValue" , 0 , 'd' , &targetValue , &targetValueRead , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status TargetTemperatureOnSetTargetValue(const char* objPath, const double targetValue)
{
    printf("TargetTemperature OnSetTargetValue : %s, targetValue: %f\n", objPath, targetValue);

    return AJ_OK;
}

#endif /* defined(ENABLE_ENVIRONMENT_TARGET_TEMPERATURE_IF) */

#if defined(ENABLE_ENVIRONMENT_WATER_LEVEL_IF)
/* Environment.WaterLevel */

AJ_Status InitCdmWaterLevelProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t supplySource = 1;
    uint8_t currentLevel = 6;
    uint8_t maxLevel = 10;
    uint8_t supplySourceRead;
    uint8_t currentLevelRead;
    uint8_t maxLevelRead;

    const char* const interfaceName = "WaterLevel";
    /* be carefull to the order */
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "SupplySource" , 0 , 'y' , &supplySource , & supplySourceRead , 1 },
        { "MaxLevel"     , 0 , 'y' , &maxLevel     , & maxLevelRead     , 1 },
        { "CurrentLevel" , 0 , 'y' , &currentLevel , & currentLevelRead , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

#endif /* defined(ENABLE_ENVIRONMENT_WATER_LEVEL_IF) */

#if defined(ENABLE_ENVIRONMENT_WIND_DIRECTION_IF)
/* Environment.WindDirection */

AJ_Status InitCdmWindDirectionProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint16_t horizontalDirection = 2;
    uint16_t horizontalMax = 3;
    uint8_t horizontalAutoMode = 0;
    uint16_t verticalDirection = 1;
    uint16_t verticalMax = 5;
    uint8_t verticalAutoMode = 1;
    uint16_t horizontalDirectionRead;
    uint16_t horizontalMaxRead;
    uint8_t horizontalAutoModeRead;
    uint16_t verticalDirectionRead;
    uint16_t verticalMaxRead;
    uint8_t verticalAutoModeRead;

    const char* const interfaceName = "WindDirection";
    /* be carefull to the order */
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "HorizontalMax"       , 0 , 'q' , &horizontalMax       , &horizontalMaxRead        , 1 },
        { "HorizontalDirection" , 0 , 'q' , &horizontalDirection , &horizontalDirectionRead  , 1 },
        { "HorizontalAutoMode"  , 0 , 'y' , &horizontalAutoMode  , &horizontalAutoModeRead   , 1 },
        { "VerticalMax"         , 0 , 'q' , &verticalMax         , &verticalMaxRead          , 1 },
        { "VerticalDirection"   , 0 , 'q' , &verticalDirection   , &verticalDirectionRead    , 1 },
        { "VerticalAutoMode"    , 0 , 'y' , &verticalAutoMode    , &verticalAutoModeRead     , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status WindDirectionOnSetHorizontalDirection(const char* objPath, const uint16_t horizontalDirection)
{
    printf("WindDirection OnSetHorizontalDirection : %s, horizontalDirection: %u\n", objPath, horizontalDirection);

    return AJ_OK;
}

AJ_Status WindDirectionOnSetHorizontalAutoMode(const char* objPath, const uint8_t horizontalAutoMode)
{
    printf("WindDirection OnSetHorizontalAutoMode : %s, horizontalAutoMode: %u\n", objPath, horizontalAutoMode);

    return AJ_OK;
}

AJ_Status WindDirectionOnSetVerticalDirection(const char* objPath, const uint16_t verticalDirection)
{
    printf("WindDirection OnSetVerticalDirection : %s, verticalDirection: %u\n", objPath, verticalDirection);

    return AJ_OK;
}

AJ_Status WindDirectionOnSetVerticalAutoMode(const char* objPath, const uint8_t verticalAutoMode)
{
    printf("WindDirection OnSetVerticalAutoMode : %s, verticalAutoMode: %u\n", objPath, verticalAutoMode);

    return AJ_OK;
}

#endif  /* defined(ENABLE_ENVIRONMENT_WIND_DIRECTION_IF) */


#if defined(ENABLE_INPUT_HID_IF)
/* Input.Hid */

AJ_Status InitCdmHidProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    uint8_t i;

    /* array of struct */
    const SupportedInputEvent supportedEvents[] = {
        { 1, 2, 4, 10 },
        { 2, 4, 3, 10 },
        { 4, 5, 4, 20 }
    };

    SupportedInputEvent supportedEventsRead[sizeof(supportedEvents)/sizeof(supportedEvents[0])];

    printf("\n");

    status = Cdm_HidInterfaceSetSupportedEvents(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, supportedEvents, sizeof(supportedEvents)/sizeof(supportedEvents[0]));
    if (status != AJ_OK) {
        printf("Hid SupportedEvents set error : %u\n", status);
    }
    status = Cdm_HidInterfaceGetSupportedEvents(CDM_OBJECT_PATH_CONTROLLEE, supportedEventsRead);
    if (status != AJ_OK) {
        printf("Hid SupportedEvents get error : %u\n", status);
    } else {
        printf("Hid SupportedEvents get ok :\n");
        printf("\ttype,\tcode,\tmin,\tmax,\n");
        for (i = 0; i < sizeof(supportedEvents)/sizeof(supportedEventsRead[0]); i++) {
            printf("\t%u,\t%u,\t%i,\t%i,\n", supportedEventsRead[i].type, supportedEventsRead[i].code, supportedEventsRead[i].min, supportedEventsRead[i].max);
        }
    }
    return status;
}

AJ_Status HidOnInjectEvents(const char* objPath, const InputEvent* inputEvent, const size_t numRecords)
{
    int i = 0;
    printf("HidOnInjectEvents : %s, numRecords : %lu\n", objPath, numRecords);

    for (i = 0; i < numRecords; i++) {
        printf("type : %u, code : %u, value : %d\n ", inputEvent[i].type, inputEvent[i].code, inputEvent[i].value);
    }

    return AJ_OK;
}

#endif /* defined(ENABLE_INPUT_HID_IF) */

#if defined(ENABLE_OPERATION_AIR_RECIRCULATION_MODE_IF)
/* Operation.AirRecirculationMode */

AJ_Status InitCdmAirRecirculationModeProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    bool isRecirculating = true;
    bool isRecirculatingRead;

    const char* const interfaceName = "AirRecirculationMode";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "IsRecirculating" , 0 , 'b' , &isRecirculating       , &isRecirculatingRead , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status AirRecirculationModeOnSetIsRecirculating(const char* objPath, const bool isRecirculating)
{
    printf("AirRecirculationMode OnSetIsRecirculating : %s, isRecirculating: %u\n", objPath, isRecirculating);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_AIR_RECIRCULATION_MODE_IF) */

#if defined(ENABLE_OPERATION_ALERTS_IF)
/* Operation.Alerts */

AlertCodesDescriptor AlertCodesDescriptorList[] = {
    { 0x0001  , (char*)"Alert Code 1" },
    { 0x0002  , (char*)"Alert Code 2" },
    { 0x0003  , (char*)"Alert Code 3" },
    { 0x0004  , (char*)"Alert Code 4" },
    { 0x0005  , (char*)"Alert Code 5" },
    { 0x0006  , (char*)"Alert Code 6" },
    { 0x0007  , (char*)"Alert Code 7" },
    { 0x0008  , (char*)"Alert Code 8" },
};

AJ_Status InitCdmAlertsProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
//    AJ_Status getStatus = AJ_OK;
//    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    /* array of struct */
    const AlertRecord alertsList[] = {
        { 0, 0x0001, true  },
        { 1, 0x0002, true  },
        { 2, 0x0003, false },
        { 0, 0x0004, true  },
        { 1, 0x0005, true  },
        { 2, 0x0006, true  },
        { 0, 0x0007, false },
    };

    AlertRecord alertsListRead[sizeof(alertsList)/sizeof(alertsList[0])];

    printf("\n");

    status = Cdm_AlertsInterfaceSetAlerts(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, alertsList, sizeof(alertsList)/sizeof(alertsList[0]));
    if (status != AJ_OK) {
        printf("Alerts Alerts set error : %u\n", status);
    }
    status = Cdm_AlertsInterfaceGetAlerts(CDM_OBJECT_PATH_CONTROLLEE, alertsListRead);
    if (status != AJ_OK) {
        printf("Alerts Alerts get error : %u\n", status);
    } else {
        printf("Alerts Alerts get ok :\n");
        printf("\tseverity,\talertCode,\needAcknowledgement,\n");
        for (i = 0; i < sizeof(alertsList)/sizeof(alertsList[0]); i++) {
            printf("\t%u,\t%u,\t%u,\n",\
                alertsListRead[i].severity,
                alertsListRead[i].alertCode,\
                alertsListRead[i].needAcknowledgement);
        }
    }
    return status;
}

AJ_Status AlertsOnGetAlertCodesDescription (const char* objPath, const char* languageTag,
    AlertCodesDescriptor** description, size_t* alertsNumber, ErrorCode* errorCode)
{
    printf("Alerts OnGetAlertCodesDescription : %s, %s\n", objPath, languageTag);

    if(!strncmp(languageTag, "en", strlen(languageTag))) {
        *description = &AlertCodesDescriptorList[0];
        *alertsNumber = sizeof(AlertCodesDescriptorList) / sizeof(AlertCodesDescriptorList[0]);
        return AJ_OK;
    } else {
        if (errorCode) {
            *errorCode = LANGUAGE_NOT_SUPPORTED;
        }

        return AJ_ERR_FAILURE;
    }
}

AJ_Status AlertsListenerOnAcknowledgeAlert (const char* objPath, const uint16_t alertCode, ErrorCode* errorCode)
{
    printf("Alerts OnAcknowledgeAlert : %s, %u\n", objPath, alertCode);

    return AJ_OK;
}

AJ_Status AlertsListenerOnAcknowledgeAllAlerts (const char* objPath, ErrorCode* errorCode)  
{
    printf("Alerts OnAcknowledgeAlert : %s\n", objPath);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_ALERTS_IF) */

#if defined(ENABLE_OPERATION_AUDIO_VIDEO_INPUT_IF)
/* Operation.AudioVideoInput */

AJ_Status InitCdmAudioVideoInputProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    /* array of struct */
    const InputSource supportedInputSources[] = {
        {1, 2, 3, 4, "WUSUCUMA"},
        {2, 3, 6, 4, "WUSUCUMA1"}
    };
    uint16_t inputSourceId = 1;

    InputSource supportedInputSourcesRead[sizeof(supportedInputSources)/sizeof(supportedInputSources[0])];
    uint16_t inputSourceIdRead;

    const char* const interfaceName = "AudioVideoInput";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "InputSourceId" , 0 , 'q' , &inputSourceId , &inputSourceIdRead , 1 },
    };

    printf("\n");

    status = Cdm_AudioVideoInputInterfaceSetSupportedInputSources(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, supportedInputSources, sizeof(supportedInputSources)/sizeof(supportedInputSources[0]));
    if (status != AJ_OK) {
        printf("AudioVideoInput SupportedInputSources set error : %u\n", status);
    }
    status = Cdm_AudioVideoInputInterfaceGetSupportedInputSources(CDM_OBJECT_PATH_CONTROLLEE, supportedInputSourcesRead);
    if (status != AJ_OK) {
        printf("AudioVideoInput SupportedInputSources get error : %u\n", status);
    } else {
        printf("AudioVideoInput SupportedInputSources get ok :\n");
        printf("\tid,\tsourceType,\tdetectedState,\tportNumber,\tfriendlyName,\n");
        for (i = 0; i < sizeof(supportedInputSources)/sizeof(supportedInputSources[0]); i++) {
            printf("\t%u,\t%u,\t%u,\t%u,\t%s,\n",\
                supportedInputSourcesRead[i].id, supportedInputSourcesRead[i].sourceType,\
                supportedInputSourcesRead[i].detectedState, supportedInputSourcesRead[i].portNumber,\
                supportedInputSourcesRead[i].friendlyName);
        }
    }

    i = 0;
    setStatus = Cdm_AudioVideoInputInterfaceSetInputSourceId(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, inputSourceId);
    getStatus = Cdm_AudioVideoInputInterfaceGetInputSourceId(CDM_OBJECT_PATH_CONTROLLEE, &inputSourceIdRead);
    InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);

    return status;
}


AJ_Status AudioVideoInputOnSetInputSourceId(const char* objPath, const uint16_t inputSourceId)
{
    printf("AudioVideoInput OnSetInputSourceId : %s, inputSourceId: %u\n", objPath, inputSourceId);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_AUDIO_VIDEO_INPUT_IF) */

#if defined(ENABLE_OPERATION_AUDIO_VOLUME_IF)
/* Operation.AudioVolume */

AJ_Status InitCdmAudioVolumeProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t volume = 3;
    uint8_t maxVolume = 8;
    bool mute = false;
    uint8_t volumeRead;
    uint8_t maxVolumeRead;
    bool muteRead;

    const char* const interfaceName = "AudioVolume";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "MaxVolume" , 0 , 'y' , &maxVolume , &maxVolumeRead , 1 },
        { "Mute"      , 0 , 'b' , &mute      , &muteRead      , 1 },
        { "Volume"    , 0 , 'y' , &volume    , &volumeRead    , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status AudioVolumeOnSetVolume(const char* objPath, const uint8_t volume)
{
    printf("AudioVolume OnSetVolume : %s, %u\n", objPath, volume);

    return AJ_OK;
}

AJ_Status AudioVolumeOnSetMute(const char* objPath, const bool mute)
{
    printf("AudioVolume OnSetMute : %s, %u\n", objPath, mute);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_AUDIO_VOLUME_IF) */

#if defined(ENABLE_OPERATION_BATTERY_STATUS_IF)
/* Operation.BatteryStatus */

AJ_Status InitCdmBatteryStatusProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t currentValue = 70;
    bool isCharging = true;
    uint8_t currentValueRead;
    bool isChargingRead;

    const char* const interfaceName = "BatteryStatus";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "CurrentValue" , 0 , 'y' , &currentValue , &currentValueRead , 1 },
        { "IsCharging"   , 0 , 'b' , &isCharging   , &isChargingRead   , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

#endif /* defined(ENABLE_OPERATION_BATTERY_STATUS_IF) */

#if defined(ENABLE_OPERATION_CHANNEL_IF)
/* Operation.Channel */

#define MAX_CHANNEL_STRING_SIZE  6
#define TOTAL_NUM_OF_CHANNEL 12

char *ChannelInfoValues[][3] = {
    {"1-1"  , "1"  , "aaa"},
    {"1-2"  , "2"  , "bbb"},
    {"1-3"  , "3"  , "ccc"},
    {"1-4"  , "4"  , "ddd"},
    {"1-5"  , "5"  , "eee"},
    {"1-6"  , "6"  , "fff"},
    {"1-7"  , "7"  , "ggg"},
    {"1-8"  , "8"  , "hhh"},
    {"1-9"  , "9"  , "iii"},
    {"1-10" , "10" , "jjj"},
    {"1-11" , "11" , "kkk"},
    {"1-12" , "12" , "lll"},
};

ChannelInfoRecord ChannelInfoListSample[sizeof(ChannelInfoValues)/sizeof(ChannelInfoValues[0])];

AJ_Status InitCdmChannelProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    /* initialize the channel list structure */
    for (i=0 ; i<sizeof(ChannelInfoListSample)/sizeof(ChannelInfoListSample[0]); i++) {
        ChannelInfoListSample[i].channelId = ChannelInfoValues[i][0];
        ChannelInfoListSample[i].channelNumber = ChannelInfoValues[i][1];
        ChannelInfoListSample[i].channelName = ChannelInfoValues[i][2];
    }

    char channelId[MAX_CHANNEL_STRING_SIZE];
    strcpy(channelId, ChannelInfoListSample[0].channelId);
    channelId[strlen(ChannelInfoListSample[0].channelId)] = '\0';
    uint16_t totalNumberOfChannels = sizeof(ChannelInfoListSample)/sizeof(ChannelInfoListSample[0]);

    char channelIdRead[sizeof(channelId)/sizeof(channelId[0])];
    uint16_t totalNumberOfChannelsRead;

    const char* const interfaceName = "Channel";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "ChannelId"             , 0 , 's' , &channelId             , &channelIdRead             , 1 },
        { "TotalNumberOfChannels" , 0 , 'q' , &totalNumberOfChannels , &totalNumberOfChannelsRead , 1 },
    };

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {

        if (strcmp(proprietyDescriptor[i].name, "ChannelId") == 0) {
            setStatus = Cdm_ChannelInterfaceSetChannelId(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, channelId);
            getStatus = Cdm_ChannelInterfaceGetChannelId(CDM_OBJECT_PATH_CONTROLLEE, channelIdRead);
        }
        else
        {
            setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
            getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        }
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status ChannelOnSetChannelId (const char* objPath, const char* channelId)
{
    AJ_Status status = AJ_ERR_INVALID;
    int i = 0;
    printf("Channel OnSetChannelId : %s %s\n", objPath, channelId);

    for (i = 0; i < TOTAL_NUM_OF_CHANNEL; i++) {
        if (strcmp(ChannelInfoValues[i][0], channelId) == 0) {
            status = AJ_OK;
            break;
        }
    }

    return status;
}

AJ_Status ChannelOnGetChannelList (const char* objPath, const uint16_t startingRecord, const uint16_t numRecords,
                                   ChannelInfoRecord** listOfChannelInfoRecords, uint16_t* numReturnedRecords,
                                   ErrorCode* errorCode)
{
    int16_t size = sizeof(ChannelInfoListSample)/sizeof(ChannelInfoListSample[0]);

    printf("Channel OnGetChannelList : %s, startingRecord : %u, numRecords : %u\n", objPath, startingRecord, numRecords);

    if (startingRecord >= size) {
        if (errorCode) {
            *errorCode = INVALID_VALUE;
        }
        return AJ_ERR_INVALID;
    }

    *listOfChannelInfoRecords = &ChannelInfoListSample[startingRecord];
    *numReturnedRecords = ((startingRecord + numRecords) <= size) ? numRecords : (size - startingRecord);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_CHANNEL_IF) */

#if defined(ENABLE_OPERATION_CLIMATE_CONTROL_MODE_IF)
/* Operation.ClimateControlMode */

AJ_Status InitCdmClimateControlModeProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;
    const uint16_t supportedModes[] = { 0, 1, 2, 3, 4, 5 };
    uint16_t mode = 2;
    uint16_t operationalState = 1;
    uint16_t supportedModesRead[sizeof(supportedModes)/sizeof(supportedModes[0])];
    uint16_t modeRead;
    uint16_t operationalStateRead;
    const char* const interfaceName = "ClimateControlMode";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "SupportedModes"   , 'a' , 'q' , &supportedModes   , &supportedModesRead   , sizeof(supportedModes)/sizeof(supportedModes[0]) },
        { "Mode"             ,  0  , 'q' , &mode             , &modeRead             , 1                                                },
        { "OperationalState" ,  0  , 'q' , &operationalState , &operationalStateRead , 1                                                },
    };
    printf("\n");
    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }
    return status;
}

AJ_Status ClimateControlModeOnSetMode (const char* objPath, const uint16_t mode)
{
    printf("ClimateControlMode OnSetMode : %s, %u\n", objPath, mode);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_CLIMATE_CONTROL_MODE_IF) */

#if defined(ENABLE_OPERATION_CLOSED_STATUS_IF)
/* Operation.ClosedStatus */

AJ_Status InitCdmClosedStatusProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;
    bool isClosed = true;
    bool isClosedRead;
    const char* const interfaceName = "ClosedStatus";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "IsClosed" , 0 , 'b' , &isClosed , &isClosedRead , 1 },
    };
    printf("\n");
    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }
    return status;
}
#endif /* defined(ENABLE_OPERATION_CLOSED_STATUS_IF) */

#if defined(ENABLE_OPERATION_CURRENT_POWER_IF)
/* Operation.CurrentPower */

AJ_Status InitCdmCurrentPowerProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    double currentPower = 8.0;
    double precision = 5.0;
    uint16_t updateMinTime = 1;
    double currentPowerRead;
    double precisionRead;
    uint16_t updateMinTimeRead;

    const char* const interfaceName = "CurrentPower";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "CurrentPower" ,  0 , 'd' , &currentPower  , &currentPowerRead  , 1 },
        { "Precision"    ,  0 , 'd' , &precision     , &precisionRead     , 1 },
        { "UpdateMinTime",  0 , 'q' , &updateMinTime , &updateMinTimeRead , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}
#endif /* defined(ENABLE_OPERATION_CURRENT_POWER_IF) */

#if defined(ENABLE_OPERATION_CYCLE_CONTROL_IF)
/* Operation.CycleControl */

AJ_Status InitCdmCycleControlProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t operationalState = 1;
    uint8_t supportedOperationalStates[] = { 0, 1, 2, 3, 4, 5 };
    uint8_t supportedOperationalCommands[] = { 0, 1, 2, 3 };
    uint8_t operationalStateRead;
    uint8_t supportedOperationalStatesRead[sizeof(supportedOperationalStates)/sizeof(supportedOperationalStates[0])];
    uint8_t supportedOperationalCommandsRead[sizeof(supportedOperationalCommands)/sizeof(supportedOperationalCommands[0])];

    const char* const interfaceName = "CycleControl";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "SupportedOperationalStates"   ,  'a' , 'y' , &supportedOperationalStates   , &supportedOperationalStatesRead   , sizeof(supportedOperationalStates)/sizeof(supportedOperationalStates[0])     },
        { "SupportedOperationalCommands" ,  'a' , 'y' , &supportedOperationalCommands , &supportedOperationalCommandsRead , sizeof(supportedOperationalCommands)/sizeof(supportedOperationalCommands[0]) },
        { "OperationalState"             ,   0  , 'y' , &operationalState             , &operationalStateRead             , 1                                                                            },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status CycleControlOnExecuteOperationalCommand (const char* objPath, uint8_t command, ErrorCode* errorCode)
{
    printf("CycleControl OnExecuteOperationalCommand : %s, %u\n", objPath, command);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_CYCLE_CONTROL_IF) */

#if defined(ENABLE_OPERATION_DISH_WASHING_CYCLE_PHASE_IF)
/* Operation.DishWashingCyclePhase */

DishWashingCyclePhaseCyclePhaseDescriptor DishWashingVendorPhases[] = {
    { 0x81  , (char*)"DW phase a"  , (char*)"DW descr. a"},
    { 0x82  , (char*)"DW phase b"  , (char*)"DW descr. b"},
    { 0xAA  , (char*)"DW phase c"  , (char*)"DW descr. c"},
};

AJ_Status InitCdmDishWashingCyclePhaseProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t cyclePhase = 1;
    uint8_t supportedCyclePhases[]  = {
        0, 1, 2, 3, 4,
        DishWashingVendorPhases[0].phase,
        DishWashingVendorPhases[1].phase,
        DishWashingVendorPhases[2].phase
    };
    uint8_t cyclePhaseRead;
    uint8_t supportedCyclePhasesRead[sizeof(supportedCyclePhases)/sizeof(supportedCyclePhases[0])];

    const char* const interfaceName = "DishWashingCyclePhase";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "SupportedCyclePhases" , 'a' , 'y' , &supportedCyclePhases , &supportedCyclePhasesRead , sizeof(supportedCyclePhases)/sizeof(supportedCyclePhases[0]) },
        { "CyclePhase"           ,  0  , 'y' , &cyclePhase           , &cyclePhaseRead           , 1                                                            },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status DishWashingCyclePhaseOnGetVendorPhasesDescription (const char* objPath, const char* languageTag,
                            DishWashingCyclePhaseCyclePhaseDescriptor** phasesDescription, uint16_t* numReturnedRecords,
                            ErrorCode* errorCode)
{
    printf("DishWashingCyclePhase OnGetVendorPhasesDescription : %s, %s\n", objPath, languageTag);

    if(!strncmp(languageTag, "en", strlen(languageTag))) {
        *phasesDescription = &DishWashingVendorPhases[0];
        *numReturnedRecords = sizeof(DishWashingVendorPhases) / sizeof(DishWashingVendorPhases[0]);

        return AJ_OK;
    } else {
        if (errorCode) {
            *errorCode = LANGUAGE_NOT_SUPPORTED;
        }

        return AJ_ERR_FAILURE;
    }
}

#endif /* defined(ENABLE_OPERATION_DISH_WASHING_CYCLE_PHASE_IF) */

#if defined(ENABLE_OPERATION_ENERGY_USAGE_IF)
/* Operation.EnergyUsage */

AJ_Status InitCdmEnergyUsageProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    double cumulativeEnergy = 70.0;
    double precision = 5.0;
    uint16_t updateMinTime = 1;
    double cumulativeEnergyRead;
    double precisionRead;
    uint16_t updateMinTimeRead;

    const char* const interfaceName = "EnergyUsage";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "CumulativeEnergy" ,  0 , 'd' , &cumulativeEnergy  , &cumulativeEnergyRead  , 1 },
        { "Precision"        ,  0 , 'd' , &precision         , &precisionRead         , 1 },
        { "UpdateMinTime"    ,  0 , 'q' , &updateMinTime     , &updateMinTimeRead     , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status EnergyUsageOnResetCumulativeEnergy (const char* objPath, ErrorCode* errorCode)
{
    printf("EnergyUsage OnResetCumulativeEnergy : %s\n", objPath);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_ENERGY_USAGE_IF) */

#if defined(ENABLE_OPERATION_FAN_SPEED_LEVEL_IF)
/* Operation.FanSpeedLevel */

AJ_Status InitCdmFanSpeedLevelProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t fanSpeedLevel = 3;
    uint8_t maxFanSpeedLevel = 10;
    uint8_t autoMode = 1;
    uint8_t fanSpeedLevelRead;
    uint8_t maxFanSpeedLevelRead;
    uint8_t autoModeRead;

    const char* const interfaceName = "FanSpeedLevel";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "MaxFanSpeedLevel" ,  0  , 'y' , &maxFanSpeedLevel  , &maxFanSpeedLevelRead , 1 },
        { "AutoMode"         ,  0  , 'y' , &autoMode          , &autoModeRead         , 1 },
        { "FanSpeedLevel"    ,  0  , 'y' , &fanSpeedLevel     , &fanSpeedLevelRead    , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status FanSpeedLevelOnSetFanSpeedLevel(const char* objPath, const uint8_t fanSpeedLevel)
{
    printf("FanSpeedLevel OnSetFanSpeedLevel : %s, %u\n", objPath, fanSpeedLevel);

    return AJ_OK;
}

AJ_Status FanSpeedLevelOnSetAutoMode(const char* objPath, const uint8_t autoMode)
{
    printf("FanSpeedLevel OnSetAutoMode : %s, %u\n", objPath, autoMode);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_FAN_SPEED_LEVEL_IF) */

#if defined(ENABLE_OPERATION_FILTER_STATUS_IF)
/* Operation.FilterStatus */

#define MAX_MANUFACTURER_STRING_SIZE 10
#define MAX_PARTNUMBER_STRING_SIZE 10
#define MAX_URL_STRING_SIZE 30

AJ_Status InitCdmFilterStatusProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint16_t expectedLifeInDays = 30;
    bool isCleanable = true;
    uint8_t orderPercentage = 10;
    char manufacturer[MAX_MANUFACTURER_STRING_SIZE] = "CDM";
    char partNumber[MAX_PARTNUMBER_STRING_SIZE] = "A1";
    char url[MAX_URL_STRING_SIZE] = "http://";
    uint8_t lifeRemaining = 100;
    uint16_t expectedLifeInDaysRead;
    bool isCleanableRead;
    uint8_t orderPercentageRead;
    char manufacturerRead[MAX_MANUFACTURER_STRING_SIZE];
    char partNumberRead[MAX_PARTNUMBER_STRING_SIZE];
    char urlRead[MAX_URL_STRING_SIZE];
    uint8_t lifeRemainingRead;

    const char* const interfaceName = "FilterStatus";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "ExpectedLifeInDays" ,  0  , 'q' , &expectedLifeInDays  , &expectedLifeInDaysRead , 1 },
        { "IsCleanable"        ,  0  , 'b' , &isCleanable         , &isCleanableRead        , 1 },
        { "OrderPercentage"    ,  0  , 'y' , &orderPercentage     , &orderPercentageRead    , 1 },
        { "Manufacturer"       ,  0  , 's' , &manufacturer        , &manufacturerRead       , 1 },
        { "PartNumber"         ,  0  , 's' , &partNumber          , &partNumberRead         , 1 },
        { "Url"                ,  0  , 's' , &url                 , &urlRead                , 1 },
        { "LifeRemaining"      ,  0  , 'y' , &lifeRemaining       , &lifeRemainingRead      , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}
#endif /* defined(ENABLE_OPERATION_FILTER_STATUS_IF) */

#if defined(ENABLE_OPERATION_HEATING_ZONE_IF)
/* Operation.HeatingZone */

AJ_Status InitCdmHeatingZoneProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t numberOfHeatingZones;
    uint8_t maxHeatingLevels[] = { 10, 10, 10, 10};
    uint8_t heatingLevels[sizeof(maxHeatingLevels)/sizeof(maxHeatingLevels[0])] = { 2 , 4 , 6 , 8 };
    uint8_t numberOfHeatingZonesRead;
    uint8_t maxHeatingLevelsRead[sizeof(maxHeatingLevels)/sizeof(maxHeatingLevels[0])];
    uint8_t heatingLevelsRead[sizeof(maxHeatingLevels)/sizeof(maxHeatingLevels[0])];

    const char* const interfaceName = "HeatingZone";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "NumberOfHeatingZones" ,  0  , 'y' , &numberOfHeatingZones , &numberOfHeatingZonesRead , 1                                                     },
        { "MaxHeatingLevels"     , 'a' , 'y' , &maxHeatingLevels     , &maxHeatingLevelsRead     , sizeof(maxHeatingLevels)/sizeof(maxHeatingLevels[0]) },
        { "HeatingLevels"        , 'a' , 'y' , &heatingLevels        , &heatingLevelsRead        , sizeof(heatingLevels)/sizeof(heatingLevels[0])       },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

#endif /* defined(ENABLE_OPERATION_HEATING_ZONE_IF) */

#if defined(ENABLE_OPERATION_LAUNDRY_CYCLE_PHASE_IF)
/* Operation.LaundryCyclePhase */

LaundryCyclePhaseCyclePhaseDescriptor LaundryVendorPhases[] = {
    { 0x88 , (char*)"Laundry phase a"  , (char*)"Laundry descr. a"},
    { 0x99 , (char*)"Laundry phase b"  , (char*)"Laundry descr. b"},
};

AJ_Status InitCdmLaundryCyclePhaseProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t cyclePhase = 1;
    uint8_t supportedCyclePhases[]  = {
        0, 1, 2, 3, 4 , 5, 6, 7, 8,
        LaundryVendorPhases[0].phase,
        LaundryVendorPhases[1].phase,
    };
    uint8_t cyclePhaseRead;
    uint8_t supportedCyclePhasesRead[sizeof(supportedCyclePhases)/sizeof(supportedCyclePhases[0])];

    const char* const interfaceName = "LaundryCyclePhase";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "SupportedCyclePhases" , 'a' , 'y' , &supportedCyclePhases , &supportedCyclePhasesRead , sizeof(supportedCyclePhases)/sizeof(supportedCyclePhases[0]) },
        { "CyclePhase"           ,  0  , 'y' , &cyclePhase           , &cyclePhaseRead           , 1                                                            },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status LaundryCyclePhaseOnGetVendorPhasesDescription (const char* objPath, const char* languageTag,
                            LaundryCyclePhaseCyclePhaseDescriptor** phasesDescription, uint16_t* numReturnedRecords,
                            ErrorCode* errorCode)
{
    printf("LaundryCyclePhase OnGetVendorPhasesDescription : %s, %s\n", objPath, languageTag);

    if(!strncmp(languageTag, "en", strlen(languageTag))) {
        *phasesDescription = &LaundryVendorPhases[0];
        *numReturnedRecords = sizeof(LaundryVendorPhases) / sizeof(LaundryVendorPhases[0]);

        return AJ_OK;
    } else {
        if (errorCode) {
            *errorCode = LANGUAGE_NOT_SUPPORTED;
        }

        return AJ_ERR_FAILURE;
    }
}

#endif /* defined(ENABLE_OPERATION_LAUNDRY_CYCLE_PHASE_IF) */

#if defined (ENABLE_OPERATION_MOISTURE_OUTPUT_LEVEL_IF)
/* Operation.MoistureOutputLevel */

AJ_Status InitCdmMoistureOutputLevelProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t moistureOutputLevel = 0;
    uint8_t maxMoistureOutputLevel = 10;
    uint8_t autoMode = 0;
    uint8_t moistureOutputLevelRead;
    uint8_t maxMoistureOutputLevelRead;
    uint8_t autoModeRead;

    const char* const interfaceName = "MoistureOutputLevel";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "MaxMoistureOutputLevel" , 0 , 'y' , &maxMoistureOutputLevel , &maxMoistureOutputLevelRead , 1 },
        { "MoistureOutputLevel"    , 0 , 'y' , &moistureOutputLevel    , &moistureOutputLevelRead    , 1 },
        { "AutoMode"               , 0 , 'y' , &autoMode               , &autoModeRead               , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status MoistureOutputLevelOnSetMoistureOutputLevel(const char* objPath, const uint8_t value)
{
    printf("MoistureOutputLevel OnSetMoistureOutputLevel : %s %u\n", objPath, value);

    return AJ_OK;
}

AJ_Status MoistureOutputLevelOnSetAutoMode(const char* objPath, const uint8_t autoMode)
{
    printf("MoistureOutputLevel OnSetAutoMode : %s, autoMode: %u\n", objPath, autoMode);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_MOISTURE_OUTPUT_LEVEL_IF) */

#if defined(ENABLE_OPERATION_OFF_CONTROL_IF)
/* Operation.OffControl */

AJ_Status OffControlOnSwitchOff (const char* objPath, ErrorCode* errorCode)
{
    printf("OffControl OnSwitchOff : %s\n", objPath);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_OFF_CONTROL_IF) */

#if defined(ENABLE_OPERATION_ON_CONTROL_IF)
/* Operation.OnControl */

AJ_Status OnControlOnSwitchOn (const char* objPath, ErrorCode* errorCode)
{
    printf("OnControl OnSwitchOn : %s\n", objPath);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_ON_CONTROL_IF) */

#if defined(ENABLE_OPERATION_ON_OFF_STATUS_IF)
/* Operation.OnOffStatus */

AJ_Status InitCdmOnOffStatusProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;
    bool isOn = true;
    bool isOnRead;
    const char* const interfaceName = "OnOffStatus";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "IsOn" , 0 , 'b' , &isOn , &isOnRead , 1 },
    };
    printf("\n");
    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }
    return status;
}
#endif /* defined(ENABLE_OPERATION_ON_OFF_STATUS_IF) */

#if defined(ENABLE_OPERATION_OVEN_CYCLE_PHASE_IF)
/* Operation.OvenCyclePhase */

OvenCyclePhaseCyclePhaseDescriptor OvenVendorPhases[] = {
    { 0xAA , (char*)"Oven phase a"  , (char*)"Oven descr. a"},
    { 0xBB , (char*)"Oven phase b"  , (char*)"Oven descr. b"},
    { 0xCC , (char*)"Oven phase b"  , (char*)"Oven descr. b"},
};

AJ_Status InitCdmOvenCyclePhaseProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;
    uint8_t cyclePhase = 1;
    uint8_t supportedCyclePhases[]  = {
        0, 1, 2, 3,
        OvenVendorPhases[0].phase,
        OvenVendorPhases[1].phase,
        OvenVendorPhases[3].phase,
    };
    uint8_t cyclePhaseRead;
    uint8_t supportedCyclePhasesRead[sizeof(supportedCyclePhases)/sizeof(supportedCyclePhases[0])];
    const char* const interfaceName = "OvenCyclePhase";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "SupportedCyclePhases" , 'a' , 'y' , &supportedCyclePhases , &supportedCyclePhasesRead , sizeof(supportedCyclePhases)/sizeof(supportedCyclePhases[0]) },
        { "CyclePhase"           ,  0  , 'y' , &cyclePhase           , &cyclePhaseRead           , 1                                                            },
    };
    printf("\n");
    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }
    return status;
}

AJ_Status OvenCyclePhaseOnGetVendorPhasesDescription (const char* objPath, const char* languageTag,
                            OvenCyclePhaseCyclePhaseDescriptor** phasesDescription, uint16_t* numReturnedRecords,
                            ErrorCode* errorCode)
{
    printf("OvenCyclePhase OnGetVendorPhasesDescription : %s, %s\n", objPath, languageTag);

    if(!strncmp(languageTag, "en", strlen(languageTag))) {
        *phasesDescription = &OvenVendorPhases[0];
        *numReturnedRecords = sizeof(OvenVendorPhases) / sizeof(OvenVendorPhases[0]);

        return AJ_OK;
    } else {
        if (errorCode) {
            *errorCode = LANGUAGE_NOT_SUPPORTED;
        }

        return AJ_ERR_FAILURE;
    }
}

#endif /* defined(ENABLE_OPERATION_OVEN_CYCLE_PHASE_IF) */

#if defined(ENABLE_OPERATION_RAPID_MODE_IF)
/* Operation.RapidMode */

AJ_Status InitCdmRapidModeProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    bool rapidMode = true;
    bool rapidModeRead;

    const char* const interfaceName = "RapidMode";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "RapidMode" , 0 , 'b' , &rapidMode , &rapidModeRead , 1 },
    };

    printf("\n");


    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status RapidModeOnSetRapidMode(const char* objPath, const bool rapidMode)
{
    printf("RapidMode OnSetRapidMode : %s, %u\n", objPath, rapidMode);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_RAPID_MODE_IF) */

#if defined(ENABLE_OPERATION_RAPID_MODE_TIMED_IF)

AJ_Status InitCdmRapidModeTimedProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint16_t rapidModeMinutesRemaining = 0;
    uint16_t rapidModeMinutesRead;
    uint16_t maxSetMinutes = 60;
    uint16_t maxSetMinutesRead;

    const char* const interfaceName = "RapidModeTimed";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "MaxSetMinutes"             , 0 , 'q' , &maxSetMinutes             , &maxSetMinutesRead    , 1 },
        { "RapidModeMinutesRemaining" , 0 , 'q' , &rapidModeMinutesRemaining , &rapidModeMinutesRead , 1 },
    };

    printf("\n");


    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status RapidModeTimedOnSetRapidModeMinutesRemaining(const char* objPath, const uint16_t value)
{
    printf("RapidModeTimed OnSetRapidModeMinutesRemaining : %s %u\n", objPath, value);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_RAPID_MODE_TIMED_IF) */

#if defined(ENABLE_OPERATION_REMOTE_CONTROLLABILITY_IF)
/* Operation.RemoteControllability */

AJ_Status InitCdmRemoteControllabilityProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;
    bool isControllable = true;
    bool isControllableRead;
    const char* const interfaceName = "RemoteControllability";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "IsControllable" , 0 , 'b' , &isControllable , &isControllableRead , 1 },
    };
    printf("\n");
    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }
    return status;
}
#endif /* defined(ENABLE_OPERATION_REMOTE_CONTROLLABILITY_IF) */

#if defined(ENABLE_OPERATION_REPEAT_MODE_IF)
/* Operation.RepeatMode */

AJ_Status InitCdmRepeatModeProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;
    bool repeatMode = true;
    bool repeatModeRead;
    const char* const interfaceName = "RepeatMode";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "RepeatMode" , 0 , 'b' , &repeatMode , &repeatModeRead , 1 },
    };
    printf("\n");
    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }
    return status;
}
#endif /* defined(ENABLE_OPERATION_REPEAT_MODE_IF) */

AJ_Status RepeatModeOnSetRepeatMode(const char* objPath, const bool repeatMode)
{
    printf("RepeatMode OnSetRepeatMode : %s, %u\n", objPath, repeatMode);

    return AJ_OK;
}


#if defined(ENABLE_OPERATION_RESOURCE_SAVING_IF)
/* Operation.ResourceSaving */

AJ_Status InitCdmResourceSavingProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    bool resourceSavingMode = true;
    bool resourceSavingModeRead;

    const char* const interfaceName = "ResourceSaving";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "ResourceSavingMode" , 0 , 'b' , &resourceSavingMode , &resourceSavingModeRead , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status ResourceSavingOnSetResourceSavingMode(const char* objPath, const bool resourceSavingMode)
{
    printf("ResourceSaving OnSetResourceSavingMode : %s, %u\n", objPath, resourceSavingMode);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_RESOURCE_SAVING_IF) */

#if defined(ENABLE_OPERATION_ROBOT_CLEANING_CYCLE_PHASE_IF)
/* Operation.RobotCleaningCyclePhase */

CyclePhaseDescriptor RobotCleaningVendorPhases[] = {
    { 0x80 , (char*)"Robot phase a"  , (char*)"Robot descr. a"},
    { 0x81 , (char*)"Robot phase b"  , (char*)"Robot descr. b"},
};

AJ_Status InitCdmRobotCleaningCyclePhaseProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t cyclePhase = 1;
    uint8_t supportedCyclePhases[]  = {
        0, 1,
        RobotCleaningVendorPhases[0].phase,
        RobotCleaningVendorPhases[1].phase,
    };
    uint8_t cyclePhaseRead;
    uint8_t supportedCyclePhasesRead[sizeof(supportedCyclePhases)/sizeof(supportedCyclePhases[0])];

    const char* const interfaceName = "RobotCleaningCyclePhase";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "SupportedCyclePhases" , 'a' , 'y' , &supportedCyclePhases , &supportedCyclePhasesRead , sizeof(supportedCyclePhases)/sizeof(supportedCyclePhases[0]) },
        { "CyclePhase"           ,  0  , 'y' , &cyclePhase           , &cyclePhaseRead           , 1                                                            },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status RobotCleaningCyclePhaseOnGetVendorPhasesDescription (const char* objPath, const char* languageTag,
                            CyclePhaseDescriptor** phasesDescription, uint16_t* numReturnedRecords,
                            ErrorCode* errorCode)
{
    printf("RobotCleaningCyclePhase OnGetVendorPhasesDescription : %s, %s\n", objPath, languageTag);

    if(!strncmp(languageTag, "en", strlen(languageTag))) {
        *phasesDescription = &RobotCleaningVendorPhases[0];
        *numReturnedRecords = sizeof(RobotCleaningVendorPhases) / sizeof(RobotCleaningVendorPhases[0]);

        return AJ_OK;
    } else {
        if (errorCode) {
            *errorCode = LANGUAGE_NOT_SUPPORTED;
        }

        return AJ_ERR_FAILURE;
    }
}

#endif /* defined(ENABLE_OPERATION_ROBOT_CLEANING_CYCLE_PHASE_IF) */

#if defined(ENABLE_OPERATION_SOIL_LEVEL_IF)
/* Operation.SoilLevel */

AJ_Status InitCdmSoilLevelProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t maxLevel = 5;
    uint8_t targetLevel = 1;
    uint8_t selectableLevels[] = { 0, 1, 2, 3 };
    uint8_t maxLevelRead;
    uint8_t targetLevelRead;
    uint8_t selectableLevelsRead[sizeof(selectableLevels)/sizeof(selectableLevels[0])];

    const char* const interfaceName = "SoilLevel";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "MaxLevel"         ,  0  , 'y' , &maxLevel         , &maxLevelRead         , 1                                                                          },
        { "SelectableLevels" , 'a' , 'y' , &selectableLevels , &selectableLevelsRead , sizeof(selectableLevels)/sizeof(selectableLevels[0]) },
        { "TargetLevel"      ,  0  , 'y' , &targetLevel      , &targetLevelRead      , 1                                                                          },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status SoilLevelListenerOnSetTargetLevel(const char* objPath, const uint8_t targetLevel)
{
    printf("SoilLevel OnSetTargetLevel  : %s, targetLevel : %u\n", objPath, targetLevel);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_SOIL_LEVEL_IF) */

#if defined(ENABLE_OPERATION_SPIN_SPEED_LEVEL_IF)
/* Operation.SpinSpeedLevel */

AJ_Status InitCdmSpinSpeedLevelProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t maxLevel = 5;
    uint8_t targetLevel = 1;
    uint8_t selectableLevels[] = { 0, 1, 2, 3 };
    uint8_t maxLevelRead;
    uint8_t targetLevelRead;
    uint8_t selectableLevelsRead[sizeof(selectableLevels)/sizeof(selectableLevels[0])];

    const char* const interfaceName = "SpinSpeedLevel";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "MaxLevel"         ,  0  , 'y' , &maxLevel         , &maxLevelRead         , 1                                                                          },
        { "SelectableLevels" , 'a' , 'y' , &selectableLevels , &selectableLevelsRead , sizeof(selectableLevels)/sizeof(selectableLevels[0]) },
        { "TargetLevel"      ,  0  , 'y' , &targetLevel      , &targetLevelRead      , 1                                                                          },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status SpinSpeedLevelListenerOnSetTargetLevel(const char* objPath, const uint8_t targetLevel)
{
    printf("SpinSpeedLevel OnSetTargetLevel  : %s, targetLevel : %u\n", objPath, targetLevel);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_SPIN_SPEED_LEVEL_IF) */

#if defined(ENABLE_OPERATION_TIMER_IF)
/* Operation.SpinSpeedLevel */

AJ_Status InitCdmTimerProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

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

    const char* const interfaceName = "TimerInterface";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "ReferenceTimer"     , 0 , 'i' , &referenceTimer     , &referenceTimerRead     , 1 },
        { "TargetTimeToStart"  , 0 , 'i' , &targetTimeToStart  , &targetTimeToStartRead  , 1 },
        { "TargetTimeToStop"   , 0 , 'i' , &targetTimeToStop   , &targetTimeToStopRead   , 1 },
        { "EstimatedTimeToEnd" , 0 , 'i' , &estimatedTimeToEnd , &estimatedTimeToEndRead , 1 },
        { "RunningTime"        , 0 , 'i' , &runningTime        , &runningTimeRead        , 1 },
        { "TargetDuration"     , 0 , 'i' , &targetDuration     , &targetDurationRead     , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status TimerListenerOnSetTargetTimeToStart(const char* objPath, const int32_t targetTimeToStart,
    ErrorCode* errorCode)
{
    printf("TimerListener OnSetTargetTimeToStart  : %s, targetTimeToStart : %i\n", objPath, targetTimeToStart);
    if(targetTimeToStart == TIMER_FEATURE_NOT_APPLIED ||
       targetTimeToStart == TIMER_NOT_SUPPORTED )
    {
        *errorCode = INVALID_VALUE;
        return AJ_ERR_INVALID;
    }

    return AJ_OK;
}

AJ_Status TimerListenerOnSetTargetTimeToStop(const char* objPath, const int32_t targetTimeToStop,
    ErrorCode* errorCode)
{
    printf("TimerListener OnSetTargetTimeToStop  : %s, targetTimeToStop : %i\n", objPath, targetTimeToStop);
    if(targetTimeToStop == TIMER_FEATURE_NOT_APPLIED ||
       targetTimeToStop == TIMER_NOT_SUPPORTED )
    {
        *errorCode = INVALID_VALUE;
        return AJ_ERR_INVALID;
    }

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_TIMER_IF) */

#if defined(ENABLE_ENVIRONMENT_CURRENT_HUMIDITY_IF)
/* Environment.CurrentHumidity */

AJ_Status InitCdmCurrentHumidityProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t currentValue = 30;
    uint8_t maxValue = 90;
    uint8_t currentValueRead;
    uint8_t maxValueRead;

    const char* const interfaceName = "CurrentHumidity";

    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "CurrentValue"  , 0 , 'y' , &currentValue  , &currentValueRead  , 1 },
        { "MaxValue"      , 0 , 'y' , &maxValue      , &maxValueRead      , 1 },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

#endif /* defined(ENABLE_ENVIRONMENT_CURRENT_HUMIDITY_IF) */

#if defined(ENABLE_ENVIRONMENT_TARGET_HUMIDITY_IF)
/* Environment.TargetHumidity */

AJ_Status InitCdmTargetHumidityProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t targetValue = 40;
    uint8_t minValue = 10;
    uint8_t maxValue = 70;
    uint8_t stepValue = 5;
    uint8_t selectableHumidityLevels[] = { 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70 };
    uint8_t targetValueRead;
    uint8_t minValueRead;
    uint8_t maxValueRead;
    uint8_t stepValueRead;
    uint8_t selectableHumidityLevelsRead[sizeof(selectableHumidityLevels)/sizeof(selectableHumidityLevels[0])];

    const char* const interfaceName = "TargetHumidity";
    /* be carefull to the order */
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "MinValue"                ,  0 , 'y' , &minValue                   , &minValueRead                   , 1 },
        { "MaxValue"                ,  0 , 'y' , &maxValue                   , &maxValueRead                   , 1 },
        { "StepValue"               ,  0 , 'y' , &stepValue                  , &stepValueRead                  , 1 },
        { "TargetValue"             ,  0 , 'y' , &targetValue                , &targetValueRead                , 1 },
        { "SelectableHumidityLevels", 'a', 'y' , &selectableHumidityLevels   , &selectableHumidityLevelsRead   , sizeof(selectableHumidityLevels)/sizeof(selectableHumidityLevels[0]) },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status TargetHumidityOnSetTargetValue(const char* objPath, const uint8_t targetValue)
{
    printf("TargetHumidity OnSetTargetValue : %s, targetValue: %u\n", objPath, targetValue);

    return AJ_OK;
}

#endif /* defined(ENABLE_ENVIRONMENT_TARGET_HUMIDITY_IF) */

#if defined(ENABLE_ENVIRONMENT_TARGET_TEMPERATURE_LEVEL_IF)
/* Environment.TargetTemperatureLevel */

AJ_Status InitCdmTargetTemperatureLevelProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;

    uint8_t targetLevel = 40;
    uint8_t maxLevel = 70;
    uint8_t selectableTemperatureLevels[] = { 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70 };
    uint8_t targetLevelRead;
    uint8_t maxLevelRead;
    uint8_t selectableTemperatureLevelsRead[sizeof(selectableTemperatureLevels)/sizeof(selectableTemperatureLevels[0])];

    const char* const interfaceName = "TargetTemperatureLevel";
    /* be carefull to the order */
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "MaxLevel"                ,  0 , 'y' , &maxLevel                   , &maxLevelRead                   , 1 },
        { "TargetLevel"             ,  0 , 'y' , &targetLevel                , &targetLevelRead                , 1 },
        { "SelectableTemperatureLevels", 'a', 'y' , &selectableTemperatureLevels   , &selectableTemperatureLevelsRead   , sizeof(selectableTemperatureLevels)/sizeof(selectableTemperatureLevels[0]) },
    };

    printf("\n");

    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }

    return status;
}

AJ_Status TargetTemperatureLevelOnSetTargetLevel(const char* objPath, const uint8_t targetLevel)
{
    printf("TargetTemperatureLevel OnSetTargetLevel : %s, targetLevel: %u\n", objPath, targetLevel);

    return AJ_OK;
}

#endif /* defined(ENABLE_ENVIRONMENT_TARGET_TEMPERATURE_LEVEL_IF) */

#if defined(ENABLE_OPERATION_HVAC_FAN_MODE_IF)
/* Operation.HvacFanMode */

AJ_Status InitCdmHvacFanModeProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    uint8_t i = 0;
    const uint16_t supportedModes[] = { 0, 1, 2, 3, 4, 5 };
    uint16_t mode = 2;
    uint16_t supportedModesRead[sizeof(supportedModes)/sizeof(supportedModes[0])];
    uint16_t modeRead;

    const char* const interfaceName = "HvacFanMode";
    const CDM_ProprietyDescriptor proprietyDescriptor[] = {
        { "SupportedModes"   , 'a' , 'q' , &supportedModes   , &supportedModesRead   , sizeof(supportedModes)/sizeof(supportedModes[0]) },
        { "Mode"             ,  0  , 'q' , &mode             , &modeRead             , 1                                                },
    };
    printf("\n");
    for (i = 0; i < sizeof(proprietyDescriptor)/sizeof(proprietyDescriptor[0]); i++) {
        setStatus = CdmInterfaceSetProperty(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        getStatus = CdmInterfaceGetProperty(CDM_OBJECT_PATH_CONTROLLEE, interfaceName, &proprietyDescriptor[i]);
        InitCdmPropertiesPrintResult(setStatus, getStatus, interfaceName, &proprietyDescriptor[i]);
    }
    return status;
}

AJ_Status HvacFanModeOnSetMode (const char* objPath, const uint16_t mode)
{
    printf("HvacFanMode OnSetMode : %s, %u\n", objPath, mode);

    return AJ_OK;
}

#endif /* defined(ENABLE_OPERATION_HVAC_FAN_MODE_IF) */

#if defined(ENABLE_OPERATION_PLUG_IN_UNITS_IF)
/* Operation.PlugInUnits*/

AJ_Status InitCdmPlugInUnitsProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    AJ_Status getStatus = AJ_OK;
    AJ_Status setStatus = AJ_OK;
    PlugInInfo units[2] = { { CDM_OBJECT_PATH_CONTROLLEE, 1, false }, { CDM_OBJECT_PATH_CONTROLLEE, 2, true } };
    PlugInInfo unitsRead[2];

    setStatus = Cdm_PlugInUnitsInterfaceSetPlugInUnits(busAttachment, CDM_OBJECT_PATH_CONTROLLEE, units, 2);
    getStatus = Cdm_PlugInUnitsInterfaceGetPlugInUnits(CDM_OBJECT_PATH_CONTROLLEE, unitsRead);
    printf("PlugInUnits Read \n");
    int i = 0;
    for (i = 0; i < 2; i++) {
        printf("%s, %d, %s\n", unitsRead[i].objectPath, unitsRead[i].deviceId, unitsRead[i].pluggedIn ? "true" : "false");
    }

    return status;
}

#endif /* defined(ENABLE_OPERATION_PLUG_IN_UNITS) */

AJ_Status InitCdmProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;

    /* init properties for each interface */

#if defined(ENABLE_ENVIRONMENT_CURRENT_AIR_QUALITY_IF)
    /* Environment.CurrentAirQuality */
    status = InitCdmCurrentAirQualityProperties(busAttachment);
#endif

#if defined(ENABLE_ENVIRONMENT_CURRENT_AIR_QUALITY_LEVEL_IF)
    /* Environment.CurrentAirQualityLevel */
    status = InitCdmCurrentAirQualityLevelProperties(busAttachment);
#endif

#if defined(ENABLE_ENVIRONMENT_CURRENT_TEMPERATURE_IF)
    /* Environment.CurrentTemperature */
    status = InitCdmCurrentTemperatureProperties(busAttachment);
#endif

#if defined(ENABLE_ENVIRONMENT_TARGET_TEMPERATURE_IF)
    /* Environment.TargetTemperature */
    status = InitCdmTargetTemperatureProperties(busAttachment);
#endif

#if defined(ENABLE_ENVIRONMENT_WATER_LEVEL_IF)
    /* Environment.WaterLevel */
    status = InitCdmWaterLevelProperties(busAttachment);
#endif

#if defined(ENABLE_ENVIRONMENT_WIND_DIRECTION_IF)
    /* Environment.WindDirection */
    status = InitCdmWindDirectionProperties(busAttachment);
#endif

#if defined(ENABLE_INPUT_HID_IF)
    /* Input.Hid */
    status = InitCdmHidProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_AIR_RECIRCULATION_MODE_IF)
    /* Operation.AirRecirculationMode */
    status = InitCdmAirRecirculationModeProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_ALERTS_IF)
    /* Operation.Alerts */
    status = InitCdmAlertsProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_AUDIO_VIDEO_INPUT_IF)
    /* Operation.AudioVideoInput */
    status = InitCdmAudioVideoInputProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_AUDIO_VOLUME_IF)
    /* Operation.AudioVolume */
    status = InitCdmAudioVolumeProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_BATTERY_STATUS_IF)
    /* Operation.BatteryStatus */
    status = InitCdmBatteryStatusProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_CHANNEL_IF)
    /* Operation.Channel */
    status = InitCdmChannelProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_CLIMATE_CONTROL_MODE_IF)
    /* Operation.ClimateControlMode */
    status = InitCdmClimateControlModeProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_CLOSED_STATUS_IF)
    /* Operation.ClosedStatus */
    status = InitCdmClosedStatusProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_CURRENT_POWER_IF)
    /* Operation.CurrentPower */
    status = InitCdmCurrentPowerProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_CYCLE_CONTROL_IF)
    /* Operation.CycleControl */
    status = InitCdmCycleControlProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_DISH_WASHING_CYCLE_PHASE_IF)
    /* Operation.DishWashingCyclePhase */
    status = InitCdmDishWashingCyclePhaseProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_ENERGY_USAGE_IF)
    /* Operation.EnergyUsage */
    status = InitCdmEnergyUsageProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_FAN_SPEED_LEVEL_IF)
    /* Operation.FanSpeedLevel */
    status = InitCdmFanSpeedLevelProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_FILTER_STATUS_IF)
    /* Operation.FilterStatus */
    status = InitCdmFilterStatusProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_HEATING_ZONE_IF)
    /* Operation.HeatingZone */
    status = InitCdmHeatingZoneProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_LAUNDRY_CYCLE_PHASE_IF)
    /* Operation.LaundryCyclePhase */
    status = InitCdmLaundryCyclePhaseProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_MOISTURE_OUTPUT_LEVEL_IF)
    /* Operation.MoistureOutputLevel */
    status = InitCdmMoistureOutputLevelProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_OFF_CONTROL_IF)
    /* Operation.OffControl */
//    status = InitCdmOffControlProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_ON_CONTROL_IF)
    /* Operation.OnControl */
//    status = InitCdmOnControlProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_ON_OFF_STATUS_IF)
    /* Operation.OnOffStatus */
    status = InitCdmOnOffStatusProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_OVEN_CYCLE_PHASE_IF)
    /* Operation.OvenCyclePhase */
    status = InitCdmOvenCyclePhaseProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_RAPID_MODE_IF)
    /* Operation.RapidMode */
    status = InitCdmRapidModeProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_RAPID_MODE_TIMED_IF)
    /* Operation.RapidModetimed */
    status = InitCdmRapidModeTimedProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_REMOTE_CONTROLLABILITY_IF)
    /* Operation.RemoteControllability */
    status = InitCdmRemoteControllabilityProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_REPEAT_MODE_IF)
    /* Operation.RepeatMode */
    status = InitCdmRepeatModeProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_RESOURCE_SAVING_IF)
    /* Operation.ResourceSaving */
    status = InitCdmResourceSavingProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_ROBOT_CLEANING_CYCLE_PHASE_IF)
    /* Operation.RobotCleaningCyclePhase */
    status = InitCdmRobotCleaningCyclePhaseProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_SOIL_LEVEL_IF)
    /* Operation.SoilLevel */
    status = InitCdmSoilLevelProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_SPIN_SPEED_LEVEL_IF)
    /* Operation.SpinSpeedLevel */
    status = InitCdmSpinSpeedLevelProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_TIMER_IF)
    /* Operation.Timer */
    status = InitCdmTimerProperties(busAttachment);
#endif

#if defined(ENABLE_ENVIRONMENT_CURRENT_HUMIDITY_IF)
    /* Environment.CurrentHumidity */
    status = InitCdmCurrentHumidityProperties(busAttachment);
#endif

#if defined(ENABLE_ENVIRONMENT_TARGET_HUMIDITY_IF)
    /* Environment.TargetHumidity */
    status = InitCdmTargetHumidityProperties(busAttachment);
#endif

#if defined(ENABLE_ENVIRONMENT_TARGET_TEMPERATURE_LEVEL_IF)
    /* Environment.TargetTemperatureLevel */
    status = InitCdmTargetTemperatureLevelProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_HVAC_FAN_MODE_IF)
    /* Operation.HvacFanMode */
    status = InitCdmHvacFanModeProperties(busAttachment);
#endif

#if defined(ENABLE_OPERATION_PLUG_IN_UNITS_IF)
    /* Operation.PlugInUnits*/
    status = InitCdmPlugInUnitsProperties(busAttachment);
#endif


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
#if defined(ENABLE_ENVIRONMENT_CURRENT_AIR_QUALITY_IF)
    /* Environment.CurrentAirQuality*/
    CurrentAirQualityListener currentAirQualityListener;
#endif

#if defined(ENABLE_ENVIRONMENT_CURRENT_AIR_QUALITY_LEVEL_IF)
    /* Environment.CurrentAirQualityLevel*/
    CurrentAirQualityLevelListener currentAirQualityLevelListener;
#endif

#if defined(ENABLE_ENVIRONMENT_CURRENT_TEMPERATURE_IF)
    /* Environment.CurrentTemperature */
    CurrentTemperatureListener currentTemperatureListener;
#endif

#if defined(ENABLE_ENVIRONMENT_TARGET_TEMPERATURE_IF)
    /* Environment.TargetTemperature */
    TargetTemperatureListener targetTemperatureListener;
#endif

#if defined(ENABLE_ENVIRONMENT_WATER_LEVEL_IF)
    /* Environment.WaterLevel */
    WaterLevelListener waterLevelListener;
#endif

#if defined(ENABLE_ENVIRONMENT_WIND_DIRECTION_IF)
    /* Environment.WindDirection */
    WindDirectionListener windDirectionListener;
#endif

#if defined(ENABLE_INPUT_HID_IF)
    /* Input.Hid */
    HidListener hidListener;
#endif

#if defined(ENABLE_OPERATION_AIR_RECIRCULATION_MODE_IF)
    /* Operation.AirRecirculationMode */
    AirRecirculationModeListener airRecirculationModeListener;
#endif

#if defined(ENABLE_OPERATION_ALERTS_IF)
    /* Operation.Alerts */
    AlertsListener alertsListener;
#endif

#if defined(ENABLE_OPERATION_AUDIO_VIDEO_INPUT_IF)
    /* Operation.AudioVideoInput */
    AudioVideoInputListener audioVideoInputListener;
#endif

#if defined(ENABLE_OPERATION_AUDIO_VOLUME_IF)
    /* Operation.AudioVolume */
    AudioVolumeListener audioVolumeListener;
#endif

#if defined(ENABLE_OPERATION_BATTERY_STATUS_IF)
    /* Operation.BatteryStatus */
    BatteryStatusListener batteryStatusListener;
#endif

#if defined(ENABLE_OPERATION_CHANNEL_IF)
    /* Operation.Channel */
    ChannelListener channelListener;
#endif

#if defined(ENABLE_OPERATION_CLIMATE_CONTROL_MODE_IF)
    /* Operation.ClimateControlMode */
    ClimateControlModeListener climateControlModeListener;
#endif

#if defined(ENABLE_OPERATION_CLOSED_STATUS_IF)
    /* Operation.ClosedStatus */
    ClosedStatusListener closedStatusListener;
#endif

#if defined(ENABLE_OPERATION_CURRENT_POWER_IF)
    /* Operation.CurrentPower */
    CurrentPowerListener currentPowerListener;
#endif

#if defined(ENABLE_OPERATION_CYCLE_CONTROL_IF)
    /* Operation.CycleControl */
    CycleControlListener cycleControlListener;
#endif

#if defined(ENABLE_OPERATION_DISH_WASHING_CYCLE_PHASE_IF)
    /* Operation.DishWashingCyclePhase */
    DishWashingCyclePhaseListener dishWashingCyclePhaseListener;
#endif

#if defined(ENABLE_OPERATION_ENERGY_USAGE_IF)
    /* Operation.EnergyUsage */
    EnergyUsageListener energyUsageListener;
#endif

#if defined(ENABLE_OPERATION_FAN_SPEED_LEVEL_IF)
    /* Operation.FanSpeedLevel */
    FanSpeedLevelListener fanSpeedLevelListener;
#endif

#if defined(ENABLE_OPERATION_FILTER_STATUS_IF)
    /* Operation.FilterStatus */
    FilterStatusListener filterStatusListener;
#endif

#if defined(ENABLE_OPERATION_HEATING_ZONE_IF)
    /* Operation.HeatingZone */
    HeatingZoneListener heatingZoneListener;
#endif

#if defined(ENABLE_OPERATION_LAUNDRY_CYCLE_PHASE_IF)
    /* Operation.LaundryCyclePhase */
    LaundryCyclePhaseListener laundryCyclePhaseListener;
#endif

#if defined(ENABLE_OPERATION_MOISTURE_OUTPUT_LEVEL_IF)
    /* Operation.MoistureOutputLevel */
    MoistureOutputLevelListener moistureOutputLevelListener;
#endif

#if defined(ENABLE_OPERATION_OFF_CONTROL_IF)
    /* Operation.OffControl */
    OffControlListener offControlListener;
#endif

#if defined(ENABLE_OPERATION_ON_CONTROL_IF)
    /* Operation.OnControl */
    OnControlListener onControlListener;
#endif

#if defined(ENABLE_OPERATION_ON_OFF_STATUS_IF)
    /* Operation.OnOffStatus */
    OnOffStatusListener onOffStatusListener;
#endif

#if defined(ENABLE_OPERATION_OVEN_CYCLE_PHASE_IF)
    /* Operation.OvenCyclePhase */
    OvenCyclePhaseListener ovenCyclePhaseListener;
#endif

#if defined(ENABLE_OPERATION_RAPID_MODE_IF)
    /* Operation.RapidMode */
    RapidModeListener rapidModeListener;
#endif

#if defined(ENABLE_OPERATION_RAPID_MODE_TIMED_IF)
    /* Operation.RapidModeTimed */
    RapidModeTimedListener rapidModeTimedListener;
#endif

#if defined(ENABLE_OPERATION_REMOTE_CONTROLLABILITY_IF)
    /* Operation.RemoteControllability */
    RemoteControllabilityListener remoteControllabilityListener;
#endif

#if defined(ENABLE_OPERATION_REPEAT_MODE_IF)
    /* Operation.RepeatMode */
    RepeatModeListener repeatModeListener;
#endif

#if defined(ENABLE_OPERATION_RESOURCE_SAVING_IF)
    /* Operation.ResourceSaving */
    ResourceSavingListener resourceSavingListener;
#endif

#if defined(ENABLE_OPERATION_ROBOT_CLEANING_CYCLE_PHASE_IF)
    /* Operation.RobotCleaningCyclePhase */
    RobotCleaningCyclePhaseListener robotCleaningCyclePhaseListener;
#endif

#if defined(ENABLE_OPERATION_SOIL_LEVEL_IF)
    /* Operation.SoilLevel */
    SoilLevelListener soilLevelListener;
#endif

#if defined(ENABLE_OPERATION_SPIN_SPEED_LEVEL_IF)
    /* Operation.SpinSpeedLevel */
    SpinSpeedLevelListener spinSpeedLevelListener;
#endif

#if defined(ENABLE_OPERATION_TIMER_IF)
    /* Operation.Timer */
    TimerListener timerListener;
#endif

#if defined(ENABLE_ENVIRONMENT_CURRENT_HUMIDITY_IF)
    /* Environment.CurrentHumidity */
    CurrentHumidityListener currentHumidityListener;
#endif

#if defined(ENABLE_ENVIRONMENT_TARGET_HUMIDITY_IF)
    /* Environment.TargetHumidity */
    TargetHumidityListener targetHumidityListener;
#endif

#if defined(ENABLE_ENVIRONMENT_TARGET_TEMPERATURE_LEVEL_IF)
    /* Environment.TargetTemperatureLevel */
    TargetTemperatureLevelListener targetTemperatureLevelListener;
#endif

#if defined(ENABLE_OPERATION_HVAC_FAN_MODE_IF)
    /* Operation.HvacFanMode */
    HvacFanModeListener hvacFanModeListener;
#endif

#if defined(ENABLE_OPERATION_PLUG_IN_UNITS_IF)
    /* Operation.PlugInUnits*/
    PlugInUnitsListener plugInUnitsListener;
#endif


    AJ_Initialize();

    AJ_AboutSetIcon(aboutIconContent, aboutIconContentSize, aboutIconMimetype, aboutIconUrl);

    status = PropertyStore_Init();
    if (status != AJ_OK) {
        goto Exit;
    }

    status = Cdm_Init();

#if defined(ENABLE_ENVIRONMENT_CURRENT_AIR_QUALITY_IF)
    /* Environment.CurrentAirQuality */
    currentAirQualityListener.OnGetContaminantType = NULL;
    currentAirQualityListener.OnGetCurrentValue = NULL;
    currentAirQualityListener.OnGetMaxValue = NULL;
    currentAirQualityListener.OnGetMinValue = NULL;
    currentAirQualityListener.OnGetPrecision = NULL;
    currentAirQualityListener.OnGetUpdateMinTime = NULL;
    status = Cdm_CreateInterface(CURRENT_AIR_QUALITY_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &currentAirQualityListener);
#endif

#if defined(ENABLE_ENVIRONMENT_CURRENT_AIR_QUALITY_LEVEL_IF)
    /* Environment.CurrentAirQualityLevel */
    currentAirQualityLevelListener.OnGetContaminantType = NULL;
    currentAirQualityLevelListener.OnGetCurrentLevel = NULL;
    currentAirQualityLevelListener.OnGetMaxLevel = NULL;
    status = Cdm_CreateInterface(CURRENT_AIR_QUALITY_LEVEL_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &currentAirQualityLevelListener);
#endif

#if defined(ENABLE_ENVIRONMENT_CURRENT_TEMPERATURE_IF)
    /* Environment.CurrentTemperature */
    currentTemperatureListener.OnGetCurrentValue = NULL;
    currentTemperatureListener.OnGetPrecision = NULL;
    currentTemperatureListener.OnGetUpdateMinTime = NULL;
    status = Cdm_CreateInterface(CURRENT_TEMPERATURE_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &currentTemperatureListener);
#endif

#if defined(ENABLE_ENVIRONMENT_TARGET_TEMPERATURE_IF)
    /* Environment.TargetTemperature */
    targetTemperatureListener.OnGetTargetValue = NULL;
    targetTemperatureListener.OnSetTargetValue = TargetTemperatureOnSetTargetValue;
    targetTemperatureListener.OnGetMinValue = NULL;
    targetTemperatureListener.OnGetMaxValue = NULL;
    targetTemperatureListener.OnGetStepValue = NULL;
    status = Cdm_CreateInterface(TARGET_TEMPERATURE_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &targetTemperatureListener);
#endif

#if defined(ENABLE_ENVIRONMENT_WATER_LEVEL_IF)
    /* Environment.WaterLevel */
    waterLevelListener.OnGetSupplySource = NULL;
    waterLevelListener.OnGetCurrentLevel = NULL;
    waterLevelListener.OnGetMaxLevel = NULL;
    waterLevelListener.OnGetMaxLevel = NULL;
    status = Cdm_CreateInterface(WATER_LEVEL_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &waterLevelListener);
#endif

#if defined(ENABLE_ENVIRONMENT_WIND_DIRECTION_IF)
    /* Environment.WindDirection */
    windDirectionListener.OnGetHorizontalDirection = NULL;
    windDirectionListener.OnSetHorizontalDirection = WindDirectionOnSetHorizontalDirection;
    windDirectionListener.OnGetHorizontalMax = NULL;
    windDirectionListener.OnGetHorizontalAutoMode = NULL;
    windDirectionListener.OnSetHorizontalAutoMode = WindDirectionOnSetHorizontalAutoMode;
    windDirectionListener.OnGetVerticalDirection = NULL;
    windDirectionListener.OnSetVerticalDirection = WindDirectionOnSetVerticalDirection;
    windDirectionListener.OnGetVerticalMax = NULL;
    windDirectionListener.OnGetVerticalAutoMode = NULL;
    windDirectionListener.OnSetVerticalAutoMode = WindDirectionOnSetVerticalAutoMode;
    status = Cdm_CreateInterface(WIND_DIRECTION_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &windDirectionListener);
#endif

#if defined(ENABLE_INPUT_HID_IF)
    /* Input.Hid */
    hidListener.OnGetSupportedEvents = NULL;
    hidListener.OnInjectEvents = HidOnInjectEvents;
    status = Cdm_CreateInterface(HID_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &hidListener);
#endif

#if defined(ENABLE_OPERATION_AIR_RECIRCULATION_MODE_IF)
    /* Operation.AirRecirculationMode */
    airRecirculationModeListener.OnGetIsRecirculating = NULL;
    airRecirculationModeListener.OnSetIsRecirculating = AirRecirculationModeOnSetIsRecirculating;
    status = Cdm_CreateInterface(AIR_RECIRCULATION_MODE_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &airRecirculationModeListener);
#endif

#if defined(ENABLE_OPERATION_ALERTS_IF)
    /* Operation.Alerts */
    alertsListener.OnGetAlerts = NULL;
    alertsListener.OnGetAlertCodesDescription = AlertsOnGetAlertCodesDescription;
    alertsListener.OnAcknowledgeAlert = AlertsListenerOnAcknowledgeAlert;
    alertsListener.OnAcknowledgeAllAlerts = AlertsListenerOnAcknowledgeAllAlerts;
    status = Cdm_CreateInterface(ALERTS_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &alertsListener);
#endif

#if defined(ENABLE_OPERATION_AUDIO_VIDEO_INPUT_IF)
    /* Operation.AudioVideoInput */
    audioVideoInputListener.OnGetInputSourceId = NULL;
    audioVideoInputListener.OnSetInputSourceId = AudioVideoInputOnSetInputSourceId;
    audioVideoInputListener.OnGetSupportedInputSources = NULL;
    status = Cdm_CreateInterface(AUDIO_VIDEO_INPUT_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &audioVideoInputListener);
#endif

#if defined(ENABLE_OPERATION_AUDIO_VOLUME_IF)
    /* Operation.AudioVolume */
    audioVolumeListener.OnGetVolume = NULL;
    audioVolumeListener.OnSetVolume = AudioVolumeOnSetVolume;
    audioVolumeListener.OnGetMaxVolume = NULL;
    audioVolumeListener.OnGetMute = NULL;
    audioVolumeListener.OnSetMute = AudioVolumeOnSetMute;
    status = Cdm_CreateInterface(AUDIO_VOLUME_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &audioVolumeListener);
#endif

#if defined(ENABLE_OPERATION_BATTERY_STATUS_IF)
    /* Operation.BatteryStatus */
    batteryStatusListener.OnGetCurrentValue = NULL;
    batteryStatusListener.OnGetIsCharging = NULL;
    status = Cdm_CreateInterface(BATTERY_STATUS_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &batteryStatusListener);
#endif

#if defined(ENABLE_OPERATION_CHANNEL_IF)
    /* Operation.Channel */
    channelListener.OnGetChannelId = NULL;
    channelListener.OnSetChannelId = ChannelOnSetChannelId;
    channelListener.OnGetTotalNumberOfChannels = NULL;
    channelListener.OnGetChannelList = ChannelOnGetChannelList;
    status = Cdm_CreateInterface(CHANNEL_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &channelListener);
#endif

#if defined(ENABLE_OPERATION_CLIMATE_CONTROL_MODE_IF)
    /* Operation.ClimateControlMode */
    climateControlModeListener.OnSetMode = ClimateControlModeOnSetMode;
    climateControlModeListener.OnGetMode = NULL;
    climateControlModeListener.OnGetSupportedModes = NULL;
    climateControlModeListener.OnGetOperationalState = NULL;
    status = Cdm_CreateInterface(CLIMATE_CONTROL_MODE_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &climateControlModeListener);
#endif

#if defined(ENABLE_OPERATION_CLOSED_STATUS_IF)
    /* Operation.ClosedStatus */
    closedStatusListener.OnGetIsClosed = NULL;
    status = Cdm_CreateInterface(CLOSED_STATUS_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &closedStatusListener);
#endif

#if defined(ENABLE_OPERATION_CURRENT_POWER_IF)
    /* Operation.CurrentPower */
    currentPowerListener.OnGetCurrentPower = NULL;
    currentPowerListener.OnGetPrecision = NULL;
    currentPowerListener.OnGetUpdateMinTime = NULL;
    status = Cdm_CreateInterface(CURRENT_POWER_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &currentPowerListener);
#endif

#if defined(ENABLE_OPERATION_CYCLE_CONTROL_IF)
    /* Operation.CycleControl */
    cycleControlListener.OnGetOperationalState = NULL;
    cycleControlListener.OnGetSupportedOperationalStates = NULL;
    cycleControlListener.OnGetSupportedOperationalCommands = NULL;
    cycleControlListener.OnExecuteOperationalCommand = CycleControlOnExecuteOperationalCommand;
    status = Cdm_CreateInterface(CYCLE_CONTROL_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &cycleControlListener);
#endif

#if defined(ENABLE_OPERATION_DISH_WASHING_CYCLE_PHASE_IF)
    /* Operation.DishWashingCyclePhase */
    dishWashingCyclePhaseListener.OnGetCyclePhase = NULL;
    dishWashingCyclePhaseListener.OnGetSupportedCyclePhases = NULL;
    dishWashingCyclePhaseListener.OnGetVendorPhasesDescription = DishWashingCyclePhaseOnGetVendorPhasesDescription;
    status = Cdm_CreateInterface(DISH_WASHING_CYCLE_PHASE_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &dishWashingCyclePhaseListener);
#endif

#if defined(ENABLE_OPERATION_ENERGY_USAGE_IF)
    /* Operation.EnergyUsage */
    energyUsageListener.OnGetCumulativeEnergy = NULL;
    energyUsageListener.OnGetPrecision = NULL;
    energyUsageListener.OnGetUpdateMinTime = NULL;
    energyUsageListener.OnResetCumulativeEnergy = EnergyUsageOnResetCumulativeEnergy;
    status = Cdm_CreateInterface(ENERGY_USAGE_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &energyUsageListener);
#endif

#if defined(ENABLE_OPERATION_FAN_SPEED_LEVEL_IF)
    /* Operation.FanSpeedLevel */
    fanSpeedLevelListener.OnGetFanSpeedLevel = NULL;
    fanSpeedLevelListener.OnSetFanSpeedLevel = FanSpeedLevelOnSetFanSpeedLevel;
    fanSpeedLevelListener.OnGetMaxFanSpeedLevel = NULL;
    fanSpeedLevelListener.OnGetAutoMode = NULL;
    fanSpeedLevelListener.OnSetAutoMode = FanSpeedLevelOnSetAutoMode;
    status = Cdm_CreateInterface(FAN_SPEED_LEVEL_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &fanSpeedLevelListener);
#endif

#if defined(ENABLE_OPERATION_FILTER_STATUS_IF)
    /* Operation.FilterStatus */
    filterStatusListener.OnGetExpectedLifeInDays = NULL;
    filterStatusListener.OnGetIsCleanable = NULL;
    filterStatusListener.OnGetOrderPercentage = NULL;
    filterStatusListener.OnGetManufacturer = NULL;
    filterStatusListener.OnGetPartNumber = NULL;
    filterStatusListener.OnGetUrl = NULL;
    filterStatusListener.OnGetLifeRemaining = NULL;
    status = Cdm_CreateInterface(FILTER_STATUS_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &filterStatusListener);
#endif

#if defined(ENABLE_OPERATION_HEATING_ZONE_IF)
    /* Operation.HeatingZone */
    heatingZoneListener.OnGetNumberOfHeatingZones = NULL;
    heatingZoneListener.OnGetMaxHeatingLevels = NULL;
    heatingZoneListener.OnGetHeatingLevels = NULL;
    status = Cdm_CreateInterface(HEATING_ZONE_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &heatingZoneListener);
#endif

#if defined(ENABLE_OPERATION_LAUNDRY_CYCLE_PHASE_IF)
    /* Operation.LaundryCyclePhase */
    laundryCyclePhaseListener.OnGetCyclePhase = NULL;
    laundryCyclePhaseListener.OnGetSupportedCyclePhases = NULL;
    laundryCyclePhaseListener.OnGetVendorPhasesDescription = LaundryCyclePhaseOnGetVendorPhasesDescription;
    status = Cdm_CreateInterface(LAUNDRY_CYCLE_PHASE_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &laundryCyclePhaseListener);
#endif

#if defined(ENABLE_OPERATION_MOISTURE_OUTPUT_LEVEL_IF)
    /* Operation.MoistureOutputLevel */
    moistureOutputLevelListener.OnGetMoistureOutputLevel = NULL;
    moistureOutputLevelListener.OnSetMoistureOutputLevel = MoistureOutputLevelOnSetMoistureOutputLevel;
    moistureOutputLevelListener.OnGetMaxMoistureOutputLevel = NULL;
    moistureOutputLevelListener.OnGetAutoMode = NULL;
    moistureOutputLevelListener.OnSetAutoMode = MoistureOutputLevelOnSetAutoMode;
    status = Cdm_CreateInterface(MOISTURE_OUTPUT_LEVEL_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &moistureOutputLevelListener);
#endif

#if defined(ENABLE_OPERATION_OFF_CONTROL_IF)
    /* Operation.OffControl */
    offControlListener.OnSwitchOff = OffControlOnSwitchOff;
    status = Cdm_CreateInterface(OFF_CONTROL_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &offControlListener);
#endif

#if defined(ENABLE_OPERATION_ON_CONTROL_IF)
    /* Operation.OnControl */
    onControlListener.OnSwitchOn = OnControlOnSwitchOn;
    status = Cdm_CreateInterface(ON_CONTROL_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &onControlListener);
#endif

#if defined(ENABLE_OPERATION_ON_OFF_STATUS_IF)
    /* Operation.OnOffStatus */
    onOffStatusListener.OnGetIsOn = NULL;
    status = Cdm_CreateInterface(ON_OFF_STATUS_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &onOffStatusListener);
#endif

#if defined(ENABLE_OPERATION_OVEN_CYCLE_PHASE_IF)
    /* Operation.OvenCyclePhase */
    ovenCyclePhaseListener.OnGetCyclePhase = NULL;
    ovenCyclePhaseListener.OnGetSupportedCyclePhases = NULL;
    ovenCyclePhaseListener.OnGetVendorPhasesDescription = OvenCyclePhaseOnGetVendorPhasesDescription;
    status = Cdm_CreateInterface(OVEN_CYCLE_PHASE_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &ovenCyclePhaseListener);
#endif

#if defined(ENABLE_OPERATION_RAPID_MODE_IF)
    /* Operation.RapidMode */
    rapidModeListener.OnGetRapidMode = NULL;
    rapidModeListener.OnSetRapidMode = RapidModeOnSetRapidMode;
    status = Cdm_CreateInterface(RAPID_MODE_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &rapidModeListener);
#endif

#if defined(ENABLE_OPERATION_RAPID_MODE_TIMED_IF)
    /* Operation.RapidModeTimed */
    rapidModeTimedListener.OnGetRapidModeMinutesRemaining = NULL;
    rapidModeTimedListener.OnGetMaxSetMinutes = NULL;
    rapidModeTimedListener.OnSetRapidModeMinutesRemaining = RapidModeTimedOnSetRapidModeMinutesRemaining;
    status = Cdm_CreateInterface(RAPID_MODE_TIMED_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &rapidModeTimedListener);
#endif

#if defined(ENABLE_OPERATION_REMOTE_CONTROLLABILITY_IF)
    /* Operation.RemoteControllability */
    remoteControllabilityListener.OnGetIsControllable = NULL;
    status = Cdm_CreateInterface(REMOTE_CONTROLLABILITY_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &remoteControllabilityListener);
#endif

#if defined(ENABLE_OPERATION_REPEAT_MODE_IF)
    /* Operation.RepeatMode */
    repeatModeListener.OnGetRepeatMode = NULL;
    repeatModeListener.OnSetRepeatMode = RepeatModeOnSetRepeatMode;
    status = Cdm_CreateInterface(REPEAT_MODE_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &repeatModeListener);
#endif

#if defined(ENABLE_OPERATION_RESOURCE_SAVING_IF)
    /* Operation.ResourceSaving */
    resourceSavingListener.OnGetResourceSavingMode = NULL;
    resourceSavingListener.OnSetResourceSavingMode = ResourceSavingOnSetResourceSavingMode;
    status = Cdm_CreateInterface(RESOURCE_SAVING_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &resourceSavingListener);
#endif

#if defined(ENABLE_OPERATION_ROBOT_CLEANING_CYCLE_PHASE_IF)
    /* Operation.RobotCleaningCyclePhase */
    robotCleaningCyclePhaseListener.OnGetCyclePhase = NULL;
    robotCleaningCyclePhaseListener.OnGetSupportedCyclePhases = NULL;
    robotCleaningCyclePhaseListener.OnGetVendorPhasesDescription = RobotCleaningCyclePhaseOnGetVendorPhasesDescription;
    status = Cdm_CreateInterface(ROBOT_CLEANING_CYCLE_PHASE_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &robotCleaningCyclePhaseListener);
#endif

#if defined(ENABLE_OPERATION_SOIL_LEVEL_IF)
    /* Operation.SoilLevel */
    soilLevelListener.OnGetMaxLevel = NULL;
    soilLevelListener.OnGetTargetLevel = NULL;
    soilLevelListener.OnSetTargetLevel = SoilLevelListenerOnSetTargetLevel;
    soilLevelListener.OnGetSelectableLevels = NULL;
    status = Cdm_CreateInterface(SOIL_LEVEL_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &soilLevelListener);
#endif

#if defined(ENABLE_OPERATION_SPIN_SPEED_LEVEL_IF)
    /* Operation.SpinSpeedLevel */
    spinSpeedLevelListener.OnGetMaxLevel = NULL;
    spinSpeedLevelListener.OnGetTargetLevel = NULL;
    spinSpeedLevelListener.OnSetTargetLevel = SpinSpeedLevelListenerOnSetTargetLevel;
    spinSpeedLevelListener.OnGetSelectableLevels = NULL;
    status = Cdm_CreateInterface(SPIN_SPEED_LEVEL_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &spinSpeedLevelListener);
#endif

#if defined(ENABLE_OPERATION_TIMER_IF)
    /* Operation.Timer */
    timerListener.OnGetReferenceTimer = NULL;
    timerListener.OnGetTargetTimeToStart = NULL;
    timerListener.OnGetTargetTimeToStop = NULL;
    timerListener.OnGetEstimatedTimeToEnd = NULL;
    timerListener.OnGetRunningTime = NULL;
    timerListener.OnGetTargetDuration = NULL;
    timerListener.OnSetTargetTimeToStart = TimerListenerOnSetTargetTimeToStart;
    timerListener.OnSetTargetTimeToStop = TimerListenerOnSetTargetTimeToStop;
    status = Cdm_CreateInterface(TIMER_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &timerListener);
#endif

#if defined(ENABLE_ENVIRONMENT_CURRENT_HUMIDITY_IF)
    /* Environment.CurrentHumidity */
    currentHumidityListener.OnGetCurrentValue = NULL;
    currentHumidityListener.OnGetMaxValue = NULL;
    status = Cdm_CreateInterface(CURRENT_HUMIDITY_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &currentHumidityListener);
#endif

#if defined(ENABLE_ENVIRONMENT_TARGET_HUMIDITY_IF)
    /* Environment.TargetHumidity */
    targetHumidityListener.OnGetTargetValue = NULL;
    targetHumidityListener.OnSetTargetValue = TargetHumidityOnSetTargetValue;
    targetHumidityListener.OnGetMinValue = NULL;
    targetHumidityListener.OnGetMaxValue = NULL;
    targetHumidityListener.OnGetStepValue = NULL;
    targetHumidityListener.OnGetSelectableHumidityLevels = NULL;
    status = Cdm_CreateInterface(TARGET_HUMIDITY_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &targetHumidityListener);
#endif

#if defined(ENABLE_ENVIRONMENT_TARGET_TEMPERATURE_LEVEL_IF)
    /* Environment.TargetTemperatureLevel */
    targetTemperatureLevelListener.OnGetTargetLevel = NULL;
    targetTemperatureLevelListener.OnSetTargetLevel = TargetTemperatureLevelOnSetTargetLevel;
    targetTemperatureLevelListener.OnGetMaxLevel = NULL;
    targetTemperatureLevelListener.OnGetSelectableTemperatureLevels = NULL;
    status = Cdm_CreateInterface(TARGET_TEMPERATURE_LEVEL_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &targetTemperatureLevelListener);
#endif

#if defined(ENABLE_OPERATION_HVAC_FAN_MODE_IF)
    /* Operation.HvacFanMode */
    hvacFanModeListener.OnGetMode = NULL;
    hvacFanModeListener.OnSetMode = HvacFanModeOnSetMode;
    hvacFanModeListener.OnGetSupportedModes = NULL;
    status = Cdm_CreateInterface(HVAC_FAN_MODE_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &hvacFanModeListener);
#endif

#if defined(ENABLE_OPERATION_PLUG_IN_UNITS_IF)
    /* Operation.PlugInUnits*/
    plugInUnitsListener.OnGetPlugInUnits = NULL;
    status = Cdm_CreateInterface(PLUG_IN_UNITS_INTERFACE, CDM_OBJECT_PATH_CONTROLLEE, &plugInUnitsListener);
#endif


    status = Cdm_Start();

    while (TRUE) {
        status = AJ_OK;
        serviceStatus = AJSVC_SERVICE_STATUS_NOT_HANDLED;

        if (!isBusConnected) {
            status = AJSVC_RoutingNodeConnect(&busAttachment, ROUTING_NODE_NAME, AJAPP_CONNECT_TIMEOUT, AJAPP_CONNECT_PAUSE, AJAPP_BUS_LINK_TIMEOUT, &isBusConnected);
            if (!isBusConnected) { // Failed to connect to Routing Node?
                continue; // Retry establishing connection to Routing Node.
            }

            status = Cdm_EnableSecurity(&busAttachment, suites, numsuites, AuthListenerCallback);
            if (status != AJ_OK) {
                AJSVC_RoutingNodeDisconnect(&busAttachment, 1, AJAPP_SLEEP_TIME, AJAPP_SLEEP_TIME, &isBusConnected);
                break;
            }
        }

        status = AJApp_ConnectedHandler(&busAttachment);
        if (!cdmInitState) {
            status = InitCdmProperties(&busAttachment);
            if (status == AJ_OK) {
                cdmInitState = true;
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
                    serviceStatus = Cdm_MessageProcessor(&busAttachment, &msg, &status);
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

    Cdm_Deinit();

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

