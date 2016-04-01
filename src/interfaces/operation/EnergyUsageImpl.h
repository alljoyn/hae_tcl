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

#ifndef ENERGYUSAGEIMPL_H_
#define ENERGYUSAGEIMPL_H_

#include <ajtcl/alljoyn.h>

const char* const intfDescOperationEnergyUsage[7];

AJ_Status CreateEnergyUsageInterface(void** properties);
void DestroyEnergyUsageInterface(void* properties);
AJ_Status EnergyUsageInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener);
AJ_Status EnergyUsageInterfaceOnMethodHandler(AJ_Message* msg, const char* objPath, uint8_t memberIndex, void* listener, HaePropertiesChangedByMethod* propChangedByMethod);
AJ_Status EnergyUsageInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex);

#endif /* ENERGYUSAGEIMPL_H_ */
