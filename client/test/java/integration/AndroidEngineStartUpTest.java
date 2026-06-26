package test.java.integration;

import android.content.Context;
import androidx.test.core.app.ApplicationProvider;
import io.envoyproxy.envoyclient.AndroidEngineBuilder;
import io.envoyproxy.envoyclient.Engine;
import io.envoyproxy.envoyclient.LogLevel;
import io.envoyproxy.envoyclient.engine.JniLibrary;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.robolectric.RobolectricTestRunner;
import org.junit.BeforeClass;

import static com.google.common.truth.Truth.assertThat;

@RunWith(RobolectricTestRunner.class)
public class AndroidEngineStartUpTest {
  private final Context appContext = ApplicationProvider.getApplicationContext();

  @BeforeClass
  public static void loadJniLibrary() {
    JniLibrary.loadTestLibrary();
  }

  @Test
  public void ensure_engine_starts_and_terminates() throws InterruptedException {
    Engine engine = new AndroidEngineBuilder(appContext)
                        .setLogLevel(LogLevel.DEBUG)
                        .setLogger((level, message) -> {
                          System.out.print(message);
                          return null;
                        })
                        .build();
    Thread.sleep(1000);
    engine.terminate();
    assertThat(true).isTrue();
  }
}
