#include "CefViewBridgeObject.h"

#include <Common/CefViewCoreLog.h>

#include <CefViewCoreProtocol.h>

CefViewBridgeObject::V8Handler::V8Handler(CefViewBridgeObject* object)
  : object_(object)
{
}

bool
CefViewBridgeObject::V8Handler::Execute(const CefString& function,
                                        CefRefPtr<CefV8Value> object,
                                        const CefV8ValueList& arguments,
                                        CefRefPtr<CefV8Value>& retval,
                                        CefString& exception)
{
  if (function == kCefViewInvokeMethodFunctionName)
    ExecuteNativeMethod(object, arguments, retval, exception);
  else if (function == kCefViewAddEventListenerFunctionName)
    ExecuteAddEventListener(object, arguments, retval, exception);
  else if (function == kCefViewRemoveEventListenerFunctionName)
    ExecuteRemoveEventListener(object, arguments, retval, exception);
  else if (function == kCefViewReportJSResultFunctionName)
    ExecuteReportJSResult(object, arguments, retval, exception);
  else
    return false;

  return true;
}

void
CefViewBridgeObject::V8Handler::ExecuteNativeMethod(CefRefPtr<CefV8Value> object,
                                                    const CefV8ValueList& arguments,
                                                    CefRefPtr<CefV8Value>& retval,
                                                    CefString& exception)
{
  object_->AsyncExecuteNativeMethod(arguments);
  retval = CefV8Value::CreateUndefined();
}

void
CefViewBridgeObject::V8Handler::ExecuteAddEventListener(CefRefPtr<CefV8Value> object,
                                                        const CefV8ValueList& arguments,
                                                        CefRefPtr<CefV8Value>& retval,
                                                        CefString& exception)
{
  bool bRet = false;

  if (arguments.size() == 2) {
    if (arguments[0]->IsString()) {
      if (arguments[1]->IsFunction()) {
        CefString eventName = arguments[0]->GetStringValue();
        EventListener listener;
        listener.callback_ = arguments[1];
        listener.context_ = CefV8Context::GetCurrentContext();
        object_->AddEventListener(eventName, listener);
        bRet = true;
      } else
        exception = "Invalid arguments; argument 2 must be a function";
    } else
      exception = "Invalid arguments; argument 1 must be a string";
  } else
    exception = "Invalid arguments; expecting 2 arguments";

  retval = CefV8Value::CreateBool(bRet);
}

void
CefViewBridgeObject::V8Handler::ExecuteRemoveEventListener(CefRefPtr<CefV8Value> object,
                                                           const CefV8ValueList& arguments,
                                                           CefRefPtr<CefV8Value>& retval,
                                                           CefString& exception)
{
  bool bRet = false;

  if (arguments.size() == 2) {
    if (arguments[0]->IsString()) {
      if (arguments[1]->IsFunction()) {
        CefString eventName = arguments[0]->GetStringValue();
        EventListener listener;
        listener.callback_ = arguments[1];
        listener.context_ = CefV8Context::GetCurrentContext();
        object_->RemoveEventListener(eventName, listener);
        bRet = true;
      } else
        exception = "Invalid arguments; argument 2 must be a function";
    } else
      exception = "Invalid arguments; argument 1 must be a string";
  } else
    exception = "Invalid arguments; expecting 2 arguments";

  retval = CefV8Value::CreateBool(bRet);
}

void
CefViewBridgeObject::V8Handler::ExecuteReportJSResult(CefRefPtr<CefV8Value> object,
                                                      const CefV8ValueList& arguments,
                                                      CefRefPtr<CefV8Value>& retval,
                                                      CefString& exception)
{
  if (arguments.size() == 2) {
    if (arguments[0]->IsString()) {
      object_->AsyncExecuteReportJSResult(arguments);
    } else
      exception = "Invalid argument; argument 1 must be a double";
  } else
    exception = "Invalid argument; expecting 2 argument";

  retval = CefV8Value::CreateUndefined();
}

//////////////////////////////////////////////////////////////////////////

