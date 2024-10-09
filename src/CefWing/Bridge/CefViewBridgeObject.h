#pragma once
#pragma region stl_headers
#include <list>
#include <map>
#pragma endregion

#pragma region cef_headers
#include <include/cef_v8.h>
#pragma endregion

/// <summary>
///
/// </summary>
class CefViewBridgeObject : public CefBaseRefCounted
{
  IMPLEMENT_REFCOUNTING(CefViewBridgeObject);

  /// <summary>
  ///
  /// </summary>
  typedef struct _EventListener
  {
    CefRefPtr<CefV8Value> callback_;
    CefRefPtr<CefV8Context> context_;
  } EventListener;

  /// <summary>
  ///
  /// </summary>
  typedef std::list<EventListener> EventListenerList;

  /// <summary>
  ///
  /// </summary>
  typedef std::map<CefString, std::list<EventListener>> EventListenerListMap;

  /// <summary>
  ///
  /// </summary>
  class V8Handler : public CefV8Handler
  {
  public:
    /// <summary>
    ///
    /// </summary>
    /// <param name="client"></param>
    V8Handler(CefViewBridgeObject* object);

    /// <summary>
    ///
    /// </summary>
    /// <param name="function"></param>
    /// <param name="object"></param>
    /// <param name="arguments"></param>
    /// <param name="retval"></param>
    /// <param name="exception"></param>
    /// <returns></returns>
    virtual bool Execute(const CefString& function,
                         CefRefPtr<CefV8Value> object,
                         const CefV8ValueList& arguments,
                         CefRefPtr<CefV8Value>& retval,
                         CefString& exception) override;

  protected:
    /// <summary>
    ///
    /// </summary>
    /// <param name="object"></param>
    /// <param name="arguments"></param>
    /// <param name="retval"></param>
    /// <param name="exception"></param>
    void ExecuteNativeMethod(CefRefPtr<CefV8Value> object,
                             const CefV8ValueList& arguments,
                             CefRefPtr<CefV8Value>& retval,
                             CefString& exception);

    /// <summary>
    ///
    /// </summary>
    /// <param name="object"></param>
    /// <param name="arguments"></param>
    /// <param name="retval"></param>
    /// <param name="exception"></param>
    void ExecuteAddEventListener(CefRefPtr<CefV8Value> object,
                                 const CefV8ValueList& arguments,
                                 CefRefPtr<CefV8Value>& retval,
                                 CefString& exception);

    /// <summary>
    ///
    /// </summary>
    /// <param name="object"></param>
    /// <param name="arguments"></param>
    /// <param name="retval"></param>
    /// <param name="exception"></param>
    void ExecuteRemoveEventListener(CefRefPtr<CefV8Value> object,
                                    const CefV8ValueList& arguments,
                                    CefRefPtr<CefV8Value>& retval,
                                    CefString& exception);

    /// <summary>
    ///
    /// </summary>
    /// <param name="object"></param>
    /// <param name="arguments"></param>
    /// <param name="retval"></param>
    /// <param name="exception"></param>
    void ExecuteReportJSResult(CefRefPtr<CefV8Value> object,
                               const CefV8ValueList& arguments,
                               CefRefPtr<CefV8Value>& retval,
                               CefString& exception);

  private:
    /// <summary>
    ///
    /// </summary>
    CefViewBridgeObject* object_;

  private:
    IMPLEMENT_REFCOUNTING(V8Handler);
  };

public:
  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="global"></param>
  /// <param name="name"></param>
  CefViewBridgeObject(CefRefPtr<CefBrowser> browser,
                      CefRefPtr<CefFrame> frame,
                      CefRefPtr<CefV8Value> global,
                      const std::string& name);

  /// <summary>
  ///
  /// </summary>
  /// <param name="v8Value"></param>
  /// <param name="cefValue"></param>
  /// <returns></returns>
  CefRefPtr<CefV8Value> CefValueToV8Value(CefValue* cefValue);

  /// <summary>
  ///
  /// </summary>
  /// <param name="v8Value"></param>
  /// <returns></returns>
  CefRefPtr<CefValue> V8ValueToCefValue(CefV8Value* v8Value);

  /// <summary>
  ///
  /// </summary>
  /// <param name="arguments"></param>
  void AsyncExecuteNativeMethod(const CefV8ValueList& arguments);

  /// <summary>
  ///
  /// </summary>
  /// <param name="arguments"></param>
  void AsyncExecuteReportJSResult(const CefV8ValueList& arguments);

  /// <summary>
  ///
  /// </summary>
  /// <param name="name"></param>
  /// <param name="listener"></param>
  /// <returns></returns>
  void AddEventListener(const CefString& name, const EventListener& listener);

  /// <summary>
  ///
  /// </summary>
  /// <param name="name"></param>
  /// <param name="listener"></param>
  void RemoveEventListener(const CefString& name, const EventListener& listener);

  /// <summary>
  ///
  /// </summary>
  /// <param name="eventName"></param>
  /// <param name="args"></param>
  void ExecuteEventListener(const CefString eventName, CefRefPtr<CefListValue> args);

private:
  /// <summary>
  ///
  /// </summary>
  std::string name_;

  /// <summary>
  ///
  /// </summary>
  CefRefPtr<CefV8Value> bridgeObject_;

  /// <summary>
  ///
  /// </summary>
  CefRefPtr<CefV8Value> reportJSResultFunction_;

  /// <summary>
  ///
  /// </summary>
  CefRefPtr<CefBrowser> browser_;

  /// <summary>
  ///
  /// </summary>
  CefRefPtr<CefFrame> frame_;

  /// <summary>
  ///
  /// </summary>
  CefRefPtr<V8Handler> v8Handler_;

  /// <summary>
  ///
  /// </summary>
  CefViewBridgeObject::EventListenerListMap eventListenerListMap_;
};
