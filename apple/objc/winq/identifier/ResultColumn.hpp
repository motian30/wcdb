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

#include <WCDB/SQL.hpp>

namespace WCDB {

class ResultColumnAll;

class ResultColumn : public TypedSyntax<Syntax::ResultColumn, SQL> {
public:
    virtual ~ResultColumn();
    using TypedSyntax<Syntax::ResultColumn, SQL>::TypedSyntax;

    template<typename T, typename Enable = typename std::enable_if<ResultColumnConvertible<T>::value>::type>
    ResultColumn(const T& t)
    : ResultColumn(ResultColumnConvertible<T>::asResultColumn(t))
    {
    }

    ResultColumn(const Expression& expression);
    ResultColumn& as(const UnsafeStringView& alias);

    static ResultColumnAll all();
};

class ResultColumnAll final : public ResultColumn {
public:
    ResultColumnAll();

    Expression count() const;
    ResultColumnAll& inTable(const UnsafeStringView& table);

protected:
    using ResultColumn::as;
};

} // namespace WCDB
