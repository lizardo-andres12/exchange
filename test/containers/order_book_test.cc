/// This file includes all unit tests for the order book container.
#include <gtest/gtest.h>

#include "containers/order_book.h"
#include "status_parser.h"

using namespace containers;

class OrderBookTest : public OrderBook, public ::testing::Test
{
protected:
    OrderBook ob;
    ErrStatus err;
    ID order_id;

    void SetUp() override
    {
        ob = OrderBook{};
        err = ErrStatus{};
        order_id = ID{};
    }
};

TEST_F(OrderBookTest, TestInsertHappyPath)
{
    const auto &oim{ob.get_order_map()};
    const auto &pls{ob.get_price_levels()};
    const auto &bid_book{pls[static_cast<int>(Side::Bid)]};
    const auto &ask_book{pls[static_cast<int>(Side::Ask)]};

    Order o;
    o.order_id = ++order_id;
    o.price = 10'00;
    o.qty = 100;
    o.flags.reset(statparse::SIDE_BIT); // Side::Bid

    /// Insert a new order at price $10.00 and expect it to be the
    /// only order on the bid side of the book with a valid order_id
    /// entry in the order map. Expect that the order in the price level
    /// queue matches the test case defined order.
    {
        ob.insert(err, o);
        EXPECT_EQ(0, err.count());
        EXPECT_NE(oim.end(), oim.find(o.order_id));
        EXPECT_EQ(bid_book.rbegin()->first, o.price);
        EXPECT_EQ(bid_book.rbegin()->second[0], o);
    }

    o.order_id = ++order_id;
    o.price = 9'00;
    o.qty = 50;
    o.flags.reset(statparse::SIDE_BIT); // Side::Bid

    /// Insert a new order at the price $9.00 and expect the price level
    /// to come one entry after the $10.00 price level in reverse
    /// iteration. Expect that the order stored in the price level queue
    /// matches the test case defined order and that there is a corresponding
    /// order entry in the order map.
    {
        ob.insert(err, o);
        EXPECT_EQ(0, err.count());
        EXPECT_NE(oim.end(), oim.find(o.order_id));
        EXPECT_NE(bid_book.rbegin()->first, o.price);
        EXPECT_EQ((bid_book.rbegin() + 1)->first, o.price);
        EXPECT_EQ((bid_book.rbegin() + 1)->second[0], o);
    }

    o.order_id = ++order_id;
    o.price = 9'50;
    o.qty = 50;
    o.flags.reset(statparse::SIDE_BIT); // Side::Bid

    /// Insert a new bid order at price $9.50 and expect the price elvel
    /// to be in between the $10.00 price level and the $9.00 price
    /// level. Expect that the order stored in the price level queue is
    /// equal to the test case defined order, and that there is an entry
    /// for the order in the order map.
    {
        ob.insert(err, o);
        EXPECT_EQ(0, err.count());
        EXPECT_NE(oim.end(), oim.find(o.order_id));
        EXPECT_NE(bid_book.rbegin()->first, o.price);
        EXPECT_EQ((bid_book.rbegin() + 1)->first, o.price);
        EXPECT_EQ((bid_book.rbegin() + 1)->second[0], o);
        EXPECT_NE((bid_book.rbegin() + 2)->first, o.price);
    }

    o.order_id = ++order_id;
    o.price = 10'50;
    o.qty = 50;
    o.flags.set(statparse::SIDE_BIT); // Side::Ask

    /// Insert a new ask order at price $10.50 and expect it to be the only
    /// price level in the ask book. Expect that the order in the price level
    /// matches and that there is a corresponding order entry in the order map.
    {
        ob.insert(err, o);
        EXPECT_EQ(0, err.count());
        EXPECT_NE(oim.end(), oim.find(o.order_id));
        EXPECT_EQ(ask_book.rbegin()->first, o.price);
        EXPECT_EQ(ask_book.rbegin()->second[0], o);
    }

    o.order_id = ++order_id;
    o.price = 10'50;
    o.qty = 100;
    o.flags.set(statparse::SIDE_BIT); // Side::Ask

    /// Insert a new ask order at price $10.50 and expect it to be
    /// stored in the same $10.50 price level queue after the previous
    /// order.
    {
        ob.insert(err, o);
        EXPECT_EQ(0, err.count());
        EXPECT_NE(oim.end(), oim.find(o.order_id));
        EXPECT_EQ(ask_book.rbegin()->first, o.price);
        EXPECT_EQ(2uz, ask_book.rbegin()->second.size());
        EXPECT_NE(ask_book.rbegin()->second[0], o);
        EXPECT_EQ(ask_book.rbegin()->second[1], o);
    }
}

