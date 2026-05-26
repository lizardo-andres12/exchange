/// This file includes all unit tests for the status parser functions.
#include <gtest/gtest.h>

#include "status_parser.h"

using namespace statparse;

/// @brief Test suite fixture that creates new StatusFlags for all tests
/// cases to run logic against.
class StatusParserTest : public ::testing::Test {
protected:
    StatusFlags flags;
    ErrStatus err;

    void SetUp() override {
        flags = StatusFlags{};
        err = ErrStatus{};
    }
};

TEST_F(StatusParserTest, TestFnOrderIsActive) {
    flags.reset(ACTIVE_BIT);
    EXPECT_EQ(false, order_is_active(flags));
    EXPECT_EQ(0, err.count());

    flags.set(ACTIVE_BIT);
    EXPECT_EQ(true, order_is_active(flags));
    EXPECT_EQ(0, err.count());
}

TEST_F(StatusParserTest, TestFnOrderSide) {
    flags.reset(SIDE_BIT);
    EXPECT_EQ(Side::Bid, order_side(flags));
    EXPECT_EQ(0, err.count());

    flags.set(SIDE_BIT);
    EXPECT_EQ(Side::Ask, order_side(flags));
    EXPECT_EQ(0, err.count());
}

TEST_F(StatusParserTest, TestFnOrderType) {
    /// 0: Market, 1: Limit
    /// See STATUS_FLAGS.md for more info
    OrderType type;

    flags.reset(ORDER_BIT_LO);
    flags.reset(ORDER_BIT_HI);
    EXPECT_EQ(OrderType::Market, order_type(err, flags));
    EXPECT_EQ(0, err.count());

    flags.set(ORDER_BIT_LO);
    flags.reset(ORDER_BIT_HI);
    EXPECT_EQ(OrderType::Limit, order_type(err, flags));
    EXPECT_EQ(0, err.count());

    flags.reset(ORDER_BIT_LO);
    flags.set(ORDER_BIT_HI);
    type = order_type(err, flags);
    EXPECT_NE(OrderType::Market, type);
    EXPECT_NE(OrderType::Limit, type);
    EXPECT_TRUE(err.test(STATUS_PARSE_LOGIC_ERROR));

    err.reset(STATUS_PARSE_LOGIC_ERROR);
    flags.set(ORDER_BIT_LO);
    flags.set(ORDER_BIT_HI);
    type = order_type(err, flags);
    EXPECT_NE(OrderType::Market, type);
    EXPECT_NE(OrderType::Limit, type);
    EXPECT_TRUE(err.test(STATUS_PARSE_LOGIC_ERROR));
}

TEST_F(StatusParserTest, TestFnOrderTimeInForce) {
    /// 0: GoodTilCancel, 1: ImmOrCancel, 2: FillOrKill
    /// See STATUS_FLAGS.md for more info
    TimeInForce tif;

    flags.reset(TIF_BIT_LO);
    flags.reset(TIF_BIT_HI);
    EXPECT_EQ(TimeInForce::GoodTilCancel, order_time_in_force(err, flags));
    EXPECT_EQ(0, err.count());

    flags.set(TIF_BIT_LO);
    flags.reset(TIF_BIT_HI);
    EXPECT_EQ(TimeInForce::ImmOrCancel, order_time_in_force(err, flags));
    EXPECT_EQ(0, err.count());

    flags.reset(TIF_BIT_LO);
    flags.set(TIF_BIT_HI);
    EXPECT_EQ(TimeInForce::FillOrKill, order_time_in_force(err, flags));
    EXPECT_EQ(0, err.count());

    flags.set(TIF_BIT_LO);
    flags.set(TIF_BIT_HI);
    tif = order_time_in_force(err, flags);
    EXPECT_NE(TimeInForce::GoodTilCancel, tif);
    EXPECT_NE(TimeInForce::ImmOrCancel, tif);
    EXPECT_NE(TimeInForce::FillOrKill, tif);
    EXPECT_TRUE(err.test(STATUS_PARSE_LOGIC_ERROR));
}