#include "contrib/envoy/extensions/xds/kv_store_xds_delegate_config.pb.h"
#include "contrib/envoy/extensions/xds/kv_store_xds_delegate_config.pb.validate.h"
#include "contrib/envoy/extensions/xds/persisted_resources.pb.h"
#include "contrib/xds/source/kv_store_xds_delegate.h"

#include "envoy/registry/registry.h"
#include "envoy/service/discovery/v3/discovery.pb.h"

#include "source/common/config/utility.h"
#include "source/common/protobuf/utility.h"

#include "absl/strings/str_cat.h"

namespace Envoy {
namespace Extensions {
namespace Config {
namespace {

using envoy::extensions::xds::KeyValueStoreXdsDelegateConfig;
using envoy::extensions::xds::ResourceList;

// The supplied KeyValueStore may be shared with other parts of the application
// (e.g. SharedPreferences on Android). Therefore, we introduce a prefix to the key to create a
// distinct key namespace.
constexpr char KEY_PREFIX[] = "XDS_CONFIG";
// The delimiter between parts of the key.
constexpr char DELIMITER[] = "*+";

// Constructs the key for the KeyValueStore from the xDS authority and resource type URL.
std::string constructKey(const std::string& authority_id, const std::string& resource_type_url) {
  return absl::StrCat(KEY_PREFIX, DELIMITER, authority_id, DELIMITER, resource_type_url);
}

} // namespace

KeyValueStoreXdsDelegate::KeyValueStoreXdsDelegate(KeyValueStorePtr&& xds_config_store,
                                                   Api::Api& api)
    : xds_config_store_(std::move(xds_config_store)), api_(api) {}

std::vector<envoy::service::discovery::v3::Resource>
KeyValueStoreXdsDelegate::getResources(const std::string& authority_id,
                                       const std::string& resource_type_url) const {
  const std::string key = constructKey(authority_id, resource_type_url);
  if (auto existing_resources = xds_config_store_->get(key)) {
    ResourceList resource_list;
    resource_list.ParseFromString(std::string(*existing_resources));
    return std::vector<envoy::service::discovery::v3::Resource>{resource_list.resources().begin(),
                                                                resource_list.resources().end()};
  }
  return {};
}

// TODO(abeyad): Handle key eviction.
void KeyValueStoreXdsDelegate::onConfigUpdated(
    const std::string& authority_id, const std::string& resource_type_url,
    const std::vector<Envoy::Config::DecodedResourceRef>& resources) {
  ResourceList resource_list;
  for (const auto& resource_ref : resources) {
    const auto& decoded_resource = resource_ref.get();
    if (decoded_resource.hasResource()) {
      envoy::service::discovery::v3::Resource r;
      // TODO(abeyad): Support dynamic parameter constraints.
      r.set_name(decoded_resource.name());
      r.set_version(decoded_resource.version());
      r.mutable_resource()->PackFrom(decoded_resource.resource());
      if (decoded_resource.ttl()) {
        r.mutable_ttl()->CopyFrom(Protobuf::util::TimeUtil::MillisecondsToDuration(
            decoded_resource.ttl().value().count()));
      }
      *resource_list.add_resources() = std::move(r);
    }
  }

  const std::string key = constructKey(authority_id, resource_type_url);

  if (resource_list.resources_size() == 0) {
    xds_config_store_->remove(key);
    return;
  }

  TimestampUtil::systemClockToTimestamp(api_.timeSource().systemTime(),
                                        *resource_list.mutable_last_updated());
  xds_config_store_->addOrUpdate(key, resource_list.SerializeAsString());
}

Envoy::ProtobufTypes::MessagePtr KeyValueStoreXdsDelegateFactory::createEmptyConfigProto() {
  return std::make_unique<KeyValueStoreXdsDelegateConfig>();
}

std::string KeyValueStoreXdsDelegateFactory::name() const {
  return "envoy.xds_delegates.KeyValueStoreXdsDelegate";
};

Envoy::Config::XdsResourcesDelegatePtr KeyValueStoreXdsDelegateFactory::createXdsResourcesDelegate(
    const ProtobufWkt::Any& config, ProtobufMessage::ValidationVisitor& validation_visitor,
    Api::Api& api, Event::Dispatcher& dispatcher) {
  const auto& validator_config =
      Envoy::MessageUtil::anyConvertAndValidate<KeyValueStoreXdsDelegateConfig>(config,
                                                                                validation_visitor);
  auto& kv_store_factory = Envoy::Config::Utility::getAndCheckFactory<Envoy::KeyValueStoreFactory>(
      validator_config.key_value_store_config().config());
  KeyValueStorePtr xds_config_store = kv_store_factory.createStore(
      validator_config.key_value_store_config(), validation_visitor, dispatcher, api.fileSystem());
  return std::make_unique<KeyValueStoreXdsDelegate>(std::move(xds_config_store), api);
}

REGISTER_FACTORY(KeyValueStoreXdsDelegateFactory, Envoy::Config::XdsResourcesDelegateFactory);

} // namespace Config
} // namespace Extensions
} // namespace Envoy
