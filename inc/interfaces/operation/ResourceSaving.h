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

#ifndef RESOURCESSAVING_H_
#define RESOURCESSAVING_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/cdm/CdmControllee.h>

/**
 * ResourceSaving interface listener
 */
typedef struct {
    /**
     * Handler for getting ResourceSavingMode property
     * @param[in] objPath object path
     * @param[out] resourceSavingMode The current resource saving mode of the device
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetResourceSavingMode) (const char* objPath, bool* resourceSavingMode);

    /**
     * Handler for setting ResourceSavingMode property
     * @param[in] objPath object path
     * @param[in] resourceSavingMode The current resource saving mode of the device
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetResourceSavingMode) (const char* objPath, const bool resourceSavingMode);

} ResourceSavingListener;

/**
 * Get resourceSavingMode of ResourceSaving interface
 * @param[in] objPath the object path including the interface
 * @param[out] resourceSavingMode The current resource saving mode of the device
 * @return AJ_OK on success
 */
AJ_Status Cdm_ResourceSavingInterfaceGetResourceSavingMode(const char* objPath, bool* resourceSavingMode);

/**
 * Set resourceSavingMode of Channel interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] resourceSavingMode The current resource saving mode of the device
 * @return AJ_OK on success
 */
AJ_Status Cdm_ResourceSavingInterfaceSetResourceSavingMode(AJ_BusAttachment* busAttachment, const char* objPath, const bool resourceSavingMode);

#endif /* RESOURCESSAVING_H_ */
