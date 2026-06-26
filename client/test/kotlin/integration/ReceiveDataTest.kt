package test.kotlin.integration

import com.google.common.truth.Truth.assertThat
import io.envoyproxy.envoyclient.EngineBuilder
import io.envoyproxy.envoyclient.LogLevel
import io.envoyproxy.envoyclient.RequestHeadersBuilder
import io.envoyproxy.envoyclient.RequestMethod
import io.envoyproxy.envoyclient.engine.EnvoyConfiguration
import io.envoyproxy.envoyclient.engine.JniLibrary
import io.envoyproxy.envoyclient.engine.testing.HttpTestServerFactory
import java.nio.ByteBuffer
import java.util.concurrent.CountDownLatch
import java.util.concurrent.TimeUnit
import org.junit.After
import org.junit.Assert.fail
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class ReceiveDataTest {
  init {
    JniLibrary.loadTestLibrary()
  }

  private lateinit var httpTestServer: HttpTestServerFactory.HttpTestServer

  @Before
  fun setUp() {
    httpTestServer =
      HttpTestServerFactory.start(
        HttpTestServerFactory.Type.HTTP2_WITH_TLS,
        0,
        mapOf(),
        "data",
        mapOf()
      )
  }

  @After
  fun tearDown() {
    httpTestServer.shutdown()
  }

  @Test
  fun `response headers and response data call onResponseHeaders and onResponseData`() {
    val engine =
      EngineBuilder()
        .setLogLevel(LogLevel.DEBUG)
        .setLogger { _, msg -> print(msg) }
        .setTrustChainVerification(EnvoyConfiguration.TrustChainVerification.ACCEPT_UNTRUSTED)
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

    val headersExpectation = CountDownLatch(1)
    val dataExpectation = CountDownLatch(1)

    var status: Int? = null
    val body: ByteBuffer = ByteBuffer.allocate(4) // 4 bytes for "data"
    client
      .newStreamPrototype()
      .setOnResponseHeaders { responseHeaders, _, _ ->
        status = responseHeaders.httpStatus
        headersExpectation.countDown()
      }
      .setOnResponseData { data, _, _ ->
        body.put(data)
        dataExpectation.countDown()
      }
      .setOnError { _, _ -> fail("Unexpected error") }
      .start()
      .sendHeaders(requestHeaders, true)

    headersExpectation.await(10, TimeUnit.SECONDS)
    dataExpectation.await(10, TimeUnit.SECONDS)
    engine.terminate()

    assertThat(headersExpectation.count).isEqualTo(0)
    assertThat(dataExpectation.count).isEqualTo(0)

    assertThat(status).isEqualTo(200)
    assertThat(body.array().toString(Charsets.UTF_8)).isEqualTo("data")
  }
}