CefViewBridgeObject::CefViewBridgeObject(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         CefRefPtr<CefV8Value> global,
                                         const std::string& name)
  : name_(name.empty() ? kCefViewDefaultBridgeObjectName : name)
  , bridgeObject_(nullptr)
  , reportJSResultFunction_(nullptr)
  , browser_(browser)
  , frame_(frame)
  , v8Handler_(new V8Handler(this))
{
  // create bridge object and mount it on the global context(window)
  bridgeObject_ = CefV8Value::CreateObject(nullptr, nullptr);

  // create function "invokeMethod"
  CefRefPtr<CefV8Value> funcInvokeMethod = CefV8Value::CreateFunction(kCefViewInvokeMethodFunctionName, v8Handler_);
  // add this function to window object
  bridgeObject_->SetValue(kCefViewInvokeMethodFunctionName,
                          funcInvokeMethod,
                          static_cast<CefV8Value::PropertyAttribute>(V8_PROPERTY_ATTRIBUTE_READONLY |
                                                                     V8_PROPERTY_ATTRIBUTE_DONTENUM |
                                                                     V8_PROPERTY_ATTRIBUTE_DONTDELETE));

  // create function addEventListener
  CefRefPtr<CefV8Value> funcAddEventListener =
    CefV8Value::CreateFunction(kCefViewAddEventListenerFunctionName, v8Handler_);
  // add this function to window object
  bridgeObject_->SetValue(kCefViewAddEventListenerFunctionName,
                          funcAddEventListener,
                          static_cast<CefV8Value::PropertyAttribute>(V8_PROPERTY_ATTRIBUTE_READONLY |
                                                                     V8_PROPERTY_ATTRIBUTE_DONTENUM |
                                                                     V8_PROPERTY_ATTRIBUTE_DONTDELETE));

  // create function removeListener
  CefRefPtr<CefV8Value> funcRemoveEventListener =
    CefV8Value::CreateFunction(kCefViewRemoveEventListenerFunctionName, v8Handler_);
  // add this function to window object
  bridgeObject_->SetValue(kCefViewRemoveEventListenerFunctionName,
                          funcRemoveEventListener,
                          static_cast<CefV8Value::PropertyAttribute>(V8_PROPERTY_ATTRIBUTE_READONLY |
                                                                     V8_PROPERTY_ATTRIBUTE_DONTENUM |
                                                                     V8_PROPERTY_ATTRIBUTE_DONTDELETE));

  // mount the client object to the global context(usually the window object)
  global->SetValue(name_,
                   bridgeObject_,
                   static_cast<CefV8Value::PropertyAttribute>(V8_PROPERTY_ATTRIBUTE_READONLY |
                                                              V8_PROPERTY_ATTRIBUTE_DONTENUM |
                                                              V8_PROPERTY_ATTRIBUTE_DONTDELETE));
  frame_->ExecuteJavaScript("console.info('[JSRuntime]:window." + name_ + " [object] created');", frame_->GetURL(), 0);

  // create "__cefview_report_js_result__" function and mount it on the global context(window)
  reportJSResultFunction_ = CefV8Value::CreateFunction(kCefViewReportJSResultFunctionName, v8Handler_);
  global->SetValue(kCefViewReportJSResultFunctionName,
                   reportJSResultFunction_,
                   static_cast<CefV8Value::PropertyAttribute>(V8_PROPERTY_ATTRIBUTE_READONLY |
                                                              V8_PROPERTY_ATTRIBUTE_DONTENUM |
                                                              V8_PROPERTY_ATTRIBUTE_DONTDELETE));
  frame_->ExecuteJavaScript("console.info('[JSRuntime]:window." kCefViewReportJSResultFunctionName
                            " [function] created');",
                            frame_->GetURL(),
                            0);
}

