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

#include "napi_data_ability_predicates.h"

#include "common.h"
#include "js_utils.h"
#include "napi_async_proxy.h"

using namespace OHOS::NativeRdb;
using namespace OHOS::JsKit;

namespace OHOS {
namespace DataAbilityJsKit {
napi_ref DataAbilityPredicatesProxy::constructor_ = nullptr;

void DataAbilityPredicatesProxy::Init(napi_env env, napi_value exports)
{
    LOG_INFO("Init DataAbilityPredicatesProxy");
    napi_property_descriptor descriptors[] = {
        DECLARE_NAPI_FUNCTION("equalTo", EqualTo),
        DECLARE_NAPI_FUNCTION("notEqualTo", NotEqualTo),
        DECLARE_NAPI_FUNCTION("beginWrap", BeginWrap),
        DECLARE_NAPI_FUNCTION("endWrap", EndWrap),
        DECLARE_NAPI_FUNCTION("or", Or),
        DECLARE_NAPI_FUNCTION("and", And),
        DECLARE_NAPI_FUNCTION("contains", Contains),
        DECLARE_NAPI_FUNCTION("beginsWith", BeginsWith),
        DECLARE_NAPI_FUNCTION("endsWith", EndsWith),
        DECLARE_NAPI_FUNCTION("isNull", IsNull),
        DECLARE_NAPI_FUNCTION("isNotNull", IsNotNull),
        DECLARE_NAPI_FUNCTION("like", Like),
        DECLARE_NAPI_FUNCTION("glob", Glob),
        DECLARE_NAPI_FUNCTION("between", Between),
        DECLARE_NAPI_FUNCTION("notBetween", NotBetween),
        DECLARE_NAPI_FUNCTION("greaterThan", GreaterThan),
        DECLARE_NAPI_FUNCTION("lessThan", LessThan),
        DECLARE_NAPI_FUNCTION("greaterThanOrEqualTo", GreaterThanOrEqualTo),
        DECLARE_NAPI_FUNCTION("lessThanOrEqualTo", LessThanOrEqualTo),
        DECLARE_NAPI_FUNCTION("orderByAsc", OrderByAsc),
        DECLARE_NAPI_FUNCTION("orderByDesc", OrderByDesc),
        DECLARE_NAPI_FUNCTION("distinct", Distinct),
        DECLARE_NAPI_FUNCTION("limitAs", Limit),
        DECLARE_NAPI_FUNCTION("offsetAs", Offset),
        DECLARE_NAPI_FUNCTION("groupBy", GroupBy),
        DECLARE_NAPI_FUNCTION("indexedBy", IndexedBy),
        DECLARE_NAPI_FUNCTION("in", In),
        DECLARE_NAPI_FUNCTION("notIn", NotIn),

        DECLARE_NAPI_GETTER("whereClause", GetWhereClause),
        DECLARE_NAPI_GETTER("whereArgs", GetWhereArgs),
        DECLARE_NAPI_GETTER("order", GetOrder),
        DECLARE_NAPI_GETTER("limit", GetLimit),
        DECLARE_NAPI_GETTER("offset", GetOffset),
        DECLARE_NAPI_GETTER("isDistinct", IsDistinct),
        DECLARE_NAPI_GETTER("group", GetGroup),
        DECLARE_NAPI_GETTER("index", GetIndex),
        DECLARE_NAPI_GETTER("isNeedAnd", IsNeedAnd),
        DECLARE_NAPI_GETTER("isSorted", IsSorted),
    };

    napi_value cons;
    NAPI_CALL_RETURN_VOID(env, napi_define_class(env, "DataAbilityPredicates", NAPI_AUTO_LENGTH, New, nullptr,
                                   sizeof(descriptors) / sizeof(napi_property_descriptor), descriptors, &cons));

    NAPI_CALL_RETURN_VOID(env, napi_create_reference(env, cons, 1, &constructor_));

    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, exports, "DataAbilityPredicates", cons));
    LOG_DEBUG("Init DataAbilityPredicatesProxy end");
}

