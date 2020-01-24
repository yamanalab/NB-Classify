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

#ifndef NBC_CS_SRV1_HPP
#define NBC_CS_SRV1_HPP

#include <memory>
#include <string>

namespace stdsc
{
class CallbackFunctionContainer;
class StateContext;
}

namespace nbc_cs
{
namespace srv1
{

/**
 * @brief Provides Server#1 on CS.
 */
class CSServer
{
public:
    CSServer(const char* port, stdsc::CallbackFunctionContainer& callback,
             stdsc::StateContext& state);
    ~CSServer(void) = default;

    void start(void);
    void stop(void);
    void wait(void);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace srv1 */
} /* namespace nbc_cs */

#endif /* NBC_CS_SRV1_HPP */
