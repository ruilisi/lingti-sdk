'use strict';

// Run on Windows x64 as administrator. See ../docs/CONSOLE_ACCELERATION.zh-CN.md.
const path = require('node:path');
const fs = require('node:fs');
const { isIPv4 } = require('node:net');
const lingti = require('./index');

const sleep = ms => new Promise(resolve => setTimeout(resolve, ms));
let stopRequested = false;

// Let the main function stop the SDK before allowing the process to exit.
process.on('SIGINT', () => { stopRequested = true; });
process.on('SIGTERM', () => { stopRequested = true; });

function sdkError(operation) {
    return new Error(`${operation}: ${lingti.getLastErrorMessage()}`);
}

async function waitForConsoleConfig() {
    const deadline = Date.now() + 30_000;
    while (!stopRequested && Date.now() < deadline) {
        if (!lingti.isServiceRunning()) throw sdkError('启动期间服务已停止');

        const config = lingti.getConsoleConfig();
        if (config.state === lingti.ConsoleIPState.FAILED) {
            throw sdkError('主机 IP 配置失败');
        }
        if (![0, 1, 2, 3].includes(config.state)) {
            throw new Error(`未知的主机 IP 状态：${config.state}`);
        }
        if (config.state === lingti.ConsoleIPState.COMPLETED &&
            [config.ip, config.mask, config.gateway, config.dns].every(isIPv4)) {
            return config;
        }
        await sleep(500);
    }
    if (stopRequested) return null;
    throw sdkError('等待主机 IP 配置超时（30 秒），请检查配置是否属于主机游戏');
}

async function main() {
    if (!lingti.isAddonAvailable()) {
        throw new Error('原生扩展不可用，请在 Windows x64 上完成构建并检查 DLL、驱动文件');
    }
    if (!process.argv[2]) {
        throw new Error('用法：node console-example.js <encrypted_config.txt 路径>');
    }
    const configPath = path.resolve(process.argv[2]);
    fs.accessSync(configPath, fs.constants.R_OK);
    console.log('SDK 版本：', lingti.getSDKVersion());

    const result = lingti.startTun2RWithConfigFile(configPath);
    if (result !== lingti.ErrorCodes.SUCCESS) {
        throw sdkError(`启动失败（${result}）`);
    }

    try {
        const config = await waitForConsoleConfig();
        if (!config) return;

        console.log('请在游戏主机上手动填写以下网络参数：');
        console.table({
            'IP 地址': config.ip,
            '子网掩码': config.mask,
            '网关': config.gateway,
            '首选 DNS': config.dns,
        });
        console.log('保持程序运行，完成主机联网测试后进入游戏。按 Ctrl+C 停止。');

        while (!stopRequested) {
            if (!lingti.isServiceRunning()) throw sdkError('加速服务意外停止');
            const { txBytes, rxBytes } = lingti.getTrafficStats();
            console.log(`累计发送 ${txBytes} 字节，累计接收 ${rxBytes} 字节`);
            await sleep(1000);
        }
    } finally {
        try {
            if (lingti.isServiceRunning()) {
                const stopped = lingti.stopTun2R();
                if (stopped !== lingti.ErrorCodes.SUCCESS) {
                    throw sdkError(`停止失败（${stopped}）`);
                }
            }
            // Keep the host alive briefly so native cleanup can settle.
            await sleep(2000);
            if (lingti.isServiceRunning()) throw new Error('停止后服务仍报告运行中');
        } finally {
            console.log('请把主机的 IP 和 DNS 恢复为自动获取，或恢复原有网络设置。');
        }
    }
}

main().catch(error => {
    console.error(error.message);
    process.exitCode = 1;
});
