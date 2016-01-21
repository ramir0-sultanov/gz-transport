/*
 * Copyright (C) 2014 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
#include <chrono>
#include <cstdlib>
#include <string>
#include "ignition/transport/Node.hh"
#include "ignition/transport/test_config.h"
#include "ignition/transport/TopicUtils.hh"
#include "gtest/gtest.h"
#include "msgs/ign_int.pb.h"
#include "msgs/ign_vector3d.pb.h"

using namespace ignition;

bool srvExecuted;
bool responseExecuted;
bool wrongResponseExecuted;

std::string partition;
std::string topic = "/foo";
int data = 5;
int counter = 0;

//////////////////////////////////////////////////
/// \brief Initialize some global variables.
void reset()
{
  responseExecuted = false;
  wrongResponseExecuted = false;
  counter = 0;
}

//////////////////////////////////////////////////
/// \brief Service call response callback.
void response(const transport::msgs::IgnInt &_rep, const bool _result)
{
  EXPECT_EQ(_rep.data(), data);
  EXPECT_TRUE(_result);

  responseExecuted = true;
  ++counter;
}

//////////////////////////////////////////////////
/// \brief Service call response callback.
void wrongResponse(const transport::msgs::IgnVector3d &/*_rep*/,
  bool /*_result*/)
{
  wrongResponseExecuted = true;
}

//////////////////////////////////////////////////
/// \brief Two different nodes running in two different processes. One node
/// advertises a service and the other requests a few service calls.
TEST(twoProcSrvCall, SrvTwoProcs)
{
  std::string responser_path = testing::portablePathUnion(
    PROJECT_BINARY_PATH,
    "test/integration/INTEGRATION_twoProcessesSrvCallReplier_aux");

  testing::forkHandlerType pi = testing::forkAndRun(responser_path.c_str(),
    partition.c_str());

  responseExecuted = false;
  counter = 0;
  transport::msgs::IgnInt req;
  req.set_data(data);

  transport::Node node;
  EXPECT_TRUE(node.Request(topic, req, response));

  int i = 0;
  while (i < 300 && !responseExecuted)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ++i;
  }

  // Check that the service call response was executed.
  EXPECT_TRUE(responseExecuted);
  EXPECT_EQ(counter, 1);

  // Make another request.
  responseExecuted = false;
  counter = 0;
  EXPECT_TRUE(node.Request(topic, req, response));

  i = 0;
  while (i < 300 && !responseExecuted)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ++i;
  }

  // Check that the service call response was executed.
  EXPECT_TRUE(responseExecuted);
  EXPECT_EQ(counter, 1);

  // Wait for the child process to return.
  testing::waitAndCleanupFork(pi);
}

//////////////////////////////////////////////////
/// \brief This test spawns a service responser and a service requester. The
/// requester uses a wrong type for the request argument. The test should verify
/// that the service call does not succeed.
TEST(twoProcSrvCall, SrvRequestWrongReq)
{
  transport::msgs::IgnVector3d wrongReq;
  transport::msgs::IgnInt rep;
  bool result;
  unsigned int timeout = 1000;

  std::string responser_path = testing::portablePathUnion(
     PROJECT_BINARY_PATH,
     "test/integration/INTEGRATION_twoProcessesSrvCallReplier_aux");

  testing::forkHandlerType pi = testing::forkAndRun(responser_path.c_str(),
    partition.c_str());

  wrongReq.set_x(1);
  wrongReq.set_y(2);
  wrongReq.set_z(3);

  reset();

  transport::Node node;

  // Request an asynchronous service call with wrong type in the request.
  EXPECT_TRUE(node.Request(topic, wrongReq, response));
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  EXPECT_FALSE(responseExecuted);

  // Request a synchronous service call with wrong type in the request.
  EXPECT_FALSE(node.Request(topic, wrongReq, timeout, rep, result));

  reset();

  // Wait for the child process to return.
  testing::waitAndCleanupFork(pi);
}

//////////////////////////////////////////////////
/// \brief This test spawns a service responser and a service requester. The
/// requester uses a wrong type for the response argument. The test should
/// verify that the service call does not succeed.
TEST(twoProcSrvCall, SrvRequestWrongRep)
{
  transport::msgs::IgnInt req;
  transport::msgs::IgnVector3d wrongRep;
  bool result;
  unsigned int timeout = 1000;

  std::string responser_path = testing::portablePathUnion(
     PROJECT_BINARY_PATH,
     "test/integration/INTEGRATION_twoProcessesSrvCallReplier_aux");


  testing::forkHandlerType pi = testing::forkAndRun(responser_path.c_str(),
    partition.c_str());

  req.set_data(data);

  reset();

  transport::Node node;

  // Request an asynchronous service call with wrong type in the response.
  EXPECT_TRUE(node.Request(topic, req, wrongResponse));
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  EXPECT_FALSE(wrongResponseExecuted);

  // Request a synchronous service call with wrong type in the response.
  EXPECT_FALSE(node.Request(topic, req, timeout, wrongRep, result));

  reset();

  // Wait for the child process to return.
  testing::waitAndCleanupFork(pi);
}

//////////////////////////////////////////////////
/// \brief This test spawns a service responser and two service requesters. One
/// requester uses wrong type arguments. The test should verify that only one
/// of the requesters receives the response.
TEST(twoProcSrvCall, SrvTwoRequestsOneWrong)
{
  transport::msgs::IgnInt req;
  transport::msgs::IgnInt goodRep;
  transport::msgs::IgnVector3d badRep;
  bool result;
  unsigned int timeout = 1000;

  std::string responser_path = testing::portablePathUnion(
     PROJECT_BINARY_PATH,
     "test/integration/INTEGRATION_twoProcessesSrvCallReplier_aux");

  testing::forkHandlerType pi = testing::forkAndRun(responser_path.c_str(),
    partition.c_str());

  req.set_data(data);

  reset();

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  transport::Node node;

  // Request service calls with wrong types in the response.
  EXPECT_FALSE(node.Request(topic, req, timeout, badRep, result));
  EXPECT_TRUE(node.Request(topic, req, wrongResponse));
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  EXPECT_FALSE(wrongResponseExecuted);

  reset();

  // Valid service requests.
  EXPECT_TRUE(node.Request(topic, req, timeout, goodRep, result));
  EXPECT_TRUE(node.Request(topic, req, response));
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  EXPECT_TRUE(responseExecuted);

  reset();

  // Wait for the child process to return.
  testing::waitAndCleanupFork(pi);
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  // Get a random partition name.
  partition = testing::getRandomNumber();

  // Set the partition name for this process.
  setenv("IGN_PARTITION", partition.c_str(), 1);

  // Enable verbose mode.
  // setenv("IGN_VERBOSE", "1", 1);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
