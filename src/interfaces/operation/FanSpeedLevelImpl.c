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
#include <ajtcl/cdm/interfaces/operation/FanSpeedLevel.h>
#include "../../CdmControllee/CdmControlleeImpl.h"
#include "FanSpeedLevelImpl.h"

#define INTERFACE_VERSION 1
#define FAN_TURNED_OFF 0x00
#define AUTO_MODE_ON 0x00
#define AUTO_MODE_OFF 0x01
#define AUTO_MODE_NOT_SUPPORTED 0xFF

const char* const intfDescOperationFanSpeedLevel[] = {
    "$org.alljoyn.SmartSpaces.Operation.FanSpeedLevel",
    "@Version>q",
    "@FanSpeedLevel=y",
    "@MaxFanSpeedLevel>y",
    "@AutoMode=y",
    NULL
};

typedef struct {
    uint16_t version;
    uint8_t fanSpeedLevel;
    uint8_t maxFanSpeedLevel;
    uint8_t autoMode;
} FanSpeedLevelProperties;

AJ_Status FanSpeedLevelValidationCheck(const char* objPath, uint8_t fanSpeedLevel)
{
    AJ_Status status = AJ_OK;
    FanSpeedLevelProperties* props = NULL;

    props = (FanSpeedLevelProperties*)GetProperties(objPath, FAN_SPEED_LEVEL_INTERFACE);
    if (props) {
        if (fanSpeedLevel == FAN_TURNED_OFF) {
            status = AJ_ERR_RANGE;
        }
        else if (fanSpeedLevel > props->maxFanSpeedLevel) {
            status = AJ_ERR_RANGE;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status AutoModeValidationCheck(uint8_t autoMode)
{
    AJ_Status status = AJ_OK;
    if (autoMode != AUTO_MODE_OFF && autoMode != AUTO_MODE_ON) {
        status = AJ_ERR_INVALID;
    }
    return status;
}

AJ_Status CreateFanSpeedLevelInterface(void** properties)
{
    *properties = malloc(sizeof(FanSpeedLevelProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(FanSpeedLevelProperties));

    ((FanSpeedLevelProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyFanSpeedLevelInterface(void* properties)
{
    if (properties) {
        FanSpeedLevelProperties* props = (FanSpeedLevelProperties*)properties;
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
        AJ_MarshalArgs(&msg, "s", intfDescOperationFanSpeedLevel[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "y")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint8_t*)val);
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

AJ_Status FanSpeedLevelInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    FanSpeedLevelProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (FanSpeedLevelProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "FanSpeedLevel", "y", &(props->fanSpeedLevel));
        break;
    case 3 :
        status = EmitPropChanged(busAttachment, objPath, "AutoMode", "y", &(props->autoMode));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status FanSpeedLevelInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    FanSpeedLevelProperties* props = NULL;
    FanSpeedLevelListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (FanSpeedLevelListener*)listener;
    }
    props = (FanSpeedLevelProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            uint8_t fanSpeedLevel;
            if (lt && lt->OnGetFanSpeedLevel) {
                status = lt->OnGetFanSpeedLevel(objPath, &fanSpeedLevel);
                if (status == AJ_OK) {
                    props->fanSpeedLevel = fanSpeedLevel;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->fanSpeedLevel);
        }
        break;
    case 2 :
        {
            uint8_t maxFanSpeedLevel;

            if (lt && lt->OnGetMaxFanSpeedLevel) {
                status = lt->OnGetMaxFanSpeedLevel(objPath, &maxFanSpeedLevel);
                 if (status == AJ_OK) {
                      props->maxFanSpeedLevel = maxFanSpeedLevel;
                 }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->maxFanSpeedLevel);
        }
        break;
    case 3 :
        {
            uint8_t autoMode;

            if (lt && lt->OnGetAutoMode) {
                status = lt->OnGetAutoMode(objPath, &autoMode);
                if (status == AJ_OK) {
                    props->autoMode = autoMode;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->autoMode);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status FanSpeedLevelInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    FanSpeedLevelProperties* props = (FanSpeedLevelProperties*)properties;
    FanSpeedLevelListener* lt = (FanSpeedLevelListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        if (!lt->OnSetFanSpeedLevel) {
            status = AJ_ERR_NULL;
        } else {
            uint8_t fanSpeedLevel = 0;
            status = AJ_UnmarshalArgs(replyMsg, "y", &fanSpeedLevel);

            if (status == AJ_OK) {
                status = FanSpeedLevelValidationCheck(objPath, fanSpeedLevel);
                if (status != AJ_OK) {
                    return status;
                }

                status = lt->OnSetFanSpeedLevel(objPath, fanSpeedLevel);

                if (status == AJ_OK) {
                    if (props->fanSpeedLevel != fanSpeedLevel) {
                        props->fanSpeedLevel = fanSpeedLevel;
                         *propChanged = true;
                    }
                }
            }
        }
        break;
    case 2 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 3 :
        if (!lt->OnSetAutoMode) {
            status = AJ_ERR_NULL;
        } else {
            uint8_t autoMode = 0;
            status = AJ_UnmarshalArgs(replyMsg, "y", &autoMode);

            if (status == AJ_OK) {
                status = AutoModeValidationCheck(autoMode);
                if (status != AJ_OK) {
                    return status;
                }
                status = lt->OnSetAutoMode(objPath, autoMode);

                if (status == AJ_OK) {
                    if (props->autoMode != autoMode) {
                        props->autoMode = autoMode;
                        *propChanged = true;
                    }
                }
            }
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Cdm_FanSpeedLevelInterfaceGetFanSpeedLevel(const char* objPath, uint8_t* fanSpeedLevel)
{
    AJ_Status status = AJ_OK;
    FanSpeedLevelProperties* props = NULL;

    if (!fanSpeedLevel) {
        return AJ_ERR_INVALID;
    }

    props = (FanSpeedLevelProperties*)GetProperties(objPath, FAN_SPEED_LEVEL_INTERFACE);
    if (props) {
        *fanSpeedLevel = props->fanSpeedLevel;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_FanSpeedLevelInterfaceSetFanSpeedLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t fanSpeedLevel)
{
    AJ_Status status = AJ_OK;
    FanSpeedLevelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status = FanSpeedLevelValidationCheck(objPath, fanSpeedLevel);
    if (status != AJ_OK) {
        return status;
    }

    props = (FanSpeedLevelProperties*)GetProperties(objPath, FAN_SPEED_LEVEL_INTERFACE);
    if (props) {
        if (props->fanSpeedLevel != fanSpeedLevel) {
            props->fanSpeedLevel = fanSpeedLevel;

            status = EmitPropChanged(busAttachment, objPath, "FanSpeedLevel", "y", &(props->fanSpeedLevel));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_FanSpeedLevelInterfaceGetMaxFanSpeedLevel(const char* objPath, uint8_t* maxFanSpeedLevel)
{
    AJ_Status status = AJ_OK;
    FanSpeedLevelProperties* props = NULL;

    if (!maxFanSpeedLevel) {
        return AJ_ERR_INVALID;
    }

    props = (FanSpeedLevelProperties*)GetProperties(objPath, FAN_SPEED_LEVEL_INTERFACE);
    if (props) {
        *maxFanSpeedLevel = props->maxFanSpeedLevel;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_FanSpeedLevelInterfaceSetMaxFanSpeedLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t maxFanSpeedLevel)
{
    AJ_Status status = AJ_OK;
    FanSpeedLevelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (FanSpeedLevelProperties*)GetProperties(objPath, FAN_SPEED_LEVEL_INTERFACE);
    if (props) {
        if (props->maxFanSpeedLevel != maxFanSpeedLevel) {
            props->maxFanSpeedLevel = maxFanSpeedLevel;

        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_FanSpeedLevelInterfaceGetAutoMode(const char* objPath, uint8_t* autoMode)
{
    AJ_Status status = AJ_OK;
    FanSpeedLevelProperties* props = NULL;

    if (!autoMode) {
        return AJ_ERR_INVALID;
    }

    props = (FanSpeedLevelProperties*)GetProperties(objPath, FAN_SPEED_LEVEL_INTERFACE);
    if (props) {
        *autoMode = props->autoMode;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_FanSpeedLevelInterfaceSetAutoMode(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t autoMode)
{
    AJ_Status status = AJ_OK;
    FanSpeedLevelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (autoMode != AUTO_MODE_OFF &&
        autoMode != AUTO_MODE_ON &&
        autoMode != AUTO_MODE_NOT_SUPPORTED) {
        return AJ_ERR_INVALID;
    }
    props = (FanSpeedLevelProperties*)GetProperties(objPath, FAN_SPEED_LEVEL_INTERFACE);
    if (props) {
        if (props->autoMode != autoMode) {
            props->autoMode = autoMode;

            status = EmitPropChanged(busAttachment, objPath, "AutoMode", "y", &(props->autoMode));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
