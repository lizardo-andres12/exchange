/// This file contains the standard-compliant status parser function
/// definitions and constants.
#pragma once

#include "common.h"

/// @brief Encapsulates all order status flag parsing logic.
namespace statparse {
    /// @brief The flag parser version to use.
    static constexpr int FLAGS_VERSION = 1;

    /// @brief The flag parse logic error bit. Use this bit to indicate
    /// no flag was set or too many flags were set.
    static constexpr size_t STATUS_PARSE_LOGIC_ERROR = 0;

    /// @brief The bit index of the is active flag.
    static constexpr size_t ACTIVE_BIT = 0uz;

    /// @brief The bit index of the side flag.
    static constexpr size_t SIDE_BIT = 1uz;
    
    /// @brief The lower bit index of the order flags.
    static constexpr size_t ORDER_BIT_LO = 2uz;
    /// @brief The higher bit index of the order flags.
    static constexpr size_t ORDER_BIT_HI = 3uz;
    /// @brief The upper bound of order type value.
    static constexpr int MAX_ORDER_VAL = 1;


    /// @brief The lower bit index of the GoodTilCancel time
    /// in force flag.
    static constexpr size_t TIF_BIT_LO = 4uz;
    /// @brief The higher bit index of the ImmediateOrCancel
    /// time in force flags.
    static constexpr size_t TIF_BIT_HI = 5uz;
    /// @brief The upper bound of order type value.
    static constexpr int MAX_TIF_VAL = 2;

    /// @brief Parses the is active flag in the status flags.
    /// @param error The error flags.
    /// @param st The status flags.
    /// @return True if active, false if not.
    static __attribute__((always_inline)) 
    constexpr bool order_is_active(ErrStatus& error, const StatusFlags& st) {
        return st.test(ACTIVE_BIT);
    }

    /// @brief Parses the side of the order from the status flags.
    /// @param error The error flags.
    /// @param st The status flags.
    /// @return The side of the order as an enum.
    static __attribute__((always_inline)) 
    constexpr Side order_side(ErrStatus& error, const StatusFlags& st) {
        return static_cast<Side>(
            static_cast<uint8_t>(st.test(SIDE_BIT))
        );
    }

    /// @brief Parses the type of the order from the status flags.
    /// @param error The error flags.
    /// @param st The status flags.
    /// @return The type of the order as an enum.
    static __attribute__((always_inline)) 
    constexpr OrderType order_type(ErrStatus& error, const StatusFlags& st) {
        int val{ static_cast<int>(st.test(ORDER_BIT_LO))
            + ( static_cast<int>(st.test(ORDER_BIT_HI)) << 1 ) };
        if (val > MAX_TIF_VAL) {
            error.set(STATUS_PARSE_LOGIC_ERROR);
        }
        return static_cast<OrderType>(val);
    }

    /// @brief Parses the time in force of the order fom the status flags.
    /// @param error The error flags.
    /// @param st The status flags.
    /// @return The time in force of the order as an enum.
    static __attribute__((always_inline)) 
    constexpr TimeInForce order_time_in_force(ErrStatus& error, const StatusFlags& st) {
        int val{ static_cast<int>(st.test(TIF_BIT_LO))
            + ( static_cast<int>(st.test(TIF_BIT_HI)) << 1 ) };
        if (val > MAX_TIF_VAL) {
            error.set(STATUS_PARSE_LOGIC_ERROR);
        }
        return static_cast<TimeInForce>(val);
    }
}