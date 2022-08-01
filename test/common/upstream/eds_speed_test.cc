// Note: this should be run with --compilation_mode=opt, and would benefit from a
// quiescent system with disabled cstate power management.

#include "envoy/config/cluster/v3/cluster.pb.h"
#include "envoy/config/core/v3/health_check.pb.h"
#include "envoy/config/endpoint/v3/endpoint.pb.h"
#include "envoy/config/endpoint/v3/endpoint_components.pb.h"
#include "envoy/service/discovery/v3/discovery.pb.h"
#include "envoy/stats/scope.h"

#include "source/common/config/grpc_mux_impl.h"
#include "source/common/config/grpc_subscription_impl.h"
#include "source/common/config/protobuf_link_hacks.h"
#include "source/common/config/utility.h"
#include "source/common/config/xds_mux/grpc_mux_impl.h"
#include "source/common/singleton/manager_impl.h"
#include "source/common/upstream/eds.h"
#include "source/server/transport_socket_config_impl.h"

#include "test/benchmark/main.h"
#include "test/common/upstream/utility.h"
#include "test/mocks/config/custom_config_validators.h"
#include "test/mocks/local_info/mocks.h"
#include "test/mocks/protobuf/mocks.h"
#include "test/mocks/runtime/mocks.h"
#include "test/mocks/server/admin.h"
#include "test/mocks/server/instance.h"
#include "test/mocks/server/options.h"
#include "test/mocks/ssl/mocks.h"
#include "test/mocks/upstream/cluster_manager.h"
#include "test/test_common/test_runtime.h"
#include "test/test_common/utility.h"

#include "benchmark/benchmark.h"

using ::benchmark::State;
using Envoy::benchmark::skipExpensiveBenchmarks;

namespace Envoy {
namespace Upstream {

class EdsSpeedTest {
public:
  EdsSpeedTest(State& state, bool use_unified_mux)
      : state_(state), use_unified_mux_(use_unified_mux),
        type_url_("type.googleapis.com/envoy.config.endpoint.v3.ClusterLoadAssignment"),
        subscription_stats_(Config::Utility::generateStats(stats_)),
        api_(Api::createApiForTest(stats_)), async_client_(new Grpc::MockAsyncClient()),
        config_validators_(std::make_unique<NiceMock<Config::MockCustomConfigValidators>>()) {
    if (use_unified_mux_) {
      grpc_mux_.reset(new Config::XdsMux::GrpcMuxSotw(
          std::unique_ptr<Grpc::MockAsyncClient>(async_client_), dispatcher_,
          *Protobuf::DescriptorPool::generated_pool()->FindMethodByName(
              "envoy.service.endpoint.v3.EndpointDiscoveryService.StreamEndpoints"),
          random_, stats_, {}, local_info_, true, std::move(config_validators_)));
    } else {
      grpc_mux_.reset(new Config::GrpcMuxImpl(
          local_info_, std::unique_ptr<Grpc::MockAsyncClient>(async_client_), dispatcher_,
          *Protobuf::DescriptorPool::generated_pool()->FindMethodByName(
              "envoy.service.endpoint.v3.EndpointDiscoveryService.StreamEndpoints"),
          random_, stats_, {}, true, std::move(config_validators_),
          /*xds_resources_delegate=*/nullptr));
    }
    resetCluster(R"EOF(
      name: name
      connect_timeout: 0.25s
      type: EDS
      eds_cluster_config:
        service_name: fare
        eds_config:
          api_config_source:
            cluster_names:
            - eds
            refresh_delay: 1s
    )EOF",
                 Envoy::Upstream::Cluster::InitializePhase::Secondary);

    EXPECT_CALL(*cm_.subscription_factory_.subscription_, start(_));
    cluster_->initialize([this] { initialized_ = true; });
    EXPECT_CALL(*async_client_, startRaw(_, _, _, _)).WillOnce(testing::Return(&async_stream_));
    subscription_->start({"fare"});
  }

