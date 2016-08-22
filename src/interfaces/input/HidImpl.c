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

#include <ajtcl/cdm/interfaces/input/Hid.h>
#include "../../CdmControllee/CdmControlleeImpl.h"
#include "HidImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescInputHid[] = {
    "$org.alljoyn.Input.Hid",
    "@Version>q",
    "@SupportedEvents>a(qqii)",
    "?InjectEvents inputEvents<a(qqi)",
    NULL
};

typedef struct {
    uint16_t version;
    SupportedInputEvent* supportedEvents;
} HidProperties;

static size_t supportedEventsSize = 0;

AJ_Status CreateHidInterface(void** properties)
{
    *properties = malloc(sizeof(HidProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(HidProperties));

    ((HidProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;
    ((HidProperties*)*properties)->supportedEvents = NULL;

    return AJ_OK;
}

void DestroyHidInterface(void* properties)
{
    if (properties) {
        HidProperties* props = (HidProperties*)properties;
        if (props->supportedEvents) {
            free(props->supportedEvents);
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
        AJ_MarshalArgs(&msg, "s", intfDescInputHid[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "a(qqii)")) {
            SupportedInputEvent* supportedEvents = (SupportedInputEvent*)val;
            AJ_Arg array2, strc2;
            int i = 0;

            AJ_MarshalArgs(&msg, "s", propName);
            AJ_MarshalVariant(&msg, signature);
            AJ_MarshalContainer(&msg, &array2, AJ_ARG_ARRAY);
            for (i = 0; i < supportedEventsSize; i++) {
                AJ_MarshalContainer(&msg, &strc2, AJ_ARG_STRUCT);
                AJ_MarshalArgs(&msg, "qqii", supportedEvents[i].type, supportedEvents[i].code,
                               supportedEvents[i].min, supportedEvents[i].max);
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

AJ_Status HidInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    HidProperties* props = NULL;
    HidListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (HidListener*)listener;
    }
    props = (HidProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {

            SupportedInputEvent* supportedEvents = (SupportedInputEvent*)malloc(sizeof(SupportedInputEvent) * supportedEventsSize);
            if (lt && lt->OnGetSupportedEvents) {
                status = lt->OnGetSupportedEvents(objPath, supportedEvents);
                if (status == AJ_OK) {
                    if (props->supportedEvents) {
                        free(props->supportedEvents);
                    }
                    props->supportedEvents = (SupportedInputEvent*)malloc(sizeof(SupportedInputEvent) * supportedEventsSize);
                    memcpy(props->supportedEvents, supportedEvents, sizeof(SupportedInputEvent) * supportedEventsSize);
                    free(supportedEvents);
                }
            }
            if (!props->supportedEvents) {
                free(supportedEvents);
                return AJ_ERR_NULL;
            }
            int i = 0;
            AJ_Arg array, strc;
            AJ_MarshalContainer(replyMsg, &array, AJ_ARG_ARRAY);
            for (i = 0; i < supportedEventsSize; i++) {
                AJ_MarshalContainer(replyMsg, &strc, AJ_ARG_STRUCT);
                status = AJ_MarshalArgs(replyMsg, "qqii", (props->supportedEvents)[i].type,
                                                          (props->supportedEvents)[i].code,
                                                          (props->supportedEvents)[i].min,
                                                          (props->supportedEvents)[i].max);
                AJ_MarshalCloseContainer(replyMsg, &strc);
            }
            AJ_MarshalCloseContainer(replyMsg, &array);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status HidInterfaceOnMethodHandler(AJ_Message* msg, const char* objPath, uint8_t memberIndex, void* listener, CdmPropertiesChangedByMethod* propChangedByMethod)
{
    AJ_Status status = AJ_OK;

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    if (!propChangedByMethod) {
        return AJ_ERR_INVALID;
    }

    HidListener* lt = (HidListener*)listener;

    switch (memberIndex) {
    case 2 :
        if (!lt->OnInjectEvents) {
            status = AJ_ERR_NULL;
        } else {
            AJ_Arg array;
            InputEvent* inputEvent = (InputEvent*)malloc(sizeof(InputEvent) * 1);
            if (inputEvent) {
                status = AJ_UnmarshalContainer(msg, &array, AJ_ARG_ARRAY);
                size_t size = 0;
                while (status == AJ_OK) {
                    status = AJ_UnmarshalArgs(msg, "(qqi)", &inputEvent[size].type, &inputEvent[size].code, &inputEvent[size].value);
                    if (status == AJ_OK) {
                        size++;
                        InputEvent* tempInputEvent = (InputEvent*)malloc(sizeof(InputEvent) * size);
                        memcpy(tempInputEvent, inputEvent, sizeof(InputEvent) * size);
                        free(inputEvent);
                        inputEvent = (InputEvent*)malloc(sizeof(InputEvent) * (size + 1));
                        memcpy(inputEvent, tempInputEvent, sizeof(InputEvent) * size);
                        free(tempInputEvent);
                    }
                }
                AJ_UnmarshalCloseContainer(msg, &array);

                if (status == AJ_ERR_NO_MORE) {
                    status = AJ_OK;
                }

                if (status == AJ_OK) {
                    status = lt->OnInjectEvents(objPath, inputEvent, size);
                }
                free(inputEvent);
            }
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Cdm_HidInterfaceGetSupportedEvents(const char* objPath, SupportedInputEvent* supportedEvents)
{
    AJ_Status status = AJ_OK;
    HidProperties* props = NULL;

    if (!supportedEvents) {
        return AJ_ERR_INVALID;
    }

    props = (HidProperties*)GetProperties(objPath, HID_INTERFACE);
    if (props) {
        if (!props->supportedEvents) {
            return AJ_ERR_NULL;
        }

        memcpy(supportedEvents, props->supportedEvents, sizeof(SupportedInputEvent) * supportedEventsSize);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_HidInterfaceSetSupportedEvents(AJ_BusAttachment* busAttachment, const char* objPath, const SupportedInputEvent* supportedEvents, const size_t supportedEventListSize)
{
    AJ_Status status = AJ_OK;
    HidProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!supportedEvents) {
        return AJ_ERR_INVALID;
    }

    props = (HidProperties*)GetProperties(objPath, HID_INTERFACE);

    if (props) {
        if (props->supportedEvents) {
            free(props->supportedEvents);
        }
        supportedEventsSize = supportedEventListSize;
        props->supportedEvents = (SupportedInputEvent*)malloc(sizeof(SupportedInputEvent) * supportedEventsSize);

        memcpy(props->supportedEvents, supportedEvents, sizeof(SupportedInputEvent) * supportedEventsSize);
        status = EmitPropChanged(busAttachment, objPath, "SupportedEvents", "a(qqii)", props->supportedEvents);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
