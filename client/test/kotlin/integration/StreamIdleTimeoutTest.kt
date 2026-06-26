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
import org.junit.Assert.fail
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class StreamIdleTimeoutTest {
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
  private val callbackExpectation = CountDownLatch(1)

  class IdleTimeoutValidationFilter(private val latch: CountDownLatch) : ResponseFilter {
    override fun onResponseHeaders(
      headers: ResponseHeaders,
      endStream: Boolean,
      streamIntel: StreamIntel
    ): FilterHeadersStatus<ResponseHeaders> {
      return FilterHeadersStatus.StopIteration()
    }

    override fun onResponseData(
      body: ByteBuffer,
      endStream: Boolean,
      streamIntel: StreamIntel
    ): FilterDataStatus<ResponseHeaders> {
      return FilterDataStatus.StopIterationNoBuffer()
    }

    override fun onResponseTrailers(
      trailers: ResponseTrailers,
      streamIntel: StreamIntel
    ): FilterTrailersStatus<ResponseHeaders, ResponseTrailers> {
      return FilterTrailersStatus.StopIteration()
    }

    override fun onError(error: EnvoyError, finalStreamIntel: FinalStreamIntel) {
      assertThat(error.errorCode).isEqualTo(4)
      latch.countDown()
    }

    override fun onComplete(finalStreamIntel: FinalStreamIntel) {}

    override fun onCancel(finalStreamIntel: FinalStreamIntel) {
      fail("Unexpected call to onCancel filter callback")
    }
  }

  @Test
  fun `stream idle timeout triggers onError callbacks`() {
    val engine =
      EngineBuilder()
        .setLogLevel(LogLevel.DEBUG)
        .setLogger { _, msg -> print(msg) }
        .setTrustChainVerification(EnvoyConfiguration.TrustChainVerification.ACCEPT_UNTRUSTED)
        .addPlatformFilter(
          name = "idle_timeout_validation_filter",
          factory = { IdleTimeoutValidationFilter(filterExpectation) }
        )
        .addStreamIdleTimeoutSeconds(1)
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
      .setOnError { error, _ ->
        assertThat(error.errorCode).isEqualTo(4)
        callbackExpectation.countDown()
      }
      .start(Executors.newSingleThreadExecutor())
      .sendHeaders(requestHeaders, false)

    filterExpectation.await(10, TimeUnit.SECONDS)
    callbackExpectation.await(10, TimeUnit.SECONDS)

    engine.terminate()

    assertThat(filterExpectation.count).isEqualTo(0)
    assertThat(callbackExpectation.count).isEqualTo(0)
  }
}