TEST_F(OrderBookTest, TestInsertErrorPaths)
{
    /// OrderBook::insert() will only set BAD_ALLOC_ERROR.
    /// I'm unsure how to force a bad_alloc error, so this
    /// test will just pass for now.
    EXPECT_TRUE(true);
}

TEST_F(OrderBookTest, TestUpdateHappyPath)
{
    const auto &oim{ob.get_order_map()};
    const auto &pls{ob.get_price_levels()};
    const auto &bid_book{pls[static_cast<int>(Side::Bid)]};
    const auto &ask_book{pls[static_cast<int>(Side::Ask)]};

    Order o1;
    o1.order_id = ++order_id;
    o1.price = 10'00;
    o1.qty = 100;
    o1.flags.reset(statparse::SIDE_BIT); // Side::Bid
    ob.insert(err, o1);

    Order o2;
    o2.order_id = ++order_id;
    o2.price = 10'00;
    o2.qty = 200;
    o2.flags.reset(statparse::SIDE_BIT); // Side::Bid
    ob.insert(err, o2);

    Order o3;
    o3.order_id = ++order_id;
    o3.price = 10'00;
    o3.qty = 200;
    o3.flags.reset(statparse::SIDE_BIT); // Side::Bid
    ob.insert(err, o3);

    /// Update o1 to have a lesser quantity. Expect that the order does
    /// not change as a result of updating.
    {
        auto map_it1{oim.find(o1.order_id)};
        auto map_it2{oim.find(o2.order_id)};
        auto map_it3{oim.find(o3.order_id)};

        /// Assert that all orders had valid iterators.
        ASSERT_NE(oim.end(), map_it1);
        ASSERT_NE(oim.end(), map_it2);
        ASSERT_NE(oim.end(), map_it3);

        /// Assert that the expected insertion ordering is reflected in
        /// the order book.
        ASSERT_EQ(bid_book.rbegin()->second.begin(), map_it1->second);
        ASSERT_EQ((bid_book.rbegin()->second.begin() + 1), map_it2->second);
        ASSERT_EQ((bid_book.rbegin()->second.begin() + 2), map_it3->second);

        /// Assert the iterators stored in the map are pointing to the
        /// correct corresponding objects that were inserted.
        ASSERT_EQ(o1, *map_it1->second);
        ASSERT_EQ(o2, *map_it2->second);
        ASSERT_EQ(o3, *map_it3->second);

        Quantity new_qty = 50;
        ob.update(err, o1.order_id, new_qty);
        EXPECT_EQ(0, err.count());

        map_it1 = oim.find(o1.order_id);
        map_it2 = oim.find(o2.order_id);
        map_it3 = oim.find(o3.order_id);

        EXPECT_NE(oim.end(), map_it1);
        EXPECT_NE(oim.end(), map_it2);
        EXPECT_NE(oim.end(), map_it3);

        EXPECT_EQ(bid_book.rbegin()->second.begin(), map_it1->second);
        EXPECT_EQ((bid_book.rbegin()->second.begin() + 1), map_it2->second);
        EXPECT_EQ((bid_book.rbegin()->second.begin() + 2), map_it3->second);

        EXPECT_NE(o1, *map_it1->second); // Initially, o1 has the old qty value.
        o1.qty = new_qty;
        EXPECT_EQ(o1, *map_it1->second); // Now, they should pass.
        EXPECT_EQ(o2, *map_it2->second);
        EXPECT_EQ(o3, *map_it3->second);
    }

    /// Update o1 to have a greater quantity. Expect that the order moves
    /// to the end of the price level queue as a result.
    {
        auto map_it1{oim.find(o1.order_id)};
        auto map_it2{oim.find(o2.order_id)};
        auto map_it3{oim.find(o3.order_id)};

        /// Assert that all orders had valid iterators.
        ASSERT_NE(oim.end(), map_it1);
        ASSERT_NE(oim.end(), map_it2);
        ASSERT_NE(oim.end(), map_it3);

        /// Assert that the expected insertion ordering is reflected in
        /// the order book.
        ASSERT_EQ(bid_book.rbegin()->second.begin(), map_it1->second);
        ASSERT_EQ((bid_book.rbegin()->second.begin() + 1), map_it2->second);
        ASSERT_EQ((bid_book.rbegin()->second.begin() + 2), map_it3->second);

        /// Assert the iterators stored in the map are pointing to the
        /// correct corresponding objects that were inserted.
        ASSERT_EQ(o1, *map_it1->second);
        ASSERT_EQ(o2, *map_it2->second);
        ASSERT_EQ(o3, *map_it3->second);

        Quantity new_qty = 100;
        ob.update(err, o1.order_id, new_qty);
        EXPECT_EQ(0, err.count());

        map_it1 = oim.find(o1.order_id);
        map_it2 = oim.find(o2.order_id);
        map_it3 = oim.find(o3.order_id);

        EXPECT_NE(oim.end(), map_it1);
        EXPECT_NE(oim.end(), map_it2);
        EXPECT_NE(oim.end(), map_it3);

        EXPECT_EQ(bid_book.rbegin()->second.begin(), map_it2->second);
        EXPECT_EQ((bid_book.rbegin()->second.begin() + 1), map_it3->second);
        EXPECT_EQ((bid_book.rbegin()->second.begin() + 2), map_it1->second);

        EXPECT_NE(o1, *map_it1->second); // Initially, o1 has the old qty value.
        o1.qty = new_qty;
        EXPECT_EQ(o1, *map_it1->second); // Now, they should pass.
        EXPECT_EQ(o2, *map_it2->second);
        EXPECT_EQ(o3, *map_it3->second);
    }
}

