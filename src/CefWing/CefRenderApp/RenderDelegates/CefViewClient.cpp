#pragma region projet_headers
#include "Common/CefViewCoreLog.h"
#include "CefViewClient.h"
#pragma endregion projet_headers

CefViewClient::V8Handler::V8Handler(CefViewClient* client)
  : client_(client)
{}

bool
CefViewClient::V8Handler::Execute(const CefString& function,
                                  CefRefPtr<CefV8Value> object,
                                  const CefV8ValueList& arguments,
                                  CefRefPtr<CefV8Value>& retval,
                                  CefString& exception)
{
  if (function == CEFVIEW_INVOKEMETHOD)
    ExecuteInvokeMethod(function, object, arguments, retval, exception);
  else if (function == CEFVIEW_ADDEVENTLISTENER)
    ExecuteAddEventListener(function, object, arguments, retval, exception);
  else if (function == CEFVIEW_REMOVEEVENTLISTENER)
    ExecuteRemoveEventListener(function, object, arguments, retval, exception);
  else
    return false;

  return true;
}

void
CefViewClient::V8Handler::ExecuteInvokeMethod(const CefString& function,
                                              CefRefPtr<CefV8Value> object,
                                              const CefV8ValueList& arguments,
                                              CefRefPtr<CefV8Value>& retval,
                                              CefString& exception)
{
  client_->ExecutedInvokeMethod(arguments);
  retval = CefV8Value::CreateUndefined();
}

void
CefViewClient::V8Handler::ExecuteAddEventListener(const CefString& function,
                                                  CefRefPtr<CefV8Value> object,
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
        client_->AddEventListener(eventName, listener);
        bRet = true;
      } else
        exception = "Invalid parameters; parameter 2 should be a function";
    } else
      exception = "Invalid parameters; parameter 1 should be a string";
  } else
    exception = "Invalid parameters; expecting 2 parameters";

  retval = CefV8Value::CreateBool(bRet);
}

void
CefViewClient::V8Handler::ExecuteRemoveEventListener(const CefString& function,
                                                     CefRefPtr<CefV8Value> object,
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
        client_->RemoveEventListener(eventName, listener);
      } else
        exception = "Invalid parameters; parameter 2 is expecting a function";
    } else
      exception = "Invalid parameters; parameter 1 is expecting a string";
  } else
    exception = "Invalid parameters; expecting 2 parameters";

  retval = CefV8Value::CreateBool(bRet);
}

//////////////////////////////////////////////////////////////////////////

CefViewClient::CefViewClient(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
  : object_(CefV8Value::CreateObject(nullptr, nullptr))
  , browser_(browser)
  , frame_(frame)
  , v8Handler_(new V8Handler(this))
{
  // create function "InvokeMethod"
  CefRefPtr<CefV8Value> funcInvokeMethod = CefV8Value::CreateFunction(CEFVIEW_INVOKEMETHOD, v8Handler_);
  // add this function to window object
  object_->SetValue(CEFVIEW_INVOKEMETHOD, funcInvokeMethod, V8_PROPERTY_ATTRIBUTE_READONLY);

  // create function addEventListener
  CefRefPtr<CefV8Value> funcAddEventListener = CefV8Value::CreateFunction(CEFVIEW_ADDEVENTLISTENER, v8Handler_);
  // add this function to window object
  object_->SetValue(CEFVIEW_ADDEVENTLISTENER, funcAddEventListener, V8_PROPERTY_ATTRIBUTE_READONLY);

  // create function removeListener
  CefRefPtr<CefV8Value> funcRemoveEventListener = CefV8Value::CreateFunction(CEFVIEW_REMOVEEVENTLISTENER, v8Handler_);
  // add this function to window object
  object_->SetValue(CEFVIEW_REMOVEEVENTLISTENER, funcRemoveEventListener, V8_PROPERTY_ATTRIBUTE_READONLY);
}

CefRefPtr<CefV8Value>
CefViewClient::GetObject()
{
  return object_;
}

CefRefPtr<CefV8Value>
CefViewClient::CefValueToV8Value(CefValue* cefValue)
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
        auto v8Val = CefValueToV8Value(cVal);
        v8Val->SetValue(key, v8Val, V8_PROPERTY_ATTRIBUTE_NONE);
      }
    } break;
    case CefValueType::VTYPE_LIST: {
      auto cList = cefValue->GetList();
      int cCount = static_cast<int>(cList->GetSize());
      v8Value = CefV8Value::CreateArray(static_cast<int>(cCount));
      for (int i = 0; i < cCount; i++) {
        auto cVal = cList->GetValue(i);
        auto v8Val = CefValueToV8Value(cVal);
        v8Value->SetValue(i, v8Value);
      }
    } break;
    default:
      break;
  }

  return v8Value;
}

