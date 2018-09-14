#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif

#include "ranking_event.h"
#include <boost/test/unit_test.hpp>
#include "ranking_response.h"
#include "utility/data_buffer.h"

using namespace reinforcement_learning;
using namespace std;

BOOST_AUTO_TEST_CASE(serialize_outcome)
{
  const auto event_id = "event_id";
  const auto outcome = 1.0f;

  utility::data_buffer oss;
  outcome_event::serialize(oss, event_id, outcome);
  const auto serialized_str = oss.str();
  const char * expected = R"({"EventId":"event_id","v":1.000000})";

  BOOST_CHECK_EQUAL(serialized_str.c_str(), expected);
}

BOOST_AUTO_TEST_CASE(serialize_empty_outcome)
{
  const auto event_id = "";
  const auto outcome = "{}";

  utility::data_buffer oss;
  outcome_event::serialize(oss, event_id, outcome);
  const auto serialized = oss.str();
  const auto expected = R"({"EventId":"","v":{}})";

  BOOST_CHECK_EQUAL(serialized.c_str(), expected);
}

BOOST_AUTO_TEST_CASE(serialize_ranking)
{
  const auto event_id = "event_id";
  const auto context = "{context}";
  ranking_response resp;
  resp.push_back(1, 0.8f);
  resp.push_back(0, 0.2f);
  resp.set_model_id("model_id");

  utility::data_buffer oss;
  ranking_event::serialize(oss, event_id, context, resp);
  const std::string serialized = oss.str();
  const auto expected = R"({"Version":"1","EventId":"event_id","a":[2,1],"c":{context},"p":[0.800000,0.200000],"VWState":{"m":"model_id"}})";

  BOOST_CHECK_EQUAL(serialized.c_str(), expected);
}

BOOST_AUTO_TEST_CASE(serialize_empty_ranking)
{
  const auto event_id = "event_id";
  const auto context = "{context}";
  ranking_response ranking;
  ranking.set_model_id("model_id");

  utility::data_buffer oss;
  ranking_event::serialize(oss, event_id, context, ranking);
  const auto serialized = oss.str();
  const auto expected = R"({"Version":"1","EventId":"event_id","a":[],"c":{context},"p":[],"VWState":{"m":"model_id"}})";

  BOOST_CHECK_EQUAL(serialized.c_str(), expected);
}

BOOST_AUTO_TEST_CASE(interaction_message_survive_test) {
  utility::data_buffer buffer;
  utility::data_buffer expected_buffer;
  ranking_response resp("interaction_id");
  resp.push_back(1, 0.1);
  resp.push_back(2, 0.2);
  resp.set_chosen_action_id(1);

  ranking_event evt(buffer, "interaction_id", "interaction_context", resp);

  ranking_event expected(expected_buffer, "interaction_id", "interaction_context", resp, 0.25);

  evt.try_drop(0.5, 1);
  evt.try_drop(0.5, 1);

  BOOST_CHECK_EQUAL(evt.str(), expected.str());
}
