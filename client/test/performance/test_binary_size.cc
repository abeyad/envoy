#include "library/common/internal_engine.h"

// NOLINT(namespace-envoy)

// This binary is used to perform stripped down binary size investigations of the Envoy codebase.
//
// Please refer to the development docs for more information:
// https://envoyclient.io/docs/envoy-client/latest/development/performance/binary_size.html
int main() {
  Envoy::InternalEngine engine(nullptr, nullptr, nullptr);
  auto options = std::make_shared<Envoy::OptionsImplBase>();
  engine.run(std::move(options));
}
