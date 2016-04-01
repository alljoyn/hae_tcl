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

#ifndef ONCONTROLIMPL_H_
#define ONCONTROLIMPL_H_

#include <ajtcl/alljoyn.h>

const char* const intfDescOperationOnControl[4];

AJ_Status CreateOnControlInterface(void** properties);
void DestroyOnControlInterface(void* properties);
AJ_Status OnControlInterfaceOnMethodHandler(AJ_Message* msg, const char* objPath, uint8_t memberIndex, void* listener, HaePropertiesChangedByMethod* propChangedByMethod);

#endif /* ONCONTROLIMPL_H_ */
