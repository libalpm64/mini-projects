const buttons = document.querySelectorAll('button[aria-label="Unarchive conversation"]');
let index = 0;

function clickNextButton() {
    if (index < buttons.length) {
        buttons[index].click();
        console.log(`Clicked button ${index + 1} of ${buttons.length}`);
        index++;
        setTimeout(clickNextButton, 300); 
    } else {
        console.log("All buttons clicked!");
    }
}

if (buttons.length > 0) {
    console.log(`Found ${buttons.length} buttons. Starting to click...`);
    clickNextButton();
} else {
    console.log("No buttons found.");
}