napi_value DataAbilityPredicatesProxy::New(napi_env env, napi_callback_info info)
{
    napi_value new_target;
    NAPI_CALL(env, napi_get_new_target(env, info, &new_target));
    bool is_constructor = (new_target != nullptr);

    napi_value _this;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &_this, nullptr));

    if (is_constructor) {
        auto *proxy = new DataAbilityPredicatesProxy();
        proxy->env_ = env;
        NAPI_CALL(env, napi_wrap(env, _this, proxy, DataAbilityPredicatesProxy::Destructor, nullptr, &proxy->wrapper_));
        LOG_INFO("DataAbilityPredicatesProxy::New constructor ref:%{public}p", proxy->wrapper_);
        return _this;
    }

    napi_value cons;
    NAPI_CALL(env, napi_get_reference_value(env, constructor_, &cons));

    napi_value output;
    NAPI_CALL(env, napi_new_instance(env, cons, 0, nullptr, &output));

    return output;
}

void DataAbilityPredicatesProxy::Destructor(napi_env env, void *nativeObject, void *)
{
    DataAbilityPredicatesProxy *proxy = static_cast<DataAbilityPredicatesProxy *>(nativeObject);
    delete proxy;
}

DataAbilityPredicatesProxy::~DataAbilityPredicatesProxy()
{
    napi_delete_reference(env_, wrapper_);
}

DataAbilityPredicatesProxy::DataAbilityPredicatesProxy()
    : predicates_(new DataAbilityPredicates()), env_(nullptr), wrapper_(nullptr)
{
}

DataAbilityPredicatesProxy::DataAbilityPredicatesProxy(DataAbilityPredicates &predicates)
    : predicates_(&predicates), env_(nullptr), wrapper_(nullptr)
{
}

NativeRdb::DataAbilityPredicates *DataAbilityPredicatesProxy::GetNativePredicates(napi_env env, napi_callback_info info)
{
    DataAbilityPredicatesProxy *predicatesProxy = nullptr;
    napi_value _this;
    napi_get_cb_info(env, info, nullptr, nullptr, &_this, nullptr);
    napi_unwrap(env, _this, reinterpret_cast<void **>(&predicatesProxy));
    return predicatesProxy->predicates_;
}

napi_value DataAbilityPredicatesProxy::EqualTo(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::EqualTo on called.");
    napi_value _this;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::EqualTo Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);
    LOG_DEBUG(
        "DataAbilityPredicatesProxy::EqualTo {field=%{public}s, value=%{public}s}.", field.c_str(), value.c_str());

    GetNativePredicates(env, info)->EqualTo(field, value);
    return _this;
}

napi_value DataAbilityPredicatesProxy::NotEqualTo(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::NotEqualTo on called.");
    napi_value _this;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::NotEqualTo Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);
    LOG_DEBUG(
        "DataAbilityPredicatesProxy::NotEqualTo {field=%{public}s, value=%{public}s}.", field.c_str(), value.c_str());

    GetNativePredicates(env, info)->NotEqualTo(field, value);
    return _this;
}

napi_value DataAbilityPredicatesProxy::BeginWrap(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::BeginWrap on called.");
    napi_value _this;
    napi_get_cb_info(env, info, nullptr, nullptr, &_this, nullptr);
    GetNativePredicates(env, info)->BeginWrap();
    return _this;
}

napi_value DataAbilityPredicatesProxy::EndWrap(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::EndWrap on called.");
    napi_value _this;
    napi_get_cb_info(env, info, nullptr, nullptr, &_this, nullptr);
    GetNativePredicates(env, info)->EndWrap();
    return _this;
}

napi_value DataAbilityPredicatesProxy::Or(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Or on called.");
    napi_value _this;
    napi_get_cb_info(env, info, nullptr, nullptr, &_this, nullptr);
    GetNativePredicates(env, info)->Or();
    return _this;
}

napi_value DataAbilityPredicatesProxy::And(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::And on called.");
    napi_value _this;
    napi_get_cb_info(env, info, nullptr, nullptr, &_this, nullptr);
    GetNativePredicates(env, info)->And();
    return _this;
}

napi_value DataAbilityPredicatesProxy::Contains(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Contains on called.");
    napi_value _this;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::Contains Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);

    GetNativePredicates(env, info)->Contains(field, value);
    return _this;
}

