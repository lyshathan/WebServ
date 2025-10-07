#!/usr/bin/env php
<?php
// PHP CGI Environment Display - Similar to index.py

// Output HTTP headers (CGI style)
// echo "Content-Type: text/html; charset=UTF-8\r\n\r\n";

?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Environment - Webserv (PHP)</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: #f5f5f7;
            color: #1d1d1f;
            padding: 40px 20px;
        }
        .container {
            max-width: 980px;
            margin: 0 auto;
        }
        h1 {
            font-size: 40px;
            font-weight: 600;
            margin-bottom: 12px;
        }
        .subtitle {
            font-size: 17px;
            color: #6e6e73;
            margin-bottom: 40px;
        }
        .card {
            background: white;
            border-radius: 18px;
            padding: 32px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.07);
            margin-bottom: 20px;
        }
        h2 {
            font-size: 24px;
            font-weight: 600;
            margin-bottom: 16px;
        }
        table {
            width: 100%;
            border-collapse: collapse;
        }
        th, td {
            text-align: left;
            padding: 12px;
            border-bottom: 1px solid #f5f5f7;
        }
        th {
            font-weight: 600;
            color: #1d1d1f;
        }
        td {
            font-family: 'SF Mono', Monaco, monospace;
            font-size: 14px;
            color: #6e6e73;
            word-break: break-all;
        }
        .body-content {
            background: #f5f5f7;
            padding: 16px;
            border-radius: 8px;
            font-family: 'SF Mono', Monaco, monospace;
            font-size: 14px;
            white-space: pre-wrap;
            word-break: break-all;
        }
        .empty {
            color: #6e6e73;
            font-style: italic;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>CGI Environment</h1>
        <p class="subtitle">Environment variables and request body (PHP <?= phpversion() ?>)</p>

        <div class="card">
            <h2>Environment Variables</h2>
            <table>
                <tr>
                    <th>Variable</th>
                    <th>Value</th>
                </tr>
<?php
// Sort and display all environment variables
ksort($_SERVER);
foreach ($_SERVER as $key => $value) {
    // Handle arrays and other non-string values
    if (is_array($value)) {
        $displayValue = json_encode($value, JSON_PRETTY_PRINT);
    } elseif (is_object($value)) {
        $displayValue = get_class($value) . ' object';
    } elseif (is_bool($value)) {
        $displayValue = $value ? 'true' : 'false';
    } elseif (is_null($value)) {
        $displayValue = 'null';
    } else {
        $displayValue = (string)$value;
    }

    echo "                <tr><td>" . htmlspecialchars($key) . "</td><td>" . htmlspecialchars($displayValue) . "</td></tr>\n";
}
?>
            </table>
        </div>
    </div>
</body>
</html>