/*
 * Copyright 2018 Yamana Laboratory, Waseda University
 * Supported by JST CREST Grant Number JPMJCR1503, Japan.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE‐2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_log.hpp>
#include <nbc_ta/nbc_ta_srv2_state.hpp>

namespace nbc_ta
{
namespace srv2
{

struct StateReady::Impl
{
    Impl(void)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        STDSC_LOG_TRACE("StateReady: event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventSessionCreate:
                sc.next_state(StateSessionCreated::create());
                break;
            default:
                break;
        }
    }
};

struct StateSessionCreated::Impl
{
    Impl()
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        STDSC_LOG_TRACE("SessionCreated: event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventBeginRequest:
                sc.next_state(StateComputable::create());
                break;
            default:
                break;
        }
    }
};

struct StateComputable::Impl
{
    Impl(void)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        STDSC_LOG_TRACE("StateComputable: event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventComputeRequest:
                sc.next_state(StateComputing::create());
                break;
            case kEventEndRequest:
                sc.next_state(StateSessionCreated::create());
                break;
            default:
                break;
        }
    }
};

struct StateComputing::Impl
{
    Impl(void)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        STDSC_LOG_TRACE("StateComputing(%lu): event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventBeginRequest:
                sc.next_state(StateComputable::create());
                break;
            case kEventEndRequest:
                sc.next_state(StateSessionCreated::create());
                break;
            default:
                break;
        }
    }
};

// Ready

std::shared_ptr<stdsc::State> StateReady::create(void)
{
    return std::shared_ptr<stdsc::State>(new StateReady());
}

StateReady::StateReady(void)
  : pimpl_(new Impl())
{
}

void StateReady::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

// SessionCreated

std::shared_ptr<stdsc::State> StateSessionCreated::create(void)
{
    return std::shared_ptr<stdsc::State>(new StateSessionCreated());
}

StateSessionCreated::StateSessionCreated(void)
  : pimpl_(new Impl())
{
}

void StateSessionCreated::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

// Computable

std::shared_ptr<stdsc::State> StateComputable::create()
{
    return std::shared_ptr<stdsc::State>(new StateComputable());
}

StateComputable::StateComputable(void)
    : pimpl_(new Impl())
{
}

void StateComputable::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

// Computing

std::shared_ptr<stdsc::State> StateComputing::create()
{
    return std::shared_ptr<stdsc::State>(new StateComputing());
}

StateComputing::StateComputing(void)
    : pimpl_(new Impl())
{
}

void StateComputing::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

} /* srv2 */
} /* nbc_ta */