napi_value DataAbilityPredicatesProxy::BeginsWith(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::BeginsWith on called.");
    napi_value _this;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::BeginsWith Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);

    GetNativePredicates(env, info)->BeginsWith(field, value);
    return _this;
}

napi_value DataAbilityPredicatesProxy::EndsWith(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::EndsWith on called.");
    napi_value _this;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::EndsWith Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);

    GetNativePredicates(env, info)->EndsWith(field, value);
    return _this;
}

napi_value DataAbilityPredicatesProxy::IsNull(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::IsNull on called.");
    napi_value _this;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::IsNull Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);

    GetNativePredicates(env, info)->IsNull(field);
    return _this;
}

napi_value DataAbilityPredicatesProxy::IsNotNull(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::IsNotNull on called.");
    napi_value _this;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::IsNotNull Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);

    GetNativePredicates(env, info)->IsNotNull(field);
    return _this;
}

napi_value DataAbilityPredicatesProxy::Like(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Like on called.");
    napi_value _this;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::Like Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);

    GetNativePredicates(env, info)->Like(field, value);
    return _this;
}

napi_value DataAbilityPredicatesProxy::Glob(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Glob on called.");
    napi_value _this;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::Glob Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);

    GetNativePredicates(env, info)->Glob(field, value);
    return _this;
}

napi_value DataAbilityPredicatesProxy::Between(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Between on called.");
    napi_value _this;
    size_t argc = 3;
    napi_value args[3] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::Between Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string low = JSUtils::ConvertAny2String(env, args[1]);
    std::string high = JSUtils::ConvertAny2String(env, args[2]);
    LOG_DEBUG("DataAbilityPredicatesProxy::Between {field=%{public}s, low=%{public}s, high=%{public}s}.",
        field.c_str(), low.c_str(), high.c_str());

    GetNativePredicates(env, info)->Between(field, low, high);
    return _this;
}

napi_value DataAbilityPredicatesProxy::NotBetween(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::NotBetween on called.");
    napi_value _this;
    size_t argc = 3;
    napi_value args[3] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::NotBetween Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string low = JSUtils::ConvertAny2String(env, args[1]);
    std::string high = JSUtils::ConvertAny2String(env, args[2]);

    GetNativePredicates(env, info)->NotBetween(field, low, high);
    return _this;
}

napi_value DataAbilityPredicatesProxy::GreaterThan(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::GreaterThan on called.");
    napi_value _this;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::GreaterThan Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);

    GetNativePredicates(env, info)->GreaterThan(field, value);
    return _this;
}

napi_value DataAbilityPredicatesProxy::LessThan(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::LessThan on called.");
    napi_value _this;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::LessThan Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);

    GetNativePredicates(env, info)->LessThan(field, value);
    return _this;
}

napi_value DataAbilityPredicatesProxy::GreaterThanOrEqualTo(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::GreaterThanOrEqualTo on called.");
    napi_value _this;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::GreaterThanOrEqualTo Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);

    GetNativePredicates(env, info)->GreaterThanOrEqualTo(field, value);
    return _this;
}

napi_value DataAbilityPredicatesProxy::LessThanOrEqualTo(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::LessThanOrEqualTo on called.");
    napi_value _this;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::LessThanOrEqualTo Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);

    GetNativePredicates(env, info)->LessThanOrEqualTo(field, value);
    return _this;
}

napi_value DataAbilityPredicatesProxy::OrderByAsc(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::OrderByAsc on called.");
    napi_value _this;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::OrderByAsc Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);

    GetNativePredicates(env, info)->OrderByAsc(field);
    return _this;
}

napi_value DataAbilityPredicatesProxy::OrderByDesc(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::OrderByDesc on called.");
    napi_value _this;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::OrderByDesc Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);

    GetNativePredicates(env, info)->OrderByDesc(field);
    return _this;
}

napi_value DataAbilityPredicatesProxy::Distinct(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Distinct on called.");
    napi_value _this;
    napi_get_cb_info(env, info, nullptr, nullptr, &_this, nullptr);
    GetNativePredicates(env, info)->Distinct();
    return _this;
}

napi_value DataAbilityPredicatesProxy::Limit(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Limit on called.");
    napi_value _this;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::Limit Invalid argvs!");
    int32_t limit = 0;
    napi_get_value_int32(env, args[0], &limit);
    GetNativePredicates(env, info)->Limit(limit);
    return _this;
}

