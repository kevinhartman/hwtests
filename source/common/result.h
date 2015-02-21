// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

#include "common/bit_field.h"

// All the constants in this file come from http://3dbrew.org/wiki/Error_codes

/// Detailed description of the error. This listing is likely incomplete.
enum class ErrorDescription : u32 {
    Success = 0,
    FS_NotFound = 100,
    FS_NotFormatted = 340, ///< This is used by the FS service when creating a SaveData archive
    InvalidSection = 1000,
    TooLarge = 1001,
    NotAuthorized = 1002,
    AlreadyDone = 1003,
    InvalidSize = 1004,
    InvalidEnumValue = 1005,
    InvalidCombination = 1006,
    NoData = 1007,
    Busy = 1008,
    MisalignedAddress = 1009,
    MisalignedSize = 1010,
    OutOfMemory = 1011,
    NotImplemented = 1012,
    InvalidAddress = 1013,
    InvalidPointer = 1014,
    InvalidHandle = 1015,
    NotInitialized = 1016,
    AlreadyInitialized = 1017,
    NotFound = 1018,
    CancelRequested = 1019,
    AlreadyExists = 1020,
    OutOfRange = 1021,
    Timeout = 1022,
    InvalidResultValue = 1023,
};

/**
 * Identifies the module which caused the error. Error codes can be propagated through a call
 * chain, meaning that this doesn't always correspond to the module where the API call made is
 * contained.
 */
enum class ErrorModule : u32 {
    Common = 0,
    Kernel = 1,
    Util = 2,
    FileServer = 3,
    LoaderServer = 4,
    TCB = 5,
    OS = 6,
    DBG = 7,
    DMNT = 8,
    PDN = 9,
    GX = 10,
    I2C = 11,
    GPIO = 12,
    DD = 13,
    CODEC = 14,
    SPI = 15,
    PXI = 16,
    FS = 17,
    DI = 18,
    HID = 19,
    CAM = 20,
    PI = 21,
    PM = 22,
    PM_LOW = 23,
    FSI = 24,
    SRV = 25,
    NDM = 26,
    NWM = 27,
    SOC = 28,
    LDR = 29,
    ACC = 30,
    RomFS = 31,
    AM = 32,
    HIO = 33,
    Updater = 34,
    MIC = 35,
    FND = 36,
    MP = 37,
    MPWL = 38,
    AC = 39,
    HTTP = 40,
    DSP = 41,
    SND = 42,
    DLP = 43,
    HIO_LOW = 44,
    CSND = 45,
    SSL = 46,
    AM_LOW = 47,
    NEX = 48,
    Friends = 49,
    RDT = 50,
    Applet = 51,
    NIM = 52,
    PTM = 53,
    MIDI = 54,
    MC = 55,
    SWC = 56,
    FatFS = 57,
    NGC = 58,
    CARD = 59,
    CARDNOR = 60,
    SDMC = 61,
    BOSS = 62,
    DBM = 63,
    Config = 64,
    PS = 65,
    CEC = 66,
    IR = 67,
    UDS = 68,
    PL = 69,
    CUP = 70,
    Gyroscope = 71,
    MCU = 72,
    NS = 73,
    News = 74,
    RO_1 = 75,
    GD = 76,
    CardSPI = 77,
    EC = 78,
    RO_2 = 79,
    WebBrowser = 80,
    Test = 81,
    ENC = 82,
    PIA = 83,

    Application = 254,
    InvalidResult = 255
};

/// A less specific error cause.
enum class ErrorSummary : u32 {
    Success = 0,
    NothingHappened = 1,
    WouldBlock = 2,
    OutOfResource = 3,      ///< There are no more kernel resources (memory, table slots) to
                            ///< execute the operation.
    NotFound = 4,           ///< A file or resource was not found.
    InvalidState = 5,
    NotSupported = 6,       ///< The operation is not supported or not implemented.
    InvalidArgument = 7,    ///< Returned when a passed argument is invalid in the current runtime
                            ///< context. (Invalid handle, out-of-bounds pointer or size, etc.)
    WrongArgument = 8,      ///< Returned when a passed argument is in an incorrect format for use
                            ///< with the function. (E.g. Invalid enum value)
    Canceled = 9,
    StatusChanged = 10,
    Internal = 11,

    InvalidResult = 63
};

/// The severity of the error.
enum class ErrorLevel : u32 {
    Success = 0,
    Info = 1,

    Status = 25,
    Temporary = 26,
    Permanent = 27,
    Usage = 28,
    Reinitialize = 29,
    Reset = 30,
    Fatal = 31
};

/// Encapsulates a CTR-OS error code, allowing it to be separated into its constituent fields.
union ResultCode {
    u32 raw;

    BitField<0, 10, ErrorDescription> description;
    BitField<10, 8, ErrorModule> module;

    BitField<21, 6, ErrorSummary> summary;
    BitField<27, 5, ErrorLevel> level;

    // The last bit of `level` is checked by apps and the kernel to determine if a result code is an error
    BitField<31, 1, u32> is_error;

    explicit ResultCode(u32 raw) : raw(raw) {}
    ResultCode(ErrorDescription description_, ErrorModule module_,
            ErrorSummary summary_, ErrorLevel level_) : raw(0) {
        description = description_;
        module = module_;
        summary = summary_;
        level = level_;
    }

    ResultCode& operator=(const ResultCode& o) { raw = o.raw; return *this; }

    bool IsSuccess() const {
        return is_error == 0;
    }

    bool IsError() const {
        return is_error == 1;
    }
};

inline bool operator==(const ResultCode a, const ResultCode b) {
    return a.raw == b.raw;
}

inline bool operator!=(const ResultCode a, const ResultCode b) {
    return a.raw != b.raw;
}

// Convenience functions for creating some common kinds of errors:

/// The default success `ResultCode`.
const ResultCode RESULT_SUCCESS(0);