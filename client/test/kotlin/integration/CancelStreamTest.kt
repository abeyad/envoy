package test.kotlin.integration

import com.google.common.truth.Truth.assertThat
import io.envoyproxy.envoyclient.EngineBuilder
import io.envoyproxy.envoyclient.EnvoyError
import io.envoyproxy.envoyclient.FilterDataStatus
import io.envoyproxy.envoyclient.FilterHeadersStatus
import io.envoyproxy.envoyclient.FilterTrailersStatus
import io.envoyproxy.envoyclient.FinalStreamIntel
import io.envoyproxy.envoyclient.LogLevel
import io.envoyproxy.envoyclient.RequestHeadersBuilder
import io.envoyproxy.envoyclient.RequestMethod
import io.envoyproxy.envoyclient.ResponseFilter
import io.envoyproxy.envoyclient.ResponseHeaders
import io.envoyproxy.envoyclient.ResponseTrailers
import io.envoyproxy.envoyclient.StreamIntel
import io.envoyproxy.envoyclient.engine.EnvoyConfiguration
import io.envoyproxy.envoyclient.engine.JniLibrary
import io.envoyproxy.envoyclient.engine.testing.HttpTestServerFactory
import java.nio.ByteBuffer
import java.util.concurrent.CountDownLatch
import java.util.concurrent.Executors
import java.util.concurrent.TimeUnit
import org.junit.After
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class CancelStreamTest {
  init {
    JniLibrary.loadTestLibrary()
  }

  private lateinit var httpTestServer: HttpTestServerFactory.HttpTestServer

  @Before
  fun setUp() {
    httpTestServer = HttpTestServerFactory.start(HttpTestServerFactory.Type.HTTP2_WITH_TLS)
  }

  @After
  fun tearDown() {
    httpTestServer.shutdown()
  }

  private val filterExpectation = CountDownLatch(1)
  private val runExpectation = CountDownLatch(1)

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
  fun `cancel stream calls onCancel callback`() {
    val engine =
      EngineBuilder()
        .setLogLevel(LogLevel.DEBUG)
        .setLogger { _, msg -> print(msg) }
        .setTrustChainVerification(EnvoyConfiguration.TrustChainVerification.ACCEPT_UNTRUSTED)
        .addPlatformFilter(
          name = "cancel_validation_filter",
          factory = { CancelValidationFilter(filterExpectation) }
        )
        .build()

    val client = engine.streamClient()

    val requestHeaders =
      RequestHeadersBuilder(
          method = RequestMethod.GET,
          scheme = "https",
          authority = httpTestServer.address,
          path = "/simple.txt"
        )
        .build()

    client
      .newStreamPrototype()
      .setOnCancel { _ -> runExpectation.countDown() }
      .start(Executors.newSingleThreadExecutor())
      .sendHeaders(requestHeaders, false)
      .cancel()

    filterExpectation.await(10, TimeUnit.SECONDS)
    runExpectation.await(10, TimeUnit.SECONDS)

    engine.terminate()

    assertThat(filterExpectation.count).isEqualTo(0)
    assertThat(runExpectation.count).isEqualTo(0)
  }
}
