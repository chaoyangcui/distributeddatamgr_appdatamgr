/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DISTRIBUTEDDATAMGR_APPDATAMGR_JS_RESULT_SET_H
#define DISTRIBUTEDDATAMGR_APPDATAMGR_JS_RESULT_SET_H

#include <memory>
#include "abs_shared_result_set.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace RdbJsKit {
class ResultSetProxy final {
public:
    ResultSetProxy() = default;
    ~ResultSetProxy();
    ResultSetProxy(std::unique_ptr<NativeRdb::AbsSharedResultSet> resultSet);
    ResultSetProxy &operator = (std::unique_ptr<NativeRdb::AbsSharedResultSet> resultSet);
    static napi_value NewInstance(napi_env env, std::unique_ptr<NativeRdb::AbsSharedResultSet> resultSet);

private:
    static std::unique_ptr<NativeRdb::AbsSharedResultSet> &GetInnerResultSet(napi_env env, napi_callback_info info);
    static napi_value GetConstructor(napi_env env);
    static napi_value Initialize(napi_env env, napi_callback_info info);
    static napi_value GetAllColumnNames(napi_env env, napi_callback_info info);
    static napi_value GoToRow(napi_env env, napi_callback_info info);
    static napi_value GetColumnCount(napi_env env, napi_callback_info info);
    static napi_value GetLong(napi_env env, napi_callback_info info);
    static napi_value GetColumnType(napi_env env, napi_callback_info info);
    static napi_value GoTo(napi_env env, napi_callback_info info);
    static napi_value GetColumnIndex(napi_env env, napi_callback_info info);
    static napi_value GetInt(napi_env env, napi_callback_info info);
    static napi_value GetColumnName(napi_env env, napi_callback_info info);
    static napi_value Close(napi_env env, napi_callback_info info);
    static napi_value GetRowCount(napi_env env, napi_callback_info info);
    static napi_value GetRowIndex(napi_env env, napi_callback_info info);
    static napi_value IsEnded(napi_env env, napi_callback_info info);
    static napi_value IsBegin(napi_env env, napi_callback_info info);
    static napi_value GoToFirstRow(napi_env env, napi_callback_info info);
    static napi_value GoToLastRow(napi_env env, napi_callback_info info);
    static napi_value GoToNextRow(napi_env env, napi_callback_info info);
    static napi_value GoToPreviousRow(napi_env env, napi_callback_info info);
    static napi_value IsAtFirstRow(napi_env env, napi_callback_info info);
    static napi_value IsAtLastRow(napi_env env, napi_callback_info info);
    static napi_value GetBlob(napi_env env, napi_callback_info info);
    static napi_value GetString(napi_env env, napi_callback_info info);
    static napi_value GetDouble(napi_env env, napi_callback_info info);
    static napi_value IsColumnNull(napi_env env, napi_callback_info info);
    static napi_value IsClosed(napi_env env, napi_callback_info info);

    static napi_value GetSharedBlockName(napi_env env, napi_callback_info info);
    static napi_value GetSharedBlockAshmemFd(napi_env env, napi_callback_info info);

    static napi_ref ctorRef_;
    std::unique_ptr<NativeRdb::AbsSharedResultSet> resultSet_;
    napi_ref ref_ = nullptr;

    std::string sharedBlockName_;
    int32_t sharedBlockAshmemFd_;
};
}
}
#endif // DISTRIBUTEDDATAMGR_APPDATAMGR_JS_RESULT_SET_H
