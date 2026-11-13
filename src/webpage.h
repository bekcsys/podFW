#pragma once

const char WEBPAGE[] = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP32 Control</title>
    <style>
        * { box-sizing: border-box; }

        body {
            margin: 0;
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 24px 16px;
            background:
                radial-gradient(900px 420px at 15% -10%, rgba(56, 189, 248, 0.16), transparent 55%),
                radial-gradient(700px 380px at 100% 0%, rgba(34, 197, 94, 0.12), transparent 50%),
                #0b1220;
            color: #f8fafc;
            font-family: "Segoe UI", system-ui, sans-serif;
        }

        .card {
            width: 100%;
            max-width: 460px;
            padding: 26px 26px 22px;
            background: rgba(30, 41, 59, 0.92);
            border: 1px solid #334155;
            border-radius: 24px;
            box-shadow: 0 24px 60px rgba(0, 0, 0, 0.35);
        }

        .header {
            display: flex;
            align-items: flex-start;
            justify-content: space-between;
            gap: 16px;
            padding-bottom: 18px;
            border-bottom: 1px solid #334155;
        }

        .brand {
            font-size: 18px;
            font-weight: 700;
            letter-spacing: 0.01em;
        }

        .brand-sub {
            margin-top: 4px;
            color: #94a3b8;
            font-size: 13px;
        }

        .connection {
            flex: none;
            margin-top: 2px;
            padding: 6px 10px;
            border-radius: 999px;
            background: #0f172a;
            color: #94a3b8;
            font-size: 12px;
            font-weight: 600;
        }

        .connection.online { color: #4ade80; }

        .clock-panel {
            padding: 22px 0 8px;
            text-align: center;
        }

        .clock {
            font-size: 44px;
            font-weight: 650;
            letter-spacing: -0.03em;
            font-variant-numeric: tabular-nums;
            line-height: 1.05;
        }

        .date {
            margin-top: 8px;
            color: #cbd5e1;
            font-size: 15px;
        }

        .clock-note {
            margin-top: 6px;
            color: #64748b;
            font-size: 12px;
        }

        .info-grid {
            display: grid;
            grid-template-columns: 1.3fr 0.9fr 1fr;
            gap: 10px;
            margin-top: 20px;
        }

        .info {
            min-width: 0;
            padding: 12px 12px 11px;
            background: #0f172a;
            border: 1px solid #1e293b;
            border-radius: 14px;
        }

        .label {
            display: block;
            margin-bottom: 6px;
            color: #64748b;
            font-size: 11px;
            font-weight: 700;
            letter-spacing: 0.06em;
            text-transform: uppercase;
        }

        .value {
            display: block;
            overflow: hidden;
            color: #f8fafc;
            font-size: 14px;
            font-weight: 650;
            text-overflow: ellipsis;
            white-space: nowrap;
        }

        .content {
            text-align: center;
            margin-top: 22px;
            padding-top: 22px;
            border-top: 1px solid #334155;
        }

        .led-icon {
            width: 84px;
            height: 84px;
            margin: 0 auto 16px;
            border-radius: 50%;
            background: #450a0a;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 38px;
            color: #f87171;
            transition: background .3s, box-shadow .3s;
        }

        .led-icon.on {
            background: #14532d;
            box-shadow: 0 0 32px rgba(74, 222, 128, 0.28);
        }

        h2 {
            margin: 0 0 8px;
            font-size: 22px;
        }

        .status {
            color: #f87171;
            font-size: 14px;
            margin-bottom: 22px;
        }

        .status.on { color: #4ade80; }

        .toggle {
            position: relative;
            width: 220px;
            max-width: 100%;
            height: 64px;
            margin: auto;
            display: block;
            border: none;
            border-radius: 40px;
            background: #dc2626;
            cursor: pointer;
            transition: background .3s ease;
        }

        .toggle.on { background: #16a34a; }

        .toggle-knob {
            position: absolute;
            top: 7px;
            left: 7px;
            width: 50px;
            height: 50px;
            border-radius: 50%;
            background: white;
            display: flex;
            align-items: center;
            justify-content: center;
            color: #dc2626;
            font-size: 24px;
            transition: transform .3s ease, color .3s;
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.25);
        }

        .toggle.on .toggle-knob {
            transform: translateX(156px);
            color: #16a34a;
        }

        .toggle:disabled {
            opacity: .6;
            cursor: wait;
        }

        .hint {
            margin-top: 16px;
            font-size: 12px;
            color: #94a3b8;
        }

        @media (max-width: 480px) {
            .card { padding: 20px 16px 18px; }
            .clock { font-size: 36px; }
            .info-grid { grid-template-columns: 1fr; }
            .value { white-space: normal; }
        }

        @media (prefers-reduced-motion: reduce) {
            .toggle, .toggle-knob, .led-icon { transition: none; }
        }
    </style>
</head>
<body>
    <div class="card">
        <div class="header">
            <div>
                <div class="brand">ESP32 Control</div>
                <div id="ssidHeader" class="brand-sub">Wi-Fi —</div>
            </div>
            <div id="connection" class="connection">Connecting</div>
        </div>

        <div class="clock-panel">
            <div id="clock" class="clock">--:--:--</div>
            <div id="date" class="date">Waiting for ESP32 clock</div>
            <div id="clockNote" class="clock-note">Time is read from the ESP32</div>
        </div>

        <div class="info-grid">
            <div class="info">
                <span class="label">Wi-Fi SSID</span>
                <span id="ssid" class="value">—</span>
            </div>
            <div class="info">
                <span class="label">Signal</span>
                <span id="rssi" class="value">—</span>
            </div>
            <div class="info">
                <span class="label">Address</span>
                <span id="ip" class="value">—</span>
            </div>
        </div>

        <div class="content">
            <div id="ledIcon" class="led-icon">&#128161;</div>
            <h2>Onboard LED</h2>
            <div id="status" class="status">Reading status...</div>
            <button id="toggle" class="toggle" onclick="toggleLED()" aria-label="Toggle LED" aria-pressed="false" disabled>
                <span class="toggle-knob">&#x23FB;</span>
            </button>
            <div class="hint">Click the switch to toggle the LED</div>
        </div>
    </div>

    <script>
        const statusEl = document.getElementById("status");
        const iconEl = document.getElementById("ledIcon");
        const toggleEl = document.getElementById("toggle");
        const connectionEl = document.getElementById("connection");
        const clockEl = document.getElementById("clock");
        const dateEl = document.getElementById("date");
        const clockNoteEl = document.getElementById("clockNote");
        const ssidEl = document.getElementById("ssid");
        const ssidHeaderEl = document.getElementById("ssidHeader");
        const rssiEl = document.getElementById("rssi");
        const ipEl = document.getElementById("ip");

        const weekdays = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];
        const months = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];

        let ledOn = false;
        let connected = false;
        let busy = false;
        let deviceEpoch = 0;
        let deviceOffset = 0;
        let epochMarkedAt = 0;
        let clockSynced = false;

        function pad(value) {
            return String(value).padStart(2, "0");
        }

        function deviceNow() {
            if (!clockSynced) return 0;
            const elapsed = Math.floor((Date.now() - epochMarkedAt) / 1000);
            return deviceEpoch + elapsed;
        }

        function formatClock(epoch, offset) {
            const shifted = new Date((epoch + offset) * 1000);
            let hours = shifted.getUTCHours();
            const ampm = hours >= 12 ? "PM" : "AM";
            hours = hours % 12 || 12;
            return hours + ":" + pad(shifted.getUTCMinutes()) + ":" + pad(shifted.getUTCSeconds()) + " " + ampm;
        }

        function formatDate(epoch, offset) {
            const shifted = new Date((epoch + offset) * 1000);
            return weekdays[shifted.getUTCDay()] + ", " +
                months[shifted.getUTCMonth()] + " " +
                shifted.getUTCDate() + ", " +
                shifted.getUTCFullYear();
        }

        function signalText(rssi) {
            let quality = "Weak";
            if (rssi >= -55) quality = "Strong";
            else if (rssi >= -70) quality = "Fair";
            return rssi + " dBm · " + quality;
        }

        function renderClock() {
            if (!clockSynced) {
                clockEl.textContent = "--:--:--";
                return;
            }
            clockEl.textContent = formatClock(deviceNow(), deviceOffset);
        }

        function updateUI(on) {
            ledOn = on;
            statusEl.textContent = on ? "LED is ON" : "LED is OFF";
            statusEl.classList.toggle("on", on);
            iconEl.classList.toggle("on", on);
            toggleEl.classList.toggle("on", on);
            toggleEl.setAttribute("aria-pressed", String(on));
        }

        function setConnection(online) {
            connected = online;
            connectionEl.textContent = online ? "Online" : "Offline";
            connectionEl.classList.toggle("online", online);
            toggleEl.disabled = !online || busy;
        }

        function applyStatus(data) {
            updateUI(Boolean(data.led));

            const ssid = data.ssid || "Unknown";
            ssidEl.textContent = ssid;
            ssidHeaderEl.textContent = "Wi-Fi  " + ssid;
            rssiEl.textContent = Number.isFinite(data.rssi) ? signalText(data.rssi) : "—";
            ipEl.textContent = data.ip || window.location.hostname;

            clockSynced = Boolean(data.synced);
            if (clockSynced) {
                deviceEpoch = Number(data.epoch) || 0;
                deviceOffset = Number(data.offset) || 0;
                epochMarkedAt = Date.now();
                dateEl.textContent = formatDate(deviceEpoch, deviceOffset);
                clockNoteEl.textContent = "ESP32 local time";
                renderClock();
            } else {
                dateEl.textContent = data.date || "Waiting for network time";
                clockNoteEl.textContent = "ESP32 is still syncing NTP";
                renderClock();
            }
        }

        async function getStatus() {
            if (busy) return;

            try {
                const response = await fetch("/status", { cache: "no-store" });
                if (!response.ok) throw new Error("Status request failed");
                applyStatus(await response.json());
                setConnection(true);
            } catch (error) {
                setConnection(false);
                statusEl.textContent = "Connection error";
            }
        }

        async function toggleLED() {
            if (busy || !connected) return;

            busy = true;
            toggleEl.disabled = true;
            const nextState = !ledOn;

            try {
                const response = await fetch(nextState ? "/on" : "/off", { method: "POST" });
                if (!response.ok) throw new Error("Command failed");
                const data = await response.json();
                updateUI(data.led);
                setConnection(true);
            } catch (error) {
                setConnection(false);
                statusEl.textContent = "Command failed";
            } finally {
                busy = false;
                toggleEl.disabled = !connected;
            }
        }

        getStatus();
        setInterval(getStatus, 2000);
        setInterval(renderClock, 250);
    </script>
</body>
</html>
)rawliteral";
