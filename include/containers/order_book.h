/// This file includes the API for the central order book used per
/// instrument traded on this exchange.
#pragma once

#include <vector>
#include <deque>
#include <unordered_map>

#include "common.h"

/// @brief Encapsulates all custom container logic.
namespace containers {
    /// @brief An order container that behaves like the classic central order
    /// book used by many exchanges.
    class order_book {
    public:
        /// @brief Default order book constructor. Creates an order book with
        /// DEFAULT_RESERVE_COUNT many price levels reserved on both sides.
        explicit order_book() : order_book(DEFAULT_RESERVE_COUNT) {}

        /// @brief Orderbook size constructor. Creates an order book with
        /// reserve_count many price levels reserved on both sides.
        /// @param reserve_count The number of price levels to reserve on both
        /// bid and ask sides.
        explicit order_book(const size_t reserve_count)
            : bids_(reserve_count), asks_(reserve_count) {}

        /// @brief Inserts a new order in the corresponding side of the order book.
        /// This method may allocate memory if the price level is out of the reserved
        /// range.
        /// @param error The status bitset used to indicate an error occured.
        /// @param order The order to insert into the book
        void insert(ErrStatus &error, const Order &order);

        /// @brief Updates the quantity of the order keyed by order_id to contain the new quantity
        /// and shifts the ordering if the new quantity is greater than the original quantity.
        /// @param error The status bitset used to indicate an error occured.
        /// @param order_id The ID of the order to update.
        /// @param new_qty The new quantity to set the order to. If this value is greater than the
        /// current quantity, the order will be moved to the back of the ordering queue.
        void update(ErrStatus &error, ID order_id, Quantity new_qty) noexcept;

        /// @brief Deletes the order from the book.
        /// @param error The status bitset used to indicate an error occured.
        /// @param order_id The ID of the order to delete.
        void erase(ErrStatus &error, ID order_id) noexcept;

        order_book(const order_book &other) = delete;
        order_book(order_book &&other) = delete;
        order_book &operator=(const order_book &other) = delete;
        order_book &operator=(order_book &&other) = delete;

    private:
        /// @brief Vector of price to price level stores sorted by price in ascending order (worst bid is
        /// at index 0, best bid is at last index). Use rbegin() as the best bid iterator.
        std::vector<std::pair<Price, std::deque<Order>>> bids_;

        /// @brief Vector of price to price level stores sorted by price in descending order (worst ask is
        /// at index 0, best ask is at last index). Use rbegin() as the best ask iterator.
        std::vector<std::pair<Price, std::deque<Order>>> asks_;

        /// @brief Maps order ID to Order metadata. This will be the main store
        /// for order status.
        std::unordered_map<ID, std::deque<Order>::iterator> id_order_map_;

        /// @brief The default number of price levels to reserve.
        static constexpr size_t DEFAULT_RESERVE_COUNT = 50uz;
    };
};