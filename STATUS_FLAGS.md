# Status Flag Standard v1

### Overview

This standard defines the way an order's status flags are to be interpreted and set. Status bit parsing depends on the version defined in `include/common.h`. The rest of this doc defines what bits each range of bits represent, how many can be set at a time, and a table of each bit with a description. Any patches to a parser should also include an update to this document with what changed.

Currently, the status flags should have **32 bits** total.

### Bit 0 (Is Active)

This bit represents the active state of an order. This bit should always be checked before making any trades and should be set when an order is no longer valid due to being fulfilled or canceled.

| Value | Description |
| 0 | This order is active with shares remaining to be filled. |
| 1 | This order is inactive either by all shares filled or cancellation. |

### Bit 1 (Side)

This bit represents the side of the order as `Bid` or `Ask`.

| Value | Description |
| 0 | This order is a `Bid` order. |
| 1 | This order is an `Ask` order. |

### Bits 2-3 (Order Type)

These bits represent the type selected for an order. Only one of these bits should be set at any time, and multiple bits set will result in the order being discarded. The possible order types are represented in code via the `OrderType` enum in `include/common.h`. 

| Bit # | Type | Description |
| 2 | Market | This order is a market order that always matches at the best price |
| 3 | Limit | This order is a limit order that always matches at a price at or better the limit price |

### Bits 4-6 (Time In Force)

These bits represent the time in force for an order. Only one of these bits should be set at any time, and multiple bits set will result in the order being discarded. The possible time in force types are represented in code via the `TimeInForce` enum in `include/common.h`.

| Bit # | Type | Description |
| 4 | GoodTilCancel | This order will remain active until the order is canceled by the client or filled |
| 5 | ImmOrCancel | This order will fill as many shares as it can when it is sent (as defined by the order type) and cancel the rest of the shares that cannot be filled. This order never rests on the book. |
| 6 | FillOrKill | This order will either completely fill all shares (as defined by the order type) or cancel all shares and not trade at all. This order never rests on the book. |