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

#ifndef CURRENTHUMIDITY_H_
#define CURRENTHUMIDITY_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * CurrentHumidity interface listener
 */
typedef struct {
    /**
     * Handler for getting CurrentValue property
     * @param[in] objPath object path
     * @param[out] currentValue the current value
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetCurrentValue) (const char* objPath, uint8_t* currentValue);

    /**
     * Handler for getting MaxValue property
     * @param[in] objPath object path
     * @param[out] maxValue the max value
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetMaxValue) (const char* objPath, uint8_t* maxValue);

} CurrentHumidityListener;

/**
 * Get the current value of CurrentHumidity interface
 * @param[in] objPath the object path including the interface
 * @param[out] currentValue the current value
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentHumidityInterfaceGetCurrentValue(const char* objPath, uint8_t* currentValue);

/**
 * Set the current value of CurrentHumidity interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] currentValue the current value
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentHumidityInterfaceSetCurrentValue(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t currentValue);

/**
 * Get the maxValue of CurrentHumidity interface
 * @param[in] objPath the object path including the interface
 * @param[out] maxValue the maxValue
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentHumidityInterfaceGetMaxValue(const char* objPath, uint8_t* maxValue);

/**
 * Set the maxValue of CurrentHumidity interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] maxValue the maxValue
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentHumidityInterfaceSetMaxValue(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t maxValue);

#endif /* CURRENTHUMIDITY_H_ */
