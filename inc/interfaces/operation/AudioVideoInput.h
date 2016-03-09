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

#ifndef AUDIOVIDEOINPUT_H_
#define AUDIOVIDEOINPUT_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * InputSource struct
 */
typedef struct  {
    uint16_t id;
    uint16_t sourceType;
    uint8_t detectedState;
    uint16_t portNumber;
    char* friendlyName;
} InputSource;

/**
 * AudioVideoInput interface listener
 */
typedef struct {
    /**
     * Handler for getting inputSourceId property
     * @param[in] objPath object path
     * @param[out] inputSourceId Activated input source id
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetInputSourceId) (const char* objPath, uint16_t* inputSourceId);

    /**
     * Handler for setting inputSourceId property
     * @param[in] objPath object path
     * @param[in] inputSourceId Activated input source id
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetInputSourceId) (const char* objPath, uint16_t inputSourceId);

    /**
     * Handler for getting InputSource property
     * @param[in] objPath object path
     * @param[out] supportedInputSources Array of supported input sources
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetSupportedInputSources) (const char* objPath, InputSource* supportedInputSources);

} AudioVideoInputListener;

/**
 * Get inputSourceId of AudioVideoInput interface
 * @param[in] objPath the object path including the interface
 * @param[out] channelId channel id
 * @return AJ_OK on success
 */
AJ_Status Hae_AudioVideoInputInterfaceGetInputSourceId(const char* objPath, uint16_t* inputSourceId);

/**
 * Set inputSourceId of AudioVideoInput interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] inputSourceId Activated input source id
 * @return AJ_OK on success
 */
AJ_Status Hae_AudioVideoInputInterfaceSetInputSourceId(AJ_BusAttachment* busAttachment, const char* objPath, uint16_t inputSourceId);

/**
 * Get InputSource of AudioVideoInput interface
 * @param[in] objPath the object path including the interface
 * @param[out] supportedInputSources Array of supported input sources
 * @return AJ_OK on success
 */
AJ_Status Hae_AudioVideoInputInterfaceGetSupportedInputSources(const char* objPath, InputSource* supportedInputSources);

/**
 * Set InputSource of AudioVideoInput interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] supportedInputSources Array of supported input sources
 * @param[in] InputSourceSize InputSource List size
 * @return AJ_OK on success
 */
AJ_Status Hae_AudioVideoInputInterfaceSetSupportedInputSources(AJ_BusAttachment* busAttachment, const char* objPath, const InputSource* supportedInputSources, size_t inputSourceListSize);

#endif /* AUDIOVIDEOINPUT_H_ */
