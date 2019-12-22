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

#ifndef NBC_CS_SRV1_CALLBACK_FUNCTION_HPP
#define NBC_CS_SRV1_CALLBACK_FUNCTION_HPP

#include <stdsc/stdsc_callback_function.hpp>

namespace nbc_cs
{
struct CallbackParam;
}

namespace nbc_cs
{
namespace srv1
{

using CallbackParam = nbc_cs::CallbackParam;

/**
 * @brief Provides callback function in receiving session create.
 */
DECLARE_DOWNLOAD_CLASS(CallbackFunctionSessionCreate);

/**
 * @brief Provides callback function in receiving enc model.
 */
DECLARE_DATA_CLASS(CallbackFunctionEncModel);

/**
 * @brief Provides callback function in receiving enc input.
 */
DECLARE_DATA_CLASS(CallbackFunctionEncInput);

///**
// * @brief Provides callback function in receiving perm vector.
// */
//DECLARE_DATA_CLASS(CallbackFunctionPermVec);

/**
 * @brief Provides callback function in receiving compute request.
 */
DECLARE_DATA_CLASS(CallbackFunctionComputeRequest);

} /* namespace srv1 */
} /* namespace nbc_cs */

#endif /* NBC_CS_SRV1_CALLBACK_FUNCTION_HPP */
