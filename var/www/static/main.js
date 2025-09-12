document.addEventListener('DOMContentLoaded', function() {
    console.log('🌿 GreenLeaf Plant Catalog loaded successfully!');

    // Add smooth scrolling to CTA buttons
    const ctaButtons = document.querySelectorAll('.cta-button');
    ctaButtons.forEach(button => {
        button.addEventListener('click', function(e) {
            // Add a subtle click animation
            this.style.transform = 'scale(0.95)';
            setTimeout(() => {
                this.style.transform = '';
            }, 150);
        });
    });

    // Add hover effects to plant cards
    const plantCards = document.querySelectorAll('.plant-card');
    plantCards.forEach(card => {
        card.addEventListener('mouseenter', function() {
            this.style.transition = 'all 0.3s ease';
        });
    });

    // Add a welcome message for first-time visitors
    if (!localStorage.getItem('visited')) {
        setTimeout(() => {
            console.log('🌱 Welcome to GreenLeaf! Discover amazing plants today.');
            localStorage.setItem('visited', 'true');
        }, 1000);
    }

    // Add current year to footer if it exists
    const footer = document.querySelector('footer p');
    if (footer && footer.textContent.includes('2025')) {
        const currentYear = new Date().getFullYear();
        if (currentYear !== 2025) {
            footer.textContent = footer.textContent.replace('2025', currentYear);
        }
    }

    // Simple image lazy loading for better performance
    const images = document.querySelectorAll('img');
    images.forEach(img => {
        img.addEventListener('load', function() {
            this.style.opacity = '1';
        });

        // Set initial opacity for smooth loading
        img.style.opacity = '0';
        img.style.transition = 'opacity 0.3s ease';
    });
});
