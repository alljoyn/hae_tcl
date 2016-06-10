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

#ifndef CURRENTAIRQUALITY_H_
#define CURRENTAIRQUALITY_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * CurrentAirQuality interface listener
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
     * Handler for getting CurrentValue property
     * @param[in] objPath object path
     * @param[out] currentValue the current value
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetCurrentValue) (const char* objPath, double* currentValue);

    /**
     * Handler for getting MaxValue property
     * @param[in] objPath object path
     * @param[out] maxValue the max value
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetMaxValue) (const char* objPath, double* maxValue);

    /**
     * Handler for getting MinValue property
     * @param[in] objPath object path
     * @param[out] minValue the min value
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetMinValue) (const char* objPath, double* minValue);

    /**
     * Handler for getting Precision property
     * @param[in] objPath object path
     * @param[out] precision the precision
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetPrecision) (const char* objPath, double* precision);

    /**
     * Handler for getting UpdateMinTime property
     * @param[in] objPath object path
     * @param[out] updateMinTime the update minimum time
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetUpdateMinTime) (const char* objPath, uint16_t* updateMinTime);

} CurrentAirQualityListener;

/**
 * Get the contaminant type of CurrentAirQuality interface
 * @param[in] objPath the object path including the interface
 * @param[out] contaminantType the contaminant type
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentAirQualityInterfaceGetContaminantType(const char* objPath, uint8_t* contaminantType);

/**
 * Set the contaminant type of CurrentAirQuality interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] contaminantType the contaminant type
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentAirQualityInterfaceSetContaminantType(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t contaminantType);

/**
 * Get the current value of CurrentAirQuality interface
 * @param[in] objPath the object path including the interface
 * @param[out] currentValue the current value
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentAirQualityInterfaceGetCurrentValue(const char* objPath, double* currentValue);

/**
 * Set the current value of CurrentAirQuality interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] currentValue the current value
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentAirQualityInterfaceSetCurrentValue(AJ_BusAttachment* busAttachment, const char* objPath, const double currentValue);

/**
 * Get the maximum value of CurrentAirQuality interface
 * @param[in] objPath the object path including the interface
 * @param[out] maxValue the maximum value
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentAirQualityInterfaceGetMaxValue(const char* objPath, double* maxValue);

/**
 * Set the maximum value of CurrentAirQuality interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] maxValue the maximum value
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentAirQualityInterfaceSetMaxValue(AJ_BusAttachment* busAttachment, const char* objPath, const double maxValue);

/**
 * Get the minimum value of CurrentAirQuality interface
 * @param[in] objPath the object path including the interface
 * @param[out] minValue the minimum value
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentAirQualityInterfaceGetMinValue(const char* objPath, double* minValue);

/**
 * Set the minimum value of CurrentAirQuality interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] minValue the minimum value
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentAirQualityInterfaceSetMinValue(AJ_BusAttachment* busAttachment, const char* objPath, const double minValue);

/**
 * Get the precision of CurrentAirQuality interface
 * @param[in] objPath the object path including the interface
 * @param[out] precision the precision
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentAirQualityInterfaceGetPrecision(const char* objPath, double* precision);

/**
 * Set the precision of CurrentAirQuality interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] precision the precision
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentAirQualityInterfaceSetPrecision(AJ_BusAttachment* busAttachment, const char* objPath, const double precision);

/**
 * Get the update minimum time of CurrentAirQuality interface
 * @param[in] objPath the object path including the interface
 * @param[out] updateMinTime the update minimum time
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentAirQualityInterfaceGetUpdateMinTime(const char* objPath, uint16_t* updateMinTime);

/**
 * Set the update minimum time of CurrentAirQuality interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] updateMinTime the update minimum time
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentAirQualityInterfaceSetUpdateMinTime(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t updateMinTime);

#endif /* CURRENTAIRQUALITY_H_*/
