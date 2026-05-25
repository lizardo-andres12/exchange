/// This file includes the commonly used types, structs, and constants
/// for all components of the exchange. Extend this file as needed.
#pragma once

#include <bitset>
#include <cstdint>
#include <string_view>

/// @brief The number of bits used in the Status Flag Standard.
static constexpr size_t NUM_STATUS_BITS = 32uz;

/// @brief The number of bits used in error bitsets.
static constexpr size_t NUM_ERROR_BITS = 32uz;

/// @brief Nanoseconds sice UNIX epoch.
using Timestamp = uint64_t;

/// @brief The price in cent standardization e.g. ($1.00 = Price{100})
using Price = int64_t;

/// @brief The minimum price change for all instruments.
/// TODO: Change tick size to be instrumet specific.
static constexpr Price TICK_SIZE = static_cast<Price>(25);

/// @brief The quantity of shares.
using Quantity = uint32_t;

/// @brief The number of shares traded for every one quantity.
static constexpr Quantity ROUND_LOT = static_cast<Quantity>(100);

/// @brief The symbol of the instrument traded. This is a non-owning
/// type, ensure that the symbol stores a stable reference to the
/// symbol representation
using Symbol = std::string_view; /// Store known symbols in the symbol table

/// @brief The error status bitset used to indicate something went wrong.
using ErrStatus = std::bitset<NUM_ERROR_BITS>;

/// @brief Status bits per order. This should only be parsed using the dedicated
/// parser and should adhere to the standard defined by STATUS_FLAGS.md.
/// Information like isActive, side, and time in force are stored here.
using StatusFlags = std::bitset<NUM_STATUS_BITS>;

/// @brief The ID type to use for clients and orders.
using ID = uint32_t;

/// @brief Represents the side of the book the order acts in.
enum class Side : uint8_t
{
    Bid = 0,
    Ask
};

/// @brief Represents the order's matching behavior.
enum class OrderType : uint8_t
{
    Market = 0,
    Limit
    // ... (more support soon, starting with theseb)
};

/// @brief Represents the validity of the order (i.e. when to cancel it)
enum class TimeInForce : uint8_t
{
    GoodTilCancel,
    ImmOrCancel,
    FillOrKill
};

/// @brief POD type for an order that comes into the exchange. This is the order to
/// be matched against
struct Order
{
    Timestamp ts;      // 64
    Price price;       // 64
    Quantity qty;      // 32
    StatusFlags flags; // 32
    ID orderId;        // 32
    ID userId;         // 32
}; // 32 bytes, 2 per cache line, 4 for 2 line hardware prefetch.

/// @brief A pair of symbol and corresponding order. Since orders do not
/// store symbols, these are used as arugments to the matching engine.
using TaggedOrder = std::pair<Symbol, Order>;