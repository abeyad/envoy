package io.envoyproxy.envoyclient.helloenvoykotlin

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

/**
 * Example of a more complex HTTP filter that pauses processing on the response filter chain,
 * buffers until the response is complete, then resumes filter iteration while setting a new header.
 */
class BufferDemoFilter : ResponseFilter {
  private lateinit var headers: ResponseHeaders
  private lateinit var body: ByteBuffer

  override fun onResponseHeaders(
    headers: ResponseHeaders,
    endStream: Boolean,
    streamIntel: StreamIntel
  ): FilterHeadersStatus<ResponseHeaders> {
    this.headers = headers
    return FilterHeadersStatus.StopIteration()
  }

  override fun onResponseData(
    body: ByteBuffer,
    endStream: Boolean,
    streamIntel: StreamIntel
  ): FilterDataStatus<ResponseHeaders> {
    // Since we request buffering, each invocation will include all data buffered so far.
    this.body = body

    // If this is the end of the stream, resume processing of the (now fully-buffered) response.
    if (endStream) {
      val builder = headers.toResponseHeadersBuilder().add("buffer-filter-demo", "1")
      return FilterDataStatus.ResumeIteration(builder.build(), body)
    }
    return FilterDataStatus.StopIterationAndBuffer()
  }

  override fun onResponseTrailers(
    trailers: ResponseTrailers,
    streamIntel: StreamIntel
  ): FilterTrailersStatus<ResponseHeaders, ResponseTrailers> {
    // Trailers imply end of stream; resume processing of the (now fully-buffered) response.
    val builder = headers.toResponseHeadersBuilder().add("buffer-filter-demo", "1")
    return FilterTrailersStatus.ResumeIteration(builder.build(), this.body, trailers)
  }

  @Suppress("EmptyFunctionBlock")
  override fun onError(error: EnvoyError, finalStreamIntel: FinalStreamIntel) {}

  @Suppress("EmptyFunctionBlock") override fun onCancel(finalStreamIntel: FinalStreamIntel) {}

  @Suppress("EmptyFunctionBlock") override fun onComplete(finalStreamIntel: FinalStreamIntel) {}
}
