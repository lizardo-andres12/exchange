/// This file contains source implementation for `containers::order_book`.
#include "containers/order_book.h"
#include "status_parser.h"

/// TODO: Figure out a cold-path non-trashing speculation error handler
/// for container usage.

namespace containers
{
    OrderBook::PriceLevelsStructure::reverse_iterator
    OrderBook::__find_price_level_bid(const Price price,
                                      OrderBook::PriceLevelsStructure &pl) const noexcept
    {
        auto rit{pl.rbegin()};
        for (; rit != pl.rend() && rit->first > price; ++rit)
            ;
        return rit;
    }

    OrderBook::PriceLevelsStructure::reverse_iterator
    OrderBook::__find_price_level_ask(const Price price,
                                      OrderBook::PriceLevelsStructure &pl) const noexcept
    {
        auto rit{pl.rbegin()};
        for (; rit != pl.rend() && rit->first < price; ++rit)
            ;
        return rit;
    }

    OrderBook::PriceLevelsStructure::reverse_iterator
    OrderBook::__find_price_level_disptacher(const Price price,
                                              OrderBook::PriceLevelsStructure &pl, const Side side) const noexcept
    {
        switch (side)
        {
        case Side::Bid:
            return __find_price_level_bid(price, pl);
        case Side::Ask:
            return __find_price_level_ask(price, pl);
        }
        /// No default case is needed because side is encoded as a bit.
        /// The statparse::order_side() function can only return 0 or 1
        /// static cast to Side enum, meaning it is impossible for side to
        /// be anything other than Side::Bid or Side::Ask. Thus, this is dead
        /// code, but it is included to silence compiler errors.
        return pl.rend();
    }

    void OrderBook::__add_order(ErrStatus &error,
                                 PriceLevelQueue &plq, const Order &order) noexcept
    {
        try
        {
            plq.push_back(order);
        }
        catch (const std::bad_alloc &e)
        {
            error.set(BAD_ALLOC_ERROR);
            return;
        }

        try
        {
            id_order_map_.insert({order.order_id, plq.end() - 1});
        }
        catch (const std::bad_alloc &e)
        {
            plq.pop_back();
            error.set(BAD_ALLOC_ERROR);
        }
    }

    void OrderBook::insert(ErrStatus &error, const Order &order) noexcept
    {
        Side side{statparse::order_side(order.flags)};
        PriceLevelsStructure &pl{price_levels_[static_cast<size_t>(side)]};
        // rit.first: Price, rit.second: PriceLevelQueue
        auto rit{__find_price_level_disptacher(
            order.price, pl, side)};
        if (error.count() > 0) {
            return;
        }

        if (rit == pl.rend() || rit->first != order.price)
        {
            PriceLevelsStructure::iterator it;
            try
            {
                it = pl.insert(rit.base(), std::make_pair(
                                               order.price, PriceLevelQueue{}));
            }
            catch (const std::bad_alloc &e)
            {
                error.set(BAD_ALLOC_ERROR);
                return;
            }

            __add_order(error, it->second, order);
            return;
        }

        __add_order(error, rit->second, order);
    }

    void OrderBook::update(
        ErrStatus &error, ID order_id, Quantity new_qty) noexcept
    {
        auto it{id_order_map_.find(order_id)};
        if (it == id_order_map_.end()) {
            error.set(ORDER_NOT_FOUND_ERROR);
            return;
        }

        Quantity prev_qty{it->second->qty};
        it->second->qty = new_qty;
        if (new_qty > prev_qty) {
            Order order{*it->second};
            Side side{statparse::order_side(it->second->flags)};
            PriceLevelsStructure &pl{price_levels_[static_cast<size_t>(side)]};
            auto rit{__find_price_level_disptacher(
                it->second->price, pl, side)};

            rit->second.erase(it->second);
            rit->second.push_back(order);
            id_order_map_[order_id] = rit->second.end() - 1;
            return;
        }
    }

    void OrderBook::erase(ErrStatus &error, ID order_id) noexcept {
        auto it{id_order_map_.find(order_id)};
        if (it == id_order_map_.end()) {
            error.set(ORDER_NOT_FOUND_ERROR);
            return;
        }

        Side side{statparse::order_side(it->second->flags)};
        PriceLevelsStructure &pl{price_levels_[static_cast<size_t>(side)]};
        auto rit{__find_price_level_disptacher(
            it->second->price, pl, side)};

        rit->second.erase(it->second);
        id_order_map_.erase(it);
    }
}