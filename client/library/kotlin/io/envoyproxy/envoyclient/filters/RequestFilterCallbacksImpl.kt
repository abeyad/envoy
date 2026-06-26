package io.envoyproxy.envoyclient

import io.envoyproxy.envoyclient.engine.types.EnvoyHTTPFilterCallbacks

/** Envoy implementation of `RequestFilterCallbacks`. */
internal class RequestFilterCallbacksImpl(internal val callbacks: EnvoyHTTPFilterCallbacks) :
  RequestFilterCallbacks {

  override fun resumeRequest() {
    callbacks.resumeIteration()
  }

  override fun resetIdleTimer() {
    callbacks.resetIdleTimer()
  }
}
