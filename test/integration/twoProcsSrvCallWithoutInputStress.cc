/*
 * Copyright (C) 2016 Open Source Robotics Foundation
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
#include <gz/msgs/int32.pb.h>

#include <chrono>
#include <cstdlib>
#include <string>

#include "gz/transport/Node.hh"

#include <gz/utils/Environment.hh>
#include <gz/utils/Subprocess.hh>

#include "gtest/gtest.h"

#include "test_config.hh"
#include "test_utils.hh"

using namespace gz;

using twoProcSrvCallWithoutInput = testing::PartitionedTransportTest;

static std::string g_topic = "/foo"; // NOLINT(*)

//////////////////////////////////////////////////
TEST_F(twoProcSrvCallWithoutInput, ThousandCalls)
{
  this->SpawnSubprocess({test_executables::kTwoProcsSrvCallWithoutInputReplierInc});

  msgs::Int32 response;
  bool result;
  unsigned int timeout = 1000;
  transport::Node node;

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < 15000; i++)
  {
    ASSERT_TRUE(node.Request(g_topic, timeout, response, result));

    // Check the service response.
    ASSERT_TRUE(result);
  }
}
