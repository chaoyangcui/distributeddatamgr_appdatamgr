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

#include "napi_predicates_utils.h"

#include "common.h"
#include "js_utils.h"
#include "napi_data_ability_predicates.h"
#include "napi_rdb_predicates.h"
#include "predicates_utils.h"

namespace OHOS {
namespace DataAbilityJsKit {
napi_value CreateRdbPredicates(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_value _this;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));
    NAPI_ASSERT(env, argc == 2, "DataAbilityJsKit::CreateRdbPredicates Invalid argvs!");

    LOG_DEBUG("DataAbilityJsKit::CreateRdbPredicates argc is %{public}zu", argc);
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, args[0], &valueType));
    NAPI_ASSERT(env, valueType == napi_string, "Table name should be a string.");
    std::string tableName = JsKit::JSUtils::Convert2String(env, args[0], JsKit::JSUtils::DEFAULT_BUF_SIZE);

    NAPI_CALL(env, napi_typeof(env, args[1], &valueType));
    NAPI_ASSERT(env, valueType == napi_object, "Table name should be an object.");
    DataAbilityPredicatesProxy *dataAbilityPredicatesProxy = nullptr;
    NAPI_CALL(env, napi_unwrap(env, args[1], reinterpret_cast<void **>(&dataAbilityPredicatesProxy)));

    auto absPredicates = dataAbilityPredicatesProxy->GetPredicates();
    auto proxy = new RdbJsKit::RdbPredicatesProxy(tableName);
    NativeRdb::PredicatesUtils::SetWhereClauseAndArgs(
        proxy->GetPredicates(), absPredicates->GetWhereClause(), absPredicates->GetWhereArgs());
    NativeRdb::PredicatesUtils::SetAttributes(proxy->GetPredicates(), absPredicates->IsDistinct(),
        absPredicates->GetIndex(), absPredicates->GetGroup(), absPredicates->GetOrder(), absPredicates->GetLimit(),
        absPredicates->GetOffset());

    napi_ref wrapper;
    NAPI_CALL(env, napi_wrap(env, _this, proxy, RdbJsKit::RdbPredicatesProxy::Destructor, nullptr, &wrapper));
    proxy->setProperties(env, wrapper);
    return _this;
}

napi_value InitPredicatesUtils(napi_env env, napi_value exports)
{
    LOG_INFO("Init InitPredicatesUtils");
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("createRdbPredicates", CreateRdbPredicates),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(*properties), properties));
    LOG_INFO("Init InitPredicatesUtils end");
    return exports;
}
} // namespace DataAbilityJsKit
} // namespace OHOS
