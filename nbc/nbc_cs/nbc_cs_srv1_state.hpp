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

#ifndef NBC_CS_SRV1_STATE_HPP
#define NBC_CS_SRV1_STATE_HPP

#include <cstdbool>

#include <memory>
#include <stdsc/stdsc_state.hpp>

namespace nbc_cs
{
namespace srv1
{

/**
 * @brief Enumeration for state of Server#1 on CS.
 */
enum StateId_t : int32_t
{
    kStateNil            = 0,
    kStateReady          = 1,
    kStateSessionCreated = 2,
    kStateComputable     = 3,
    kStateComputing       = 4,
};

/**
 * @brief Enumeration for events of PSP.
 */
enum Event_t : uint64_t
{
    kEventNil            = 0,
    kEventSessionCreate  = 1,
    kEventEncModel       = 2,
    kEventEncInput       = 3,
    kEventPermVec        = 4,
    kEventComputeRequest = 5,
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

/**
 * @brief Provides 'SessionCreated' state.
 */
struct StateSessionCreated : public stdsc::State
{
    static std::shared_ptr<stdsc::State> create(const bool model_received   = false,
                                                const bool input_received   = false,
                                                const bool permvec_received = false);
    StateSessionCreated(const bool model_received   = false,
                        const bool input_received   = false,
                        const bool permvec_received = false);
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    STDSC_STATE_DEFID(kStateSessionCreated);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief Provides 'Computable' state.
 */
struct StateComputable : public stdsc::State
{
    static std::shared_ptr<State> create(void);
    StateComputable(void);
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    STDSC_STATE_DEFID(kStateComputable);
    
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief Provides 'Computing' state.
 */
struct StateComputing : public stdsc::State
{
    static std::shared_ptr<State> create(void);
    StateComputing(void);
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    STDSC_STATE_DEFID(kStateComputing);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* srv1 */
} /* nbc_cs */

#endif /* NBC_CS_SRV1_STATE_HPP */
