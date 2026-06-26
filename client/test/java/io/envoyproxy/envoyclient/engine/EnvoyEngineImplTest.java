package io.envoyproxy.envoyclient.engine;

import static com.google.common.truth.Truth.assertThat;
import static org.junit.Assert.assertThrows;

import io.envoyproxy.envoyclient.engine.types.EnvoyEventTracker;
import io.envoyproxy.envoyclient.engine.types.EnvoyLogger;
import io.envoyproxy.envoyclient.engine.types.EnvoyOnEngineRunning;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.robolectric.RobolectricTestRunner;

@RunWith(RobolectricTestRunner.class)
public class EnvoyEngineImplTest {

  @BeforeClass
  public static void loadJniLibrary() {
    JniLibrary.loadTestLibrary();
  }

  @Test
  public void getEngineHandle_returnsNonZeroHandleAndThrowsAfterTerminate() {
    EnvoyOnEngineRunning onEngineRunning = () -> null;
    EnvoyLogger logger = (logLevel, str) -> {};
    EnvoyEventTracker eventTracker = events -> {};

    EnvoyEngineImpl engine =
        new EnvoyEngineImpl(onEngineRunning, logger, eventTracker, false /* disableDnsRefresh */);

    long handle = engine.getEngineHandle();
    assertThat(handle).isNotEqualTo(0L);

    engine.terminate();

    assertThrows(IllegalStateException.class, engine::getEngineHandle);
  }
}