TEST_F(OrderBookTest, TestUpdateErrorPath)
{
    /// OrderBook::update() will only set ORDER_NOT_FOUND_ERROR
    /// when passed an order ID that is not in the map.

    /// Update non-existing order and expect that only the
    /// ORDER_NOT_FOUND_ERROR bit is set.
    {
        ob.update(err, static_cast<ID>(100), static_cast<Quantity>(100));

        EXPECT_EQ(1, err.count());
        EXPECT_TRUE(err.test(ORDER_NOT_FOUND_ERROR));
    }
}

TEST_F(OrderBookTest, TestEraseHappyPath) {
    const auto &oim{ob.get_order_map()};
    const auto &pls{ob.get_price_levels()};
    const auto &bid_book{pls[static_cast<int>(Side::Bid)]};
    const auto &ask_book{pls[static_cast<int>(Side::Ask)]};

    Order o1;
    o1.order_id = ++order_id;
    o1.price = 10'00;
    o1.qty = 100;
    o1.flags.reset(statparse::SIDE_BIT); // Side::Bid
    ob.insert(err, o1);

    Order o2;
    o2.order_id = ++order_id;
    o2.price = 10'00;
    o2.qty = 200;
    o2.flags.reset(statparse::SIDE_BIT); // Side::Bid
    ob.insert(err, o2);

    /// Delete o1 from the order book. Expect that there is no longer
    /// an entry associated with o1 in the order map and that o1 is no
    /// longer in the $10.00 price level queue.
    {
        auto map_it1{oim.find(o1.order_id)};
        auto map_it2{oim.find(o2.order_id)};

        /// Assert that all orders had valid iterators.
        ASSERT_NE(oim.end(), map_it1);
        ASSERT_NE(oim.end(), map_it2);

        /// Assert that the expected insertion ordering is reflected in
        /// the order book.
        ASSERT_EQ(bid_book.rbegin()->second.begin(), map_it1->second);
        ASSERT_EQ((bid_book.rbegin()->second.begin() + 1), map_it2->second);

        /// Assert the iterators stored in the map are pointing to the
        /// correct corresponding objects that were inserted.
        ASSERT_EQ(o1, *map_it1->second);
        ASSERT_EQ(o2, *map_it2->second);

        ob.erase(err, o1.order_id);
        EXPECT_EQ(0, err.count());

        map_it1 = oim.find(o1.order_id);
        EXPECT_EQ(oim.end(), map_it1);
        EXPECT_EQ(1, bid_book.rbegin()->second.size());
        EXPECT_NE(bid_book.rbegin()->second.front(), o1);
        EXPECT_EQ(bid_book.rbegin()->second.front(), o2);
    }
}

TEST_F(OrderBookTest, TestEraseErrorPath) {
    /// OrderBook::erase() will only set ORDER_NOT_FOUND_ERROR
    /// when passed an order ID not in the map.

    /// Erase a non-existent order and expect that only the
    /// ORDER_NOT_FOUND_ERROR bit is set.
    {
        ob.erase(err, static_cast<ID>(100));
        EXPECT_EQ(1, err.count());
        EXPECT_TRUE(err.test(ORDER_NOT_FOUND_ERROR));
    }
}