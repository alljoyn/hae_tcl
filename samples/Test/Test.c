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
#include <ajtcl/cdm/interfaces/operation/AudioVolume.h>
#include <ajtcl/cdm/interfaces/operation/Channel.h>
#include "../VendorDefinedInterfaces/VendorDefinedInterface.h"
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

#define CDM_OBJECT_PATH_TV "/Cdm/Tv"

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
    {TELEVISION, CDM_OBJECT_PATH_TV},
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

ChannelInfoRecord channels[5] = {
    {"1-1", "1", "aaa"},
    {"1-2", "2", "bbb"},
    {"1-3", "3", "ccc"},
    {"1-4", "4", "ddd"},
    {"1-5", "5", "eee"}
};

AJ_Status OnSetVolume(const char* objPath, uint8_t volume)
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

AJ_Status OnSetMute(const char* objPath, bool mute)
{
    printf("OnSetMute : %s %d\n", objPath, mute);

    return AJ_OK;
}

AJ_Status OnSetChannelId(const char* objPath, const char* channelId)
{
    printf("OnSetChannelId : %s %s\n", objPath, channelId);

    return AJ_OK;
}

AJ_Status OnGetChannelList(const char* objPath, uint16_t startingRecord, uint16_t numRecords,
                           ChannelInfoRecord** listOfChannelInfoRecords, uint16_t* numReturnedRecords, ErrorCode* errorCode)
{
    int size = sizeof(channels) / sizeof(ChannelInfoRecord);

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

AJ_Status OnSetTestProperty(const char* objPath, int32_t* testProperty)
{
    printf("OnSetTestProperty : %s %d\n", objPath, *testProperty);

    return AJ_OK;
}

AJ_Status OnTestMethod(const char* objPath, int32_t arg1)
{
    printf("OnTestMethod : %s arg1 : %d\n", objPath, arg1);

    return AJ_OK;
}

AJ_Status InitCdmProperties(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    uint8_t vol = 8;
    uint8_t volRead = 0;
    char channelId[81] = "1-1";
    char channelIdBuf[128];
    uint16_t numOfChannels = 70;
    int32_t testProperty = 3;
    int32_t testPropertyRead = 0;

    status = Cdm_AudioVolumeInterfaceSetVolume(busAttachment, CDM_OBJECT_PATH_TV, vol);
    status = Cdm_AudioVolumeInterfaceGetVolume(CDM_OBJECT_PATH_TV, &volRead);
    printf("volume : %u\n", volRead);

    status = Cdm_ChannelInterfaceSetChannelId(busAttachment, CDM_OBJECT_PATH_TV, channelId);
    status = Cdm_ChannelInterfaceGetChannelId(CDM_OBJECT_PATH_TV, channelIdBuf);
    printf("channelId : %s\n", channelIdBuf);
    status = Cdm_ChannelInterfaceSetTotalNumberOfChannels(busAttachment, CDM_OBJECT_PATH_TV, numOfChannels);

    status = Cdm_VendorDefinedInterfaceSetTestProperty(busAttachment, CDM_OBJECT_PATH_TV, testProperty);
    status = Cdm_VendorDefinedInterfaceGetTestProperty(CDM_OBJECT_PATH_TV, &testPropertyRead);
    printf("testProperty : %d\n", testPropertyRead);

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
    AudioVolumeListener audioVolumeListener;
    ChannelListener channelListener;
    VendorDefinedInterfaceHandler vendorDefinedIntfHandler;
    CdmInterfaceTypes vendorDefinedIntfType;
    VendorDefinedListener vendorDefinedListener;

    AJ_Initialize();

    AJ_AboutSetIcon(aboutIconContent, aboutIconContentSize, aboutIconMimetype, aboutIconUrl);

    status = PropertyStore_Init();
    if (status != AJ_OK) {
        goto Exit;
    }

    status = Cdm_Init();

    audioVolumeListener.OnGetVolume = NULL;
    audioVolumeListener.OnSetVolume = OnSetVolume;
    audioVolumeListener.OnGetMaxVolume = OnGetMaxVolume;
    audioVolumeListener.OnGetMute = NULL;
    audioVolumeListener.OnSetMute = OnSetMute;
    channelListener.OnGetChannelId = NULL;
    channelListener.OnSetChannelId = OnSetChannelId;
    channelListener.OnGetTotalNumberOfChannels = NULL;
    channelListener.OnGetChannelList = OnGetChannelList;

    status = Cdm_CreateInterface(AUDIO_VOLUME_INTERFACE, CDM_OBJECT_PATH_TV, &audioVolumeListener);
    status = Cdm_CreateInterface(CHANNEL_INTERFACE, CDM_OBJECT_PATH_TV, &channelListener);

    vendorDefinedIntfHandler.InterfaceRegistered = VendorDefinedInterfaceRegistered;
    vendorDefinedIntfHandler.InterfaceCreator = CreateVendorDefinedInterface;
    vendorDefinedIntfHandler.InterfaceDestructor = DestroyVendorDefinedInterface;
    vendorDefinedIntfHandler.OnGetProperty = VendorDefinedInterfaceOnGetProperty;
    vendorDefinedIntfHandler.OnSetProperty = VendorDefinedInterfaceOnSetProperty;
    vendorDefinedIntfHandler.EmitPropertiesChanged = VendorDefinedInterfaceEmitPropertiesChanged;
    vendorDefinedIntfHandler.OnMethodHandler = VendorDefinedInterfaceOnMethodHandler;
    status = Cdm_RegisterVendorDefinedInterface(VENDOR_DEFINED_INTERFACE_NAME, intfDescVendorDefined, &vendorDefinedIntfHandler, &vendorDefinedIntfType);
    if (status == AJ_OK) {
        vendorDefinedListener.OnGetTestProperty = NULL;
        vendorDefinedListener.OnSetTestProperty = OnSetTestProperty;
        vendorDefinedListener.OnTestMethod = OnTestMethod;
        status = Cdm_CreateInterface(vendorDefinedIntfType, CDM_OBJECT_PATH_TV, &vendorDefinedListener);
    }

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

