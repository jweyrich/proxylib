#pragma once

#include <proxylib/compat/types.h>
#include <proxylib/dump.h>
#include <proxylib/log.h>

namespace proxylib {
namespace asio {
namespace socks4a {

const uint8 version = 0x04;

#define	IN_000X(i)	(((uint32)(i) | 0x000000ff) == 0x000000ff)

} } } // namespace proxylib::asio::socks4a
