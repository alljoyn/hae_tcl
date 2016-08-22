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

#ifndef TARGETHUMIDITY_H_
#define TARGETHUMIDITY_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/cdm/CdmControllee.h>

/**
 * TargetHumidity interface listener
 */
typedef struct {
    /**
     * Handler for getting TargetValue property
     * @param[in] objPath object path
     * @param[out] targetValue the target value
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetTargetValue) (const char* objPath, uint8_t* targetValue);

    /**
     * Handler for setting TargetLevel property
     * @param[in] objPath object path
     * @param[out] targetLevel the target value
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetTargetValue) (const char* objPath, const uint8_t targetValue);

    /**
     * Handler for getting MinValue property
     * @param[in] objPath object path
     * @param[out] minValue the minimum value
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetMinValue) (const char* objPath, uint8_t* minValue);

    /**
     * Handler for getting MaxValue property
     * @param[in] objPath object path
     * @param[out] maxValue the maximum value
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetMaxValue) (const char* objPath, uint8_t* maxValue);

    /**
     * Handler for getting StepValue property
     * @param[in] objPath object path
     * @param[out] stepValue the step value
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetStepValue) (const char* objPath, uint8_t* stepValue);

    /**
     * Handler for getting SelectableHumidityLevels property
     * @param[in] objPath object path
     * @param[out] humidityLevels the selectable humidity levels
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetSelectableHumidityLevels) (const char* objPath, uint8_t* humidityLevels);
} TargetHumidityListener;


/**
 * Get the target value of TargetHumidity interface
 * @param[in] objPath the object path including the interface
 * @param[out] targetValue the target value
 * @return AJ_OK on success
 */
AJ_Status Cdm_TargetHumidityInterfaceGetTargetValue(const char* objPath, uint8_t* targetValue);

/**
 * Set the target value of TargetHumidity interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] targetValue the target value
 * @return AJ_OK on success
 */
AJ_Status Cdm_TargetHumidityInterfaceSetTargetValue(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t targetValue);

/**
 * Get the minimum value of TargetHumidity interface
 * @param[in] objPath the object path including the interface
 * @param[out] minValue the minimum value
 * @return AJ_OK on success
 */
AJ_Status Cdm_TargetHumidityInterfaceGetMinValue(const char* objPath, uint8_t* minValue);

/**
 * Set the minimum value of TargetHumidity interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] minValue the minimum value
 * @return AJ_OK on success
 */
AJ_Status Cdm_TargetHumidityInterfaceSetMinValue(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t minValue);

/**
 * Get the maximum value of TargetHumidity interface
 * @param[in] objPath the object path including the interface
 * @param[out] maxValue the maximum value
 * @return AJ_OK on success
 */
AJ_Status Cdm_TargetHumidityInterfaceGetMaxValue(const char* objPath, uint8_t* maxValue);

/**
 * Set the maximum value of TargetHumidity interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] maxValue the maximum value
 * @return AJ_OK on success
 */
AJ_Status Cdm_TargetHumidityInterfaceSetMaxValue(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t maxValue);

/**
 * Get the step value of TargetHumidity interface
 * @param[in] objPath the object path including the interface
 * @param[out] stepValue the step value
 * @return AJ_OK on success
 */
AJ_Status Cdm_TargetHumidityInterfaceGetStepValue(const char* objPath, uint8_t* stepValue);

/**
 * Set the step value of TargetHumidity interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] stepValue the step value
 * @return AJ_OK on success
 */
AJ_Status Cdm_TargetHumidityInterfaceSetStepValue(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t stepValue);

/**
 * Get the SelectableHumidityLevels of TargetHumidity interface
 * @param[in] objPath the object path including the interface
 * @param[out] humidityLevels the selectable humidity levels
 * @return AJ_OK on success
 */
AJ_Status Cdm_TargetHumidityInterfaceGetSelectableHumidityLevels(const char* objPath, uint8_t* humidityLevel);

/**
 * Set the SelectableHumidityLevels of TargetHumidity interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] humidityLevels the selectable humidity levels
 * @param[in] humidityLevelSize the size of the selectable humidity levels
 * @return AJ_OK on success
 */
AJ_Status Cdm_TargetHumidityInterfaceSetSelectableHumidityLevels(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t* humidityLevels, const size_t humidityLevelSize);

#endif /* TARGETHUMIDITY_H_ */
