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
#include <ajtcl/cdm/interfaces/operation/AudioVideoInput.h>
#include "../../CdmControllee/CdmControlleeImpl.h"
#include "AudioVideoInputImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationAudioVideoInput[] = {
    "$org.alljoyn.SmartSpaces.Operation.AudioVideoInput",
    "@Version>q",
    "@InputSourceId=q",
    "@SupportedInputSources>a(qqyqs)",
    NULL
};

typedef struct {
    uint16_t version;
    uint16_t inputSourceId;
    InputSource* supportedInputSources;
} AudioVideoInputProperties;

static size_t suppotedInputSourcesSize = 0;

AJ_Status InputSourceIdValidationCheck(const char* objPath, uint16_t inputSourceId)
{
    AJ_Status status = AJ_OK;
    AudioVideoInputProperties* props = NULL;
    props = (AudioVideoInputProperties*)GetProperties(objPath, AUDIO_VIDEO_INPUT_INTERFACE);

    if (props) {
        int i = 0;
        bool check = false;
        for (i = 0; i < suppotedInputSourcesSize; i++) {
            if ((props->supportedInputSources)[i].id == inputSourceId) {
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

AJ_Status CreateAudioVideoInputInterface(void** properties)
{
    *properties = malloc(sizeof(AudioVideoInputProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(AudioVideoInputProperties));

    ((AudioVideoInputProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;
    ((AudioVideoInputProperties*)*properties)->supportedInputSources = NULL;

    return AJ_OK;
}

void DestroyAudioVideoInputInterface(void* properties)
{
    if (properties) {
        AudioVideoInputProperties* props = (AudioVideoInputProperties*)properties;
        if (props->supportedInputSources) {
            int i = 0;
            for (i = 0; i < suppotedInputSourcesSize; i++) {
                free((props->supportedInputSources)[i].friendlyName);
            }
            free(props->supportedInputSources);
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
        AJ_MarshalArgs(&msg, "s", intfDescOperationAudioVideoInput[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "q")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint16_t*)val);
        } else if (!strcmp(signature, "a(qqyqs)")) {
            InputSource* supportedInputSources = (InputSource*)val;
            AJ_Arg array2, strc2;
            int i = 0;

            AJ_MarshalArgs(&msg, "s", propName);
            AJ_MarshalVariant(&msg, signature);
            AJ_MarshalContainer(&msg, &array2, AJ_ARG_ARRAY);
            for (i = 0; i < suppotedInputSourcesSize; i++) {
                AJ_MarshalContainer(&msg, &strc2, AJ_ARG_STRUCT);
                AJ_MarshalArgs(&msg, "qqyqs", supportedInputSources[i].id, supportedInputSources[i].sourceType,
                               supportedInputSources[i].detectedState, supportedInputSources[i].portNumber,
                               supportedInputSources[i].friendlyName);
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

AJ_Status AudioVideoInputInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    AudioVideoInputProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (AudioVideoInputProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "InputSourceId", "q", &(props->inputSourceId));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status AudioVideoInputInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    AudioVideoInputProperties* props = NULL;
    AudioVideoInputListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (AudioVideoInputListener*)listener;
    }
    props = (AudioVideoInputProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            uint16_t inputSourceId;

            if (lt && lt->OnGetInputSourceId) {
                status = lt->OnGetInputSourceId(objPath, &inputSourceId);
                if (status == AJ_OK) {
                    props->inputSourceId = inputSourceId;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "q", props->inputSourceId);
        }
        break;
    case 2 :
        {
            InputSource* supportedInputSources = (InputSource*)malloc(sizeof(InputSource) * suppotedInputSourcesSize);
            int i = 0;
            if (lt && lt->OnGetSupportedInputSources) {
                status = lt->OnGetSupportedInputSources(objPath, supportedInputSources);
                if (status == AJ_OK) {
                    if (props->supportedInputSources) {
                        for (i = 0; i < suppotedInputSourcesSize; i++) {
                            free((props->supportedInputSources)[i].friendlyName);
                        }
                        free(props->supportedInputSources);
                    }

                    props->supportedInputSources = (InputSource*)malloc(sizeof(InputSource) * suppotedInputSourcesSize);
                    memcpy(props->supportedInputSources, supportedInputSources, sizeof(InputSource) * suppotedInputSourcesSize);

                    for (i = 0; i < suppotedInputSourcesSize; i++) {
                        (props->supportedInputSources)[i].friendlyName = (char*)malloc(strlen(supportedInputSources[i].friendlyName) + 1);
                        strncpy((props->supportedInputSources)[i].friendlyName, supportedInputSources[i].friendlyName, strlen(supportedInputSources[i].friendlyName));
                        ((props->supportedInputSources)[i].friendlyName)[strlen(supportedInputSources[i].friendlyName)] = '\0';
                    }
                 }
            }
            if (!props->supportedInputSources) {
                free(supportedInputSources);
                return AJ_ERR_NULL;
            }
            AJ_Arg array, strc;
            AJ_MarshalContainer(replyMsg, &array, AJ_ARG_ARRAY);
            for (i = 0; i < suppotedInputSourcesSize; i++) {
                AJ_MarshalContainer(replyMsg, &strc, AJ_ARG_STRUCT);
                status = AJ_MarshalArgs(replyMsg, "qqyqs",  (props->supportedInputSources)[i].id,
                                                            (props->supportedInputSources)[i].sourceType,
                                                            (props->supportedInputSources)[i].detectedState,
                                                            (props->supportedInputSources)[i].portNumber,
                                                            (props->supportedInputSources)[i].friendlyName);
                AJ_MarshalCloseContainer(replyMsg, &strc);
            }
            AJ_MarshalCloseContainer(replyMsg, &array);

            free(supportedInputSources);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status AudioVideoInputInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    AudioVideoInputProperties* props = (AudioVideoInputProperties*)properties;
    AudioVideoInputListener* lt = (AudioVideoInputListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        if (!lt->OnSetInputSourceId) {
            status = AJ_ERR_NULL;
        } else {
            uint16_t inputSourceId;
            status = AJ_UnmarshalArgs(replyMsg, "q", &inputSourceId);

            if (status == AJ_OK) {
                status = InputSourceIdValidationCheck(objPath, inputSourceId);
                if (status != AJ_OK) {
                    return status;
                }

                status = lt->OnSetInputSourceId(objPath, inputSourceId);

                if (status == AJ_OK) {
                    if (props->inputSourceId != inputSourceId) {
                        props->inputSourceId = inputSourceId;
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

AJ_Status Cdm_AudioVideoInputInterfaceGetInputSourceId(const char* objPath, uint16_t* inputSourceId)
{
    AJ_Status status = AJ_OK;
    AudioVideoInputProperties* props = NULL;

    if (!inputSourceId) {
        return AJ_ERR_INVALID;
    }

    props = (AudioVideoInputProperties*)GetProperties(objPath, AUDIO_VIDEO_INPUT_INTERFACE);
    if (props) {
        *inputSourceId = props->inputSourceId;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_AudioVideoInputInterfaceSetInputSourceId(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t inputSourceId)
{
    AJ_Status status = AJ_OK;
    AudioVideoInputProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status = InputSourceIdValidationCheck(objPath, inputSourceId);
    if (status != AJ_OK) {
        return status;
    }

    props = (AudioVideoInputProperties*)GetProperties(objPath, AUDIO_VIDEO_INPUT_INTERFACE);
    if (props) {
        if (props->inputSourceId != inputSourceId) {
            props->inputSourceId = inputSourceId;

            status = EmitPropChanged(busAttachment, objPath, "InputSourceId", "q", &(props->inputSourceId));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_AudioVideoInputInterfaceGetSupportedInputSources(const char* objPath, InputSource* inputSource)
{
    AJ_Status status = AJ_OK;
    AudioVideoInputProperties* props = NULL;

    if (!inputSource) {
        return AJ_ERR_INVALID;
    }

    props = (AudioVideoInputProperties*)GetProperties(objPath, AUDIO_VIDEO_INPUT_INTERFACE);

    if (props) {
        if (!props->supportedInputSources) {
            return AJ_ERR_NULL;
        }

        memcpy(inputSource, props->supportedInputSources, sizeof(InputSource) * suppotedInputSourcesSize);

    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_AudioVideoInputInterfaceSetSupportedInputSources(AJ_BusAttachment* busAttachment, const char* objPath, const InputSource* supportedInputSources, const size_t inputSourceListSize)
{
    AJ_Status status = AJ_OK;
    AudioVideoInputProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!supportedInputSources) {
        return AJ_ERR_INVALID;
    }

    props = (AudioVideoInputProperties*)GetProperties(objPath, AUDIO_VIDEO_INPUT_INTERFACE);

    if (props) {
        int i = 0;
        if (props->supportedInputSources) {
            for (i = 0; i < suppotedInputSourcesSize; i++) {
                free((props->supportedInputSources)[i].friendlyName);
            }
            free(props->supportedInputSources);
        }

        suppotedInputSourcesSize = inputSourceListSize;
        props->supportedInputSources = (InputSource*)malloc(sizeof(InputSource) * suppotedInputSourcesSize);

        memcpy(props->supportedInputSources, supportedInputSources, sizeof(InputSource) * suppotedInputSourcesSize);
        for (i = 0; i < suppotedInputSourcesSize; i++) {
            (props->supportedInputSources)[i].friendlyName = (char*)malloc(strlen(supportedInputSources[i].friendlyName) + 1);
            strncpy((props->supportedInputSources)[i].friendlyName, supportedInputSources[i].friendlyName, strlen(supportedInputSources[i].friendlyName));
            ((props->supportedInputSources)[i].friendlyName)[strlen(supportedInputSources[i].friendlyName)] = '\0';
        }
        status = EmitPropChanged(busAttachment, objPath, "SupportedInputSources", "a(qqyqs)", props->supportedInputSources);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
