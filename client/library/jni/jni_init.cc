#include "library/jni/jni_init.h"

#include "library/jni/jni_helper.h"
#include "library/jni/jni_utility.h"

namespace Envoy {
namespace JNI {

void initialize(JavaVM* jvm) {
  JniHelper::initialize(jvm);
  JniUtility::initCache();
  JniHelper::addToCache("io/envoyproxy/envoyclient/utilities/AndroidNetworkLibrary",
                        /* methods= */ {},
                        /* static_methods= */
                        {{"isCleartextTrafficPermitted", "(Ljava/lang/String;)Z"},
                         {"tagSocket", "(III)V"},
                         {"verifyServerCertificates",
                          "([[B[B[B)Lio/envoyproxy/envoyclient/utilities/AndroidCertVerifyResult;"},
                         {"addTestRootCertificate", "([B)V"},
                         {"clearTestRootCertificates", "()V"},
                         {"getDefaultNetworkHandle", "()J"},
                         {"getAllConnectedNetworks", "()[[J"}},
                        /* fields= */ {}, /* static_fields= */ {});
  JniHelper::addToCache("io/envoyproxy/envoyclient/utilities/AndroidCertVerifyResult",
                        /* methods= */
                        {
                            {"isIssuedByKnownRoot", "()Z"},
                            {"getStatus", "()I"},
                            {"getCertificateChainEncoded", "()[[B"},
                        },
                        /* static_methods= */ {},
                        /* fields= */ {}, /* static_fields= */ {});
  JniHelper::addToCache("io/envoyproxy/envoyclient/engine/types/EnvoyOnEngineRunning",
                        /* methods= */
                        {
                            {"invokeOnEngineRunning", "()Ljava/lang/Object;"},
                        },
                        /* static_methods= */ {},
                        /* fields= */ {}, /* static_fields= */ {});
  JniHelper::addToCache("io/envoyproxy/envoyclient/engine/types/EnvoyLogger",
                        /* methods= */
                        {
                            {"log", "(ILjava/lang/String;)V"},
                        },
                        /* static_methods= */ {},
                        /* fields= */ {}, /* static_fields= */ {});
  JniHelper::addToCache("io/envoyproxy/envoyclient/engine/types/EnvoyEventTracker",
                        /* methods= */
                        {
                            {"track", "(Ljava/util/Map;)V"},
                        },
                        /* static_methods= */ {},
                        /* fields= */ {}, /* static_fields= */ {});
  JniHelper::addToCache(
      "io/envoyproxy/envoyclient/engine/types/EnvoyHTTPCallbacks",
      /* methods= */
      {
          {"onHeaders",
           "(Ljava/util/Map;ZLio/envoyproxy/envoyclient/engine/types/EnvoyStreamIntel;)V"},
          {"onData",
           "(Ljava/nio/ByteBuffer;ZLio/envoyproxy/envoyclient/engine/types/EnvoyStreamIntel;)V"},
          {"onTrailers",
           "(Ljava/util/Map;Lio/envoyproxy/envoyclient/engine/types/EnvoyStreamIntel;)V"},
          {"onComplete", "(Lio/envoyproxy/envoyclient/engine/types/EnvoyStreamIntel;Lio/envoyproxy/"
                         "envoyclient/engine/types/EnvoyFinalStreamIntel;)V"},
          {"onError",
           "(ILjava/lang/String;ILio/envoyproxy/envoyclient/engine/types/EnvoyStreamIntel;Lio/"
           "envoyproxy/envoyclient/engine/types/EnvoyFinalStreamIntel;)V"},
          {"onCancel", "(Lio/envoyproxy/envoyclient/engine/types/EnvoyStreamIntel;Lio/envoyproxy/"
                       "envoyclient/engine/types/EnvoyFinalStreamIntel;)V"},
          {"onSendWindowAvailable", "(Lio/envoyproxy/envoyclient/engine/types/EnvoyStreamIntel;)V"},
      },
      /* static_methods= */ {},
      /* fields= */ {}, /* static_fields= */ {});
}

void finalize() { JniHelper::finalize(); }

} // namespace JNI
} // namespace Envoy
