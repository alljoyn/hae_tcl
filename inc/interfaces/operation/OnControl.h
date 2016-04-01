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

#ifndef ONCONTROL_H_
#define ONCONTROL_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>
#include <ajtcl/hae/interfaces/HaeInterfaceErrors.h>

/**
 * OnControl interface listener
 */
typedef struct {
    /**
     * Handler for SwitchOn method
     * @param[in] objPath object path
     * @param[out] errorCode error code
     * @return AJ_OK on success
     */
    AJ_Status (*OnSwitchOn) (const char* objPath, ErrorCode* errorCode);

} OnControlListener;

#endif /* ONCONTROL_H_ */
