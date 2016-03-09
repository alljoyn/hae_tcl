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

#ifndef REPEATMODE_H_
#define REPEATMODE_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * RepeatMode interface listener
 */
typedef struct {
    /**
     * Handler for getting RepeatMode property
     * @param[in] objPath object path
     * @param[out] repeatMode True if the device works in repeat mode
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetRepeatMode) (const char* objPath, bool* repeatMode);

      /**
     * Handler for setting RepeatMode property
     * @param[in] objPath object path
     * @param[in] repeatMode True if the device works in repeat mode
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetRepeatMode) (const char* objPath, const bool repeatMode);
} RepeatModeListener;

/**
 * Get repeatMode of RepeatMode interface
 * @param[in] objPath the object path including the interface
 * @param[out] repeatMode True if the device works in repeat mode
 * @return AJ_OK on success
 */
AJ_Status Hae_RepeatModeInterfaceGetRepeatMode(const char* objPath, bool* repeatMode);

/**
 * Set repeatMode of RepeatMode interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] repeatMode True if the device works in repeat mode
 * @return AJ_OK on success
 */
AJ_Status Hae_RepeatModeInterfaceSetRepeatMode(AJ_BusAttachment* busAttachment, const char* objPath, const bool repeatMode);

#endif /* REPEATMODE_H_ */
