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

#ifndef NBC_TA_SESSION_CONTEXT_HPP
#define NBC_TA_SESSION_CONTEXT_HPP

#include <memory>
#include <vector>

namespace nbc_ta
{

/**
 * @brief This class is used to hold the session.
 */
struct Session
{
    friend class SessionContainer;
    
    Session(void);
    virtual ~Session(void) = default;

    void initialize(const size_t initial_index_size=1);
    const std::vector<int64_t>& get_results(void) const;
    bool computed(void) const;

private:
    std::vector<int64_t> result_indexes;
    bool is_computed;
};
    
/**
 * @brief This class is container of sessions.
 */
class SessionContainer
{
public:
    
    SessionContainer(void);
    ~SessionContainer(void) = default;

    void initialize(const int32_t session_id,
                    const size_t initial_index_size);
    
    void set_results(const int32_t session_id,
                     const std::vector<int64_t>& result_indexes);

    void set_computed(const int32_t session_id,
                      const bool is_computed = true);

    const bool has_context(const int32_t session_id) const;
    const Session& get(const int32_t session_id) const;
    
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace nbc_ta */

#endif /* NBC_TA_SESSION_CONTEXT_HPP */
