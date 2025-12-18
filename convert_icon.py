from PIL import Image
import sys

def convert():
    try:
        img = Image.open('SimpleMediaSorter-MP_icon.png')
        img.save('app_icon.ico', format='ICO', sizes=[(256, 256), (128, 128), (64, 64), (48, 48), (32, 32), (16, 16)])
        print("Converted successfully")
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    convert()
