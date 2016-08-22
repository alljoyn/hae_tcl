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

#ifndef CURRENTAIRQUALITYLEVEL_H_
#define CURRENTAIRQUALITYLEVEL_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/cdm/CdmControllee.h>

/**
 * CurrentAirQualityLevel interface listener
 */
typedef struct {
    /**
     * Handler for getting ContaminantType property
     * @param[in] objPath object path
     * @param[out] contaminantType the contaminant type
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetContaminantType) (const char* objPath, uint8_t* contaminantType);

    /**
     * Handler for getting CurrentLevel property
     * @param[in] objPath object path
     * @param[out] currentLevel the current value
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetCurrentLevel) (const char* objPath, uint8_t* currentLevel);

    /**
     * Handler for getting MaxLevel property
     * @param[in] objPath object path
     * @param[out] maxLevel the max value
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetMaxLevel) (const char* objPath, uint8_t* maxLevel);

} CurrentAirQualityLevelListener;

/**
 * Get the contaminant type of CurrentAirQualityLevel interface
 * @param[in] objPath the object path including the interface
 * @param[out] contaminantType the contaminant type
 * @return AJ_OK on success
 */
AJ_Status Cdm_CurrentAirQualityLevelInterfaceGetContaminantType(const char* objPath, uint8_t* contaminantType);

/**
 * Set the contaminant type of CurrentAirQualityLevel interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] contaminantType the contaminant type
 * @return AJ_OK on success
 */
AJ_Status Cdm_CurrentAirQualityLevelInterfaceSetContaminantType(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t contaminantType);

/**
 * Get the current value of CurrentAirQualityLevel interface
 * @param[in] objPath the object path including the interface
 * @param[out] currentLevel the current value
 * @return AJ_OK on success
 */
AJ_Status Cdm_CurrentAirQualityLevelInterfaceGetCurrentLevel(const char* objPath, uint8_t* currentLevel);

/**
 * Set the current value of CurrentAirQualityLevel interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] currentLevel the current value
 * @return AJ_OK on success
 */
AJ_Status Cdm_CurrentAirQualityLevelInterfaceSetCurrentLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t currentLevel);

/**
 * Get the maximum value of CurrentAirQualityLevel interface
 * @param[in] objPath the object path including the interface
 * @param[out] maxLevel the maximum value
 * @return AJ_OK on success
 */
AJ_Status Cdm_CurrentAirQualityLevelInterfaceGetMaxLevel(const char* objPath, uint8_t* maxLevel);

/**
 * Set the maximum value of CurrentAirQualityLevel interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] maxLevel the maximum value
 * @return AJ_OK on success
 */
AJ_Status Cdm_CurrentAirQualityLevelInterfaceSetMaxLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t maxLevel);

#endif /* CURRENTAIRQUALITYLEVEL_H_*/