CefRefPtr<CefValue>
CefViewClient::V8ValueToCefValue(CefV8Value* v8Value)
{
  CefRefPtr<CefValue> cefValue = CefValue::Create();
  if (!v8Value) {
    return cefValue;
  }

  if (v8Value->IsNull() || v8Value->IsUndefined())
    cefValue->SetNull();
  else if (v8Value->IsBool())
    cefValue->SetBool(v8Value->GetBoolValue());
  else if (v8Value->IsInt())
    cefValue->SetInt(v8Value->GetIntValue());
  else if (v8Value->IsUInt() || v8Value->IsDouble())
    cefValue->SetDouble(v8Value->GetDoubleValue());
  else if (v8Value->IsString())
    cefValue->SetString(v8Value->GetStringValue());
  else if (v8Value->IsArrayBuffer()) {
    // TO-DO
    // currently not supported
  } else if (v8Value->IsObject()) {
    CefDictionaryValue::KeyList keys;
    v8Value->GetKeys(keys);
    auto cefDict = CefDictionaryValue::Create();
    for (auto& key : keys) {
      auto v8Val = v8Value->GetValue(key);
      auto cefVal = V8ValueToCefValue(v8Val);
      cefDict->SetValue(key, cefValue);
    }
    cefValue->SetDictionary(cefDict);
  } else if (v8Value->IsArray()) {
    auto s = v8Value->GetArrayLength();
    auto cefList = CefListValue::Create();
    for (int i = 0; i < s; i++) {
      auto v8Val = v8Value->GetValue(i);
      auto cefVal = V8ValueToCefValue(v8Val);
      cefList->SetValue(i, cefValue);
    }
    cefValue->SetList(cefList);
  } else
    cefValue->SetNull();

  return cefValue;
}

void
CefViewClient::ExecutedInvokeMethod(const CefV8ValueList& arguments)
{
  CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(INVOKEMETHOD_NOTIFY_MESSAGE);

  CefRefPtr<CefListValue> args = msg->GetArgumentList();
  int frameId = (int)frame_->GetIdentifier();

  int idx = 0;
  args->SetInt(idx++, frameId);
  for (std::size_t i = 0; i < arguments.size(); i++) {
    auto cefValue = V8ValueToCefValue(arguments[i]);
    args->SetValue(idx++, cefValue);
  }

  if (browser_)
    frame_->SendProcessMessage(PID_BROWSER, msg);
}

void
CefViewClient::AddEventListener(const CefString& name, const EventListener& listener)
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
CefViewClient::RemoveEventListener(const CefString& name, const EventListener& listener)
{
  auto itListenerList = eventListenerListMap_.find(name);
  if (itListenerList != eventListenerListMap_.end()) {
    EventListenerList& eventListenerList = itListenerList->second;
    for (auto itListener = eventListenerList.begin(); itListener != eventListenerList.end(); itListener++) {
      if (itListener->callback_->IsSame(listener.callback_))
        eventListenerList.erase(itListener);
    }
  }
}

void
CefViewClient::ExecuteEventListener(const CefString eventName, CefRefPtr<CefListValue> args)
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
      auto v8Value = CefValueToV8Value(cefValue);
      v8ArgList.push_back(v8Value);
    }

    listener.callback_->ExecuteFunction(object_, v8ArgList);
    listener.context_->Exit();
  }
}
