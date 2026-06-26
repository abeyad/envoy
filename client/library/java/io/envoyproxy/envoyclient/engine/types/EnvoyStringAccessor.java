package io.envoyproxy.envoyclient.engine.types;

public interface EnvoyStringAccessor {

  /**
   * Called to retrieve a string from the Application
   */
  String getEnvoyString();
}
