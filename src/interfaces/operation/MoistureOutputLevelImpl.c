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

#include <ajtcl/cdm/interfaces/operation/MoistureOutputLevel.h>
#include "../../CdmControllee/CdmControlleeImpl.h"
#include "MoistureOutputLevelImpl.h"

#define INTERFACE_VERSION 1

#define AUTO_MODE_OFF 0x00
#define AUTO_MODE_ON 0x01
#define AUTO_MODE_NOT_SUPPORTED 0xFF

const char* const intfDescOperationMoistureOutputLevel[] = {
    "$org.alljoyn.SmartSpaces.Operation.MoistureOutputLevel",
    "@Version>q",
    "@MoistureOutputLevel=y",
    "@MaxMoistureOutputLevel>y",
    "@AutoMode=y",
    NULL
};

typedef struct {
    uint16_t version;
    uint8_t moistureOutputLevel;
    uint8_t maxMoistureOutputLevel;
    uint8_t autoMode;
} MoistureOutputLevelProperties;

AJ_Status MoistureOutputLevelMoistureOutputLevelValidationCheck(const char* objPath, uint8_t value)
{
    AJ_Status status = AJ_OK;
    MoistureOutputLevelProperties* props = NULL;

    props = (MoistureOutputLevelProperties*)GetProperties(objPath, MOISTURE_OUTPUT_LEVEL_INTERFACE);
    if (props) {
        if (value > props->maxMoistureOutputLevel) {
            status = AJ_ERR_RANGE;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status MoistureOutputLevelAutoModeValidationCheck(const char* objPath, uint8_t autoMode)
{
    AJ_Status status = AJ_OK;
    MoistureOutputLevelProperties* props = NULL;

    props = (MoistureOutputLevelProperties*)GetProperties(objPath, MOISTURE_OUTPUT_LEVEL_INTERFACE);
    if (props) {
        if (props->autoMode == AUTO_MODE_NOT_SUPPORTED) {
            status = AJ_ERR_INVALID;
        } else if (autoMode != AUTO_MODE_OFF && autoMode != AUTO_MODE_ON) {
            status = AJ_ERR_INVALID;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status CreateMoistureOutputLevelInterface(void** properties)
{
    *properties = malloc(sizeof(MoistureOutputLevelProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(MoistureOutputLevelProperties));

    ((MoistureOutputLevelProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyMoistureOutputLevelInterface(void* properties)
{
    if (properties) {
        MoistureOutputLevelProperties* props = (MoistureOutputLevelProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescOperationMoistureOutputLevel[0]+1); //To remove '$'
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

AJ_Status MoistureOutputLevelInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    MoistureOutputLevelProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (MoistureOutputLevelProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "MoistureOutputLevel", "y", &(props->moistureOutputLevel));
        break;
    case 3 :
        status = EmitPropChanged(busAttachment, objPath, "AutoMode", "y", &(props->autoMode));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status MoistureOutputLevelInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    MoistureOutputLevelProperties* props = NULL;
    MoistureOutputLevelListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (MoistureOutputLevelListener*)listener;
    }
    props = (MoistureOutputLevelProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            uint8_t value;
            if (lt && lt->OnGetMoistureOutputLevel) {
                status = lt->OnGetMoistureOutputLevel(objPath, &value);
                if (status == AJ_OK) {
                    props->moistureOutputLevel = value;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->moistureOutputLevel);
        }
        break;
    case 2 :
        {
            uint8_t value;
            if (lt && lt->OnGetMaxMoistureOutputLevel) {
                status = lt->OnGetMaxMoistureOutputLevel(objPath, &value);
                if (status == AJ_OK) {
                    props->maxMoistureOutputLevel = value;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->maxMoistureOutputLevel);
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

AJ_Status MoistureOutputLevelInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    MoistureOutputLevelProperties* props = (MoistureOutputLevelProperties*)properties;
    MoistureOutputLevelListener* lt = (MoistureOutputLevelListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        if (!lt->OnSetMoistureOutputLevel) {
            status = AJ_ERR_NULL;
        } else {
            uint8_t value = 0;
            status = AJ_UnmarshalArgs(replyMsg, "y", &value);

            if (status == AJ_OK) {
                status = MoistureOutputLevelMoistureOutputLevelValidationCheck(objPath, value);
                if (status != AJ_OK) {
                    return status;
                }

                status = lt->OnSetMoistureOutputLevel(objPath, value);
                if (status == AJ_OK) {
                    if (props->moistureOutputLevel != value) {
                        props->moistureOutputLevel = value;
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
                status = MoistureOutputLevelAutoModeValidationCheck(objPath, autoMode);
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

AJ_Status Cdm_MoistureOutputLevelInterfaceGetMoistureOutputLevel(const char* objPath, uint8_t* value)
{
    AJ_Status status = AJ_OK;
    MoistureOutputLevelProperties* props = NULL;

    if (!value) {
        return AJ_ERR_INVALID;
    }

    props = (MoistureOutputLevelProperties*)GetProperties(objPath, MOISTURE_OUTPUT_LEVEL_INTERFACE);
    if (props) {
        *value = props->moistureOutputLevel;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_MoistureOutputLevelInterfaceSetMoistureOutputLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t value)
{
    AJ_Status status = AJ_OK;
    MoistureOutputLevelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status = MoistureOutputLevelMoistureOutputLevelValidationCheck(objPath, value);
    if (status != AJ_OK) {
        return status;
    }

    props = (MoistureOutputLevelProperties*)GetProperties(objPath, MOISTURE_OUTPUT_LEVEL_INTERFACE);
    if (props) {
        if (props->moistureOutputLevel != value) {
            props->moistureOutputLevel = value;

            status = EmitPropChanged(busAttachment, objPath, "MoistureOutputLevel", "y", &(props->moistureOutputLevel));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_MoistureOutputLevelInterfaceGetMaxMoistureOutputLevel(const char* objPath, uint8_t* value)
{
    AJ_Status status = AJ_OK;
    MoistureOutputLevelProperties* props = NULL;

    if (!value) {
        return AJ_ERR_INVALID;
    }

    props = (MoistureOutputLevelProperties*)GetProperties(objPath, MOISTURE_OUTPUT_LEVEL_INTERFACE);
    if (props) {
        *value = props->maxMoistureOutputLevel;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_MoistureOutputLevelInterfaceSetMaxMoistureOutputLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t value)
{
    AJ_Status status = AJ_OK;
    MoistureOutputLevelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (MoistureOutputLevelProperties*)GetProperties(objPath, MOISTURE_OUTPUT_LEVEL_INTERFACE);
    if (props) {
        props->maxMoistureOutputLevel = value;

        status = EmitPropChanged(busAttachment, objPath, "MaxMoistureOutputLevel", "y", &(props->maxMoistureOutputLevel));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_MoistureOutputLevelInterfaceGetAutoMode(const char* objPath, uint8_t* autoMode)
{
    AJ_Status status = AJ_OK;
    MoistureOutputLevelProperties* props = NULL;

    if (!autoMode) {
        return AJ_ERR_INVALID;
    }

    props = (MoistureOutputLevelProperties*)GetProperties(objPath, MOISTURE_OUTPUT_LEVEL_INTERFACE);
    if (props) {
        *autoMode = props->autoMode;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_MoistureOutputLevelInterfaceSetAutoMode(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t autoMode)
{
    AJ_Status status = AJ_OK;
    MoistureOutputLevelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (autoMode != AUTO_MODE_OFF && autoMode != AUTO_MODE_ON && autoMode != AUTO_MODE_NOT_SUPPORTED) {
        return AJ_ERR_INVALID;
    }

    props = (MoistureOutputLevelProperties*)GetProperties(objPath, MOISTURE_OUTPUT_LEVEL_INTERFACE);
    if (props) {
        props->autoMode = autoMode;

        status = EmitPropChanged(busAttachment, objPath, "AutoMode", "y", &(props->autoMode));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
