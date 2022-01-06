# CefViewCore

CefViewCore is the core abstraction layer of CefView projects. The QCefView and CocoaCefView are both based on the CefViewCore. In ther future there would be more CefView projects with different languange bindings or platform supports.

CefViewCore was written in only C++ (minor Objective-C code is used in CefViewWing application for macOS) so that it could be a common base layer for the other high level projects.

<div align="center">
<a href="docs/images/cefview-stack.jpg"><img src="docs/images/cefview-stack.jpg" alt></a>
<br/>
<em>CefView Stack</em>
</div>

 ## Build status
| triplets  | status  |
|---|---|
|  macOS-x64  | [![Build status](https://ci.appveyor.com/api/projects/status/nt6ikke1kgbl66sa?svg=true)](https://ci.appveyor.com/project/tishion/cefviewcore-macos-x64) |
|  Linux-x64  | [![Build status](https://ci.appveyor.com/api/projects/status/f3ttb2668wo5wi4d?svg=true)](https://ci.appveyor.com/project/tishion/cefviewcore-linux-x64)|
| Windows-x64 | [![Build status](https://ci.appveyor.com/api/projects/status/3v93gpt2cgqcanm3?svg=true)](https://ci.appveyor.com/project/tishion/cefviewcore-windows-x64) |