CefRefPtr<CefV8Value>
CefViewBridgeObject::CefValueToV8Value(CefValue* cefValue)
{
  CefRefPtr<CefV8Value> v8Value = CefV8Value::CreateNull();
  if (!cefValue) {
    return v8Value;
  }

  auto type = cefValue->GetType();
  switch (type) {
    case CefValueType::VTYPE_INVALID: {
      v8Value = CefV8Value::CreateUndefined();
    } break;
    case CefValueType::VTYPE_NULL: {
      v8Value = CefV8Value::CreateNull();
    } break;
    case CefValueType::VTYPE_BOOL: {
      auto v = cefValue->GetBool();
      v8Value = CefV8Value::CreateBool(v);
    } break;
    case CefValueType::VTYPE_INT: {
      auto v = cefValue->GetInt();
      v8Value = CefV8Value::CreateInt(v);
    } break;
    case CefValueType::VTYPE_DOUBLE: {
      auto v = cefValue->GetDouble();
      v8Value = CefV8Value::CreateDouble(v);
    } break;
    case CefValueType::VTYPE_STRING: {
      auto v = cefValue->GetString();
      v8Value = CefV8Value::CreateString(v);
    } break;
    case CefValueType::VTYPE_BINARY: {
      // TO-DO
      // currently not supported
    } break;
    case CefValueType::VTYPE_DICTIONARY: {
      auto cDict = cefValue->GetDictionary();
      CefDictionaryValue::KeyList cKeys;
      cDict->GetKeys(cKeys);
      v8Value = CefV8Value::CreateObject(nullptr, nullptr);
      for (auto& key : cKeys) {
        auto cVal = cDict->GetValue(key);
        auto v8Val = CefValueToV8Value(cVal.get());
        v8Value->SetValue(key, v8Val.get(), V8_PROPERTY_ATTRIBUTE_NONE);
      }
    } break;
    case CefValueType::VTYPE_LIST: {
      auto cList = cefValue->GetList();
      int cCount = static_cast<int>(cList->GetSize());
      v8Value = CefV8Value::CreateArray(static_cast<int>(cCount));
      for (int i = 0; i < cCount; i++) {
        auto cVal = cList->GetValue(i);
        auto v8Val = CefValueToV8Value(cVal.get());
        v8Value->SetValue(i, v8Val.get());
      }
    } break;
    default:
      break;
  }

  return v8Value;
}

CefRefPtr<CefValue>
CefViewBridgeObject::V8ValueToCefValue(CefV8Value* v8Value)
{
  CefRefPtr<CefValue> cefValue = CefValue::Create();
  if (!v8Value) {
    return cefValue;
  }

  /**
   * The IsDouble, IsInt and IsUint methods return a boolean value indicating whether the CefV8Value instance is an
   * target type or can be converted to the target type.If the value can be converted to the target type, the methods
   * will attempt to do so and return true. If the value is not the target type or cannot be converted to the target
   * type, the method will return false.
   *
   * For example the code below:
   *   auto v = CefV8Value::CreateInt(1000);
   *   auto isDouble = v->IsDouble();
   *   logD("isDouble: %d", isDouble);  // true
   *   auto isUnint = v->IsUInt();
   *   logD("isUnint: %d", isUnint);    // true
   *   auto isInt = v->IsInt();
   *   logD("isInt: %d", isInt);        // true
   *
   *   auto v = CefV8Value::CreateDouble(0.1);
   *   auto isDouble = v->IsDouble();
   *   logD("isDouble: %d", isDouble);  // true
   *   auto isUnint = v->IsUInt();
   *   logD("isUnint: %d", isUnint);    // false
   *   auto isInt = v->IsInt();
   *   logD("isInt: %d", isInt);        // false
   *
   * so we need to keep the testing order, IsInt/IsUint - IsDouble
   * since there is no Uint type in JavaScript, we just ignore it.
   * Please refer to this test souce:
   * https://github.com/svn2github/cef/blob/master/tests/cefclient/binding_test.cpp
   */
  if (v8Value->IsNull() || v8Value->IsUndefined())
    cefValue->SetNull();
  else if (v8Value->IsBool())
    cefValue->SetBool(v8Value->GetBoolValue());
  else if (v8Value->IsInt())
    cefValue->SetInt(v8Value->GetIntValue());
  else if (v8Value->IsDouble())
    cefValue->SetDouble(v8Value->GetDoubleValue());
  else if (v8Value->IsString())
    cefValue->SetString(v8Value->GetStringValue());
  else if (v8Value->IsArrayBuffer()) {
    // TO-DO
    // currently not supported
  } else if (v8Value->IsArray()) {
    auto s = v8Value->GetArrayLength();
    auto cefList = CefListValue::Create();
    for (int i = 0; i < s; i++) {
      auto v8Val = v8Value->GetValue(i);
      auto cefVal = V8ValueToCefValue(v8Val.get());
      cefList->SetValue(i, cefVal);
    }
    cefValue->SetList(cefList);
  } else if (v8Value->IsObject()) {
    CefDictionaryValue::KeyList keys;
    v8Value->GetKeys(keys);
    auto cefDict = CefDictionaryValue::Create();
    for (auto& key : keys) {
      auto v8Val = v8Value->GetValue(key);
      auto cefVal = V8ValueToCefValue(v8Val.get());
      cefDict->SetValue(key, cefVal.get());
    }
    cefValue->SetDictionary(cefDict);
  } else
    cefValue->SetNull();

  return cefValue;
}

