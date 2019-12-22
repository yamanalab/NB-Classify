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

#ifndef NBC_TA_SRV1_STATE_HPP
#define NBC_TA_SRV1_STATE_HPP

#include <cstdbool>

#include <memory>
#include <stdsc/stdsc_state.hpp>

namespace nbc_ta
{
namespace srv1
{

/**
 * @brief Enumeration for state of Server#1 on TA.
 */
enum StateId_t : int32_t
{
    kStateNil   = 0,
    kStateReady = 1,
    kStateExit  = 2,
};

/**
 * @brief Enumeration for events of PSP.
 */
enum Event_t : uint64_t
{
    kEventNil            = 0,
    kEventPubkeyRequest  = 1,
    kEventResultRequest  = 2,
    kEventContextRequest = 3,
};

/**
 * @brief Provides 'Ready' state.
 */
struct StateReady : public stdsc::State
{
    static std::shared_ptr<State> create(void);
    StateReady(void);
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    STDSC_STATE_DEFID(kStateReady);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* srv1 */
} /* nbc_ta */

#endif /* NBC_TA_SRV1_STATE_HPP */
