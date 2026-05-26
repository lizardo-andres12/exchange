#include <cassert>
#include <iostream>
#include <unordered_map>

#include "containers/order_book.h"
#include "status_parser.h"

static std::unordered_map<Side, std::string> side_to_str {
    {Side::Bid, "Side::Bid"},
    {Side::Ask, "Side::Ask"}
};
static std::unordered_map<OrderType, std::string> type_to_str {
    {OrderType::Market, "OrderType::Market"},
    {OrderType::Limit, "OrderType::Limit"},
};
static std::unordered_map<TimeInForce, std::string> tif_to_str {
    {TimeInForce::GoodTilCancel, "TimeInForce::GoodTilCancel"},
    {TimeInForce::ImmOrCancel, "TimeInForce::ImmOrCancel"},
    {TimeInForce::FillOrKill, "TimeInForce::FillOrKill"},
};

int main() {
    ErrStatus err;
    containers::OrderBook ob;
    Order o;
    o.price = 1000;
    o.qty = 10;

    ob.insert(err, o);
    ob.debug();

    o.qty = 20;
    ob.insert(err, o);
    ob.debug();
    assert(err.count() == 0);

    o.flags.set(statparse::SIDE_BIT);
    ob.insert(err, o);
    ob.debug();
    assert(err.count() == 0);

    StatusFlags st; // 1 0 10 100
                    // active bid market gtc
    st.set(statparse::ACTIVE_BIT);
    st.set(statparse::TIF_BIT_LO);

    std::cout << (statparse::order_is_active(st) ? "Active" : "Inactive") << '\n';
    std::cout << side_to_str[statparse::order_side(st)] << '\n';
    std::cout << type_to_str[statparse::order_type(err, st)] << '\n';
    std::cout << tif_to_str[statparse::order_time_in_force(err, st)] << '\n';

    assert(err.count() == 0);
    return 0;
}