  void resetCluster(const std::string& yaml_config, Cluster::InitializePhase initialize_phase) {
    local_info_.node_.mutable_locality()->set_zone("us-east-1a");
    eds_cluster_ = parseClusterFromV3Yaml(yaml_config);
    Envoy::Stats::ScopeSharedPtr scope = stats_.createScope(fmt::format(
        "cluster.{}.",
        eds_cluster_.alt_stat_name().empty() ? eds_cluster_.name() : eds_cluster_.alt_stat_name()));
    Envoy::Server::Configuration::TransportSocketFactoryContextImpl factory_context(
        admin_, ssl_context_manager_, *scope, cm_, local_info_, dispatcher_, stats_,
        singleton_manager_, tls_, validation_visitor_, *api_, options_, access_log_manager_);
    cluster_ = std::make_shared<EdsClusterImpl>(server_context_, eds_cluster_, runtime_,
                                                factory_context, std::move(scope), false);
    EXPECT_EQ(initialize_phase, cluster_->initializePhase());
    eds_callbacks_ = cm_.subscription_factory_.callbacks_;
    subscription_ = std::make_unique<Config::GrpcSubscriptionImpl>(
        grpc_mux_, *eds_callbacks_, resource_decoder_, subscription_stats_, type_url_, dispatcher_,
        std::chrono::milliseconds(), false, Config::SubscriptionOptions());
  }

  // Set up an EDS config with multiple priorities, localities, weights and make sure
  // they are loaded as expected.
  void priorityAndLocalityWeightedHelper(bool ignore_unknown_dynamic_fields, size_t num_hosts,
                                         bool healthy) {
    state_.PauseTiming();

    envoy::config::endpoint::v3::ClusterLoadAssignment cluster_load_assignment;
    cluster_load_assignment.set_cluster_name("fare");

    // Add a whole bunch of hosts in a single place:
    auto* endpoints = cluster_load_assignment.add_endpoints();
    endpoints->set_priority(1);
    auto* locality = endpoints->mutable_locality();
    locality->set_region("region");
    locality->set_zone("zone");
    locality->set_sub_zone("sub_zone");
    endpoints->mutable_load_balancing_weight()->set_value(1);

    uint32_t port = 1000;
    for (size_t i = 0; i < num_hosts; ++i) {
      auto* lb_endpoint = endpoints->add_lb_endpoints();
      if (healthy) {
        lb_endpoint->set_health_status(envoy::config::core::v3::HEALTHY);
      } else {
        lb_endpoint->set_health_status(envoy::config::core::v3::UNHEALTHY);
      }
      auto* socket_address =
          lb_endpoint->mutable_endpoint()->mutable_address()->mutable_socket_address();
      socket_address->set_address("10.0.1." + std::to_string(i / 60000));
      socket_address->set_port_value((port + i) % 60000);
    }

    // this is what we're actually testing:
    validation_visitor_.setSkipValidation(ignore_unknown_dynamic_fields);

    auto response = std::make_unique<envoy::service::discovery::v3::DiscoveryResponse>();
    response->set_type_url(type_url_);
    response->set_version_info(fmt::format("version-{}", version_++));
    auto* resource = response->mutable_resources()->Add();
    resource->PackFrom(cluster_load_assignment);
    state_.ResumeTiming();
    if (use_unified_mux_) {
      dynamic_cast<Config::XdsMux::GrpcMuxSotw&>(*grpc_mux_)
          .grpcStreamForTest()
          .onReceiveMessage(std::move(response));
    } else {
      dynamic_cast<Config::GrpcMuxImpl&>(*grpc_mux_)
          .grpcStreamForTest()
          .onReceiveMessage(std::move(response));
    }
    ASSERT(cluster_->prioritySet().hostSetsPerPriority()[1]->hostsPerLocality().get()[0].size() ==
           num_hosts);
  }

