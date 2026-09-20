# feat: expose client delegate hooks required by embedding hosts

## Summary

Adds four virtual hooks to `CefViewBrowserClientDelegate` that embedding
hosts currently have no way to observe. All have default implementations
and are purely additive — no existing override changes behavior.

| Hook | Before | After |
|---|---|---|
| `onBeforeBrowse` | dropped after `message_router_->OnBeforeBrowse` | forwarded; return true cancels |
| `onRenderProcessTerminated` | dropped after message-router cleanup (TerminationStatus already scoped) | forwarded with status/error_code/error_string (CEF 124+; two-arg overload otherwise) |
| `onFindResult` | **result discarded entirely** (empty `OnFindResult` body) | forwarded with identifier/count/selectionRect/activeMatchOrdinal/finalUpdate |
| `onFileDialogEx` | — (CEF 126+ accept extensions/descriptions unavailable) | new; default falls back to `onFileDialog`, so existing subclasses are unaffected |

## Why these four

- **onBeforeBrowse** is the only interception point for in-page navigations,
  redirects, iframes and `window.open` — an embedding host cannot implement a
  navigation policy without it.
- **onRenderProcessTerminated** is the renderer-crash observability hook
  (host-level auto-reload / telemetry).
- **onFindResult** powers find-in-page UI; the current code receives the
  callback and drops it.
- **onFileDialogEx** exposes the accept extensions/descriptions CEF 126+
  provides; the legacy `onFileDialog` cannot carry them.

## Compatibility

- Default implementations only: overriding none of them is byte-for-byte
  the previous behavior (verified: consumers that don't override compile and
  run identically).
- `onFileDialogEx` guards `CEF_VERSION_MAJOR >= 126`; `onRenderProcessTerminated`
  keeps a two-arg overload path for CEF < 124.
- No ABI change to existing symbols; `CefVersion.h` pin in this branch is a
  local-baseline concern (drop that commit when merging — it reflects this
  fork's 127.3.5 rollback, not an upstream recommendation).

## Validation

Downstream consumer (Unity embedding, private) runs these hooks through a
pfn callback ABI with machine-verified regressions:
- navigation-policy rejections observable end-to-end (`javascript:` scheme
  blocked, machine-readable reasons)
- renderer kill injection → host receives termination + auto-reload
- find results streamed to a host UI
- file dialog extensions/descriptions cross the ABI as JSON

Branch: `feat/unity-callbacks` (5 commits; the `CefVersion.h` pin commit
`0bba10b` is fork-local and should be excluded from the merge).
