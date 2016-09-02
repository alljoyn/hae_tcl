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
#include <ajtcl/cdm/interfaces/operation/CycleControl.h>
#include "../../CdmControllee/CdmControlleeImpl.h"
#include "CycleControlImpl.h"

#define INTERFACE_VERSION 1

#define MAX_SUPPORTED_OPER_STATES_NUM   6
#define MAX_SUPPORTED_OPER_COMMANDS_NUM 4

const char* const intfDescOperationCycleControl[] = {
    "$org.alljoyn.SmartSpaces.Operation.CycleControl",
    "@Version>q",
    "@OperationalState>y",
    "@SupportedOperationalStates>ay",
    "@SupportedOperationalCommands>ay",
    "?ExecuteOperationalCommand command<y",
    NULL
};

typedef struct {
    uint16_t version;
    uint8_t operationalState;
    uint8_t* supportedOperationalStates;
    uint8_t* supportedOperationalCommands;
} CycleControlProperties;

static size_t statesListSize = 0;
static size_t commandsListSize = 0;

AJ_Status CycleControlOperationalStateValidationCheck(const char* objPath, uint8_t operationalState)
{
    AJ_Status status = AJ_OK;
    CycleControlProperties* props = NULL;
    props = (CycleControlProperties*)GetProperties(objPath, CYCLE_CONTROL_INTERFACE);

    if (props) {
        int i = 0;
        bool check = false;

        for (i = 0; i < statesListSize; i++) {
            if (props->supportedOperationalStates[i] == operationalState) {
                check = true;
                break;
            }
        }
        if (!check) {
            status = AJ_ERR_RANGE;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status CreateCycleControlInterface(void** properties)
{
    *properties = malloc(sizeof(CycleControlProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(CycleControlProperties));

    ((CycleControlProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;
    ((CycleControlProperties*)*properties)->supportedOperationalStates = NULL;
    ((CycleControlProperties*)*properties)->supportedOperationalCommands = NULL;

    return AJ_OK;
}

void DestroyCycleControlInterface(void* properties)
{
    if (properties) {
        CycleControlProperties* props = (CycleControlProperties*)properties;
        if (props->supportedOperationalStates) {
            free(props->supportedOperationalStates);
        }
        if (props->supportedOperationalCommands) {
            free(props->supportedOperationalCommands);
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
        AJ_MarshalArgs(&msg, "s", intfDescOperationCycleControl[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "y")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint8_t*)val);
        } else if (!strcmp(signature, "ay")) {
            if (!strcmp(propName, "SupportedOperationalStates")) {
                AJ_MarshalArgs(&msg, "sv", propName, signature, (uint8_t*)val, statesListSize);
            }
            else if (!strcmp(propName, "SupportedOperationalCommands")) {
                AJ_MarshalArgs(&msg, "sv", propName, signature, (uint8_t*)val, commandsListSize);
            } else {
                return AJ_ERR_INVALID;
            }
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

AJ_Status CycleControlInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    CycleControlProperties* props = NULL;
    CycleControlListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (CycleControlListener*)listener;
    }
    props = (CycleControlProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            uint8_t operationalState;
            if (lt && lt->OnGetOperationalState) {
                status = lt->OnGetOperationalState(objPath, &operationalState);
                if (status == AJ_OK) {
                    props->operationalState = operationalState;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->operationalState);
        }
        break;
    case 2 :
        {
            uint8_t* supportedOperationalStates = (uint8_t*)malloc(sizeof(uint8_t) * statesListSize);
            if (lt && lt->OnGetSupportedOperationalStates) {
                status = lt->OnGetSupportedOperationalStates(objPath, supportedOperationalStates);
                if (status == AJ_OK) {
                    if (props->supportedOperationalStates) {
                        free(props->supportedOperationalStates);
                    }
                    props->supportedOperationalStates = (uint8_t*)malloc(sizeof(uint8_t) * statesListSize);

                    memcpy(props->supportedOperationalStates, supportedOperationalStates, sizeof(uint8_t) * statesListSize);
                }
            }
            if (!props->supportedOperationalStates) {
                free(supportedOperationalStates);
                return AJ_ERR_NULL;
            }

            status = AJ_MarshalArgs(replyMsg, "ay", props->supportedOperationalStates, sizeof(uint8_t) * statesListSize);
            free(supportedOperationalStates);
        }
        break;
    case 3 :
        {
            uint8_t* supportedOperationalCommands = (uint8_t*)malloc(sizeof(uint8_t) * commandsListSize);
            if (lt && lt->OnGetSupportedOperationalCommands) {
                status = lt->OnGetSupportedOperationalCommands(objPath, supportedOperationalCommands);
                if (status == AJ_OK) {
                    if (props->supportedOperationalCommands) {
                        free(props->supportedOperationalCommands);
                    }
                    props->supportedOperationalCommands = (uint8_t*)malloc(sizeof(uint8_t) * commandsListSize);

                    memcpy(props->supportedOperationalCommands, supportedOperationalCommands, sizeof(uint8_t) * commandsListSize);
                }
            }
            if (!props->supportedOperationalCommands) {
                free(supportedOperationalCommands);
                return AJ_ERR_NULL;
            }

            status = AJ_MarshalArgs(replyMsg, "ay", props->supportedOperationalCommands, sizeof(uint8_t) * commandsListSize);
            free(supportedOperationalCommands);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status CycleControlInterfaceOnMethodHandler(AJ_Message* msg, const char* objPath, uint8_t memberIndex, void* listener, CdmPropertiesChangedByMethod* propChangedByMethod)
{
    AJ_Status status = AJ_OK;

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    if (!propChangedByMethod) {
        return AJ_ERR_INVALID;
    }

    CycleControlListener* lt = (CycleControlListener*)listener;

    switch (memberIndex) {
    case 4 :
        if (!lt->OnExecuteOperationalCommand) {
            status = AJ_ERR_NULL;
        } else {
            uint8_t command;
 
            status = AJ_UnmarshalArgs(msg, "y", &command);
            if (status == AJ_OK) {
                ErrorCode errorCode = NOT_ERROR;
                AJ_Message reply;
                status = lt->OnExecuteOperationalCommand(objPath, command, &errorCode);

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
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}


AJ_Status Cdm_CycleControlInterfaceGetOperationalState(const char* objPath, uint8_t* operationalState)
{
    AJ_Status status = AJ_OK;
    CycleControlProperties* props = NULL;

    if (!operationalState) {
        return AJ_ERR_INVALID;
    }

    props = (CycleControlProperties*)GetProperties(objPath, CYCLE_CONTROL_INTERFACE);
    if (props) {
        *operationalState = props->operationalState;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_CycleControlInterfaceSetOperationalState(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t operationalState)
{
    AJ_Status status = AJ_OK;
    CycleControlProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status =  CycleControlOperationalStateValidationCheck(objPath, operationalState);

    if (status != AJ_OK) {
        return status;
    }

    props = (CycleControlProperties*)GetProperties(objPath, CYCLE_CONTROL_INTERFACE);
    if (props) {
        if (props->operationalState != operationalState) {
            props->operationalState = operationalState;

            status = EmitPropChanged(busAttachment, objPath, "OperationalState", "y", &(props->operationalState));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_CycleControlInterfaceGetSupportedOperationalStates(const char* objPath, uint8_t* supportedOperationalStates)
{
    AJ_Status status = AJ_OK;
    CycleControlProperties* props = NULL;

    if (!supportedOperationalStates) {
        return AJ_ERR_INVALID;
    }

    props = (CycleControlProperties*)GetProperties(objPath, CYCLE_CONTROL_INTERFACE);
    if (props) {
        if (!props->supportedOperationalStates) {
            return AJ_ERR_NULL;
        }
        memcpy(supportedOperationalStates, props->supportedOperationalStates, sizeof(uint8_t) * statesListSize);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_CycleControlInterfaceSetSupportedOperationalStates(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t* supportedOperationalStates, const size_t supportedModeSize)
{
    AJ_Status status = AJ_OK;
    CycleControlProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!supportedOperationalStates) {
        return AJ_ERR_INVALID;
    }

    if (supportedModeSize > MAX_SUPPORTED_OPER_STATES_NUM) {
        return AJ_ERR_INVALID;
    }

    props = (CycleControlProperties*)GetProperties(objPath, CYCLE_CONTROL_INTERFACE);
    if (props) {
        if (props->supportedOperationalStates) {
            free(props->supportedOperationalStates);
        }
        statesListSize = supportedModeSize;

        props->supportedOperationalStates = (uint8_t*)malloc(sizeof(uint8_t) * statesListSize);

        memcpy(props->supportedOperationalStates, supportedOperationalStates, sizeof(uint8_t) * statesListSize);

        status = EmitPropChanged(busAttachment, objPath, "SupportedOperationalStates", "ay", props->supportedOperationalStates);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_CycleControlInterfaceGetSupportedOperationalCommands(const char* objPath, uint8_t* supportedOperationalCommands)
{
    AJ_Status status = AJ_OK;
    CycleControlProperties* props = NULL;

    if (!supportedOperationalCommands) {
        return AJ_ERR_INVALID;
    }

    props = (CycleControlProperties*)GetProperties(objPath, CYCLE_CONTROL_INTERFACE);
    if (props) {
        if (!props->supportedOperationalCommands) {
            return AJ_ERR_NULL;
        }
        memcpy(supportedOperationalCommands, props->supportedOperationalCommands, sizeof(uint8_t) * commandsListSize);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_CycleControlInterfaceSetSupportedOperationalCommands(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t* supportedOperationalCommands, const size_t supportedModeSize)
{
    AJ_Status status = AJ_OK;
    CycleControlProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!supportedOperationalCommands) {
        return AJ_ERR_INVALID;
    }

    if (supportedModeSize > MAX_SUPPORTED_OPER_COMMANDS_NUM) {
        return AJ_ERR_INVALID;
    }

    props = (CycleControlProperties*)GetProperties(objPath, CYCLE_CONTROL_INTERFACE);
    if (props) {
        if (props->supportedOperationalCommands) {
            free(props->supportedOperationalCommands);
        }
        commandsListSize = supportedModeSize;

        props->supportedOperationalCommands = (uint8_t*)malloc(sizeof(uint8_t) * commandsListSize);

        memcpy(props->supportedOperationalCommands, supportedOperationalCommands, sizeof(uint8_t) * commandsListSize);

        status = EmitPropChanged(busAttachment, objPath, "SupportedOperationalCommands", "ay", props->supportedOperationalCommands);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
