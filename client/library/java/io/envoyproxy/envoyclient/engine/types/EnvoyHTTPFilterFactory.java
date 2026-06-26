package io.envoyproxy.envoyclient.engine.types;

public interface EnvoyHTTPFilterFactory {

  String getFilterName();

  EnvoyHTTPFilter create();
}
