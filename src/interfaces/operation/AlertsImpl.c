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

#include <string.h>
#include <ajtcl/hae/interfaces/operation/Alerts.h>
#include <ajtcl/hae/interfaces/HaeInterfaceErrors.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "AlertsImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationAlerts[] = {
    "$org.alljoyn.SmartSpaces.Operation.Alerts",
    "@Version>q",
    "@Alerts>a(yqb)",
    "?GetAlertCodesDescription languageTag<s description>a(qs)",
    "?AcknowledgeAlert alertCode<q",
    "?AcknowledgeAllAlerts",
    NULL
};

typedef struct {
    uint16_t version;
    AlertRecord* alerts;
} AlertsProperties;

static size_t alertsSize = 0;

AJ_Status CreateAlertsInterface(void** properties)
{
    *properties = malloc(sizeof(AlertsProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(AlertsProperties));

    ((AlertsProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;
    ((AlertsProperties*)*properties)->alerts = NULL;

    return AJ_OK;
}

void DestroyAlertsInterface(void* properties)
{
    if (properties) {
        AlertsProperties* props = (AlertsProperties*)properties;

        if (props->alerts) {
            free(props->alerts);
        } 
        free(props);
    }
}

static AJ_Status EmitPropChanged(AJ_BusAttachment* busAttachment, const char* objPath, const char* propName, const char* signature, void* val)
{
    AJ_Status status = AJ_OK;
    uint32_t msgId = 0;
    AJ_Message msg;
    AJ_Arg array, strc;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status = MakePropChangedId(objPath, &msgId);
    if (status == AJ_OK) {
        AJ_MarshalSignal(busAttachment, &msg, msgId, NULL, 0, AJ_FLAG_GLOBAL_BROADCAST, 0);
        AJ_MarshalArgs(&msg, "s", intfDescOperationAlerts[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "q")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint16_t*)val);
        } else if (!strcmp(signature, "a(yqb)")) {
            AlertRecord* alerts = (AlertRecord*)val;
            AJ_Arg array2, strc2;
            int i = 0;

            AJ_MarshalArgs(&msg, "s", propName);
            AJ_MarshalVariant(&msg, signature);
            AJ_MarshalContainer(&msg, &array2, AJ_ARG_ARRAY);
            for (i = 0; i < alertsSize; i++) {
                AJ_MarshalContainer(&msg, &strc2, AJ_ARG_STRUCT);
                AJ_MarshalArgs(&msg, "yqb",
                    alerts[i].severity,
                    alerts[i].alertCode,
                    alerts[i].needAcknowledgement);
                AJ_MarshalCloseContainer(&msg, &strc2);
            }
            AJ_MarshalCloseContainer(&msg, &array2);
        } else {
            return AJ_ERR_SIGNATURE;
        }
        AJ_MarshalCloseContainer(&msg, &strc);
        AJ_MarshalCloseContainer(&msg, &array);
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalCloseContainer(&msg, &array);
        status = AJ_DeliverMsg(&msg);
    }

    return status;
}

AJ_Status AlertsInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    AlertsProperties* props = NULL;
    AlertsListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (AlertsListener*)listener;
    }
    props = (AlertsProperties*)properties;

    switch (memberIndex) {
        case 0:
            status = AJ_MarshalArgs(replyMsg, "q", props->version);
            break;

        case 1:
            {
                AlertRecord* alerts = (AlertRecord*)malloc(sizeof(AlertRecord) * alertsSize);
                memset(alerts, 0, sizeof(AlertRecord) * alertsSize);

                if (lt && lt->OnGetAlerts) {
                    status = lt->OnGetAlerts(objPath, alerts);
                    if (status == AJ_OK) {
                        if (props->alerts) {
                            free(props->alerts);
                        }
                        props->alerts = (AlertRecord*)malloc(sizeof(AlertRecord) * alertsSize);
                        memcpy(props->alerts, alerts, sizeof(AlertRecord) * alertsSize);
                        free(alerts);
                    }
                }
                if (!props->alerts) {
                    free(alerts);
                    return AJ_ERR_NULL;
                }

                int i = 0;
                AJ_Arg array, strc;
                AJ_MarshalContainer(replyMsg, &array, AJ_ARG_ARRAY);
                for (i = 0; i < alertsSize; i++) {
                    AJ_MarshalContainer(replyMsg, &strc, AJ_ARG_STRUCT);
                    status = AJ_MarshalArgs(replyMsg, "yqb",
                        (props->alerts)[i].severity,
                        (props->alerts)[i].alertCode,
                        (props->alerts)[i].needAcknowledgement);
                    AJ_MarshalCloseContainer(replyMsg, &strc);
                }
                AJ_MarshalCloseContainer(replyMsg, &array);
                free(alerts);
            }
            break;

        default:
            status = AJ_ERR_INVALID;
            break;
    }

 
    return status;
}

