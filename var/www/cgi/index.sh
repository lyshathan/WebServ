#!/bin/bash

fortunes=(
    "🔮 A bug in your code will reveal itself at 3 AM"
    "🍪 You will find the answer on Stack Overflow... eventually"
    "⚡ Today's lucky number is 404 - something will be missing"
    "🎯 Your next commit message will be 'fixed stuff'"
    "🚀 A segfault approaches from the east"
    "🎲 You will spend 3 hours debugging a missing semicolon"
    "🌟 Your code will compile on the first try (just kidding)"
    "🎪 A wild memory leak appears!"
    "🎭 You will question your life choices around lunch time"
    "🏆 Today you will finally understand pointers (or not)"
    "🎨 Your CSS will mysteriously work on IE6"
    "🔍 You will grep for answers in /dev/null"
    "🎯 A rubber duck will solve your hardest problem"
    "🌈 Your variables will be in perfect harmony... until they're not"
    "⚡ The force will be with you, but the compiler won't"
)

fortune_index=$((RANDOM % ${#fortunes[@]}))
selected_fortune="${fortunes[$fortune_index]}"

cat << 'EOF'
Content-Type: text/html; charset=UTF-8

<!DOCTYPE html>
<html>
<head>
    <title>🎱 Programmer's Fortune Cookie</title>
    <style>
        body {
            font-family: 'Courier New', monospace;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: #fff;
            margin: 0;
            padding: 20px;
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            text-align: center;
        }
        .container {
            max-width: 600px;
            background: rgba(0,0,0,0.3);
            border-radius: 15px;
            padding: 40px;
            box-shadow: 0 8px 32px rgba(0,0,0,0.3);
        }
        .ascii-art {
            font-size: 12px;
            line-height: 1;
            margin: 20px 0;
            color: #ffd700;
        }
        .fortune {
            font-size: 24px;
            margin: 30px 0;
            padding: 20px;
            background: rgba(255,255,255,0.1);
            border-radius: 10px;
            border-left: 4px solid #ffd700;
        }
        .meta {
            font-size: 14px;
            color: #ccc;
            margin-top: 30px;
        }
        .refresh {
            margin-top: 20px;
        }
        .refresh a {
            color: #ffd700;
            text-decoration: none;
            padding: 10px 20px;
            border: 2px solid #ffd700;
            border-radius: 25px;
            transition: all 0.3s;
        }
        .refresh a:hover {
            background: #ffd700;
            color: #333;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🎱 Programmer's Fortune Cookie</h1>
EOF

echo "            <p>$selected_fortune</p>"

cat << EOF
        </div>
    </div>
</body>
</html>
EOF
