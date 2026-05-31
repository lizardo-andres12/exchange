/// This file includes the API for the central order book used per
/// instrument traded on this exchange.
#pragma once

#include <array>
#include <iostream>
#include <vector>
#include <deque>
#include <unordered_map>

#include "common.h"

/// @brief Encapsulates all custom container logic.
namespace containers
{
    /// @brief The bad alloc error bit. Use this bit to indicate an instance
    /// of std::bad_alloc was thrown when using a container.
    static constexpr size_t BAD_ALLOC_ERROR = 8;
    /// @brief The undefined enum error bit. Use this bit to indicate a value
    /// was passed as an enum that does not actually belong to the enum.
    static constexpr size_t UNDEFINED_ENUM_ERROR = 9;
    /// @brief The undefined enum error bit. Use this bit to indicate a value
    /// was passed as an enum that does not actually belong to the enum.
    static constexpr size_t ORDER_NOT_FOUND_ERROR = 10;

    /// @brief An order container that behaves like the classic central order
    /// book used by many exchanges.
    class OrderBook
    {
    public:
        /// @brief Type alias for the price-specific order queue.
        using PriceLevelQueue = std::deque<Order>;
        /// @brief Type alias for the side-specific price level storage structure.
        using PriceLevelsStructure = std::vector<std::pair<Price, PriceLevelQueue>>;

        /// @brief Default order book constructor. Creates an order book with
        /// DEFAULT_RESERVE_COUNT many price levels reserved on both sides.
        explicit OrderBook() : OrderBook(DEFAULT_RESERVE_COUNT) {}

        /// @brief Orderbook size constructor. Creates an order book with
        /// reserve_count many price levels reserved on both sides.
        /// @param reserve_count The number of price levels to reserve on both
        /// bid and ask sides.
        explicit OrderBook(const size_t reserve_count)
            : price_levels_()
        {
            price_levels_[static_cast<size_t>(Side::Bid)].reserve(reserve_count);
            price_levels_[static_cast<size_t>(Side::Ask)].reserve(reserve_count);
        }

        /// @brief Inserts a new order in the corresponding side of the order book.
        /// This method may allocate memory if the price level is out of the reserved
        /// range.
        /// @param error The status bitset used to indicate an error occured.
        /// @param order The order to insert into the book
        void insert(ErrStatus &error, const Order &order) noexcept;

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

        void debug() const noexcept
        {
            for (const auto &pls : price_levels_)
            {
                for (const auto &[price, plq] : pls)
                {
                    std::cout << price << ' ';
                    for (const auto &o : plq)
                    {
                        std::cout << o.qty << ' ';
                    }
                    std::cout << '\n';
                }
            }

            for (const auto &[id, order] : id_order_map_)
            {
                std::cout << id << ", " << order->price << '\n';
            }
        }

        /// @brief Getter method for the order ID to order metadata map.
        /// @return A const reference to the object's map.
        const std::unordered_map<ID, PriceLevelQueue::iterator>&
        get_order_map() const noexcept {
            return id_order_map_;
        }

        /// @brief Getter method for the array of bid/ask price level store.
        /// @return A const reference to the object's price level stores.
        const std::array<PriceLevelsStructure, 2>&
        get_price_levels() const noexcept {
            return price_levels_;
        }

    private:
        /// @brief Maps order ID to Order metadata. This will be the main store
        /// for order status.
        std::unordered_map<ID, PriceLevelQueue::iterator> id_order_map_;

        /// @brief Array of price level structures that can be indexed by side enum. Array indices contain
        /// a vector of price to price level stores sorted by price in ascending/descending order (worst bid/ask is
        /// at index 0, best bid/ask is at last index). Use rbegin() as the best bid/ask iterator.
        std::array<PriceLevelsStructure, 2> price_levels_;

        /// @brief The default number of price levels to reserve.
        static constexpr size_t DEFAULT_RESERVE_COUNT = 50uz;

        /// @brief Private helper method that returns a reverse iterator
        /// to the price level queue for the price or an iterator that is one
        /// past the correct location for the price level queue.
        ///
        /// This method is marked noexcept because all exceptions should be caught
        /// inside the function and should set the appropriate error bits.
        /// @param price The price of the price level to find.
        /// @param pl the side-specific price level structure (bid side or ask side).
        /// @return A reverse iterator pointing to the correct location of the price
        /// level.
        PriceLevelsStructure::reverse_iterator
        __find_price_level_disptacher(const Price price,
                                      PriceLevelsStructure &pl, const Side side) const noexcept;

        /// @brief Private helper for searching bid price level store.
        /// @param price The price of the price level to find.
        /// @param pl the side-specific price level structure (bid side or ask side).
        /// @return A reverse iterator pointing to the correct location of the price
        /// level.
        PriceLevelsStructure::reverse_iterator
        __find_price_level_bid(const Price price,
                               PriceLevelsStructure &pl) const noexcept;

        /// @brief Private helper for searching ask price level store.
        /// @param price The price of the price level to find.
        /// @param pl the side-specific price level structure (bid side or ask side).
        /// @return A reverse iterator pointing to the correct location of the price
        /// level.
        PriceLevelsStructure::reverse_iterator
        __find_price_level_ask(const Price price,
                               PriceLevelsStructure &pl) const noexcept;

        /// @brief Private helper method to add an order to the price level
        /// queue for the correct price based on order.
        /// @param error The status bitset used to indicate an error occured.
        /// @param plq The price level queue `order` should be added to.
        /// @param order The order to add to `plq`.
        void __add_order(ErrStatus &error,
                         PriceLevelQueue &plq, const Order &order) noexcept;
    };
};