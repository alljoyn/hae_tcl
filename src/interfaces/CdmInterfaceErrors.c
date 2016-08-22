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

#include <ajtcl/cdm/interfaces/CdmInterfaceErrors.h>

const char* errorName[] = {
    "", //Not Error
    "org.alljoyn.Error.InvalidValue",
    "org.alljoyn.Error.FeatureNotAvailable",
    "org.alljoyn.Error.LanguageNotSupported",
    "org.alljoyn.SmartSpaces.Error.NotAcceptableDueToInternalState",
    "org.alljoyn.SmartSpaces.Error.RemoteControlDisabled",
};

const char* errorMessage[] = {
    "", //Not Error
    "Invalid value.",
    "Feature not available.",
    "The language specified is not supported.",
    "The value is not acceptable due to internal state.",
    "Remote control disabled.",
};

const char* GetInterfaceErrorName(ErrorCode errorCode)
{
    if (errorCode > MAX_ERROR_CODE) {
        return "";
    }

    return errorName[errorCode];
}

const char* GetInterfaceErrorMessage(ErrorCode errorCode)
{
    if (errorCode > MAX_ERROR_CODE) {
        return "";
    }

    return errorMessage[errorCode];
}
