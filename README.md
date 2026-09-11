# MfcToGTK
A simple Adapter For MFC Dialog Test Tools on Linux Using GTK


本文介绍了一种将Windows MFC对话框程序移植至Linux的轻量级兼容方案。通过解析MFC资源字符串，利用GTK库实现窗口控件的动态创建与布局，结合自定义CDialog基类和CStringW等MFC兼容类型，使原有基于MFC的测试工具在少量修改后即可在Linux上编译运行。该方案支持静态文本、编辑框、按钮、复选框等常见控件，并实现了模态对话框、数据交换（DDX）及文件选择对话框等功能，有效降低了跨平台移植成本。
