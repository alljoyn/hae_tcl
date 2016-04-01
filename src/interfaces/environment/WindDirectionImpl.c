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

#include <ajtcl/hae/interfaces/environment/WindDirection.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "WindDirectionImpl.h"

#define INTERFACE_VERSION 1

#define AUTO_MODE_OFF 0x00
#define AUTO_MODE_ON 0x01
#define AUTO_MODE_NOT_SUPPORTED 0xFF

const char* const intfDescEnvironmentWindDirection[] = {
    "$org.alljoyn.SmartSpaces.Environment.WindDirection",
    "@Version>q",
    "@HorizontalDirection=q",
    "@HorizontalMax>q",
    "@HorizontalAutoMode=y",
    "@VerticalDirection=q",
    "@VerticalMax>q",
    "@VerticalAutoMode=y",
    NULL
};

typedef struct {
    uint16_t version;
    uint16_t horizontalDirection;
    uint16_t horizontalMax;
    uint8_t horizontalAutoMode;
    uint16_t verticalDirection;
    uint16_t verticalMax;
    uint8_t verticalAutoMode;
} WindDirectionProperties;

AJ_Status WindDirectionAdjustHorizontalDirection(const char* objPath, uint16_t* horizontalDirection)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        if (*horizontalDirection > props->horizontalMax) {
            *horizontalDirection = props->horizontalMax;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status WindDirectionHorizontalDirectionValidationCheck(const char* objPath, uint16_t horizontalDirection)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        if (horizontalDirection < 0 || horizontalDirection > props->horizontalMax) {
            status = AJ_ERR_RANGE;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status WindDirectionHorizontalAutoModeValidationCheck(const char* objPath, uint8_t horizontalAutoMode)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        if (props->horizontalAutoMode == AUTO_MODE_NOT_SUPPORTED) {
            status = AJ_ERR_INVALID;
        } else if (horizontalAutoMode != AUTO_MODE_OFF && horizontalAutoMode != AUTO_MODE_ON) {
            status = AJ_ERR_INVALID;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status WindDirectionAdjustVerticalDirection(const char* objPath, uint16_t* verticalDirection)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        if (*verticalDirection > props->verticalMax) {
            *verticalDirection = props->verticalMax;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status WindDirectionVerticalDirectionValidationCheck(const char* objPath, uint16_t verticalDirection)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        if (verticalDirection < 0 || verticalDirection > props->verticalMax) {
            status = AJ_ERR_RANGE;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status WindDirectionVerticalAutoModeValidationCheck(const char* objPath, uint8_t verticalAutoMode)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        if (props->verticalAutoMode == AUTO_MODE_NOT_SUPPORTED) {
            status = AJ_ERR_INVALID;
        } else if (verticalAutoMode != AUTO_MODE_OFF && verticalAutoMode != AUTO_MODE_ON) {
            status = AJ_ERR_INVALID;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status CreateWindDirectionInterface(void** properties)
{
    *properties = malloc(sizeof(WindDirectionProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }

    ((WindDirectionProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}
void DestroyWindDirectionInterface(void* properties)
{
    if (properties) {
        WindDirectionProperties* props = (WindDirectionProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescEnvironmentWindDirection[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "y")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint8_t*)val);
        } else if (!strcmp(signature, "q")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint16_t*)val);
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

AJ_Status WindDirectionInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (WindDirectionProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "HorizontalDirection", "q", &(props->horizontalDirection));
        break;
    case 3 :
        status = EmitPropChanged(busAttachment, objPath, "HorizontalAutoMode", "y", &(props->horizontalAutoMode));
        break;
    case 4 :
        status = EmitPropChanged(busAttachment, objPath, "VerticalDirection", "q", &(props->verticalDirection));
        break;
    case 6 :
        status = EmitPropChanged(busAttachment, objPath, "VerticalAutoMode", "y", &(props->verticalAutoMode));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status WindDirectionInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;
    WindDirectionListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (WindDirectionListener*)listener;
    }
    props = (WindDirectionProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            uint16_t horizontalDirection;
            if (lt && lt->OnGetHorizontalDirection) {
                status = lt->OnGetHorizontalDirection(objPath, &horizontalDirection);
                if (status == AJ_OK) {
                    props->horizontalDirection = horizontalDirection;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "q", props->horizontalDirection);
        }
        break;
    case 2 :
        {
            uint16_t horizontalMax;
            if (lt && lt->OnGetHorizontalMax) {
                status = lt->OnGetHorizontalMax(objPath, &horizontalMax);
                if (status == AJ_OK) {
                    props->horizontalMax = horizontalMax;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "q", props->horizontalMax);
        }
        break;
     case 3 :
        {
            uint8_t horizontalAutoMode;
            if (lt && lt->OnGetHorizontalAutoMode) {
                status = lt->OnGetHorizontalAutoMode(objPath, &horizontalAutoMode);
                if (status == AJ_OK) {
                    props->horizontalAutoMode = horizontalAutoMode;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->horizontalAutoMode);
        }
        break;
    case 4 :
        {
            uint16_t verticalDirection;
            if (lt && lt->OnGetVerticalDirection) {
                status = lt->OnGetVerticalDirection(objPath, &verticalDirection);
                if (status == AJ_OK) {
                    props->verticalDirection = verticalDirection;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "q", props->verticalDirection);
        }
        break;
    case 5 :
        {
            uint16_t verticalMax;
            if (lt && lt->OnGetVerticalMax) {
                status = lt->OnGetVerticalMax(objPath, &verticalMax);
                if (status == AJ_OK) {
                    props->verticalMax = verticalMax;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "q", props->verticalMax);
        }
        break;
     case 6 :
        {
            uint8_t verticalAutoMode;
            if (lt && lt->OnGetVerticalAutoMode) {
                status = lt->OnGetVerticalAutoMode(objPath, &verticalAutoMode);
                if (status == AJ_OK) {
                    props->verticalAutoMode = verticalAutoMode;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->verticalAutoMode);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status WindDirectionInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    WindDirectionProperties* props = (WindDirectionProperties*)properties;
    WindDirectionListener* lt = (WindDirectionListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        if (!lt->OnSetHorizontalDirection) {
            status = AJ_ERR_NULL;
        } else {
            uint16_t horizontalDirection = 0;
            status = AJ_UnmarshalArgs(replyMsg, "q", &horizontalDirection);

            if (status == AJ_OK) {
                status = WindDirectionAdjustHorizontalDirection(objPath, &horizontalDirection);
                if (status != AJ_OK) {
                    return status;
                }

                status = lt->OnSetHorizontalDirection(objPath, horizontalDirection);
                if (status == AJ_OK) {
                    if (props->horizontalDirection != horizontalDirection) {
                        props->horizontalDirection = horizontalDirection;
                        *propChanged = true;
                    }
                }
            }
        }
        break;
    case 3 :
        if (!lt->OnSetHorizontalAutoMode) {
            status = AJ_ERR_NULL;
        } else {
            uint8_t horizontalAutoMode = 0;
            status = AJ_UnmarshalArgs(replyMsg, "y", &horizontalAutoMode);

            if (status == AJ_OK) {
                status = WindDirectionHorizontalAutoModeValidationCheck(objPath, horizontalAutoMode);
                if (status != AJ_OK) {
                    return status;
                }

                status = lt->OnSetHorizontalAutoMode(objPath, horizontalAutoMode);
                if (status == AJ_OK) {
                    if (props->horizontalAutoMode != horizontalAutoMode) {
                        props->horizontalAutoMode = horizontalAutoMode;
                        *propChanged = true;
                    }
                }
            }
        }
        break;
    case 4 :
        if (!lt->OnSetVerticalDirection) {
            status = AJ_ERR_NULL;
        } else {
            uint16_t verticalDirection = 0;
            status = AJ_UnmarshalArgs(replyMsg, "q", &verticalDirection);

            if (status == AJ_OK) {
                status = WindDirectionAdjustVerticalDirection(objPath, &verticalDirection);
                if (status != AJ_OK) {
                    return status;
                }

                status = lt->OnSetVerticalDirection(objPath, verticalDirection);
                if (status == AJ_OK) {
                    if (props->verticalDirection != verticalDirection) {
                        props->verticalDirection = verticalDirection;
                        *propChanged = true;
                    }
                }
            }
        }
        break;
    case 6 :
        if (!lt->OnSetVerticalAutoMode) {
            status = AJ_ERR_NULL;
        } else {
            uint8_t verticalAutoMode = 0;
            status = AJ_UnmarshalArgs(replyMsg, "y", &verticalAutoMode);

            if (status == AJ_OK) {
                status = WindDirectionVerticalAutoModeValidationCheck(objPath, verticalAutoMode);
                if (status != AJ_OK) {
                    return status;
                }

                status = lt->OnSetVerticalAutoMode(objPath, verticalAutoMode);
                if (status == AJ_OK) {
                    if (props->verticalAutoMode != verticalAutoMode) {
                        props->verticalAutoMode = verticalAutoMode;
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

AJ_Status Hae_WindDirectionInterfaceGetHorizontalDirection(const char* objPath, uint16_t* horizontalDirection)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    if (!horizontalDirection) {
        return AJ_ERR_INVALID;
    }

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        *horizontalDirection = props->horizontalDirection;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_WindDirectionInterfaceSetHorizontalDirection(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t horizontalDirection)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status = WindDirectionHorizontalDirectionValidationCheck(objPath, horizontalDirection);
    if (status != AJ_OK) {
        return status;
    }

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        if (props->horizontalDirection != horizontalDirection) {
            props->horizontalDirection = horizontalDirection;

            status = EmitPropChanged(busAttachment, objPath, "HorizontalDirection", "q", &(props->horizontalDirection));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_WindDirectionInterfaceGetHorizontalMax(const char* objPath, uint16_t* horizontalMax)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    if (!horizontalMax) {
        return AJ_ERR_INVALID;
    }

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        *horizontalMax = props->horizontalMax;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_WindDirectionInterfaceSetHorizontalMax(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t horizontalMax)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (horizontalMax < 0) {
        return AJ_ERR_RANGE;
    }

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        if (props->horizontalMax != horizontalMax) {
            props->horizontalMax = horizontalMax;

            status = EmitPropChanged(busAttachment, objPath, "HorizontalMax", "q", &(props->horizontalMax));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_WindDirectionInterfaceGetHorizontalAutoMode(const char* objPath, uint8_t* horizontalAutoMode)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    if (!horizontalAutoMode) {
        return AJ_ERR_INVALID;
    }

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        *horizontalAutoMode = props->horizontalAutoMode;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}


AJ_Status Hae_WindDirectionInterfaceSetHorizontalAutoMode(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t horizontalAutoMode)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (horizontalAutoMode != AUTO_MODE_OFF && horizontalAutoMode != AUTO_MODE_ON && horizontalAutoMode != AUTO_MODE_NOT_SUPPORTED) {
        return AJ_ERR_INVALID;
    }

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        if (props->horizontalAutoMode != horizontalAutoMode) {
            props->horizontalAutoMode = horizontalAutoMode;

            status = EmitPropChanged(busAttachment, objPath, "HorizontalAutoMode", "y", &(props->horizontalAutoMode));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_WindDirectionInterfaceGetVerticalDirection(const char* objPath, uint16_t* verticalDirection)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    if (!verticalDirection) {
        return AJ_ERR_INVALID;
    }

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        *verticalDirection = props->verticalDirection;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_WindDirectionInterfaceSetVerticalDirection(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t verticalDirection)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status = WindDirectionVerticalDirectionValidationCheck(objPath, verticalDirection);
    if (status != AJ_OK) {
        return status;
    }

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        if (props->verticalDirection != verticalDirection) {
            props->verticalDirection = verticalDirection;

            status = EmitPropChanged(busAttachment, objPath, "VerticalDirection", "q", &(props->verticalDirection));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_WindDirectionInterfaceGetVerticalMax(const char* objPath, uint16_t* verticalMax)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    if (!verticalMax) {
        return AJ_ERR_INVALID;
    }

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        *verticalMax = props->verticalMax;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_WindDirectionInterfaceSetVerticalMax(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t verticalMax)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (verticalMax < 0) {
        return AJ_ERR_RANGE;
    }

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        if (props->verticalMax != verticalMax) {
            props->verticalMax = verticalMax;

            status = EmitPropChanged(busAttachment, objPath, "VerticalMax", "q", &(props->verticalMax));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_WindDirectionInterfaceGetVerticalAutoMode(const char* objPath, uint8_t* verticalAutoMode)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    if (!verticalAutoMode) {
        return AJ_ERR_INVALID;
    }

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        *verticalAutoMode = props->verticalAutoMode;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_WindDirectionInterfaceSetVerticalAutoMode(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t verticalAutoMode)
{
    AJ_Status status = AJ_OK;
    WindDirectionProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (verticalAutoMode != AUTO_MODE_OFF && verticalAutoMode != AUTO_MODE_ON && verticalAutoMode != AUTO_MODE_NOT_SUPPORTED) {
        return AJ_ERR_INVALID;
    }

    props = (WindDirectionProperties*)GetProperties(objPath, WIND_DIRECTION_INTERFACE);
    if (props) {
        if (props->verticalAutoMode != verticalAutoMode) {
            props->verticalAutoMode = verticalAutoMode;

            status = EmitPropChanged(busAttachment, objPath, "VerticalAutoMode", "y", &(props->verticalAutoMode));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