  NiceMock<Server::Configuration::MockServerFactoryContext> server_context_;
  State& state_;
  bool use_unified_mux_;
  const std::string type_url_;
  uint64_t version_{};
  bool initialized_{};
  Stats::TestUtil::TestStore stats_;
  Config::SubscriptionStats subscription_stats_;
  Ssl::MockContextManager ssl_context_manager_;
  envoy::config::cluster::v3::Cluster eds_cluster_;
  NiceMock<MockClusterManager> cm_;
  NiceMock<Event::MockDispatcher> dispatcher_;
  EdsClusterImplSharedPtr cluster_;
  Config::SubscriptionCallbacks* eds_callbacks_{};
  Config::OpaqueResourceDecoderImpl<envoy::config::endpoint::v3::ClusterLoadAssignment>
      resource_decoder_{validation_visitor_, "cluster_name"};
  NiceMock<Random::MockRandomGenerator> random_;
  NiceMock<Runtime::MockLoader> runtime_;
  NiceMock<LocalInfo::MockLocalInfo> local_info_;
  NiceMock<Server::MockAdmin> admin_;
  Singleton::ManagerImpl singleton_manager_{Thread::threadFactoryForTest()};
  NiceMock<ThreadLocal::MockInstance> tls_;
  ProtobufMessage::MockValidationVisitor validation_visitor_;
  Api::ApiPtr api_;
  Server::MockOptions options_;
  Grpc::MockAsyncClient* async_client_;
  Config::CustomConfigValidatorsPtr config_validators_;
  NiceMock<Grpc::MockAsyncStream> async_stream_;
  Config::GrpcMuxSharedPtr grpc_mux_;
  Config::GrpcSubscriptionImplPtr subscription_;
  NiceMock<AccessLog::MockAccessLogManager> access_log_manager_;
};

} // namespace Upstream
} // namespace Envoy

static void priorityAndLocalityWeighted(State& state) {
  Envoy::Thread::MutexBasicLockable lock;
  Envoy::Logger::Context logging_state(spdlog::level::warn,
                                       Envoy::Logger::Logger::DEFAULT_LOG_FORMAT, lock, false);
  for (auto _ : state) { // NOLINT: Silences warning about dead store
    Envoy::Upstream::EdsSpeedTest speed_test(state, state.range(2));
    // if we've been instructed to skip tests, only run once no matter the argument:
    uint32_t endpoints = skipExpensiveBenchmarks() ? 1 : state.range(1);

    speed_test.priorityAndLocalityWeightedHelper(state.range(0), endpoints, true);
  }
}

BENCHMARK(priorityAndLocalityWeighted)
    ->Ranges({{false, true}, {1, 100000}, {false, true}})
    ->Unit(benchmark::kMillisecond);

static void duplicateUpdate(State& state) {
  Envoy::Thread::MutexBasicLockable lock;
  Envoy::Logger::Context logging_state(spdlog::level::warn,
                                       Envoy::Logger::Logger::DEFAULT_LOG_FORMAT, lock, false);

  for (auto _ : state) { // NOLINT: Silences warning about dead store
    Envoy::Upstream::EdsSpeedTest speed_test(state, state.range(1));
    uint32_t endpoints = skipExpensiveBenchmarks() ? 1 : state.range(0);

    speed_test.priorityAndLocalityWeightedHelper(true, endpoints, true);
    speed_test.priorityAndLocalityWeightedHelper(true, endpoints, true);
  }
}

BENCHMARK(duplicateUpdate)->Ranges({{1, 100000}, {false, true}})->Unit(benchmark::kMillisecond);

static void healthOnlyUpdate(State& state) {
  Envoy::Thread::MutexBasicLockable lock;
  Envoy::Logger::Context logging_state(spdlog::level::warn,
                                       Envoy::Logger::Logger::DEFAULT_LOG_FORMAT, lock, false);
  for (auto _ : state) { // NOLINT: Silences warning about dead store
    Envoy::Upstream::EdsSpeedTest speed_test(state, state.range(1));
    uint32_t endpoints = skipExpensiveBenchmarks() ? 1 : state.range(0);

    speed_test.priorityAndLocalityWeightedHelper(true, endpoints, true);
    speed_test.priorityAndLocalityWeightedHelper(true, endpoints, false);
  }
}

BENCHMARK(healthOnlyUpdate)->Ranges({{1, 100000}, {false, true}})->Unit(benchmark::kMillisecond);