void
CefViewBridgeObject::AsyncExecuteNativeMethod(const CefV8ValueList& arguments)
{
  CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(kCefViewClientRenderInvokeMethodMessage);

  //** arguments(CefValueList)
  //** +-------+
  //** |0 name | <- the method name
  //** |1 arg1 |
  //** |2 arg2 |
  //** |3 arg3 |
  //** |4 arg4 |
  //** | ...   |
  //** | ...   |
  //** | ...   |
  //** | ...   |
  //** +-------+
  CefRefPtr<CefListValue> args = msg->GetArgumentList();

  // push back all the arguments
  for (std::size_t i = 0; i < arguments.size(); i++) {
    auto cefValue = V8ValueToCefValue(arguments[i].get());
    args->SetValue(i, cefValue);
  }

  // send the message
  if (browser_)
    frame_->SendProcessMessage(PID_BROWSER, msg);
}

void
CefViewBridgeObject::AsyncExecuteReportJSResult(const CefV8ValueList& arguments)
{
  CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(kCefViewClientRenderReportJSResultMessage);

  //** arguments(CefValueList)
  //** +_------+
  //** |0 arg  | <- the context (string)
  //** |1 arg  | <- the result value
  //** +-------+
  CefRefPtr<CefListValue> args = msg->GetArgumentList();

  // push back the result value
  for (std::size_t i = 0; i < arguments.size(); i++) {
    auto cefValue = V8ValueToCefValue(arguments[i].get());
    args->SetValue(i, cefValue.get());
  }

  // send the message
  if (browser_)
    frame_->SendProcessMessage(PID_BROWSER, msg);
}

void
CefViewBridgeObject::AddEventListener(const CefString& name, const EventListener& listener)
{
  auto itListenerList = eventListenerListMap_.find(name);
  if (itListenerList == eventListenerListMap_.end()) {
    EventListenerList eventListenerList;
    eventListenerList.push_back(listener);
    eventListenerListMap_[name] = eventListenerList;
  } else {
    EventListenerList& eventListenerList = itListenerList->second;
    // does this listener exist?
    bool found = false;
    for (auto item : eventListenerList) {
      if (item.callback_->IsSame(listener.callback_)) {
        found = true;
        break;
      }
    }

    if (!found)
      eventListenerList.push_back(listener);
  }
}

void
CefViewBridgeObject::RemoveEventListener(const CefString& name, const EventListener& listener)
{
  auto itListenerList = eventListenerListMap_.find(name);
  if (itListenerList != eventListenerListMap_.end()) {
    EventListenerList& eventListenerList = itListenerList->second;
    for (auto itListener = eventListenerList.begin(); itListener != eventListenerList.end(); itListener++) {
      if (itListener->callback_->IsSame(listener.callback_)) {
        eventListenerList.erase(itListener);
        break;
      }
    }
  }
}

void
CefViewBridgeObject::ExecuteEventListener(const CefString eventName, CefRefPtr<CefListValue> args)
{
  // find the listeners
  auto itListenerList = eventListenerListMap_.find(eventName);
  if (itListenerList == eventListenerListMap_.end()) {
    return;
  }

  EventListenerList& eventListenerList = itListenerList->second;
  for (auto listener : eventListenerList) {
    listener.context_->Enter();

    // convert argument list from CefValue to CefV8Value
    CefV8ValueList v8ArgList;
    for (size_t i = 0; i < args->GetSize(); i++) {
      auto cefValue = args->GetValue(i);
      auto v8Value = CefValueToV8Value(cefValue.get());
      v8ArgList.push_back(v8Value.get());
    }

    listener.callback_->ExecuteFunction(bridgeObject_, v8ArgList);
    listener.context_->Exit();
  }
}
