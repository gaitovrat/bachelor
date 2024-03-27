/*
 * Network.h
 *
 *  Created on: Mar 27, 2024
 *      Author: ratmirgaitov
 */

#ifndef INCLUDE_SHARED_NETWORK_H_
#define INCLUDE_SHARED_NETWORK_H_

#include <cstdint>

using RefIPv4 = const uint8_t (&)[4];
using IPv4 = uint8_t[4];

static constexpr IPv4 MCU_IP = {192, 168, 99, 101};
static constexpr IPv4 NETMASK = {255, 255, 255, 0};
static constexpr IPv4 GATEWAY = {192, 168, 99, 100};
static constexpr IPv4 PC_IP = {192, 168, 99, 104};

#endif /* INCLUDE_SHARED_NETWORK_H_ */
