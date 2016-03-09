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

#ifndef FANSPEEDLEVEL_H_
#define FANSPEEDLEVEL_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * FanSpeedLevel interface listener
 */
typedef struct {
    /**
     * Handler for getting FanSpeedLevel property
     * @param[in] objPath object path
     * @param[out] fanSpeedLevel Fan speed level of a device
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetFanSpeedLevel) (const char* objPath, uint8_t* fanSpeedLevel);

    /**
     * Handler for setting FanSpeedLevel property
     * @param[in] objPath object path
     * @param[in] fanSpeedLevel Fan speed level of a device
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetFanSpeedLevel) (const char* objPath, uint8_t fanSpeedLevel);

    /**
     * Handler for getting MaxFanSpeedLevel property
     * @param[in] objPath object path
     * @param[out] maxFanSpeedLevel target fan speed level
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetMaxFanSpeedLevel) (const char* objPath, uint8_t* maxFanSpeedLevel);

    /**
     * Handler for getting AutoMode property
     * @param[in] objPath object path
     * @param[out] autoMode state of the auto mode
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetAutoMode) (const char* objPath, uint8_t* autoMode);

    /**
     * Handler for setting FanSpeedLevel property
     * @param[in] objPath object path
     * @param[in] autoMode state of the auto mode
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetAutoMode) (const char* objPath, uint8_t autoMode);

} FanSpeedLevelListener;

/**
 * Get fanSpeedLevel of FanSpeedLevel interface
 * @param[in] objPath the object path including the interface
 * @param[out] fanSpeedLevel Fan speed level of a device
 * @return AJ_OK on success
 */
AJ_Status Hae_FanSpeedLevelInterfaceGetFanSpeedLevel(const char* objPath, uint8_t* fanSpeedLevel);

/**
 * Set fanSpeedLevel of FanSpeedLevel interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] fanSpeedLevel Fan speed level of a device
 * @return AJ_OK on success
 */
AJ_Status Hae_FanSpeedLevelInterfaceSetFanSpeedLevel(AJ_BusAttachment* busAttachment, const char* objPath, uint8_t fanSpeedLevel);

/**
 * Get maxFanSpeedLevel of FanSpeedLevel interface
 * @param[in] objPath the object path including the interface
 * @param[out] maxFanSpeedLevel target fan speed level
 * @return AJ_OK on success
 */
AJ_Status Hae_FanSpeedLevelInterfaceGetMaxFanSpeedLevel(const char* objPath, uint8_t* maxFanSpeedLevel);

/**
 * Set maxFanSpeedLevel of FanSpeedLevel interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] maxFanSpeedLevel target fan speed level
 * @return AJ_OK on success
 */
AJ_Status Hae_FanSpeedLevelInterfaceSetMaxFanSpeedLevel(AJ_BusAttachment* busAttachment, const char* objPath, uint8_t maxFanSpeedLevel);


/**
 * Get autoMode of FanSpeedLevel interface
 * @param[in] objPath the object path including the interface
 * @param[out] autoMode state of the auto mode
 * @return AJ_OK on success
 */
AJ_Status Hae_FanSpeedLevelInterfaceGetAutoMode(const char* objPath, uint8_t* autoMode);

/**
 * Set autoMode of FanSpeedLevel interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] autoMode state of the auto mode
 * @return AJ_OK on success
 */
AJ_Status Hae_FanSpeedLevelInterfaceSetAutoMode(AJ_BusAttachment* busAttachment, const char* objPath, uint8_t autoMode);

#endif /* FANSPEEDLEVEL_H_ */
