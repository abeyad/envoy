#pragma once

#include "envoy/common/key_value_store.h"
#include "envoy/config/xds_resources_delegate.h"

namespace Envoy {
namespace Extensions {
namespace Config {

// TODO(abeyad): add comments
class KeyValueStoreXdsDelegate : public Envoy::Config::XdsResourcesDelegate {
public:
  KeyValueStoreXdsDelegate(KeyValueStorePtr&& xds_config_store, Api::Api& api);

  std::vector<envoy::service::discovery::v3::Resource>
  getResources(const std::string& authority_id,
               const std::string& resource_type_url) const override;

  void onConfigUpdated(const std::string& authority_id, const std::string& resource_type_url,
                       const std::vector<Envoy::Config::DecodedResourceRef>& resources) override;

private:
  KeyValueStorePtr xds_config_store_;
  Api::Api& api_;
};

// TODO(abeyad): add comments
class KeyValueStoreXdsDelegateFactory : public Envoy::Config::XdsResourcesDelegateFactory {
public:
  KeyValueStoreXdsDelegateFactory() = default;

  Envoy::ProtobufTypes::MessagePtr createEmptyConfigProto() override;

  std::string name() const override;

  Envoy::Config::XdsResourcesDelegatePtr
  createXdsResourcesDelegate(const ProtobufWkt::Any& config,
                             ProtobufMessage::ValidationVisitor& validation_visitor, Api::Api& api,
                             Event::Dispatcher& dispatcher) override;
};

} // namespace Config
} // namespace Extensions
} // namespace Envoy
