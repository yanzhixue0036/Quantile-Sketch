# TPC-C Info

**TPC-C** 是一个模拟在线事务处理系统（OLTP）的基准测试，其中定义了一组表，用于模拟一个商业交易环境。TPC-C 主要由九个表组成，分别是 **仓库（Warehouse）**、**地区（District）**、**客户（Customer）**、**订单（Orders）**、**订单明细（Order-Line）**、**库存（Stock）**、**商品（Item）**、**新订单（New-Order）** 和 **历史（History）**。

### TPC-C 表结构及列解释

#### 1. **仓库表（Warehouse）**
用于存储仓库信息。
- `W_ID`: 仓库ID（主键）
- `W_NAME`: 仓库名称
- `W_STREET_1`, `W_STREET_2`, `W_CITY`, `W_STATE`, `W_ZIP`: 仓库地址信息
- `W_TAX`: 仓库税率
- `W_YTD`: 年初至今的销售额

#### 2. **地区表（District）**
每个仓库分为多个地区。
- `D_ID`: 地区ID（主键，联合 `D_W_ID`）
- `D_W_ID`: 仓库ID（外键，关联到 `Warehouse.W_ID`）
- `D_NAME`: 地区名称
- `D_STREET_1`, `D_STREET_2`, `D_CITY`, `D_STATE`, `D_ZIP`: 地区地址信息
- `D_TAX`: 地区税率
- `D_YTD`: 年初至今的销售额
- `D_NEXT_O_ID`: 下一个订单的ID（用于标识新订单）

#### 3. **客户表（Customer）**
存储客户信息。
- `C_ID`: 客户ID（主键，联合 `C_D_ID` 和 `C_W_ID`）
- `C_D_ID`: 地区ID（外键，关联到 `District.D_ID`）
- `C_W_ID`: 仓库ID（外键，关联到 `Warehouse.W_ID`）
- `C_FIRST`, `C_MIDDLE`, `C_LAST`: 客户的名字
- `C_STREET_1`, `C_STREET_2`, `C_CITY`, `C_STATE`, `C_ZIP`: 客户地址
- `C_PHONE`: 客户电话
- `C_SINCE`: 客户加入日期
- `C_CREDIT`: 信用类型（GC 或 BC）
- `C_CREDIT_LIM`: 信用额度
- `C_DISCOUNT`: 折扣
- `C_BALANCE`: 当前余额
- `C_YTD_PAYMENT`: 年初至今的支付金额
- `C_PAYMENT_CNT`: 支付次数
- `C_DELIVERY_CNT`: 交货次数
- `C_DATA`: 其他信息

#### 4. **历史表（History）**
用于存储客户支付的历史记录。
- `H_C_ID`: 客户ID（外键，关联到 `Customer.C_ID`）
- `H_C_D_ID`: 地区ID（外键，关联到 `Customer.C_D_ID`）
- `H_C_W_ID`: 仓库ID（外键，关联到 `Customer.C_W_ID`）
- `H_D_ID`: 地区ID（外键，关联到 `District.D_ID`）
- `H_W_ID`: 仓库ID（外键，关联到 `Warehouse.W_ID`）
- `H_DATE`: 支付日期
- `H_AMOUNT`: 支付金额
- `H_DATA`: 其他信息

#### 5. **订单表（Orders）**
存储订单信息。
- `O_ID`: 订单ID（主键，联合 `O_D_ID` 和 `O_W_ID`）
- `O_D_ID`: 地区ID（外键，关联到 `District.D_ID`）
- `O_W_ID`: 仓库ID（外键，关联到 `Warehouse.W_ID`）
- `O_C_ID`: 客户ID（外键，关联到 `Customer.C_ID`）
- `O_ENTRY_D`: 订单创建日期
- `O_CARRIER_ID`: 承运商ID（空值代表订单未发货）
- `O_OL_CNT`: 订单行数
- `O_ALL_LOCAL`: 是否为本地订单

#### 6. **新订单表（New-Order）**
存储未完成的订单。
- `NO_O_ID`: 订单ID（外键，关联到 `Orders.O_ID`）
- `NO_D_ID`: 地区ID（外键，关联到 `Orders.O_D_ID`）
- `NO_W_ID`: 仓库ID（外键，关联到 `Orders.O_W_ID`）

#### 7. **订单明细表（Order-Line）**
存储订单的每一行（商品）。
- `OL_O_ID`: 订单ID（外键，关联到 `Orders.O_ID`）
- `OL_D_ID`: 地区ID（外键，关联到 `Orders.O_D_ID`）
- `OL_W_ID`: 仓库ID（外键，关联到 `Orders.O_W_ID`）
- `OL_NUMBER`: 订单行号（主键，联合 `OL_O_ID`, `OL_D_ID`, `OL_W_ID`）
- `OL_I_ID`: 商品ID（外键，关联到 `Item.I_ID`）
- `OL_SUPPLY_W_ID`: 商品供货仓库ID（外键，关联到 `Warehouse.W_ID`）
- `OL_DELIVERY_D`: 交货日期
- `OL_QUANTITY`: 订购数量
- `OL_AMOUNT`: 订单金额
- `OL_DIST_INFO`: 订单分销信息

