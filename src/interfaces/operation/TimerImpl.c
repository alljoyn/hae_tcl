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
#include <ajtcl/hae/interfaces/operation/Timer.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "TimerImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationTimer[11] = {
    "$org.alljoyn.SmartSpaces.Operation.Timer",
    "@Version>q",
    "@ReferenceTimer>i",
    "@TargetTimeToStart>i",
    "@TargetTimeToStop>i",
    "@EstimatedTimeToEnd>i",
    "@RunningTime>i",
    "@TargetDuration>i",
    "?SetTargetTimeToStart targetTimeToStart<i",
    "?SetTargetTimeToStop targetTimeToStop<i",
    NULL
};

typedef struct {
    uint16_t version;
    int32_t referenceTimer;
    int32_t targetTimeToStart;
    int32_t targetTimeToStop;
    int32_t estimatedTimeToEnd;
    int32_t runningTime;
    int32_t targetDuration;
} TimerProperties;

AJ_Status CreateTimerInterface(void** properties)
{
    *properties = malloc(sizeof(TimerProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }

    ((TimerProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyTimerInterface(void* properties)
{
    if (properties) {
        TimerProperties* props = (TimerProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescOperationTimer[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "i")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(int32_t*)val);
        } else if (!strcmp(signature, "ay")) {
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

AJ_Status TimerInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    TimerProperties* props = NULL;
    TimerListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (TimerListener*)listener;
    }
    props = (TimerProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            int32_t referenceTimer;

            if (lt && lt->OnGetReferenceTimer) {
                status = lt->OnGetReferenceTimer(objPath, &referenceTimer);
                if (status == AJ_OK) {
                    props->referenceTimer = referenceTimer;
                }
            }
            if (!props->referenceTimer) {
                return AJ_ERR_NULL;
            }
            status = AJ_MarshalArgs(replyMsg, "i", props->referenceTimer);
        }
        break;
    case 2 :
        {
            int32_t targetTimeToStart;

            if (lt && lt->OnGetTargetTimeToStart) {
                status = lt->OnGetTargetTimeToStart(objPath, &targetTimeToStart);
                if (status == AJ_OK) {
                    props->targetTimeToStart = targetTimeToStart;
                }
            }
            if (!props->targetTimeToStart) {
                return AJ_ERR_NULL;
            }
            status = AJ_MarshalArgs(replyMsg, "i", props->targetTimeToStart);
        }
        break;
    case 3 :
        {
            int32_t targetTimeToStop;

            if (lt && lt->OnGetTargetTimeToStop) {
                status = lt->OnGetTargetTimeToStop(objPath, &targetTimeToStop);
                if (status == AJ_OK) {
                    props->targetTimeToStop = targetTimeToStop;
                }
            }
            if (!props->targetTimeToStop) {
                return AJ_ERR_NULL;
            }
            status = AJ_MarshalArgs(replyMsg, "i", props->targetTimeToStop);
        }
        break;
    case 4 :
        {
            int32_t estimatedTimeToEnd;

            if (lt && lt->OnGetEstimatedTimeToEnd) {
                status = lt->OnGetEstimatedTimeToEnd(objPath, &estimatedTimeToEnd);
                if (status == AJ_OK) {
                    props->estimatedTimeToEnd = estimatedTimeToEnd;
                }
            }
            if (!props->estimatedTimeToEnd) {
                return AJ_ERR_NULL;
            }
            status = AJ_MarshalArgs(replyMsg, "i", props->estimatedTimeToEnd);
        }
        break;
    case 5 :
        {
            int32_t runningTime;

            if (lt && lt->OnGetRunningTime) {
                status = lt->OnGetRunningTime(objPath, &runningTime);
                if (status == AJ_OK) {
                    props->runningTime = runningTime;
                }
            }
            if (!props->runningTime) {
                return AJ_ERR_NULL;
            }
            status = AJ_MarshalArgs(replyMsg, "i", props->runningTime);
        }
        break;
    case 6 :
        {
            int32_t targetDuration;

            if (lt && lt->OnGetTargetDuration) {
                status = lt->OnGetTargetDuration(objPath, &targetDuration);
                if (status == AJ_OK) {
                    props->targetDuration = targetDuration;
                }
            }
            if (!props->targetDuration) {
                return AJ_ERR_NULL;
            }
            status = AJ_MarshalArgs(replyMsg, "i", props->targetDuration);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status TimerInterfaceOnMethodHandler(AJ_Message* msg, const char* objPath, uint8_t memberIndex, void* listener, HaePropertiesChangedByMethod* propChangedByMethod)
{
    AJ_Status status = AJ_OK;

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    if (!propChangedByMethod) {
        return AJ_ERR_INVALID;
    }

    TimerListener* lt = (TimerListener*)listener;

    switch (memberIndex) {
    case 7 :
        if (!lt->OnSetTargetTimeToStart) {
            status = AJ_ERR_NULL;
        } else {
            int32_t targetTimeToStart;

            status = AJ_UnmarshalArgs(msg, "i", &targetTimeToStart);

            if (status == AJ_OK) {
                ErrorCode errorCode = NOT_ERROR;
                AJ_Message reply;
                status = lt->OnSetTargetTimeToStart(objPath, targetTimeToStart, &errorCode);

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
    case 8 :
        if (!lt->OnSetTargetTimeToStop) {
            status = AJ_ERR_NULL;
        } else {
            int32_t targetTimeToStop;

            status = AJ_UnmarshalArgs(msg, "i", &targetTimeToStop);
            if (status == AJ_OK) {
                ErrorCode errorCode = NOT_ERROR;
                AJ_Message reply;
                status = lt->OnSetTargetTimeToStop(objPath, targetTimeToStop, &errorCode);

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

AJ_Status Hae_TimerInterfaceGetReferenceTimer(const char* objPath, int32_t* referenceTimer)
{
    AJ_Status status = AJ_OK;
    TimerProperties* props = NULL;

    if (!referenceTimer) {
        status = AJ_ERR_INVALID;
    }

    props = (TimerProperties*)GetProperties(objPath, TIMER_INTERFACE);
    if (props) {
        *referenceTimer = props->referenceTimer;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TimerInterfaceSetReferenceTimer(AJ_BusAttachment* busAttachment, const char* objPath, const int32_t referenceTimer)
{
    AJ_Status status = AJ_OK;
    TimerProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (TimerProperties*)GetProperties(objPath, TIMER_INTERFACE);
    if (props->referenceTimer != referenceTimer) {
        props->referenceTimer = referenceTimer;

        status = EmitPropChanged(busAttachment, objPath, "ReferenceTimer", "i", &(props->referenceTimer));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TimerInterfaceGetTargetTimeToStart(const char* objPath, int32_t* targetTimeToStart)
{
    AJ_Status status = AJ_OK;
    TimerProperties* props = NULL;

    if (!targetTimeToStart) {
        status = AJ_ERR_INVALID;
    }

    props = (TimerProperties*)GetProperties(objPath, TIMER_INTERFACE);
    if (props) {
        *targetTimeToStart = props->targetTimeToStart;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TimerInterfaceSetTargetTimeToStart(AJ_BusAttachment* busAttachment, const char* objPath, const int32_t targetTimeToStart)
{
    AJ_Status status = AJ_OK;
    TimerProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (TimerProperties*)GetProperties(objPath, TIMER_INTERFACE);
    if (props->targetTimeToStart != targetTimeToStart) {
        props->targetTimeToStart = targetTimeToStart;

        status = EmitPropChanged(busAttachment, objPath, "TargetTimeToStart", "i", &(props->targetTimeToStart));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TimerInterfaceGetTargetTimeToStop(const char* objPath, int32_t* targetTimeToStop)
{
    AJ_Status status = AJ_OK;
    TimerProperties* props = NULL;

    if (!targetTimeToStop) {
        status = AJ_ERR_INVALID;
    }

    props = (TimerProperties*)GetProperties(objPath, TIMER_INTERFACE);
    if (props) {
        *targetTimeToStop = props->targetTimeToStop;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TimerInterfaceSetTargetTimeToStop(AJ_BusAttachment* busAttachment, const char* objPath, const int32_t targetTimeToStop)
{
    AJ_Status status = AJ_OK;
    TimerProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (TimerProperties*)GetProperties(objPath, TIMER_INTERFACE);
    if (props->targetTimeToStop != targetTimeToStop) {
        props->targetTimeToStop = targetTimeToStop;

        status = EmitPropChanged(busAttachment, objPath, "TargetTimeToStop", "i", &(props->targetTimeToStop));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TimerInterfaceGetEstimatedTimeToEnd(const char* objPath, int32_t* estimatedTimeToEnd)
{
    AJ_Status status = AJ_OK;
    TimerProperties* props = NULL;

    if (!estimatedTimeToEnd) {
        status = AJ_ERR_INVALID;
    }

    props = (TimerProperties*)GetProperties(objPath, TIMER_INTERFACE);
    if (props) {
        *estimatedTimeToEnd = props->estimatedTimeToEnd;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TimerInterfaceSetEstimatedTimeToEnd(AJ_BusAttachment* busAttachment, const char* objPath, const int32_t estimatedTimeToEnd)
{
    AJ_Status status = AJ_OK;
    TimerProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (TimerProperties*)GetProperties(objPath, TIMER_INTERFACE);
    if (props->estimatedTimeToEnd != estimatedTimeToEnd) {
        props->estimatedTimeToEnd = estimatedTimeToEnd;

        status = EmitPropChanged(busAttachment, objPath, "EstimatedTimeToEnd", "i", &(props->estimatedTimeToEnd));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TimerInterfaceGetRunningTime(const char* objPath, int32_t* runningTime)
{
    AJ_Status status = AJ_OK;
    TimerProperties* props = NULL;

    if (!runningTime) {
        status = AJ_ERR_INVALID;
    }

    props = (TimerProperties*)GetProperties(objPath, TIMER_INTERFACE);
    if (props) {
        *runningTime = props->runningTime;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TimerInterfaceSetRunningTime(AJ_BusAttachment* busAttachment, const char* objPath, const int32_t runningTime)
{
    AJ_Status status = AJ_OK;
    TimerProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (TimerProperties*)GetProperties(objPath, TIMER_INTERFACE);
    if (props->runningTime != runningTime) {
        props->runningTime = runningTime;

        status = EmitPropChanged(busAttachment, objPath, "RunningTime", "i", &(props->runningTime));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TimerInterfaceGetTargetDuration(const char* objPath, int32_t* targetDuration)
{
    AJ_Status status = AJ_OK;
    TimerProperties* props = NULL;

    if (!targetDuration) {
        status = AJ_ERR_INVALID;
    }

    props = (TimerProperties*)GetProperties(objPath, TIMER_INTERFACE);
    if (props) {
        *targetDuration = props->targetDuration;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TimerInterfaceSetTargetDuration(AJ_BusAttachment* busAttachment, const char* objPath, const int32_t targetDuration)
{
    AJ_Status status = AJ_OK;
    TimerProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (TimerProperties*)GetProperties(objPath, TIMER_INTERFACE);
    if (props->targetDuration != targetDuration) {
        props->targetDuration = targetDuration;

        status = EmitPropChanged(busAttachment, objPath, "TargetDuration", "i", &(props->targetDuration));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}


