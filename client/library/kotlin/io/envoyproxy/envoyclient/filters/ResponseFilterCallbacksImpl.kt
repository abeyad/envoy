package io.envoyproxy.envoyclient

import io.envoyproxy.envoyclient.engine.types.EnvoyHTTPFilterCallbacks

/** Envoy implementation of `ResponseFilterCallbacks`. */
internal class ResponseFilterCallbacksImpl(internal val callbacks: EnvoyHTTPFilterCallbacks) :
  ResponseFilterCallbacks {

  override fun resumeResponse() {
    callbacks.resumeIteration()
  }

  override fun resetIdleTimer() {
    callbacks.resetIdleTimer()
  }
}
