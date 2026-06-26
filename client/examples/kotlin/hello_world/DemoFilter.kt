package io.envoyproxy.envoyclient.helloenvoykotlin

import android.util.Log
import io.envoyproxy.envoyclient.EnvoyError
import io.envoyproxy.envoyclient.FilterDataStatus
import io.envoyproxy.envoyclient.FilterHeadersStatus
import io.envoyproxy.envoyclient.FilterTrailersStatus
import io.envoyproxy.envoyclient.FinalStreamIntel
import io.envoyproxy.envoyclient.ResponseFilter
import io.envoyproxy.envoyclient.ResponseHeaders
import io.envoyproxy.envoyclient.ResponseTrailers
import io.envoyproxy.envoyclient.StreamIntel
import java.nio.ByteBuffer

/** A filter implemented as a simple example of Envoy response filter. */
class DemoFilter : ResponseFilter {
  override fun onResponseHeaders(
    headers: ResponseHeaders,
    endStream: Boolean,
    streamIntel: StreamIntel
  ): FilterHeadersStatus<ResponseHeaders> {
    Log.d("DemoFilter", "On headers!")
    val builder = headers.toResponseHeadersBuilder()
    builder.add("filter-demo", "1")
    return FilterHeadersStatus.Continue(builder.build())
  }

  override fun onResponseData(
    body: ByteBuffer,
    endStream: Boolean,
    streamIntel: StreamIntel
  ): FilterDataStatus<ResponseHeaders> {
    Log.d("DemoFilter", "On data!")
    return FilterDataStatus.Continue(body)
  }

  override fun onResponseTrailers(
    trailers: ResponseTrailers,
    streamIntel: StreamIntel
  ): FilterTrailersStatus<ResponseHeaders, ResponseTrailers> {
    Log.d("DemoFilter", "On trailers!")
    return FilterTrailersStatus.Continue(trailers)
  }

  override fun onError(error: EnvoyError, finalStreamIntel: FinalStreamIntel) {
    Log.d("DemoFilter", "On error!")
  }

  override fun onCancel(finalStreamIntel: FinalStreamIntel) {
    Log.d("DemoFilter", "On cancel!")
  }

  @Suppress("EmptyFunctionBlock") override fun onComplete(finalStreamIntel: FinalStreamIntel) {}
}
