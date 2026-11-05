# SDK 日志记录与上传

需要包含诊断导出的新版 DLL、配套导入库和 Node addon 一起更新。旧 DLL 不支持这些符号。日志上传复用 desktop 的 `client_diagnostics` / `client_log_reports` 服务与管理后台。

## Electron / Node

在主进程初始化，且在第一次启动加速前调用。所有诊断方法返回 Promise，原生工作在线程池执行；上传方法返回后仍须查询最终状态。

```js
const sdk = require('lingti-sdk');
await sdk.configureDiagnostics({
  user_id: enterpriseUserId,
  token: sdkToken, // SDK Token；不是 64 位 HEX Config Key，也不是线路 Token
  app_version: 'MyApp/1.0.0',
  requested_level: 'basic', // basic | debug | trace
});
// 正常调用 startTun2R 或 startTun2RWithConfigFile。
const rows = await sdk.listDiagnosticRecords();
if (rows.length) {
  const job = await sdk.uploadDiagnosticRecord(rows[0].id);
  // 正在记录的日志生成独立快照；job.id 可能不同于原记录 ID。
  const poll = setInterval(async () => {
    try {
      const state = await sdk.getDiagnosticUploadStatus(job.id);
      if (state.state === 'uploaded' || state.state === 'retry') {
        clearInterval(poll);
        console.log(state.state, state.last_error, job.id);
      }
    } catch (err) { clearInterval(poll); console.error(err); }
  }, 1000);
}
```

- `retryDiagnosticUploads()`：在联网恢复或停止加速后调用；返回排队结果。用 `getDiagnosticUploadStatus()`（不带 ID）查询最近批次的 `busy` / `last_error`，用列表查询每份报告最终结果。可由宿主每 60 秒调用一次；busy 时跳过，退出时清理宿主定时器。
- `deleteDiagnosticRecord(id)`：删除本地历史记录；不删除云端报告。
- `configureDiagnostics()`：仅允许在停止加速且无上传任务时重新配置。Token 过期后更新配置再重试；不会把 Token 写入日志索引。配置失败保留原配置。
- 上传完成的 `id` 同时是后端报告 ID。只有 complete 接口确认成功，才显示 `uploaded`。

## C / C# / 其他 DLL 调用方

头文件提供 `ConfigureDiagnostics`、`ListDiagnosticRecords`、`UploadDiagnosticRecord`、`GetDiagnosticUploadStatus`、`RetryDiagnosticUploads`、`DeleteDiagnosticRecord`。参数和返回值均为 UTF-8；每个返回值均为 `{ "ok": true, "data": ... }` 或 `{ "ok": false, "error": "..." }`。

```c
char *result = ConfigureDiagnostics(
  "{\"user_id\":\"YOUR-UUID\",\"requested_level\":\"basic\"}");
/* 解析 result，检查 ok；上传时必须配置有效 token。 */
FreeDiagnosticString(result);
result = ListDiagnosticRecords();
FreeDiagnosticString(result);
```

所有返回字符串必须用 `FreeDiagnosticString` 释放，不能由宿主 CRT 的 free 释放。诊断函数应放到宿主工作线程。也可调用统一入口 `DiagnosticsCommand`，action 为 configure/list/upload/status/flush/delete；configure 携带 config，其余按需携带 id。

## 记录与保留

默认目录 `%LOCALAPPDATA%\LingTiSDK\diagnostics`，按 API 和用户隔离；可设置绝对路径 directory。设备 ID 默认由 SDK 获取。默认 API 为 `https://api.lingti.paiyou.co`，仅支持已有 Lingti 诊断服务域名。

初始化支持离线运行。未提供 Token 时仍可记录、列出日志，但不能上传。启动失败记录要求先初始化诊断；停止加速后再次启动失败也会建立新记录。基础记录包括 SDK 错误和诊断生命周期，debug/trace 增加原生日志。API 凭据、Config Key 和线路凭据脱敏，不上传完整配置或数据包。

沿用公共模块：保留 72 小时、单份原始日志最多 20 MiB、压缩最多 10 MiB、设备目录容量预算 80 MiB；debug 最多 30 分钟，trace 最多 3 分钟，服务端策略可进一步收紧。上传失败保留快照和重试队列；停止隧道不取消上传。进程退出会中断后台工作，下次初始化后调用 retry 恢复；不提供原生崩溃 minidump。

服务端新版 `/games/connect_sdk` 将 user_id / connection_id / connection_type / api_host 放入加密配置，便于在后台关联游戏连接。旧配置仍可通过显式初始化记录与上传，但没有连接关联。配置 user_id 必须与 SDK Token 所属账号一致；上传前会向服务端核对，不能通过填写其他 user_id 上传为其他账号。

## 验证

core 中可在 macOS 运行纯 Go 诊断接口回归：

```sh
go test -race sdk_diagnostics.go sdk_diagnostics_test.go sdk_diagnostics_native_test.go
```

Windows 上还需验收真实隧道、Electron Promise 调用、断网后恢复上传，并在管理后台确认报告可读取。
