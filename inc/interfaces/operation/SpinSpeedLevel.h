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

#ifndef SPINSPEEDLEVEL_H_
#define SPINSPEEDLEVEL_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * SpinSpeedLevel interface listener
 */
typedef struct {
    /**
     * Handler for getting MaxLevel property
     * @param[in] objPath object path
     * @param[out] maxLevel max level
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetMaxLevel) (const char* objPath, uint8_t* maxLevel);

    /**
     * Handler for getting TargetLevel property
     * @param[in] objPath object path
     * @param[out] targetLevel target level
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetTargetLevel) (const char* objPath, uint8_t* targetLevel);

    /**
     * Handler for setting TargetLevel property
     * @param[in] objPath object path
     * @param[in] targetLevel target level
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetTargetLevel) (const char* objPath, const uint8_t targetLevel);

    /**
     * Handler for getting SelectableLevels property
     * @param[in] objPath object path
     * @param[out] selectableLevels selectable levels
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetSelectableLevels) (const char* objPath, uint8_t* selectableLevels);

} SpinSpeedLevelListener;

/**
 * Get MaxLevel of SpinSpeedLevel interface
 * @param[in] objPath the object path including the interface
 * @param[out] maxLevel max level
 * @return AJ_OK on success
 */
AJ_Status Hae_SpinSpeedLevelInterfaceGetMaxLevel(const char* objPath, uint8_t* maxLevel);

/**
 * Set MaxLevel of SpinSpeedLevel interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] maxLevel max level
 * @return AJ_OK on success
 */
AJ_Status Hae_SpinSpeedLevelInterfaceSetMaxLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t maxLevel);

/**
 * Get TargetLevel of SpinSpeedLevel interface
 * @param[in] objPath the object path including the interface
 * @param[out] targetLevel target level
 * @return AJ_OK on success
 */
AJ_Status Hae_SpinSpeedLevelInterfaceGetTargetLevel(const char* objPath, uint8_t* targetLevel);

/**
 * Set TargetLevel of SpinSpeedLevel interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] targetLevel target level
 * @return AJ_OK on success
 */
AJ_Status Hae_SpinSpeedLevelInterfaceSetTargetLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t targetLevel);

/**
 * Get SelectableLevels of SpinSpeedLevel interface
 * @param[in] objPath the object path including the interface
 * @param[out] selectableLevels selectable levels
 * @return AJ_OK on success
 */
AJ_Status Hae_SpinSpeedLevelInterfaceGetSelectableLevels(const char* objPath, uint8_t* selectableLevels);

/**
 * Set SelectableLevels of SpinSpeedLevel interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] selectableLevels selectable levels
 * @param[in] selectableLevelsSize selectable levels size
 * @return AJ_OK on success
 */
AJ_Status Hae_SpinSpeedLevelInterfaceSetSelectableLevels(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t* selectableLevels, const size_t selectableLevelsSize);


#endif /* SPINSPEEDLEVEL_H_ */
