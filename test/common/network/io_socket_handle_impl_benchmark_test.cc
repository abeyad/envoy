#include "source/common/network/io_socket_handle_impl.h"

#include <memory>

#include "absl/strings/str_cat.h"
#include "benchmark/benchmark.h"

namespace Envoy {
namespace Network {

static sockaddr_storage getV6SockAddr(const std::string& ip, uint32_t port) {
  sockaddr_storage ss;
  auto ipv6_addr = reinterpret_cast<sockaddr_in6*>(&ss);
  memset(ipv6_addr, 0, sizeof(sockaddr_in6));
  ipv6_addr->sin6_family = AF_INET6;
  inet_pton(AF_INET6, ip.c_str(), &ipv6_addr->sin6_addr);
  ipv6_addr->sin6_port = htons(port);
  return ss;
}

static sockaddr_storage getV4SockAddr(const std::string& ip, uint32_t port) {
  sockaddr_storage ss;
  auto ipv4_addr = reinterpret_cast<sockaddr_in*>(&ss);
  memset(ipv4_addr, 0, sizeof(sockaddr_in));
  ipv4_addr->sin_family = AF_INET;
  inet_pton(AF_INET, ip.c_str(), &ipv4_addr->sin_addr);
  ipv4_addr->sin_port = htons(port);
  return ss;
}

static socklen_t getSockAddrLen(const sockaddr_storage& ss) {
  if (ss.ss_family == AF_INET6) {
    return sizeof(sockaddr_in6);
  }
  return sizeof(sockaddr_in);
}

// Gets a sampling of IPv6 destination addresses.
std::vector<sockaddr_storage> getV6DstAddrs() {
  std::vector<sockaddr_storage> addresses;
  for (int i = 1201; i < 1251; ++i) {
    addresses.push_back(getV6SockAddr(absl::StrCat("2001:DB8::", i), 443));
  }
  return addresses;
}

// Gets a sampling of IPv4 destination addresses.
std::vector<sockaddr_storage> getV4DstAddrs() {
  std::vector<sockaddr_storage> addresses;
  for (int i = 1; i < 51; ++i) {
    addresses.push_back(getV4SockAddr(absl::StrCat("203.0.113.", i), 443));
  }
  return addresses;
}

// Gets a sampling of IPv6 source addresses.
std::vector<sockaddr_storage> getV6SrcAddrs() {
  std::vector<sockaddr_storage> addresses;
  for (int i = 1301; i < 1351; ++i) {
    addresses.push_back(getV6SockAddr(absl::StrCat("2001:DB8::", i), 51234));
  }
  return addresses;
}

// Gets a sampling of IPv4 source addresses.
std::vector<sockaddr_storage> getV4SrcAddrs() {
  std::vector<sockaddr_storage> addresses;
  for (int i = 51; i < 101; ++i) {
    addresses.push_back(getV4SockAddr(absl::StrCat("203.0.113.", i), 52345));
  }
  return addresses;
}

void BM_GetOrCreateEnvoyAddressInstanceConnectedSocketV6(benchmark::State& state) {
  auto src_addr = getV6SrcAddrs()[0];
  auto dst_addr = getV6DstAddrs()[0];
  IoSocketHandleImpl io_handle(1, false, AF_INET6, 4);
  for (auto _ : state) {
    UNREFERENCED_PARAMETER(_);
    io_handle.getOrCreateEnvoyAddressInstance(src_addr, getSockAddrLen(src_addr));
    io_handle.getOrCreateEnvoyAddressInstance(dst_addr, getSockAddrLen(dst_addr));
  }
}
BENCHMARK(BM_GetOrCreateEnvoyAddressInstanceConnectedSocketV6);

} // namespace Network
} // namespace Envoy
