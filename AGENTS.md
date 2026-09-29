# AGENTS.md #

RatSlap — Linux / macOS CLI tool for configuring Logitech G300 / G300s gaming mice via
USB HID control messages. C program, GPL v2, uses libusb-1.0.

This repo (`alstonhsiao/G300_ratslap`) is a fork of upstream `krayon/ratslap`
(tracked via the `upstream` remote).

---

## Quick Map

| 要做什麼 | 先讀哪個檔案 |
|----------|-------------|
| 建置 / 編譯 / 了解 compile flags / 效能調校 | [`docs/build-conventions.md`](docs/build-conventions.md) |
| 了解分支命名 / commit / release 流程 | [`docs/git-workflow.md`](docs/git-workflow.md) |
| 動到 USB 通訊 / 按鍵映射 / protocol | [`docs/usb-protocol.md`](docs/usb-protocol.md) |
| 看使用範例 / 鍵名 / 已知限制 | [`README.md`](README.md) |
| 了解開發分支策略全文 | [`docs/DEV_WORKFLOW.md`](docs/DEV_WORKFLOW.md) |
| 看預設出廠設定輸出 | `docs/G300s_Default_Configuration.txt` |
| 看 USB sniffing 原始擷取與欄位解碼 | `docs/G300s_USB_sniffing.txt` |

---

## 不可違反的規則

1. **不可編輯自動生成的檔案**（`src/git.h`、`src/log.h`、`manpage.1`、
   `make.options.conf`）。改它們對應的 `templates/` 下的 `.TEMPLATE` 或
   `.DEFAULT` 檔。
2. **新程式碼必須在 `-Wall -Werror` 下乾淨編譯。** 這是唯一的 gate；
   沒有測試套件。
3. **commit 前跑 `make clean && make`** 確認建置通過。
4. **不可重新映射滾輪**（button 4/5）——硬體限制，非軟體可繞過。
5. **保留每個源碼檔的 vim modeline**（`ts=4 sw=4 tw=80 cindent`）。
6. **不自動 commit、不自動 push。** 完成後只輸出變更摘要。

---

## 派工與停損

1. 派工門檻：預估要讀超過 5 個檔案或 50KB、或需要掃整個目錄時，
   派 subagent，主對話只收結論；低於門檻自己做，不要為小事派工。
   - 正例：要理解 `main.c` 中 USB 通訊全貌 + 對照
     `G300s_USB_sniffing.txt` 的欄位佈局 → 派 subagent 做協議分析。
   - 反例：只想確認 `app.h` 裡的 `APP_NAME` 常數值 → 自己讀，不派工。
2. 派工三件套：每次派 subagent 必須寫明 (1) 目標與動機 (2) 驗收條件
   (3) 回報格式——只回結論 + 檔案:行號，長產物落檔傳路徑。
3. 停損線：同一子任務用同一種方法連錯兩次，停止重試；
   帶完整失敗軌跡（做了什麼、錯誤訊息、已排除什麼）回報使用者，
   不得換個小花樣試第三次。

---

## Source Layout（速覽）

| File          | Role                                                          |
|---------------|---------------------------------------------------------------|
| `src/main.c`  | All application logic: USB comms, CLI parsing, mode/profile IO |
| `src/log.c`   | Logging implementation                                         |
| `src/app.h`   | App name, version, author constants                            |
| `src/lang.h`  | i18n stub (`_()` macro, currently passthrough)                |
| `src/hid_compat.h` | `HID_REQ_*` constants for non-Linux builds (replaces `<linux/hid.h>`) |

USB VID:PID = `046d:c246`（定義在 `src/main.c`）。

---

## Remotes

| Remote     | URL                                    | Role      |
|------------|----------------------------------------|-----------|
| `origin`   | `github.com/alstonhsiao/G300_ratslap`  | This fork |
| `upstream` | `github.com/krayon/ratslap`            | Upstream  |

Sync with upstream via `git fetch upstream` and merge/rebase onto `main`.

---

## 常見任務

- **新增 key/button 名稱：** 改 `src/main.c` 裡的 key table（USB HID codes）。
  詳見 [`docs/usb-protocol.md`](docs/usb-protocol.md)。
- **新增 CLI 選項：** 延伸 `src/main.c` 的 `getopt` long options 表與 handler。
  確保 `-Wall -Werror` 乾淨。
- **支援新滑鼠：** VID/PID 與 USB protocol 都寫死在 `src/main.c`，新裝置需
  自行處理 protocol。
- **調校 USB 延遲：** 透過 `make.options.conf` 的 `-DUSB_DELAY_*` 巨集調整。
  詳見 [`docs/build-conventions.md`](docs/build-conventions.md)。
- **commit 前：** `make clean && make`。無測試套件；`-Werror` 通過即為 gate。

---

## 跨平台按鍵映射指南

G300s 的按鈕設定寫入滑鼠內建記憶體（onboard memory），設定跟著滑鼠走。
換到另一台 Mac 或 Windows 電腦，按鈕設定都會保留，不需要重新設定。

### 修飾鍵的跨平台差異

