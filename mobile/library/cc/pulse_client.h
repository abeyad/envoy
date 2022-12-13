#pragma once

#include <memory>

#include "engine.h"

#include "library/common/engine.h"
#include "source/common/stats/utility.h"

#include "absl/strings/string_view.h"

namespace Envoy {
namespace Platform {

class Engine;
using EngineSharedPtr = std::shared_ptr<Engine>;

// TODO(crockeo): although this is stubbed out since it's in the main directory, it depends on
// objects defined under stats. this will not be fully stubbed until stats is stubbed

class PulseClient {
public:
  explicit PulseClient(EngineSharedPtr engine);

  // Gets the counter value for the associated name, and invokes the callback on the value.
  // The callback is run on the PulseClient's Engine's dispatcher thread.
  // TODO(abeyad): add support for tags.
  // TODO(abeyad): add support for specifying a different dispatcher.
  void counter(absl::string_view name, std::function<void(uint64_t)> callback);
  // Gets the gauge value for the associated name, and invokes the callback on the value.
  // The callback is run on the PulseClient's Engine's dispatcher thread.
  // TODO(abeyad): add support for tags.
  // TODO(abeyad): add support for specifying a different dispatcher.
  void gauge(absl::string_view name, std::function<void(uint64_t)> callback);
  // TODO(abeyad): Add timer and distribution support.

private:
  envoy_engine_t rawEngine();

  EngineSharedPtr engine_;
};

using PulseClientSharedPtr = std::shared_ptr<PulseClient>;

} // namespace Platform
} // namespace Envoy