AJ_Status AlertsInterfaceOnMethodHandler(AJ_Message* msg, const char* objPath, uint8_t memberIndex, void* listener, HaePropertiesChangedByMethod* propChangedByMethod)
{
    AJ_Status status = AJ_OK;

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    if (!propChangedByMethod) {
        return AJ_ERR_INVALID;
    }

    AlertsListener* lt = (AlertsListener*)listener;
    switch (memberIndex) {
        case 2:
            if (!lt->OnGetAlertCodesDescription) {
                status = AJ_ERR_NULL;
            } else {
               char* languageTag;
                AlertCodesDescriptor* description = NULL;
                size_t alertNumber = 0;

                status = AJ_UnmarshalArgs(msg, "s", &languageTag);
                if (status == AJ_OK) {
                    ErrorCode errorCode = NOT_ERROR;
                    AJ_Message reply;
                    status = lt->OnGetAlertCodesDescription(objPath, languageTag, &description, &alertNumber, &errorCode);

                    if (status == AJ_OK) {
                        AJ_Arg array, strc;
                        int i = 0;

                        AJ_MarshalReplyMsg(msg, &reply);
                        AJ_MarshalContainer(&reply, &array, AJ_ARG_ARRAY);
                        for (i=0; i<alertNumber; i++) {
                            AJ_MarshalContainer(&reply, &strc, AJ_ARG_STRUCT);
                            AJ_MarshalArgs(&reply, "qs",
                                ((description+i)->alertCode),
                                ((description+i)->description));
                            AJ_MarshalCloseContainer(&reply, &strc);
                        }
                        AJ_MarshalCloseContainer(&reply, &array);
                    } else {
                        AJ_MarshalReplyMsg(msg, &reply);
                        AJ_MarshalErrorMsgWithInfo(msg, &reply, GetInterfaceErrorName(errorCode), GetInterfaceErrorMessage(errorCode));
                    }
                    status = AJ_DeliverMsg(&reply);
                }
            }
            break;

        case 3:
            if (!lt->OnAcknowledgeAlert) {
                status = AJ_ERR_NULL;
            } else {
                uint16_t alertCode;

                status = AJ_UnmarshalArgs(msg, "q", &alertCode);
                if (status == AJ_OK) {
                    ErrorCode errorCode = NOT_ERROR;
                    AJ_Message reply;
                    status = lt->OnAcknowledgeAlert(objPath, alertCode, &errorCode);

                    if (status == AJ_OK) {
                        AJ_MarshalReplyMsg(msg, &reply);
                    } else {
                        AJ_MarshalReplyMsg(msg, &reply);
                        AJ_MarshalErrorMsgWithInfo(msg, &reply, GetInterfaceErrorName(errorCode), GetInterfaceErrorMessage(errorCode));
                    }
                    status = AJ_DeliverMsg(&reply);
                }
            }
            break;

        case 4:
            if (!lt->OnAcknowledgeAllAlerts) {
                status = AJ_ERR_NULL;
            } else {
                ErrorCode errorCode = NOT_ERROR;
                AJ_Message reply;
                status = lt->OnAcknowledgeAllAlerts(objPath, &errorCode);

                if (status == AJ_OK) {
                    AJ_MarshalReplyMsg(msg, &reply);
                } else {
                    AJ_MarshalReplyMsg(msg, &reply);
                    AJ_MarshalErrorMsgWithInfo(msg, &reply, GetInterfaceErrorName(errorCode), GetInterfaceErrorMessage(errorCode));
                }
                status = AJ_DeliverMsg(&reply);
            }
            break;

        default:
            status = AJ_ERR_INVALID;
            break;
    }
    return status;
}

AJ_Status Hae_AlertsInterfaceGetAlerts(const char* objPath, AlertRecord* alerts)
{
    AJ_Status status = AJ_OK;

    AlertsProperties* props = NULL;

    if (!alerts) {
        return AJ_ERR_INVALID;
    }

    props = (AlertsProperties*)GetProperties(objPath, ALERTS_INTERFACE);

    if (props) {
        if (!props->alerts) {
            return AJ_ERR_NULL;
        }
        memcpy(alerts, props->alerts, sizeof(AlertRecord) * alertsSize);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_AlertsInterfaceSetAlerts(AJ_BusAttachment* busAttachment, const char* objPath, const AlertRecord* alerts, const size_t alertsNumber)
{
    AJ_Status status = AJ_OK;

    AlertsProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!alerts) {
        return AJ_ERR_INVALID;
    }

    props = (AlertsProperties*)GetProperties(objPath, ALERTS_INTERFACE);

    if (props) {
        if (props->alerts) {
            free(props->alerts);
        }

        alertsSize = alertsNumber;
        props->alerts = (AlertRecord*)malloc(sizeof(AlertRecord) * alertsSize);

        memcpy(props->alerts, alerts, sizeof(AlertRecord) * alertsSize);

        status = EmitPropChanged(busAttachment, objPath, "Alerts", "a(yqb)", props->alerts);
    } else {
        status = AJ_ERR_NO_MATCH;
    }


    return status;
}
