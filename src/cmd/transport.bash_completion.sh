#!/usr/bin/env bash
#
# Copyright (C) 2022 Open Source Robotics Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# bash tab-completion

# This is a per-library function definition, used in conjunction with the
# top-level entry point in ign-tools.

GZ_SERVICE_COMPLETION_LIST="
  -h --help
  -v --version
  -s --service
  --reqtype
  --reptype
  --timeout
  -l --list
  -i --info
  -r --req
  --help-all
"

GZ_TOPIC_COMPLETION_LIST="
  -h --help
  -v --version
  -t --topic
  -m --msgtype
  -d --duration
  -n --num
  -l --list
  -i --info
  -e --echo
  -p --pub
  -v --version
  --json-output
"

function _gz_service
{
  if [[ ${COMP_WORDS[COMP_CWORD]} == -* ]]; then
    # Specify options (-*) word list for this subcommand
    COMPREPLY=($(compgen -W "$GZ_SERVICE_COMPLETION_LIST" \
      -- "${COMP_WORDS[COMP_CWORD]}" ))
    return
  else
    # Just use bash default auto-complete, because we never have two
    # subcommands in the same line. If that is ever needed, change here to
    # detect subsequent subcommands
    COMPREPLY=($(compgen -o default -- "${COMP_WORDS[COMP_CWORD]}"))
    return
  fi
}

function _gz_service_flags
{
  for word in $GZ_SERVICE_COMPLETION_LIST; do
    echo "$word"
  done
}

function _gz_topic
{
  if [[ ${COMP_WORDS[COMP_CWORD]} == -* ]]; then
    # Specify options (-*) word list for this subcommand
    COMPREPLY=($(compgen -W "$GZ_TOPIC_COMPLETION_LIST" \
      -- "${COMP_WORDS[COMP_CWORD]}" ))
    return
  else
    # Just use bash default auto-complete, because we never have two
    # subcommands in the same line. If that is ever needed, change here to
    # detect subsequent subcommands
    COMPREPLY=($(compgen -o default -- "${COMP_WORDS[COMP_CWORD]}"))
    return
  fi
}

function _gz_topic_flags
{
  for word in $GZ_TOPIC_COMPLETION_LIST; do
    echo "$word"
  done
}