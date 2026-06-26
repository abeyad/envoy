#pragma once

#include "source/common/common/logger.h"
#include "source/common/common/thread.h"
#include "source/server/options_impl_base.h"

namespace Envoy {

// Process-wide lifecycle events for global state for Envoy Client. There should only ever be a
// singleton of this class.
class ClientProcessWide {
public:
  explicit ClientProcessWide(const OptionsImplBase& options);
  ~ClientProcessWide();

private:
  Thread::MutexBasicLockable log_lock_;
  std::unique_ptr<Logger::Context> logging_context_;
};

} // namespace Envoy
