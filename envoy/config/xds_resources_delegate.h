#pragma once

#include "envoy/api/api.h"
#include "envoy/common/optref.h"
#include "envoy/common/pure.h"
#include "envoy/config/subscription.h"
#include "envoy/config/typed_config.h"
#include "envoy/protobuf/message_validator.h"
#include "envoy/service/discovery/v3/discovery.pb.h"

namespace Envoy {
namespace Config {

/*
 * An abstract class that represents an xDS source. The toKey() function provides a unique
 * identifier for the source for a group of xDS resources in a DiscoveryResponse.
 */
class XdsSourceId {
public:
  virtual ~XdsSourceId() = default;

  /**
   * Returns a unique string representation of the source. The string can be used as an identifier
   * for the source (e.g. as a key in a hash map).
   * @return string representation of the source.
   */
  virtual std::string toKey() const PURE;
};

/**
 * An interface for hooking into xDS resource fetch and update events.
 * Currently, this interface only supports the SotW (state-of-the-world) xDS protocol.
 *
 * Instances of this interface get invoked on the main Envoy thread. Thus, it is important for
 * implementations of this interface to not execute any blocking operations on the same thread.
 * Any blocking operations (e.g. flushing config to disk) should be handed off to a separate thread
 * so we don't block the main thread.
 */
class XdsResourcesDelegate {
public:
  virtual ~XdsResourcesDelegate() = default;

  /**
   * Returns a list of xDS resources for the given authority and type. It is up to the
   * implementation to determine what resources to supply, if any.
   *
   * This function is intended to only be called on xDS fetch startup, and allows the
   * implementation to return a set of resources to be loaded and used by the Envoy instance
   * (e.g. persisted resources in local storage).
   *
   * @param source_id The xDS source for the requested resources.
   * @return A set of xDS resources for the given authority and type.
   */
  virtual std::vector<envoy::service::discovery::v3::Resource>
  getResources(const XdsSourceId& source_id) PURE;

  /**
   * Invoked when SotW xDS configuration updates have been received from an xDS authority, have been
   * applied on the Envoy instance, and are about to be ACK'ed.
   *
   * @param source_id The xDS source for the requested resources.
   * @param resources The resources for the given source received on the DiscoveryResponse.
   */
  virtual void onConfigUpdated(const XdsSourceId& source_id,
                               const std::vector<DecodedResourceRef>& resources) PURE;

  /**
   * Reset the delegate for the given xDS source. Reset in the context of this interface means that
   * for the given config source and type, a new subscription was initiated or an existing
   * subscription reset for a set of resources.
   *
   * @param source_id The xDS source being reset.
   */
  virtual void reset(const XdsSourceId& source_id) PURE;
};

using XdsResourcesDelegatePtr = std::unique_ptr<XdsResourcesDelegate>;
using XdsResourcesDelegateOptRef = OptRef<XdsResourcesDelegate>;

/**
 * A factory abstract class for creating instances of XdsResourcesDelegate.
 */
class XdsResourcesDelegateFactory : public Config::TypedFactory {
public:
  ~XdsResourcesDelegateFactory() override = default;

  /**
   * Creates a XdsResourcesDelegate using the given configuration.
   * @param config Configuration of the XdsResourcesDelegate to create.
   * @param validation_visitor Validates the configuration.
   * @param api The APIs that can be used by the delegate.
   * @return The created XdsResourcesDelegate instance
   */
  virtual XdsResourcesDelegatePtr
  createXdsResourcesDelegate(const ProtobufWkt::Any& config,
                             ProtobufMessage::ValidationVisitor& validation_visitor,
                             Api::Api& api) PURE;

  std::string category() const override { return "envoy.config.xds"; }
};

} // namespace Config
} // namespace Envoy
