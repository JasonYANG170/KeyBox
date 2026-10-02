# 凭据配置 / Credential configuration

创建不纳入 Git 的 src/wifi_credentials.local.h，定义 PROJECT_WIFI_SSID 和 PROJECT_WIFI_PASSWORD；配置后才能使用 Wi-Fi 功能。

Create src/wifi_credentials.local.h with PROJECT_WIFI_SSID and PROJECT_WIFI_PASSWORD. This header is ignored by Git. Wi-Fi actions require configuration.

已公开的真实凭据仍须撤销或更换。历史重写不能清除其他人的克隆、Fork 或 GitHub 缓存。

Revoke or rotate real credentials that were exposed. Rewriting history does not remove other clones, forks, or GitHub caches.
