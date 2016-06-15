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
#include <ajtcl/hae/interfaces/operation/HvacFanMode.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "HvacFanModeImpl.h"

#define INTERFACE_VERSION 1
#define MAXSUPPORTEDMODESIZE 7

const char* const intfDescOperationHvacFanMode[] = {
    "$org.alljoyn.SmartSpaces.Operation.HvacFanMode",
    "@Version>q",
    "@Mode=q",
    "@SupportedModes>aq",
    NULL
};

typedef struct {
    uint16_t version;
    uint16_t mode;
    uint16_t* supportedModes;
    uint16_t operationalState;
} HvacFanModeProperties;

static size_t listSize = 0;

AJ_Status HFModeValidationCheck(const char* objPath, uint16_t mode)
{
    AJ_Status status = AJ_OK;
    HvacFanModeProperties* props = NULL;
    props = (HvacFanModeProperties*)GetProperties(objPath, HVAC_FAN_MODE_INTERFACE);

    if (props) {
        int i = 0;
        bool check = false;
        for (i = 0; i < listSize; i++) {
            if (props->supportedModes[i] == mode) {
                check = true;
                break;
            }
        }

        if (!check) {
            return AJ_ERR_RANGE;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status CreateHvacFanModeInterface(void** properties)
{
    *properties = malloc(sizeof(HvacFanModeProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(HvacFanModeProperties));

    ((HvacFanModeProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;
    ((HvacFanModeProperties*)*properties)->supportedModes = NULL;

    return AJ_OK;
}

void DestroyHvacFanModeInterface(void* properties)
{
    if (properties) {
        HvacFanModeProperties* props = (HvacFanModeProperties*)properties;
        if (props->supportedModes) {
            free(props->supportedModes);
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
        AJ_MarshalArgs(&msg, "s", intfDescOperationHvacFanMode[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "q")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint16_t*)val);
        } else if (!strcmp(signature, "aq")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, (uint16_t*)val, listSize);
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

AJ_Status HvacFanModeInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    HvacFanModeProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (HvacFanModeProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "Mode", "q", &(props->mode));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status HvacFanModeInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    HvacFanModeProperties* props = NULL;
    HvacFanModeListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (HvacFanModeListener*)listener;
    }
    props = (HvacFanModeProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            uint16_t mode;
            if (lt && lt->OnGetMode) {
                status = lt->OnGetMode(objPath, &mode);
                if (status == AJ_OK) {
                    props->mode = mode;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "q", props->mode);
        }
        break;
    case 2 :
        {
            uint16_t* supportedModes = (uint16_t*)malloc(sizeof(uint16_t) * listSize);
            if (lt && lt->OnGetSupportedModes) {
                status = lt->OnGetSupportedModes(objPath, supportedModes);
                if (status == AJ_OK) {
                    if (props->supportedModes) {
                        free(props->supportedModes);
                    }
                    props->supportedModes = (uint16_t*)malloc(sizeof(uint16_t) * listSize);

                    memcpy(props->supportedModes, supportedModes, sizeof(uint16_t) * listSize);
                }
            }
            if (!props->supportedModes) {
                free(supportedModes);
                return AJ_ERR_NULL;
            }

            status = AJ_MarshalArgs(replyMsg, "aq", props->supportedModes, sizeof(uint16_t) * listSize);
            free(supportedModes);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status HvacFanModeInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    HvacFanModeProperties* props = (HvacFanModeProperties*)properties;
    HvacFanModeListener* lt = (HvacFanModeListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        if (!lt->OnSetMode) {
            status = AJ_ERR_NULL;
        } else {
            uint16_t mode = 0;
            status = AJ_UnmarshalArgs(replyMsg, "q", &mode);

            if (status == AJ_OK) {
                status = HFModeValidationCheck(objPath, mode);

                if (status != AJ_OK) {
                    return status;
                }

                status = lt->OnSetMode(objPath, mode);

                if (status == AJ_OK) {
                    if (props->mode != mode) {
                        props->mode = mode;
                        *propChanged = true;
                    }
                }
            }
        }
        break;
    case 2 :
        status = AJ_ERR_DISALLOWED;
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_HvacFanModeInterfaceGetMode(const char* objPath, uint16_t* mode)
{
    AJ_Status status = AJ_OK;
    HvacFanModeProperties* props = NULL;

    if (!mode) {
        return AJ_ERR_INVALID;
    }

    props = (HvacFanModeProperties*)GetProperties(objPath, HVAC_FAN_MODE_INTERFACE);
    if (props) {
        *mode = props->mode;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_HvacFanModeInterfaceSetMode(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t mode)
{
    AJ_Status status = AJ_OK;
    HvacFanModeProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status = HFModeValidationCheck(objPath, mode);

    if (status != AJ_OK) {
        return status;
    }

    props = (HvacFanModeProperties*)GetProperties(objPath, HVAC_FAN_MODE_INTERFACE);
    if (props) {
        if (props->mode != mode) {
            props->mode = mode;

            status = EmitPropChanged(busAttachment, objPath, "Mode", "q", &(props->mode));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_HvacFanModeInterfaceGetSupportedModes(const char* objPath, uint16_t* supportedModes)
{
    AJ_Status status = AJ_OK;
    HvacFanModeProperties* props = NULL;

    if (!supportedModes) {
        return AJ_ERR_INVALID;
    }

    props = (HvacFanModeProperties*)GetProperties(objPath, HVAC_FAN_MODE_INTERFACE);
    if (props) {
        if (!props->supportedModes) {
            return AJ_ERR_NULL;
        }
        memcpy(supportedModes, props->supportedModes, sizeof(uint16_t) * listSize);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_HvacFanModeInterfaceSetSupportedModes(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t* supportedModes, const size_t supportedModeSize)
{
    AJ_Status status = AJ_OK;
    HvacFanModeProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!supportedModes) {
        return AJ_ERR_INVALID;
    }

    if (supportedModeSize > MAXSUPPORTEDMODESIZE) {
        return AJ_ERR_INVALID;
    }

    props = (HvacFanModeProperties*)GetProperties(objPath, HVAC_FAN_MODE_INTERFACE);
    if (props) {
        if (props->supportedModes) {
            free(props->supportedModes);
        }
        listSize = supportedModeSize;

        props->supportedModes = (uint16_t*)malloc(sizeof(uint16_t) * listSize);

        memcpy(props->supportedModes, supportedModes, sizeof(uint16_t) * listSize);

        status = EmitPropChanged(busAttachment, objPath, "SupportedModes", "aq", props->supportedModes);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
