package test.kotlin.integration

import com.google.common.truth.Truth.assertThat
import io.envoyproxy.envoyclient.EngineBuilder
import io.envoyproxy.envoyclient.LogLevel
import io.envoyproxy.envoyclient.engine.JniLibrary
import java.util.concurrent.CountDownLatch
import java.util.concurrent.TimeUnit
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class EngineApiTest {
  init {
    JniLibrary.loadTestLibrary()
  }

  @Test
  fun `verify engine APIs`() {
    val countDownLatch = CountDownLatch(1)
    val engine =
      EngineBuilder()
        .setLogLevel(LogLevel.DEBUG)
        .setLogger { _, msg -> print(msg) }
        .setOnEngineRunning { countDownLatch.countDown() }
        .build()

    assertThat(countDownLatch.await(30, TimeUnit.SECONDS)).isTrue()

    engine.terminate()
  }
}
