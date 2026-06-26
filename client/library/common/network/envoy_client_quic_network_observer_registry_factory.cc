#include "library/common/network/envoy_client_quic_network_observer_registry_factory.h"

namespace Envoy {
namespace Quic {

void EnvoyClientQuicNetworkObserverRegistry::onNetworkMadeDefault(NetworkHandle network) {
  ENVOY_LOG_MISC(trace, "Default network changed.");
  dispatcher_.post([this, network]() {
    NotifyObserversOfNetworkChange(network, NetworkChangeType::MadeDefault);
  });
}

void EnvoyClientQuicNetworkObserverRegistry::onNetworkConnected(NetworkHandle network) {
  ENVOY_LOG_MISC(trace, "Network connected.");
  dispatcher_.post(
      [this, network]() { NotifyObserversOfNetworkChange(network, NetworkChangeType::Connected); });
}

void EnvoyClientQuicNetworkObserverRegistry::onNetworkDisconnected(NetworkHandle network) {
  ENVOY_LOG_MISC(trace, "Network disconnected.");
  dispatcher_.post([this, network]() {
    NotifyObserversOfNetworkChange(network, NetworkChangeType::Disconnected);
  });
}

void EnvoyClientQuicNetworkObserverRegistry::NotifyObserversOfNetworkChange(
    NetworkHandle network, NetworkChangeType change_type) {
  if (!isNetworkChangeUpToDate(network, change_type)) {
    // As this is called asynchronously, the network state might have already
    // been overridden by immediate following changes before this happens.
    return;
  }

  // Retain the existing observers in a list and iterate on the list as new
  // connections might be created and registered during iteration.
  std::vector<QuicNetworkConnectivityObserver*> existing_observers;
  existing_observers.reserve(registeredQuicObservers().size());
  for (QuicNetworkConnectivityObserver* observer : registeredQuicObservers()) {
    existing_observers.push_back(observer);
  }
  for (QuicNetworkConnectivityObserver* observer : existing_observers) {
    switch (change_type) {
    case NetworkChangeType::Connected:
      observer->onNetworkConnected(network);
      break;
    case NetworkChangeType::Disconnected:
      observer->onNetworkDisconnected(network);
      break;
    case NetworkChangeType::MadeDefault:
      observer->onNetworkMadeDefault(network);
      break;
    }
  }
}

bool EnvoyClientQuicNetworkObserverRegistry::isNetworkChangeUpToDate(
    NetworkHandle network, NetworkChangeType change_type) {
  switch (change_type) {
  case NetworkChangeType::Connected:
    return network_tracker_.getAllConnectedNetworks().contains(network);
  case NetworkChangeType::Disconnected:
    return !network_tracker_.getAllConnectedNetworks().contains(network);
  case NetworkChangeType::MadeDefault:
    return network_tracker_.getDefaultNetwork() == network;
  }
}

EnvoyClientQuicNetworkObserverRegistryFactory::EnvoyClientQuicNetworkObserverRegistryFactory(
    NetworkConnectivityTracker& network_tracker)
    : network_tracker_(network_tracker) {
  thread_local_observer_registries_.reserve(1);
}

EnvoyQuicNetworkObserverRegistryPtr
EnvoyClientQuicNetworkObserverRegistryFactory::createQuicNetworkObserverRegistry(
    Event::Dispatcher& dispatcher) {
  auto result =
      std::make_unique<EnvoyClientQuicNetworkObserverRegistry>(dispatcher, network_tracker_);
  {
    absl::WriterMutexLock lock(&mutex_);
    thread_local_observer_registries_.emplace_back(*result);
  }
  return result;
}

std::vector<std::reference_wrapper<EnvoyClientQuicNetworkObserverRegistry>>
EnvoyClientQuicNetworkObserverRegistryFactory::getCreatedObserverRegistries() {
  absl::ReaderMutexLock lock(&mutex_);
  return thread_local_observer_registries_;
}

} // namespace Quic
} // namespace Envoy
