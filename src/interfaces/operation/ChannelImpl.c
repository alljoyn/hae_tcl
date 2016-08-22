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
#include <ajtcl/cdm/interfaces/operation/Channel.h>
#include "../../CdmControllee/CdmControlleeImpl.h"
#include "ChannelImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationChannel[] = {
    "$org.alljoyn.SmartSpaces.Operation.Channel",
    "@Version>q",
    "@ChannelId=s",
    "@TotalNumberOfChannels>q",
    "?GetChannelList startingRecord<q numRecords<q listOfChannelRecords>a(sss)",
    "!ChannelListChanged",
    NULL
};

typedef struct {
    uint16_t version;
    char* channelId;
    uint16_t totalNumberOfChannels;
} ChannelProperties;

AJ_Status CreateChannelInterface(void** properties)
{
    *properties = malloc(sizeof(ChannelProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(ChannelProperties));

    ((ChannelProperties*) *properties)->version = (uint16_t) INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyChannelInterface(void* properties)
{
    if (properties) {
        ChannelProperties* props = (ChannelProperties*) properties;
        if (props->channelId) {
            free(props->channelId);
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
        AJ_MarshalArgs(&msg, "s", intfDescOperationChannel[0] + 1);  //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "s")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, (char*) val);
        } else if (!strcmp(signature, "q")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint16_t*) val);
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

AJ_Status ChannelInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    ChannelProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (ChannelProperties*) properties;

    switch (memberIndex) {
    case 1:
        status = EmitPropChanged(busAttachment, objPath, "ChannelId", "s", props->channelId);
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status ChannelInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    ChannelProperties* props = NULL;
    ChannelListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (ChannelListener*) listener;
    }
    props = (ChannelProperties*) properties;

    switch (memberIndex) {
    case 0:
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1: {
        char channelId[128];  //?? how estimate proper buffer size when the listener is exist
        if (lt && lt->OnGetChannelId) {
            status = lt->OnGetChannelId(objPath, channelId);
            if (status == AJ_OK) {
                if (props->channelId) {
                    free(props->channelId);
                }
                props->channelId = (char*) malloc(strlen(channelId) + 1);
                if (!props->channelId) {
                    return AJ_ERR_RESOURCES;
                }
                strncpy(props->channelId, channelId, strlen(channelId));
                props->channelId[strlen(channelId)] = '\0';
            }
        }
        if (!props->channelId) {
            return AJ_ERR_NULL;
        }
        status = AJ_MarshalArgs(replyMsg, "s", props->channelId);
    }
        break;
    case 2: {
        uint16_t totalNumberOfChannels;
        if (lt && lt->OnGetTotalNumberOfChannels) {
            status = lt->OnGetTotalNumberOfChannels(objPath, &totalNumberOfChannels);
            if (status == AJ_OK) {
                props->totalNumberOfChannels = totalNumberOfChannels;
            }
        }
        status = AJ_MarshalArgs(replyMsg, "q", props->totalNumberOfChannels);
    }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status ChannelInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    ChannelProperties* props = (ChannelProperties*) properties;
    ChannelListener* lt = (ChannelListener*) listener;

    switch (memberIndex) {
    case 0:
        status = AJ_ERR_DISALLOWED;
        break;
    case 1:
        if (!lt->OnSetChannelId) {
            status = AJ_ERR_NULL;
        } else {
            char* channelId;
            status = AJ_UnmarshalArgs(replyMsg, "s", &channelId);

            if (status == AJ_OK) {
                status = lt->OnSetChannelId(objPath, channelId);

                if (status == AJ_OK) {
                    if (strcmp(props->channelId, channelId)) {
                        if (props->channelId) {
                            free(props->channelId);
                        }
                        props->channelId = (char*) malloc(strlen(channelId) + 1);
                        if (!props->channelId) {
                            status = AJ_ERR_RESOURCES;
                        } else {
                            strncpy(props->channelId, channelId, strlen(channelId));
                            props->channelId[strlen(channelId)] = '\0';
                            *propChanged = true;
                        }
                    }
                }
            }
        }
        break;
    case 2:
        status = AJ_ERR_DISALLOWED;
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status ChannelInterfaceOnMethodHandler(AJ_Message* msg, const char* objPath, uint8_t memberIndex, void* listener, CdmPropertiesChangedByMethod* propChangedByMethod)
{
    AJ_Status status = AJ_OK;

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    if (!propChangedByMethod) {
        return AJ_ERR_INVALID;
    }

    ChannelListener* lt = (ChannelListener*)listener;

    switch (memberIndex) {
    case 3:
        if (!lt->OnGetChannelList) {
            status = AJ_ERR_NULL;
        } else {
            uint16_t startingRecord, numOfRecords;
            ChannelInfoRecord* records = NULL;
            uint16_t numReturnedRecords = 0;

            status = AJ_UnmarshalArgs(msg, "qq", &startingRecord, &numOfRecords);

            if (status == AJ_OK) {
                ErrorCode errorCode = NOT_ERROR;
                status = lt->OnGetChannelList(objPath, startingRecord, numOfRecords, &records, &numReturnedRecords, &errorCode);
                if (status == AJ_OK) {
                    AJ_Message reply;
                    AJ_Arg array, strc;
                    int i = 0;

                    AJ_MarshalReplyMsg(msg, &reply);
                    AJ_MarshalContainer(&reply, &array, AJ_ARG_ARRAY);
                    for (i = 0; i < numReturnedRecords; i++) {
                        AJ_MarshalContainer(&reply, &strc, AJ_ARG_STRUCT);
                        AJ_MarshalArgs(&reply, "sss", ((records + i)->channelId), ((records + i)->channelNumber), ((records + i)->channelName));
                        AJ_MarshalCloseContainer(&reply, &strc);
                    }
                    AJ_MarshalCloseContainer(&reply, &array);

                    status = AJ_DeliverMsg(&reply);
                } else {
                    AJ_Message reply;
                    AJ_MarshalReplyMsg(msg, &reply);
                    AJ_MarshalErrorMsgWithInfo(msg, &reply, GetInterfaceErrorName(errorCode), GetInterfaceErrorMessage(errorCode));
                    status = AJ_DeliverMsg(&reply);
                }
            }
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Cdm_ChannelInterfaceGetChannelId(const char* objPath, char* channelId)
{
    AJ_Status status = AJ_OK;
    ChannelProperties* props = NULL;

    if (!channelId) {
        return AJ_ERR_INVALID;
    }

    props = (ChannelProperties*) GetProperties(objPath, CHANNEL_INTERFACE);
    if (props) {
        if (!props->channelId) {
            return AJ_ERR_NULL;
        }
        strncpy(channelId, props->channelId, strlen(props->channelId));
        channelId[strlen(props->channelId)] = '\0';
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_ChannelInterfaceSetChannelId(AJ_BusAttachment* busAttachment, const char* objPath, const char* channelId)
{
    AJ_Status status = AJ_OK;
    ChannelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!channelId) {
        return AJ_ERR_INVALID;
    }

    props = (ChannelProperties*) GetProperties(objPath, CHANNEL_INTERFACE);
    if (props) {
        if (props->channelId) {
            free(props->channelId);
        }
        props->channelId = (char*) malloc(strlen(channelId) + 1);
        if (!props->channelId) {
            status = AJ_ERR_RESOURCES;
        } else {
            strncpy(props->channelId, channelId, strlen(channelId));
            props->channelId[strlen(channelId)] = '\0';

            status = EmitPropChanged(busAttachment, objPath, "ChannelId", "s", props->channelId);
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_ChannelInterfaceGetTotalNumberOfChannels(const char* objPath, uint16_t* totalNumberOfChannels)
{
    AJ_Status status = AJ_OK;
    ChannelProperties* props = NULL;

    if (!totalNumberOfChannels) {
        return AJ_ERR_INVALID;
    }

    props = (ChannelProperties*) GetProperties(objPath, CHANNEL_INTERFACE);
    if (props) {
        *totalNumberOfChannels = props->totalNumberOfChannels;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_ChannelInterfaceSetTotalNumberOfChannels(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t totalNumberOfChannels)
{
    AJ_Status status = AJ_OK;
    ChannelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (ChannelProperties*) GetProperties(objPath, CHANNEL_INTERFACE);
    if (props) {
        props->totalNumberOfChannels = totalNumberOfChannels;

        status = EmitPropChanged(busAttachment, objPath, "TotalNumberOfChannels", "q", &(props->totalNumberOfChannels));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_ChannelInterfaceEmitChannelListChanged(AJ_BusAttachment* busAttachment, const char* objPath, const uint32_t sessionId)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t msgId = 0;

    status = MakeMsgId(objPath, CHANNEL_INTERFACE, 4, &msgId);
    if (status != AJ_OK) {
        return status;
    }

    AJ_MarshalSignal(busAttachment, &msg, msgId, NULL, sessionId, AJ_FLAG_GLOBAL_BROADCAST, 0);

    return AJ_DeliverMsg(&msg);
}