同一個 USB HID 修飾鍵碼在不同作業系統對應不同功能。設定按鈕時必須考慮
目標平台，否則跨平台行為不一致。

| HID 修飾鍵 | macOS 上的效果 | Windows 上的效果 | 跨平台一致？ |
|------------|---------------|------------------|-------------|
| `LeftCtrl` | ⌃ Ctrl | Ctrl | ✅ 是 |
| `LeftShift` | ⇧ Shift | Shift | ✅ 是 |
| `LeftAlt` | ⌥ Option | Alt | ✅ 是（但 Alt 快捷鍵意義可能不同） |
| `Super_L` | ⌘ Command | Win 鍵 | ❌ 否 — Mac 是 Cmd，Windows 是 Win |

### 跨平台設定原則

1. **用 `LeftCtrl` 而非 `Super_L` 做修飾鍵組合。**
   `LeftCtrl+C/V/X` 在 Mac 和 Windows 上都是複製/貼上/剪下。
   `Super_L+C/V/X` 在 Mac 上是 Cmd+C/V/X（複製/貼上/剪下），
   但在 Windows 上是 Win+C/V/X（搜尋/剪貼簿歷史/無），功能完全不同。

2. **純滑鼠按鍵（Button1–11）跨平台無差異。**
   Button6/Button7 等 extra mouse buttons 在所有平台都是標準滑鼠事件。

3. **避免使用 `Super_L` 做跨平台快捷鍵。**
   若只針對 Mac 使用，`Super_L` 可以模擬 Cmd 組合；
   若需跨平台，改用 `LeftCtrl`。

### 推薦的跨平台按鍵組合

| 功能 | 推薦設定 | Mac 效果 | Windows 效果 |
|------|---------|---------|-------------|
| 複製 | `LeftCtrl+C` | ⌃C | Ctrl+C ✅ |
| 貼上 | `LeftCtrl+V` | ⌃V | Ctrl+V ✅ |
| 剪下 | `LeftCtrl+X` | ⌃X | Ctrl+X ✅ |
| 復原 | `LeftCtrl+Z` | ⌃Z | Ctrl+Z ✅ |
| 全選 | `LeftCtrl+A` | ⌃A | Ctrl+A ✅ |
| 尋找 | `LeftCtrl+F` | ⌃F | Ctrl+F ✅ |
| 分頁切換（下一個） | `LeftCtrl+Tab` | ⌃Tab | Ctrl+Tab ✅ |
| 分頁切換（上一個） | `LeftCtrl+LeftShift+Tab` | ⌃⇧Tab | Ctrl+Shift+Tab ✅ |
| Escape | `Escape` | Esc | Esc ✅ |
| Enter | `Enter` | Return | Enter ✅ |
| 滑鼠按鍵 | `Button1`–`Button11` | 對應滑鼠按鍵 | 對應滑鼠按鍵 ✅ |

### Logitech 軟體覆寫風險

若 Windows 電腦安裝了 Logitech Gaming Software 或 G HUB，它偵測到滑鼠時
可能用自己的設定覆寫 onboard memory。若該電腦未安裝這些軟體，滑鼠會
維持 RatSlap 寫入的設定。

---

## 文件維護規則

### 文件修改權限

修改任何治理文件前，先聲明該檔屬於哪一級。

| 級別 | 範圍 | 規則 |
| ---- | ---- | ---- |
| 🟢 可自行修改 | Quick Map 與 Source Layout 的路徑、README 使用說明 | 事實性內容，改完在回報中列出即可 |
| 🟡 改前必須先問使用者 | 不可違反的規則區、派工與停損區、`src/app.h` / `src/main.c` 中的 USB protocol 核心定義 | 即使只是「精簡措辭」也要先問，不得擅自改寫或弱化 |
| 🔵 只准追加，不准自行刪改 | 各文件的 `NEED_REVIEW` 標記 | 認為某條過時，追加「建議歸檔」標註並提報，不得直接刪。經使用者明確核准後，由 agent 執行歸檔搬移 |

### troubleshooting 升格規則

- 完整事故經過一律寫進 docs/troubleshooting.md（追加新條目，附日期）。
- 符合下列任一條件時「升格」：同類坑第二次發生、或屬高風險事故。
  升格 = 在 Hub 對應規則後追加一行反例 + troubleshooting 條目編號。
- 未升格的教訓留在 troubleshooting 檔即可，不要把 Hub 當事故簿。

### 路徑檢查與瘦身協議

- 路徑檢查：例行維護時，逐一驗證 AGENTS.md 中提到的檔案路徑
  是否存在；失效路徑立即修正，無法確定則標 NEED_REVIEW。
- 瘦身觸發：troubleshooting 檔超過約 600 行、或「建議歸檔」標註累積
  5 條以上時，agent 主動列提名表 | 條目 | 建議 | 理由 | 給使用者裁決。
- 瘦身執行：獲准條目由 agent 搬移至 docs/archive/（搬移不刪除）。
- 瘦身判準：區分「場景過時」（可歸檔）與「教訓仍通用」（保留，甚至升格），
  提名表逐條說明屬於哪種。