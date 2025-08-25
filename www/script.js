// WebServ Demo JavaScript
document.addEventListener('DOMContentLoaded', function() {
    console.log('WebServ Demo JavaScript loaded successfully!');

    // Get DOM elements
    const colorBtn = document.getElementById('colorBtn');
    const timeBtn = document.getElementById('timeBtn');
    const output = document.getElementById('output');
    const title = document.getElementById('title');
    const body = document.body;

    // Array of background colors
    const colors = [
        'linear-gradient(135deg, #667eea 0%, #764ba2 100%)',
        'linear-gradient(135deg, #f093fb 0%, #f5576c 100%)',
        'linear-gradient(135deg, #4facfe 0%, #00f2fe 100%)',
        'linear-gradient(135deg, #43e97b 0%, #38f9d7 100%)',
        'linear-gradient(135deg, #fa709a 0%, #fee140 100%)',
        'linear-gradient(135deg, #a8edea 0%, #fed6e3 100%)'
    ];

    let currentColorIndex = 0;

    // Color change functionality
    colorBtn.addEventListener('click', function() {
        currentColorIndex = (currentColorIndex + 1) % colors.length;
        body.style.background = colors[currentColorIndex];
        body.classList.add('color-transition');

        output.innerHTML = `Background changed to color ${currentColorIndex + 1}!`;
        console.log('Background color changed');
    });

    // Time display functionality
    timeBtn.addEventListener('click', function() {
        const now = new Date();
        const timeString = now.toLocaleString();

        output.innerHTML = `
            <div style="text-align: center;">
                <strong>Current Time:</strong><br>
                ${timeString}
            </div>
        `;

        console.log('Time displayed:', timeString);
    });

    // Add some interactive effects
    title.addEventListener('mouseover', function() {
        this.style.transform = 'scale(1.05)';
        this.style.transition = 'transform 0.3s ease';
    });

    title.addEventListener('mouseout', function() {
        this.style.transform = 'scale(1)';
    });

    // Demonstrate AJAX request (if your server supports it)
    function makeAjaxRequest() {
        fetch('/index.html')
            .then(response => response.text())
            .then(data => {
                console.log('Successfully fetched data from server');
                output.innerHTML = 'AJAX request successful! Check console for details.';
            })
            .catch(error => {
                console.error('AJAX request failed:', error);
                output.innerHTML = 'AJAX request failed. This is normal if CORS is not configured.';
            });
    }

    // Initial message
    output.innerHTML = 'JavaScript is working! Click the buttons above to test functionality.';
});
