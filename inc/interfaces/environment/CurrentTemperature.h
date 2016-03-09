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

#ifndef CURRENTTEMPERATURE_H_
#define CURRENTTEMPERATURE_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * CurrentTemperature interface listener
 */
typedef struct {
    /**
     * Handler for getting CurrentValue property
     * @param[in] objPath object path
     * @param[out] currentValue the current value
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetCurrentValue) (const char* objPath, double* currentValue);

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

} CurrentTemperatureListener;

/**
 * Get the current value of CurrentTemperature interface
 * @param[in] objPath the object path including the interface
 * @param[out] currentValue the current value
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentTemperatureInterfaceGetCurrentValue(const char* objPath, double* currentValue);

/**
 * Set the current value of CurrentTemperature interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] currentValue the current value
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentTemperatureInterfaceSetCurrentValue(AJ_BusAttachment* busAttachment, const char* objPath, const double currentValue);

/**
 * Get the precision of CurrentTemperature interface
 * @param[in] objPath the object path including the interface
 * @param[out] precision the precision
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentTemperatureInterfaceGetPrecision(const char* objPath, double* precision);

/**
 * Set the precision of CurrentTemperature interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] precision the precision
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentTemperatureInterfaceSetPrecision(AJ_BusAttachment* busAttachment, const char* objPath, const double precision);

/**
 * Get the update minimum time of CurrentTemperature interface
 * @param[in] objPath the object path including the interface
 * @param[out] updateMinTime the update minimum time
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentTemperatureInterfaceGetUpdateMinTime(const char* objPath, uint16_t* updateMinTime);

/**
 * Set the update minimum time of CurrentTemperature interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] updateMinTime the update minimum time
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentTemperatureInterfaceSetUpdateMinTime(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t updateMinTime);

#endif /* CURRENTTEMPERATURE_H_ */
