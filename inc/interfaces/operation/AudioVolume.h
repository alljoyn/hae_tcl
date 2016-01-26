/**
 * @file
 */
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

#ifndef AUDIOVOLUME_H_
#define AUDIOVOLUME_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * AudioVolume interface listener
 */
typedef struct {
    /**
     * Handler for getting Volume property
     * @param[in] objPath object path
     * @param[out] volume volume
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetVolume) (const char* objPath, uint8_t* volume);

    /**
     * Handler for setting Volume property
     * @param[in] objPath object path
     * @param[in] volume volume
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetVolume) (const char* objPath, uint8_t* volume);

    /**
     * Handler for getting MaxVolume property
     * @param[in] objPath object path
     * @param[out] maxVolume the maximum volume
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetMaxVolume) (const char* objPath, uint8_t* maxVolume);

    /**
     * Handler for getting Mute property
     * @param[in] objPath object path
     * @param[out] mute mute
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetMute) (const char* objPath, bool* mute);

    /**
     * Handler for setting Mute property
     * @param[in] objPath object path
     * @param[in] mute mute
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetMute) (const char* objPath, bool* mute);
} AudioVolumeListener;

/**
 * Get volume of AudioVolume interface
 * @param[in] objPath the object path including the interface
 * @param[out] volume volume
 * @return AJ_OK on success
 */
AJ_Status Hae_AudioVolumeInterfaceGetVolume(const char* objPath, uint8_t* volume);

/**
 * Set volume of AudioVolume interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] volume volume
 * @return AJ_OK on success
 */
AJ_Status Hae_AudioVolumeInterfaceSetVolume(AJ_BusAttachment* busAttachment, const char* objPath, uint8_t* volume);

/**
 * Get the maximum volume of AudioVolume interface
 * @param[in] objPath the object path including the interface
 * @param[out] maxVolume the maximum volume
 * @return AJ_OK on success
 */
AJ_Status Hae_AudioVolumeInterfaceGetMaxVolume(const char* objPath, uint8_t* maxVolume);

/**
 * Set the maximum volume of AudioVolume interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] maxVolume the maximum volume
 * @return AJ_OK on success
 */
AJ_Status Hae_AudioVolumeInterfaceSetMaxVolume(AJ_BusAttachment* busAttachment, const char* objPath, uint8_t* maxVolume);

/**
 * Get mute of AudioVolume interface
 * @param[in] objPath the object path including the interface
 * @param[out] mute mute
 * @return AJ_OK on success
 */
AJ_Status Hae_AudioVolumeInterfaceGetMute(const char* objPath, bool* mute);

/**
 * Set mute of AudioVolume interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] mute mute
 * @return AJ_OK on success
 */
AJ_Status Hae_AudioVolumeInterfaceSetMute(AJ_BusAttachment* busAttachment, const char* objPath, bool* mute);

#endif /* AUDIOVOLUME_H_ */
