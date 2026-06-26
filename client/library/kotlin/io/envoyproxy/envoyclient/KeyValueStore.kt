package io.envoyproxy.envoyclient

import io.envoyproxy.envoyclient.engine.types.EnvoyKeyValueStore

/**
 * `KeyValueStore` is an interface that may be implemented to provide access to an arbitrary
 * key-value store implementation that may be made accessible to native Envoy Client code.
 */
interface KeyValueStore : EnvoyKeyValueStore
