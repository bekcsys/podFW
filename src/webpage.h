
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
            padding: 20px;
            background: #0f172a;
            color: #f8fafc;
            font-family: Arial, sans-serif;
        }

        .card {
            width: 100%;
            max-width: 420px;
            background: #1e293b;
            border: 1px solid #334155;
            border-radius: 20px;
            padding: 30px;
            box-shadow: 0 15px 40px rgba(0,0,0,.25);
        }

        .header, .footer {
            display: flex;
            align-items: center;
            justify-content: space-between;
        }

        .header {
            padding-bottom: 20px;
            border-bottom: 1px solid #334155;
        }

        .brand { font-size: 18px; font-weight: bold; }
        .connection { font-size: 12px; color: #94a3b8; }
        .connection.online { color: #4ade80; }

        .content {
            text-align: center;
            padding: 35px 0 15px;
        }

        .led-icon {
            width: 100px;
            height: 100px;
            margin: 0 auto 22px;
            border-radius: 50%;
            background: #334155;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 46px;
            transition: .25s;
        }

        .led-icon.on {
            background: #14532d;
            box-shadow: 0 0 35px rgba(74,222,128,.22);
        }

        h2 { margin: 0 0 12px; font-size: 25px; }

        .status {
            color: #94a3b8;
            font-size: 14px;
            margin-bottom: 32px;
        }

        .status.on { color: #4ade80; }

        .buttons { display: flex; gap: 12px; }

        button {
            flex: 1;
            padding: 16px;
            border-radius: 10px;
            font-size: 15px;
            font-weight: bold;
            cursor: pointer;
        }

        .btn-on {
            background: #22c55e;
            color: #052e16;
            border: none;
        }

        .btn-off {
            background: transparent;
            color: #f8fafc;
            border: 1px solid #64748b;
        }

        button:disabled { opacity: .5; }

        .footer {
            margin-top: 30px;
            padding-top: 20px;
            border-top: 1px solid #334155;
            color: #94a3b8;
            font-size: 12px;
        }
    </style>
</head>

<body>
    <div class="card">
        <div class="header">
            <div class="brand">ESP32 Control</div>
            <div id="connection" class="connection">Connecting...</div>
        </div>

        <div class="content">
            <div id="ledIcon" class="led-icon">&#128161;</div>
            <h2>Onboard LED</h2>
            <div id="status" class="status">Reading status...</div>

            <div class="buttons">
                <button class="btn-on" onclick="setLED(true)">
                    Turn ON
                </button>
                <button class="btn-off" onclick="setLED(false)">
                    Turn OFF
                </button>
            </div>
        </div>

        <div class="footer">
            <span>ESP-WROOM-32</span>
            <span id="ip"></span>
        </div>
    </div>

    <script>
        const statusEl = document.getElementById("status");
        const iconEl = document.getElementById("ledIcon");
        const connectionEl = document.getElementById("connection");

        document.getElementById("ip").textContent =
            window.location.hostname;

        function updateUI(on) {
            statusEl.textContent = on ? "LED is ON" : "LED is OFF";
            statusEl.classList.toggle("on", on);
            iconEl.classList.toggle("on", on);
        }

        async function getStatus() {
            try {
                const response = await fetch("/status", {
                    cache: "no-store"
                });

                if (!response.ok) throw new Error("Status failed");

                const data = await response.json();

                updateUI(data.led);
                connectionEl.textContent = "Connected";
                connectionEl.classList.add("online");

            } catch (error) {
                connectionEl.textContent = "Disconnected";
                connectionEl.classList.remove("online");
                statusEl.textContent = "Connection error";
            }
        }

        async function setLED(on) {
            const buttons = document.querySelectorAll("button");
            buttons.forEach(button => button.disabled = true);

            try {
                const response = await fetch(
                    on ? "/on" : "/off",
                    { method: "POST" }
                );

                if (!response.ok) throw new Error("Command failed");

                const data = await response.json();
                updateUI(data.led);

            } catch (error) {
                statusEl.textContent = "Command failed";
            } finally {
                buttons.forEach(button => button.disabled = false);
            }
        }

        getStatus();
        setInterval(getStatus, 2000);
    </script>
</body>
</html>
)rawliteral";