napi_value DataAbilityPredicatesProxy::Offset(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Offset on called.");
    napi_value _this;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::Offset Invalid argvs!");
    int32_t offset = 0;
    napi_get_value_int32(env, args[0], &offset);
    GetNativePredicates(env, info)->Offset(offset);
    return _this;
}

napi_value DataAbilityPredicatesProxy::GroupBy(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::GroupBy on called.");
    napi_value _this;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::GroupBy Invalid argvs!");
    std::vector<std::string> fields = JSUtils::Convert2StrVector(env, args[0], JSUtils::DEFAULT_BUF_SIZE);

    GetNativePredicates(env, info)->GroupBy(fields);
    return _this;
}

napi_value DataAbilityPredicatesProxy::IndexedBy(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::IndexedBy on called.");
    napi_value _this;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::IndexedBy Invalid argvs!");
    std::string indexName = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);

    GetNativePredicates(env, info)->IndexedBy(indexName);
    return _this;
}

napi_value DataAbilityPredicatesProxy::In(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::In on called.");
    napi_value _this;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::In Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::vector<std::string> values = JSUtils::Convert2StrVector(env, args[1], JSUtils::DEFAULT_BUF_SIZE);

    GetNativePredicates(env, info)->In(field, values);
    return _this;
}

napi_value DataAbilityPredicatesProxy::NotIn(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::NotIn on called.");
    napi_value _this;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::NotIn Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::vector<std::string> values = JSUtils::Convert2StrVector(env, args[0], JSUtils::DEFAULT_BUF_SIZE);

    GetNativePredicates(env, info)->NotIn(field, values);
    return _this;
}

NativeRdb::DataAbilityPredicates *DataAbilityPredicatesProxy::GetPredicates() const
{
    return this->predicates_;
}

napi_value DataAbilityPredicatesProxy::GetWhereClause(napi_env env, napi_callback_info info)
{
    auto ret = GetNativePredicates(env, info)->GetWhereClause();
    return JSUtils::Convert2JSValue(env, ret);
}

napi_value DataAbilityPredicatesProxy::GetWhereArgs(napi_env env, napi_callback_info info)
{
    auto ret = GetNativePredicates(env, info)->GetWhereArgs();
    return JSUtils::Convert2JSValue(env, ret);
}

napi_value DataAbilityPredicatesProxy::GetOrder(napi_env env, napi_callback_info info)
{
    auto ret = GetNativePredicates(env, info)->GetOrder();
    return JSUtils::Convert2JSValue(env, ret);
}

napi_value DataAbilityPredicatesProxy::GetLimit(napi_env env, napi_callback_info info)
{
    return JSUtils::Convert2JSValue(env, GetNativePredicates(env, info)->GetLimit());
}

napi_value DataAbilityPredicatesProxy::GetOffset(napi_env env, napi_callback_info info)
{
    return JSUtils::Convert2JSValue(env, GetNativePredicates(env, info)->GetOffset());
}

napi_value DataAbilityPredicatesProxy::IsDistinct(napi_env env, napi_callback_info info)
{
    return JSUtils::Convert2JSValue(env, GetNativePredicates(env, info)->IsDistinct());
}

napi_value DataAbilityPredicatesProxy::GetGroup(napi_env env, napi_callback_info info)
{
    auto ret = GetNativePredicates(env, info)->GetGroup();
    return JSUtils::Convert2JSValue(env, ret);
}

napi_value DataAbilityPredicatesProxy::GetIndex(napi_env env, napi_callback_info info)
{
    auto ret = GetNativePredicates(env, info)->GetIndex();
    return JSUtils::Convert2JSValue(env, ret);
}

napi_value DataAbilityPredicatesProxy::IsNeedAnd(napi_env env, napi_callback_info info)
{
    return JSUtils::Convert2JSValue(env, GetNativePredicates(env, info)->IsNeedAnd());
}

napi_value DataAbilityPredicatesProxy::IsSorted(napi_env env, napi_callback_info info)
{
    return JSUtils::Convert2JSValue(env, GetNativePredicates(env, info)->IsSorted());
}
} // namespace DataAbilityJsKit
} // namespace OHOS
