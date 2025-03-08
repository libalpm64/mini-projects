import tkinter as tk
from tkinter import messagebox
import pyautogui
import pytesseract
import requests
from threading import Timer
pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'

def capture_screen_segment():
    def on_mouse_down(event):
        overlay.attributes("-alpha", 0.3) 
        canvas.delete("rect")
        canvas.data['start'] = (event.x, event.y)

    def on_mouse_move(event):
        if canvas.data['start']:
            canvas.delete("rect")
            canvas.create_rectangle(
                *canvas.data['start'],
                event.x,
                event.y,
                outline='red',
                tag="rect"
            )

    def on_mouse_up(event):
        canvas.data['end'] = (event.x, event.y)
        overlay.quit()

    overlay = tk.Toplevel()
    overlay.attributes("-fullscreen", True)
    overlay.attributes("-alpha", 0.1) 
    canvas = tk.Canvas(overlay, cursor="cross")
    canvas.pack(fill=tk.BOTH, expand=True)

    canvas.data = {}
    
    canvas.bind("<ButtonPress-1>", on_mouse_down)
    canvas.bind("<B1-Motion>", on_mouse_move)
    canvas.bind("<ButtonRelease-1>", on_mouse_up)

    overlay.mainloop()
    overlay.destroy()

    return canvas.data['start'], canvas.data['end']


def update_coordinates_display(start, end):
    coords_text.set(f"X1: {start[0]}, Y1: {start[1]}, X2: {end[0]}, Y2: {end[1]}")

def select_screen_segment():
    global start_coords, end_coords, has_selection
    start_coords, end_coords = capture_screen_segment()
    has_selection = True
    update_coordinates_display(start_coords, end_coords)


def continuous_monitoring():
    capture_and_send()
    # Reschedule the continuous_monitoring after the cooldown period
    if monitoring:
        app.after(int(cooldown_entry.get()) * 1000, continuous_monitoring)

def start_monitoring():
    global monitoring
    if not has_selection:
        messagebox.showerror("Error", "Please select a screen segment first.")
        return
    monitoring = True
    continuous_monitoring()

def stop_monitoring():
    global monitoring
    monitoring = False

last_captured_text = ""

def capture_and_send():
    global last_captured_text
    if not has_selection:
        return
    try:
        # Use existing global coordinates for capturing
        region = (start_coords[0], start_coords[1], end_coords[0] - start_coords[0], end_coords[1] - start_coords[1])
        screenshot = pyautogui.screenshot(region=region)

        # Using pytesseract to get text from the screenshot
        text = pytesseract.image_to_string(screenshot).strip()

        # Check if the captured text is different from the last captured text
        if text and text != last_captured_text:
            last_captured_text = text  # Update the last captured text

            webhook_url = webhook_entry.get()
            data = {"content": text, "username": "Screen Reader Bot"}
            response = requests.post(webhook_url, json=data)

            if response.status_code == 204:
                # Schedule the status update to the main thread
                app.after(0, status_text.set, "Webhook sent successfully!")
                # Schedule a reset of the status text after the cooldown period
                app.after(int(cooldown_entry.get()) * 1000, status_text.set, "")
            else:
                # Schedule the status update to the main thread
                app.after(0, status_text.set, f"Failed to send webhook: {response.status_code}")

    except Exception as e:
        # Schedule the error message box to the main thread
        app.after(0, messagebox.showerror, "Error", str(e))


# Main GUI application
app = tk.Tk()
app.title("Screen Segment Selector")

# Display for selected coordinates
coords_text = tk.StringVar()
coords_label = tk.Label(app, textvariable=coords_text)
coords_label.pack()

# Status message display
status_text = tk.StringVar()
status_label = tk.Label(app, textvariable=status_text)
status_label.pack()

# Entry for Discord webhook URL
webhook_label = tk.Label(app, text="Discord Webhook URL")
webhook_label.pack()
webhook_entry = tk.Entry(app)
webhook_entry.pack()

# Entry for cooldown configuration
cooldown_label = tk.Label(app, text="Cooldown (seconds)")
cooldown_label.pack()
cooldown_entry = tk.Entry(app)
cooldown_entry.insert(0, "10")  # Default cooldown of 10 seconds
cooldown_entry.pack()

# Button to capture and send once
capture_button = tk.Button(app, text="Capture Screen Segment", command=capture_and_send)
capture_button.pack()

# Button to start continuous monitoring
start_button = tk.Button(app, text="Start Monitoring", command=start_monitoring)
start_button.pack()

# Button to stop continuous monitoring
stop_button = tk.Button(app, text="Stop Monitoring", command=stop_monitoring)
stop_button.pack()

# Run the application
app.mainloop()

if __name__ == "__main__":
    app.after(0, continuous_monitoring)  # Kick off the continuous monitoring
    app.mainloop()  # Start the Tkinter event loop