#### 8. **库存表（Stock）**
存储每个仓库的库存信息。
- `S_I_ID`: 商品ID（外键，关联到 `Item.I_ID`，主键，联合 `S_W_ID`）
- `S_W_ID`: 仓库ID（外键，关联到 `Warehouse.W_ID`）
- `S_QUANTITY`: 库存数量
- `S_DIST_01` 到 `S_DIST_10`: 各地区的分销数据
- `S_YTD`: 年初至今的销售总量
- `S_ORDER_CNT`: 订单次数
- `S_REMOTE_CNT`: 外仓订单次数
- `S_DATA`: 其他信息

#### 9. **商品表（Item）**
存储商品信息。
- `I_ID`: 商品ID（主键）
- `I_IM_ID`: 商品图像ID
- `I_NAME`: 商品名称
- `I_PRICE`: 商品价格
- `I_DATA`: 商品信息

### 表之间的关系

- **仓库（Warehouse）** 和 **地区（District）**：每个仓库有多个地区，通过 `W_ID` 和 `D_W_ID` 建立一对多的关系。
- **地区（District）** 和 **客户（Customer）**：每个地区有多个客户，通过 `D_ID` 和 `C_D_ID` 建立一对多的关系。
- **客户（Customer）** 和 **订单（Orders）**：每个客户可以有多个订单，通过 `C_ID` 和 `O_C_ID` 建立一对多的关系。
- **订单（Orders）** 和 **订单明细（Order-Line）**：每个订单有多行商品，通过 `O_ID` 和 `OL_O_ID` 建立一对多的关系。
- **订单（Orders）** 和 **新订单（New-Order）**：未发货的订单记录在新订单表中，`NO_O_ID` 关联到 `Orders.O_ID`。
- **仓库（Warehouse）** 和 **库存（Stock）**：每个仓库对多个商品有库存，通过 `W_ID` 和 `S_W_ID` 建立一对多的关系。
- **商品（Item）** 和 **库存（Stock）**：每个商品在不同仓库有库存，通过 `I_ID` 和 `S_I_ID` 建立一对多的关系。

### 总结
TPC-C 数据库中有复杂的表关系结构，主要用于模拟实际业务环境中的在线事务处理工作负载。表之间通过外键进行关联，形成了一系列一对多的关系，能够支持各种复杂的事务操作，如新订单创建、支付、库存管理等。

# Relative error
## Insert only
100,000 items added

| Name     |  k   |  Number of items in Sketch   | Average relative error   | Max relative error   |
| ------- | ----- | ----- | ----- | ----- |
| `TPC_C.item[3]`   | $2^{13}$ | $2^{14}$ | 0.000316 | 0.004008 |


## Delete 50%
100,000 items added

| Name     |  k   |  Number of items in Sketch   | Average relative error   | Max relative error   |
| ------- | ----- | ----- | ----- | ----- |
| `TPC_C.item[3]`   | $2^{13}$ | $2^{14}$ | 0.123962 | 0.212354 |

# Rank error
## Inert Only


| Name     |  k   |  Number of items in Sketch   | Average rank error   | Max rank error   | Number of items added   | Table size   |
| ------- | ----- | ----- | ----- | ----- | ----- | ----- |
| `TPC_C.customer[10]`   | $2^{9}$ | $2^{10}$ | 0.352195 | 0.561806 | 150,000,000 | 85GB |
| `TPC_C.customer[10]`   | $2^{9}$ | $2^{10}$ | 0.155345 | 0.217601 | 15,000,000 | 85GB |
| `TPC_C.customer[10]`   | $2^{9}$ | $2^{10}$ | 0.044003 | 0.060728 | 1,500,000 | 85GB |
| `TPC_C.item[3]`   | $2^{9}$ | $2^{10}$ | 0.010467 | 0.015830 | 100,000 | 7MB |

## Delete 10%

| Name     |  k   |  Number of items in Sketch   | Average rank error   | Max rank error   | Number of items added   | Table size   |
| ------- | ----- | ----- | ----- | ----- | ----- | ----- |
| `TPC_C.customer[10]`   | $2^{9}$ | $2^{10}$ | 0.056682 | 0.077448 | 1,500,000 | 85GB |

## Delete 20%

| Name     |  k   |  Number of items in Sketch   | Average rank error   | Max rank error   | Number of items added   | Table size   |
| ------- | ----- | ----- | ----- | ----- | ----- | ----- |
| `TPC_C.customer[10]`   | $2^{9}$ | $2^{10}$ | 0.072997 | 0.100443 | 1,500,000 | 85GB |

## Delete 30%

| Name     |  k   |  Number of items in Sketch   | Average rank error   | Max rank error   | Number of items added   | Table size   |
| ------- | ----- | ----- | ----- | ----- | ----- | ----- |
| `TPC_C.customer[10]`   | $2^{9}$ | $2^{10}$ | 0.094191 | 0.130649 | 1,500,000 | 85GB |

