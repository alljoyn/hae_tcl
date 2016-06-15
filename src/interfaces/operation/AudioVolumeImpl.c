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

#include <ajtcl/hae/interfaces/operation/AudioVolume.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "AudioVolumeImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationAudioVolume[] = {
    "$org.alljoyn.SmartSpaces.Operation.AudioVolume",
    "@Version>q",
    "@Volume=y",
    "@MaxVolume>y",
    "@Mute=b",
    NULL
};

typedef struct {
    uint16_t version;
    uint8_t volume;
    uint8_t maxVolume;
    bool mute;
} AudioVolumeProperties;

uint8_t AudioVolumeAdjustVolume(const char* objPath, uint8_t volume)
{
    AudioVolumeProperties* props = NULL;

    props = (AudioVolumeProperties*)GetProperties(objPath, AUDIO_VOLUME_INTERFACE);
    if (props) {
        if (volume > props->maxVolume) {
            return props->maxVolume;
        }
    }
    return volume;
}

AJ_Status AudioVolumeCheckVolumeForValidation(const char* objPath, uint8_t volume)
{
    AJ_Status status = AJ_OK;
    AudioVolumeProperties* props = NULL;

    props = (AudioVolumeProperties*)GetProperties(objPath, AUDIO_VOLUME_INTERFACE);
    if (props) {
        if (volume > props->maxVolume) {
            status = AJ_ERR_RANGE;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status CreateAudioVolumeInterface(void** properties)
{
    *properties = malloc(sizeof(AudioVolumeProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(AudioVolumeProperties));

    ((AudioVolumeProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyAudioVolumeInterface(void* properties)
{
    if (properties) {
        AudioVolumeProperties* props = (AudioVolumeProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescOperationAudioVolume[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "y")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint8_t*)val);
        } else if (!strcmp(signature, "b")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(bool*)val);
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

AJ_Status AudioVolumeInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    AudioVolumeProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (AudioVolumeProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "Volume", "y", &(props->volume));
        break;
    case 3 :
        status = EmitPropChanged(busAttachment, objPath, "Mute", "b", &(props->mute));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status AudioVolumeInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    AudioVolumeProperties* props = NULL;
    AudioVolumeListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (AudioVolumeListener*)listener;
    }
    props = (AudioVolumeProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            uint8_t volume;
            if (lt && lt->OnGetVolume) {
                status = lt->OnGetVolume(objPath, &volume);
                if (status == AJ_OK) {
                    props->volume = volume;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->volume);
        }
        break;
    case 2 :
        {
            uint8_t maxVolume;
            if (lt && lt->OnGetMaxVolume) {
                status = lt->OnGetMaxVolume(objPath, &maxVolume);
                if (status == AJ_OK) {
                    props->maxVolume = maxVolume;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->maxVolume);
        }
        break;
    case 3 :
        {
            bool mute;
            if (lt && lt->OnGetMute) {
                status = lt->OnGetMute(objPath, &mute);
                if (status == AJ_OK) {
                    props->mute = mute;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "b", props->mute);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status AudioVolumeInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    AudioVolumeProperties* props = (AudioVolumeProperties*)properties;
    AudioVolumeListener* lt = (AudioVolumeListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        if (!lt->OnSetVolume) {
            status = AJ_ERR_NULL;
        } else {
            uint8_t volume = 0;
            status = AJ_UnmarshalArgs(replyMsg, "y", &volume);

            if (status == AJ_OK) {
                volume = AudioVolumeAdjustVolume(objPath, volume);

                if (status != AJ_OK) {
                    return status;
                }

                status = lt->OnSetVolume(objPath, volume);

                if (status == AJ_OK) {
                    if (props->volume != volume) {
                        props->volume = volume;
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
        if (!lt->OnSetMute) {
            status = AJ_ERR_NULL;
        } else {
            bool mute = 0;
            status = AJ_UnmarshalArgs(replyMsg, "b", &mute);

            if (status == AJ_OK) {
                status = lt->OnSetMute(objPath, mute);

                if (status == AJ_OK) {
                    if (props->mute != mute) {
                        props->mute = mute;
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

AJ_Status Hae_AudioVolumeInterfaceGetVolume(const char* objPath, uint8_t* volume)
{
    AJ_Status status = AJ_OK;
    AudioVolumeProperties* props = NULL;

    if (!volume) {
        return AJ_ERR_INVALID;
    }

    props = (AudioVolumeProperties*)GetProperties(objPath, AUDIO_VOLUME_INTERFACE);
    if (props) {
        *volume = props->volume;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_AudioVolumeInterfaceSetVolume(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t volume)
{
    AJ_Status status = AJ_OK;
    AudioVolumeProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status = AudioVolumeCheckVolumeForValidation(objPath, volume);

    if (status != AJ_OK) {
        return status;
    }

    props = (AudioVolumeProperties*)GetProperties(objPath, AUDIO_VOLUME_INTERFACE);
    if (props) {
        if (props->volume != volume) {
            props->volume = volume;

            status = EmitPropChanged(busAttachment, objPath, "Volume", "y", &(props->volume));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_AudioVolumeInterfaceGetMaxVolume(const char* objPath, uint8_t* maxVolume)
{
    AJ_Status status = AJ_OK;
    AudioVolumeProperties* props = NULL;

    if (!maxVolume) {
        return AJ_ERR_INVALID;
    }

    props = (AudioVolumeProperties*)GetProperties(objPath, AUDIO_VOLUME_INTERFACE);
    if (props) {
        *maxVolume = props->maxVolume;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_AudioVolumeInterfaceSetMaxVolume(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t maxVolume)
{
    AJ_Status status = AJ_OK;
    AudioVolumeProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (AudioVolumeProperties*)GetProperties(objPath, AUDIO_VOLUME_INTERFACE);
    if (props) {
        props->maxVolume = maxVolume;

        status = EmitPropChanged(busAttachment, objPath, "MaxVolume", "y", &(props->maxVolume));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_AudioVolumeInterfaceGetMute(const char* objPath, bool* mute)
{
    AJ_Status status = AJ_OK;
    AudioVolumeProperties* props = NULL;

    if (!mute) {
        return AJ_ERR_INVALID;
    }

    props = (AudioVolumeProperties*)GetProperties(objPath, AUDIO_VOLUME_INTERFACE);
    if (props) {
        *mute = props->mute;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_AudioVolumeInterfaceSetMute(AJ_BusAttachment* busAttachment, const char* objPath, const bool mute)
{
    AJ_Status status = AJ_OK;
    AudioVolumeProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (AudioVolumeProperties*)GetProperties(objPath, AUDIO_VOLUME_INTERFACE);
    if (props) {
        props->mute = mute;

        status = EmitPropChanged(busAttachment, objPath, "Mute", "b", &(props->mute));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
