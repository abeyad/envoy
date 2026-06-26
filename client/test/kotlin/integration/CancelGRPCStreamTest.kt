package test.kotlin.integration

import com.google.common.truth.Truth.assertThat
import com.google.protobuf.Any
import com.google.protobuf.ByteString
import io.envoyproxy.envoyclient.EngineBuilder
import io.envoyproxy.envoyclient.EnvoyError
import io.envoyproxy.envoyclient.FilterDataStatus
import io.envoyproxy.envoyclient.FilterHeadersStatus
import io.envoyproxy.envoyclient.FilterTrailersStatus
import io.envoyproxy.envoyclient.FinalStreamIntel
import io.envoyproxy.envoyclient.GRPCClient
import io.envoyproxy.envoyclient.GRPCRequestHeadersBuilder
import io.envoyproxy.envoyclient.LogLevel
import io.envoyproxy.envoyclient.ResponseFilter
import io.envoyproxy.envoyclient.ResponseHeaders
import io.envoyproxy.envoyclient.ResponseTrailers
import io.envoyproxy.envoyclient.StreamIntel
import io.envoyproxy.envoyclient.engine.JniLibrary
import java.nio.ByteBuffer
import java.util.concurrent.CountDownLatch
import java.util.concurrent.Executors
import java.util.concurrent.TimeUnit
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class CancelGRPCStreamTest {

  init {
    JniLibrary.loadTestLibrary()
  }

  private val filterExpectation = CountDownLatch(1)
  private val onCancelCallbackExpectation = CountDownLatch(1)

  class CancelValidationFilter(private val latch: CountDownLatch) : ResponseFilter {
    override fun onResponseHeaders(
      headers: ResponseHeaders,
      endStream: Boolean,
      streamIntel: StreamIntel
    ): FilterHeadersStatus<ResponseHeaders> {
      return FilterHeadersStatus.Continue(headers)
    }

    override fun onResponseData(
      body: ByteBuffer,
      endStream: Boolean,
      streamIntel: StreamIntel
    ): FilterDataStatus<ResponseHeaders> {
      return FilterDataStatus.Continue(body)
    }

    override fun onResponseTrailers(
      trailers: ResponseTrailers,
      streamIntel: StreamIntel
    ): FilterTrailersStatus<ResponseHeaders, ResponseTrailers> {
      return FilterTrailersStatus.Continue(trailers)
    }

    override fun onError(error: EnvoyError, finalStreamIntel: FinalStreamIntel) {}

    override fun onComplete(finalStreamIntel: FinalStreamIntel) {}

    override fun onCancel(finalStreamIntel: FinalStreamIntel) {
      latch.countDown()
    }
  }

  @Test
  fun `cancel grpc stream calls onCancel callback`() {

    var anyProto =
      Any.newBuilder()
        .setTypeUrl(
          "type.googleapis.com/envoyclient.extensions.filters.http.local_error.LocalError"
        )
        .setValue(ByteString.empty())
        .build()

    val engine =
      EngineBuilder()
        .setLogLevel(LogLevel.DEBUG)
        .setLogger { _, msg -> print(msg) }
        .addPlatformFilter(
          name = "cancel_validation_filter",
          factory = { CancelValidationFilter(filterExpectation) }
        )
        .addNativeFilter(
          "envoy.filters.http.local_error",
          anyProto.toByteArray().toString(Charsets.UTF_8)
        )
        .build()

    val client = GRPCClient(engine.streamClient())

    val requestHeaders =
      GRPCRequestHeadersBuilder(scheme = "https", authority = "example.com", path = "/test").build()

    client
      .newGRPCStreamPrototype()
      .setOnCancel { _ -> onCancelCallbackExpectation.countDown() }
      .start(Executors.newSingleThreadExecutor())
      .sendHeaders(requestHeaders, false)
      .cancel()

    filterExpectation.await(10, TimeUnit.SECONDS)
    onCancelCallbackExpectation.await(10, TimeUnit.SECONDS)

    engine.terminate()

    assertThat(filterExpectation.count).isEqualTo(0)
    assertThat(onCancelCallbackExpectation.count).isEqualTo(0)
  }
}
