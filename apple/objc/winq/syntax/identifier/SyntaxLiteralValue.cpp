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

#include <WCDB/Syntax.h>
#include <WCDB/SyntaxAssertion.hpp>

namespace WCDB {

namespace Syntax {

#pragma mark - Identifier
Identifier::Type LiteralValue::getType() const
{
    return type;
}

StringView LiteralValue::getValidDescription() const
{
    std::ostringstream stream;
    switch (switcher) {
    case Switch::StringView: {
        stream << "'";
        for (const auto character : stringValue) {
            if (character != '\'') {
                stream << character;
            } else {
                stream << "''";
            }
        }
        stream << "'";
    } break;
    case Switch::Null:
        stream << "NULL";
        break;
    case Switch::Float:
        stream << floatValue;
        break;
    case Switch::Integer:
        stream << integerValue;
        break;
    case Switch::UnsignedInteger:
        stream << unsignedIntegerValue;
        break;
    case Switch::Bool:
        stream << (boolValue ? "TRUE" : "FALSE");
        break;
    case Switch::CurrentTime:
        stream << "CURRENT_TIME";
        break;
    case Switch::CurrentDate:
        stream << "CURRENT_DATE";
        break;
    case Switch::CurrentTimestamp:
        stream << "CURRENT_TIMESTAMP";
        break;
    }
    return StringView(stream.str());
}

} // namespace Syntax

} // namespace WCDB
