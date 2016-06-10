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

#ifndef TARGETTEMPERATURELEVEL_H_
#define TARGETTEMPERATURELEVEL_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * TargetTemperatureLevel interface listener
 */
typedef struct {
    /**
     * Handler for getting TargetLevel property
     * @param[in] objPath object path
     * @param[out] targetLevel the target value
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetTargetLevel) (const char* objPath, uint8_t* targetLevel);

    /**
     * Handler for setting TargetLevel property
     * @param[in] objPath object path
     * @param[out] targetLevel the target value
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetTargetLevel) (const char* objPath, const uint8_t targetLevel);

    /**
     * Handler for getting MaxLevel property
     * @param[in] objPath object path
     * @param[out] maxLevel the maximum value
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetMaxLevel) (const char* objPath, uint8_t* maxLevel);

    /**
     * Handler for getting StepValue property
     * @param[in] objPath object path
     * @param[out] stepValue the step value
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetStepValue) (const char* objPath, uint8_t* stepValue);

    /**
     * Handler for getting SelectableTemperatureLevels property
     * @param[in] objPath object path
     * @param[out] temperatureLevels the selectable temperatureLevel levels
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetSelectableTemperatureLevels) (const char* objPath, uint8_t* temperatureLevels);
} TargetTemperatureLevelListener;


/**
 * Get the target value of TargetTemperatureLevel interface
 * @param[in] objPath the object path including the interface
 * @param[out] targetLevel the target value
 * @return AJ_OK on success
 */
AJ_Status Hae_TargetTemperatureLevelInterfaceGetTargetLevel(const char* objPath, uint8_t* targetLevel);

/**
 * Set the target value of TargetTemperatureLevel interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] targetLevel the target value
 * @return AJ_OK on success
 */
AJ_Status Hae_TargetTemperatureLevelInterfaceSetTargetLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t targetLevel);

/**
 * Get the maximum value of TargetTemperatureLevel interface
 * @param[in] objPath the object path including the interface
 * @param[out] maxLevel the maximum value
 * @return AJ_OK on success
 */
AJ_Status Hae_TargetTemperatureLevelInterfaceGetMaxLevel(const char* objPath, uint8_t* maxLevel);

/**
 * Set the maximum value of TargetTemperatureLevel interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] maxLevel the maximum value
 * @return AJ_OK on success
 */
AJ_Status Hae_TargetTemperatureLevelInterfaceSetMaxLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t maxLevel);

/**
 * Get the SelectableTemperatureLevels of TargetTemperatureLevel interface
 * @param[in] objPath the object path including the interface
 * @param[out] temperatureLevels the selectable temperature levels
 * @return AJ_OK on success
 */
AJ_Status Hae_TargetTemperatureLevelInterfaceGetSelectableTemperatureLevels(const char* objPath, uint8_t* temperatureLevelLevel);

/**
 * Set the SelectableTemperatureLevels of TargetTemperatureLevel interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] temperatureLevels the selectable temperatureLevel levels
 * @param[in] temperatureLevelSize the size of the selectable temperature levels
 * @return AJ_OK on success
 */
AJ_Status Hae_TargetTemperatureLevelInterfaceSetSelectableTemperatureLevels(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t* temperatureLevels, const size_t temperatureLevelSize);

#endif /* TARGETTEMPERATURELEVEL_H_ */
