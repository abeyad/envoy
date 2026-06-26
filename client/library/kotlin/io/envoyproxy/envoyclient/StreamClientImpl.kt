package io.envoyproxy.envoyclient

import io.envoyproxy.envoyclient.engine.EnvoyEngine

/** Envoy implementation of `StreamClient`. */
internal class StreamClientImpl(internal val engine: EnvoyEngine) : StreamClient {

  override fun newStreamPrototype() = StreamPrototype(engine)
}
