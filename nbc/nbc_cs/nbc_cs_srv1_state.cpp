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
#include <nbc_cs/nbc_cs_srv1_state.hpp>

namespace nbc_cs
{
namespace srv1
{

struct StateReady::Impl
{
    Impl(void)
        : model_received_(false)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        STDSC_LOG_TRACE("StateReady: event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventEncModel:
                model_received_ = true;
                break;
            case kEventSessionCreate:
                sc.next_state(StateSessionCreated::create(model_received_));
                break;
            default:
                break;
        }
    }
private:
    bool model_received_;
};

struct StateSessionCreated::Impl
{
    Impl(const bool model_received,
         const bool input_received,
         const bool permvec_received)
        : model_received_(model_received),
          input_received_(input_received),
          permvec_received_(permvec_received)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        STDSC_LOG_TRACE("SessionCreated: event#%lu, model:%d, input:%d, permvec:%d",
                        event, model_received_, input_received_, permvec_received_);
        switch (static_cast<Event_t>(event))
        {
            case kEventEncModel:
                model_received_ = true;
                break;
            case kEventEncInput:
                input_received_ = true;
                break;
            case kEventPermVec:
                permvec_received_ = true;
                break;
            default:
                break;
        }

        if (model_received_ && input_received_ && permvec_received_) {
            sc.next_state(StateComputable::create());
        }
    }

private:
    bool model_received_;
    bool input_received_;
    bool permvec_received_;
};

struct StateComputable::Impl
{
    Impl(void)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        STDSC_LOG_TRACE("StateComputable(%lu): event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventComputeRequest:
                sc.next_state(StateComputing::create());
                break;
            case kEventSessionCreate:
                sc.next_state(StateSessionCreated::create(true, false, true));
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
            case kEventSessionCreate:
                sc.next_state(StateSessionCreated::create(true, false, true));
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

std::shared_ptr<stdsc::State> StateSessionCreated::create(const bool model_received,
                                                          const bool input_received,
                                                          const bool permvec_received)
{
    return std::shared_ptr<stdsc::State>(
        new StateSessionCreated(model_received, input_received, permvec_received));
}

StateSessionCreated::StateSessionCreated(const bool model_received,
                                         const bool input_received,
                                         const bool permvec_received)
    : pimpl_(new Impl(model_received, input_received, permvec_received))
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

} /* srv1 */
} /* nbc_cs */
