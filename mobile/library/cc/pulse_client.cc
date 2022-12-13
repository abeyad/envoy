#include "pulse_client.h"

#include "envoy/stats/stats.h"
#include "library/common/engine_handle.h"
#include "library/common/types/c_types.h"

namespace Envoy {
namespace Platform {

PulseClient::PulseClient(EngineSharedPtr engine) : engine_(engine) {}

void PulseClient::counter(absl::string_view name, std::function<void(uint64_t)> callback) {
  Envoy::EngineHandle::runOnEngineDispatcher(rawEngine(), [name, callback](auto& engine) -> void {
    engine.getCounterValue(std::string(name), envoy_stats_notags, callback);
  });
}

void PulseClient::gauge(absl::string_view name, std::function<void(uint64_t)> callback) {
  Envoy::EngineHandle::runOnEngineDispatcher(rawEngine(), [name, callback](auto& engine) -> void {
    engine.getGaugeValue(std::string(name), envoy_stats_notags, callback);
  });
}

envoy_engine_t PulseClient::rawEngine() { return engine_->engine_; }

} // namespace Platform
} // namespace Envoy
