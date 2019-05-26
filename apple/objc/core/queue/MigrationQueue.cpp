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

#include <WCDB/Assertion.hpp>
#include <WCDB/CoreConst.h>
#include <WCDB/Error.hpp>
#include <WCDB/MigrationQueue.hpp>
#include <WCDB/Notifier.hpp>

namespace WCDB {

MigrationQueueEvent::~MigrationQueueEvent()
{
}

MigrationQueue::MigrationQueue(const String& name, MigrationQueueEvent* event)
: AsyncQueue(name), m_event(event)
{
    WCTInnerAssert(m_event != nullptr);
}

void MigrationQueue::register_(const String& path)
{
    LockGuard lockGuard(m_lock);
    auto iter = m_records.find(path);
    if (iter == m_records.end()) {
        m_records.emplace(path, 1);
        put(path);
    } else {
        WCTInnerAssert(iter->second > 0);
        ++iter->second;
    }
}

void MigrationQueue::unregister(const String& path)
{
    LockGuard lockGuard(m_lock);
    WCTInnerAssert(m_records[path] >= 0);
    if (--m_records[path] == 0) {
        m_records.erase(path);
        m_timedQueue.remove(path);
    }
}

void MigrationQueue::put(const String& path)
{
    put(path, MigrationQueueTimeIntervalForMigrating, 0);
}

void MigrationQueue::put(const String& path, double delay, int numberOfFailures)
{
    SharedLockGuard lockGuard(m_lock);
    auto iter = m_records.find(path);
    if (iter != m_records.end()) {
        WCTInnerAssert(iter->second > 0);
        m_timedQueue.queue(path, delay, numberOfFailures);
        lazyRun();
    }
}

void MigrationQueue::loop()
{
    m_timedQueue.loop(std::bind(
    &MigrationQueue::onTimed, this, std::placeholders::_1, std::placeholders::_2));
}

void MigrationQueue::onTimed(const String& path, const int& numberOfFailures)
{
    bool succeed, done;
    std::tie(succeed, done) = m_event->databaseShouldMigrate(path);
    if (succeed) {
        if (!done) {
            put(path, MigrationQueueTimeIntervalForMigrating, numberOfFailures);
        }
    } else {
        if (numberOfFailures + 1 < MigrationQueueTolerableFailures) {
            put(path, MigrationQueueTimeIntervalForRetryingAfterFailure, numberOfFailures + 1);
        } else {
            Error error(Error::Code::Notice, Error::Level::Notice, "Async migration stopped due to the error.");
            error.infos.set(ErrorStringKeyPath, path);
            Notifier::shared().notify(error);
        }
    }
}

} // namespace WCDB
