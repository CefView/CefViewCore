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
| Windows-x64 | [![Build on Windows](https://github.com/CefView/CefViewCore/actions/workflows/build-windows-x86_64.yml/badge.svg)](https://github.com/CefView/CefViewCore/actions/workflows/build-windows-x86_64.yml) |
|  macOS-x64  | [![Build on macOS](https://github.com/CefView/CefViewCore/actions/workflows/build-macos-x86_64.yml/badge.svg)](https://github.com/CefView/CefViewCore/actions/workflows/build-macos-x86_64.yml) |
|  Linux-x64  | [![Build on Linux](https://github.com/CefView/CefViewCore/actions/workflows/build-linux-x86_64.yml/badge.svg)](https://github.com/CefView/CefViewCore/actions/workflows/build-linux-x86_64.yml) |