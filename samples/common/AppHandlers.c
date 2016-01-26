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
 * The corresponding flag dbgAJSVCAPP is defined below statically.
 */
#define AJ_MODULE AJSVCAPP
#include <ajtcl/aj_debug.h>

#include <ajtcl/alljoyn.h>
#include "../inc/AppHandlers.h"

#include <ajtcl/services/ServicesCommon.h>
#include <ajtcl/services/ServicesHandlers.h>
#include <ajtcl/services/PropertyStore.h>
#ifdef CONFIG_SERVICE
    #include "ConfigSample.h"
#endif
#ifdef ONBOARDING_SERVICE
    #include "OnboardingSample.h"
    #include <ajtcl/services/OnboardingManager.h>
#endif
#ifdef NOTIFICATION_SERVICE_PRODUCER
    #include "NotificationProducerSample.h"
#endif
#ifdef CONTROLPANEL_SERVICE
    #include "ControlPanelSample.h"
#endif
#ifdef NOTIFICATION_SERVICE_CONSUMER
    #include "NotificationConsumerSample.h"
#endif

#include <ajtcl/aj_config.h>
#include <ajtcl/aj_link_timeout.h>

#ifndef NDEBUG
extern AJ_EXPORT uint8_t dbgAJSVCAPP;
#endif

uint16_t servicePort = 0;

typedef enum {
    INIT_START = 0,
    INIT_SERVICES = INIT_START,
    INIT_SERVICES_PORT,
    INIT_ABOUT,
    INIT_CHECK_ANNOUNCE,
    INIT_FINISHED = INIT_CHECK_ANNOUNCE
} enum_init_state_t;

static uint8_t initAttempts = 0;

static enum_init_state_t currentServicesInitializationState = INIT_START;
static enum_init_state_t nextServicesInitializationState = INIT_START;

AJ_Status AJServices_Init(uint16_t aboutServicePort, const char* deviceManufactureName, const char* deviceProductName)
{
    AJ_Status status = AJ_OK;

    servicePort = aboutServicePort;
#ifdef CONFIG_SERVICE
    status = Config_Init();
    if (status != AJ_OK) {
        goto Exit;
    }
#endif
#ifdef ONBOARDING_SERVICE
    status = Onboarding_Init(deviceManufactureName, deviceProductName);
    if (status != AJ_OK) {
        goto Exit;
    }
#endif
#ifdef NOTIFICATION_SERVICE_CONSUMER
    status = NotificationConsumer_Init();
    if (status != AJ_OK) {
        goto Exit;
    }
#endif
#ifdef NOTIFICATION_SERVICE_PRODUCER
    status = NotificationProducer_Init();
    if (status != AJ_OK) {
        goto Exit;
    }
#endif
#ifdef CONTROLPANEL_SERVICE
    status = Controlee_Init();
    if (status != AJ_OK) {
        goto Exit;
    }
#endif

Exit:

    return status;
}

AJ_Status AJApp_ConnectedHandler(AJ_BusAttachment* busAttachment, uint8_t maxNumberOfAttempts, uint32_t sleepTimeBetweenAttempts)
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
                status = AJ_BusBindSessionPort(busAttachment, servicePort, NULL, 0);
                if (status != AJ_OK) {
                    goto ErrorExit;
                }
                nextServicesInitializationState = INIT_ABOUT;
                break;

            case INIT_ABOUT:
                status = AJ_AboutInit(busAttachment, servicePort);
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
    if (status == AJ_ERR_RESOURCES) {
        initAttempts++;
        if (initAttempts > maxNumberOfAttempts) {
            status = AJ_ERR_READ; // Force disconnect from Routing Node
        } else {
            AJ_ErrPrintf(("Application ConnectedHandler attempt %u of %u\n", initAttempts, maxNumberOfAttempts));
            AJ_Sleep(sleepTimeBetweenAttempts);
        }
    } else if (status == AJ_ERR_WRITE) {
        status = AJ_ERR_READ; // Force disconnect from Routing Node
    }

    return status;
}

AJSVC_ServiceStatus AJApp_MessageProcessor(AJ_BusAttachment* busAttachment, AJ_Message* msg, AJ_Status* status)
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
        session_accepted |= (port == servicePort);
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

void AJApp_DoWork(AJ_BusAttachment* busAttachment)
{
#ifdef CONFIG_SERVICE
    Config_DoWork(busAttachment);
#endif
#ifdef ONBOARDING_SERVICE
    Onboarding_DoWork(busAttachment);
#endif
#ifdef NOTIFICATION_SERVICE_PRODUCER
    NotificationProducer_DoWork(busAttachment);
#endif
#ifdef NOTIFICATION_SERVICE_CONSUMER
    NotificationConsumer_DoWork(busAttachment);
#endif
#ifdef CONTROLPANEL_SERVICE
    Controlee_DoWork(busAttachment);
#endif
}

AJ_Status AJApp_DisconnectHandler(AJ_BusAttachment* busAttachment, uint8_t restart)
{
    AJ_Status status = AJ_OK;

    if (restart) {
        AJ_BusUnbindSession(busAttachment, servicePort);
    }

    AJ_AboutSetShouldAnnounce();
    currentServicesInitializationState = nextServicesInitializationState = INIT_START;
    initAttempts = 0;

    status = AJSVC_DisconnectHandler(busAttachment);
    return status;
}
