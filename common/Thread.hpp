/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <WCDB/SharedThreadedErrorProne.hpp>
#include <WCDB/StringView.hpp>
#include <cstddef>
#include <pthread.h>

namespace WCDB {

class Thread : public SharedThreadedErrorProne {
#pragma mark - Initialize
public:
    Thread();
    Thread& operator=(const std::nullptr_t&);

private:
    Thread(pthread_t id);
    friend class Conditional;
    pthread_t m_id;

#pragma mark - Which
public:
    static Thread current();
    static bool isMain();
    bool isCurrentThread() const;

    bool equal(const Thread& other) const;
    bool operator==(const Thread& other) const;

private:
    static bool pthreadEqual(pthread_t left, pthread_t right);

#pragma mark - Name
public:
    static void setName(const UnsafeStringView& name);
    StringView getName();
    uint64_t getIdentifier();

private:
    static constexpr const int maxLengthOfAllowedThreadName();

#pragma mark - Error
private:
    static void setThreadedError();
};

} // namespace WCDB
